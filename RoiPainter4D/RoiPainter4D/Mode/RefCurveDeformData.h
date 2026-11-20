#pragma once

#include <cfloat>
#include <cstring>
#include "DeformationStrokes.h"
#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <ostream>
#include <stdexcept>
#include <string>

// Data transformations shared by the form, the mode and regression tests.
namespace RefCurveDeformData
{
  inline std::string QuoteYamlString(const std::string& value)
  {
    std::string quoted = "'";
    for (char c : value)
    {
      quoted += c;
      if (c == '\'') quoted += '\'';
    }
    return quoted + "'";
  }

  inline void WriteCurve(std::ostream& out, const PlanarCurve& curve, int frame_idx)
  {
    for (int i = 0; i < curve.GetNumCPs(); ++i)
    {
      const EVec3f p = curve.GetCP(i);
      out << (i == 0 ? "- - [" : "  - [")
          << p[0] << ", " << p[1] << ", " << p[2] << ", " << frame_idx << "]\n";
    }
  }

  inline bool WriteConfig(
      std::ostream& out, const std::string& input_dir, const std::string& output_dir,
      int target_mask_id, const std::vector<std::vector<PlanarCurve>>& curves,
      const std::vector<SharedCurves>& shared_curves, std::string& error)
  {
    error.clear();
    if (curves.empty() || target_mask_id <= 0 || target_mask_id > 255 ||
        input_dir.empty() || output_dir.empty())
    {
      error = "The image sequence, target mask and input/output folders must be specified.";
      return false;
    }
    bool has_points = false;
    auto valid_curve = [&has_points](const PlanarCurve& curve) {
      has_points = has_points || curve.GetNumCPs() > 0;
      for (int i = 0; i < curve.GetNumCPs(); ++i)
        if (!curve.GetCP(i).allFinite()) return false;
      return true;
    };
    for (const auto& frame : curves)
      for (const auto& curve : frame)
        if (!valid_curve(curve))
        {
          error = "A curve contains invalid coordinates.";
          return false;
        }
    for (const auto& shared : shared_curves)
    {
      if (shared.GetNumFrames() != (int)curves.size())
      {
        error = "A shared curve has a different frame count from the image sequence.";
        return false;
      }
      for (int fi = 0; fi < (int)curves.size(); ++fi)
        if (!valid_curve(shared.GetCurve(fi)))
        {
          error = "A shared curve contains invalid coordinates.";
          return false;
        }
    }

    // Keep the existing backend schema and write UTF-8 paths supplied by the form.
    out.imbue(std::locale::classic());
    out << std::setprecision(std::numeric_limits<float>::max_digits10);
    out << "data: {input_dir: " << QuoteYamlString(input_dir)
        << ", output_dir: " << QuoteYamlString(output_dir)
        << ", target_id: " << target_mask_id << "}\n";
    out << "volume: {num_input_frames: null, num_output_frames: null}\n";
    out << (has_points ? "curves:\n" : "curves: []\n");
    for (int fi = 0; fi < (int)curves.size(); ++fi)
    {
      for (const auto& curve : curves[fi]) WriteCurve(out, curve, fi);
      // Include the same interpolated constraints used by DeformAllFrame().
      for (const auto& shared : shared_curves) WriteCurve(out, shared.GetCurve(fi), fi);
    }
    if (!out)
    {
      error = "Failed to write the YAML configuration.";
      return false;
    }
    return true;
  }

  inline std::vector<short> DownsampleMask(
      const byte* mask, const EVec3i& reso, int stride, byte target_mask_id)
  {
    if (!mask || stride <= 0 || target_mask_id == 0 ||
        reso.minCoeff() < stride ||
        reso[0] > std::numeric_limits<int>::max() / reso[1] ||
        reso[0] * reso[1] > std::numeric_limits<int>::max() / reso[2])
      throw std::invalid_argument("Invalid mask dimensions or mesh sampling stride.");

    const EVec3i sampled_reso = reso / stride;
    const int w = sampled_reso[0], h = sampled_reso[1], d = sampled_reso[2];
    const long long samples_per_voxel = (long long)stride * stride * stride;
    std::vector<short> sampled(w * h * d, 0);
    for (int z = 0; z < d; ++z)
      for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
        {
          long long matches = 0;
          for (int c = 0; c < stride; ++c)
            for (int b = 0; b < stride; ++b)
              for (int a = 0; a < stride; ++a)
              {
                const int idx = (z * stride + c) * reso[1] * reso[0]
                              + (y * stride + b) * reso[0] + x * stride + a;
                if (mask[idx] == target_mask_id) ++matches;
              }
          sampled[z * h * w + y * w + x] =
              matches * 100 >= samples_per_voxel * 49 ? 255 : 0;
        }
    return sampled;
  }

  // Compare numeric runs by value, so frame2 precedes frame10.
  inline bool FrameFileNameLess(const std::wstring& left, const std::wstring& right)
  {
    size_t a = 0, b = 0;
    while (a < left.size() && b < right.size())
    {
      if (left[a] >= L'0' && left[a] <= L'9' && right[b] >= L'0' && right[b] <= L'9')
      {
        size_t ae = a, be = b;
        while (ae < left.size() && left[ae] >= L'0' && left[ae] <= L'9') ++ae;
        while (be < right.size() && right[be] >= L'0' && right[be] <= L'9') ++be;
        while (a < ae && left[a] == L'0') ++a;
        while (b < be && right[b] == L'0') ++b;
        if (ae - a != be - b) return ae - a < be - b;
        const int comparison = left.compare(a, ae - a, right, b, be - b);
        if (comparison != 0) return comparison < 0;
        a = ae; b = be;
      }
      else
      {
        if (left[a] != right[b]) return left[a] < right[b];
        ++a; ++b;
      }
    }
    if (a != left.size() || b != right.size()) return a == left.size();
    return left < right;
  }
}
