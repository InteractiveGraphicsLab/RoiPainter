#pragma once

#include <iostream>
#include <Core>
#include <vector>
#include <set>


class DeformationStrokes
{
public:
  class Stroke;

private:
  typedef Eigen::Vector3f EVec3f;
  typedef Eigen::VectorXf EVecXf;
  typedef Eigen::MatrixXf EMatXf;
  static const bool m_debug = false;

  std::vector<Stroke> m_strokes;
  //std::vector<Stroke> m_interpolated_strokes;
  std::set<int> m_set_shared_idx;
  int m_selected_stroke_idx;

public:

  DeformationStrokes();
  //~DeformationStrokes();

  bool operator==(const DeformationStrokes& _src) const;
  bool operator!=(const DeformationStrokes& _src) const;

  int GetNumStrokes() const;
  int GetSelectedStrokeIdx() const;
  void SetSelectedStrokeIdx(const int& _idx);

  void AddStroke();
  void DeleteSelectedStroke();
  void ClearAllStrokes();
  std::vector<std::vector<EVec3f>> GetStroke() const;

  bool AddControlPoint(const EVec3f& _pos);
  int PickControlPoint(const EVec3f& _ray_pos, const EVec3f& _ray_dir, const float& _cp_radius, const bool& _change_selected_idx = true, const bool& _only_selected = false);
  void MoveSelectedControlPoint(const EVec3f& _pos);
  void DeleteSelectedControlPoint();
  void ReleaseStroke();
  int GetCommonXYZ() const;
  float GetCommonCoord() const;

  Stroke* ShareSelectedStroke(const int& _shared_idx);
  int UnshareSelectedStroke();
  void LockSelectedStroke();
  void UnlockSelectedStroke();
  void UnlockAllStrokes();
  void UpdateSharedStrokes(const int& _shared_idx, Stroke* _left, Stroke* _right, const float& _rate = 0.0f);
  Stroke* GetSharedStroke(const int& _shared_idx);
  bool IsSelectedStrokeShared();

  void DrawStrokes(const bool& _only_selected_stroke = false) const;
  void DrawControlPoints(const float& _cp_radius, const bool& _only_selected_stroke = false) const;

  std::string OutputAsText() const;
  void LoadState(const std::vector<int>& _vec_shared_idxs, const std::vector<std::vector<EVec3f>>& _vec_cps);
};


class DeformationStrokes::Stroke
{
private:
  std::vector<EVec3f> m_stroke;
  std::vector<EVec3f> m_cps;
  int m_selected_cp_idx;
  int m_common_xyz;
  float m_common_coord;

public:

  int m_shared_idx;
  bool m_is_locked;

  Stroke();
  //~Stroke();

  bool operator==(const Stroke& _src) const;
  bool operator!=(const Stroke& _src) const;

  int GetNumControlPoints() const;
  int GetSelectedControlPointIdx() const;
  void SetSelectedControlPointIdx(const int& _idx);
  int GetCommonXYZ() const;
  float GetCommonCoord() const;

  bool AddControlPoint(const EVec3f& _pos, const bool& _is_not_loading = true);
  void MoveSelectedControlPoint(const EVec3f& _pos);
  void DeleteSelectedControlPoint();
  void ClearAllControlPoints();
  int PickControlPoint(const EVec3f& _ray_pos, const EVec3f& _ray_dir, const float& _cp_radius, const bool& _change_selected_idx);
  void UpdateStroke();
  std::vector<EVec3f> GetStroke() const;
  std::vector<EVec3f> GetControlPoints() const;
  void SetControlPoints(const std::vector<EVec3f>& _cps);

  void DrawStroke(const bool& _is_selected) const;
  void DrawControlPoints(const float& _cp_radius, const bool& _is_selected) const;

  std::string OutputAsText() const;
  void LoadState(const std::vector<EVec3f>& _cps);
};
