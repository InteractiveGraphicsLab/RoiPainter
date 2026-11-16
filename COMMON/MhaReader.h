#pragma once

#undef min
#undef max

#include <cfloat>
#include <memory>
#include <limits>
#include <iterator>
#include <sstream>
#include <zlib.h>
#include "tmath.h"

namespace MhaReader
{
  inline bool ReadBoolean(std::istream& fields, bool& value)
  {
    std::string text;
    fields >> text;
    if (text == "True" || text == "true" || text == "1") value = true;
    else if (text == "False" || text == "false" || text == "0") value = false;
    else return false;
    return true;
  }

  // Parse only supported, local, little-endian 3D MHA payloads.
  // Validate the header before allocation and require the complete payload.
  inline bool ReadVolume(
      std::istream& input, EVec3i& reso, EVec3f& pitch,
      std::unique_ptr<byte[]>& volume_b, std::unique_ptr<short[]>& volume_s)
  {
    reso = EVec3i::Zero();
    pitch = EVec3f::Ones(); // ElementSpacing is optional in MetaImage.
    volume_b.reset();
    volume_s.reset();
    int dimensions = 0;
    std::string type, line;
    bool compressed = false, local_data = false;
    while (std::getline(input, line))
    {
      const size_t equal = line.find('=');
      if (equal == std::string::npos) continue;
      std::string key, value;
      std::istringstream(line.substr(0, equal)) >> key;
      std::istringstream fields(line.substr(equal + 1));
      if (key == "NDims") fields >> dimensions;
      else if (key == "DimSize") fields >> reso[0] >> reso[1] >> reso[2];
      else if (key == "ElementSpacing") fields >> pitch[0] >> pitch[1] >> pitch[2];
      else if (key == "ElementType") fields >> type;
      else if (key == "CompressedData")
      {
        if (!ReadBoolean(fields, compressed)) return false;
      }
      else if (key == "BinaryData" || key == "BinaryDataByteOrderMSB" || key == "ElementByteOrderMSB")
      {
        bool enabled;
        if (!ReadBoolean(fields, enabled)) return false;
        if ((key == "BinaryData" && !enabled) || (key != "BinaryData" && enabled)) return false;
      }
      else if (key == "ElementNumberOfChannels")
      {
        int channels = 0;
        fields >> channels;
        if (channels != 1) return false;
      }
      else if (key == "ElementDataFile")
      {
        fields >> value;
        local_data = value == "LOCAL";
        break;
      }
      if (!fields) return false;
    }
    if (!local_data || dimensions != 3 || reso.minCoeff() <= 0 ||
        !pitch.allFinite() || pitch.minCoeff() <= 0 ||
        (type != "MET_UCHAR" && type != "MET_SHORT")) return false;

    size_t num_voxels = 1;
    for (int axis = 0; axis < 3; ++axis)
    {
      if (num_voxels > (size_t)std::numeric_limits<int>::max() / reso[axis]) return false;
      num_voxels *= reso[axis];
    }
    const size_t num_bytes = num_voxels * (type == "MET_SHORT" ? sizeof(short) : sizeof(byte));
    if (num_bytes > std::numeric_limits<uLongf>::max()) return false;

    std::unique_ptr<byte[]> bytes;
    std::unique_ptr<short[]> shorts;
    if (type == "MET_UCHAR") bytes = std::make_unique<byte[]>(num_voxels);
    else shorts = std::make_unique<short[]>(num_voxels);
    char* destination = type == "MET_UCHAR" ? reinterpret_cast<char*>(bytes.get())
                                           : reinterpret_cast<char*>(shorts.get());
    if (compressed)
    {
      const std::vector<byte> payload((std::istreambuf_iterator<char>(input)), {});
      if (payload.empty() || payload.size() > std::numeric_limits<uLong>::max()) return false;
      uLongf actual_size = static_cast<uLongf>(num_bytes);
      if (uncompress(reinterpret_cast<Bytef*>(destination), &actual_size,
          payload.data(), static_cast<uLong>(payload.size())) != Z_OK || actual_size != num_bytes)
        return false;
    }
    else if (!input.read(destination, static_cast<std::streamsize>(num_bytes))) return false;

    volume_b = std::move(bytes);
    volume_s = std::move(shorts);
    return true;
  }


}
