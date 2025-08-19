#pragma once

#include <iostream>
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
  int m_selected_stroke_idx;

public:

  DeformationStrokes();

  bool operator==(const DeformationStrokes& _src) const;
  bool operator!=(const DeformationStrokes& _src) const;

  //Getter / Setter 
  int GetNumStrokes() const { return static_cast<int>(m_strokes.size()); }
  int GetSelectedStrokeIdx() const { return m_selected_stroke_idx; }
  void SetSelectedStrokeIdx(const int& _idx) {
    m_selected_stroke_idx = (_idx < 0 || m_strokes.size() <= _idx) ? -1 : _idx;
  }

  // Stroke Manipulation 
  void AddNewStroke();
  void ClearAllStrokes();
  int  GetStrokePlaneXYZ(const int _idx);
  void UnselectStroke();
  int PickCPs(const EVec3f& ray_pos, const EVec3f& ray_dir, const float cp_radius, 
              const bool change_selected_idx = true, const bool only_selected = false);
  std::vector<std::vector<EVec3f>> GetAllStrokeCurves() const;

  //Manipulation for selected stroke
  void  Delete_SelStroke();
  bool  AddCP_SelStroke(const EVec3f& _pos);
  int   GetPlaneXyz_SelStroke() const;
  float GetPlanePos_SelStroke() const;
  void  Lock_SelStroke();
  void  Unlock_SelStroke();
  int   GetShareIdx_SelStroke();
  std::vector<EVec3f> GetCPs_SelStroke();

  //Manipulation for selected stroke's selected CP
  void MoveSelectedCP(const EVec3f& _pos);
  void DeleteSelectedCP();

  void DrawStrokes(const bool& _only_selected_stroke = false) const;
  void DrawControlPoints(const float& _cp_radius, const bool& _only_selected_stroke = false) const;

  //Shared stroke関連
  void MakeShare_SelStroke(const int new_shared_idx);
  void AddNewSharedStroke(const int new_shared_idx, const int   plane_xyz, const float plane_pos,
                          const std::vector<EVec3f>& cps, bool locked = false);
  void UnsharedStroke(const int _shared_idx);
  bool GetCpsOfLockedSharedStroke(const int _shared_idx, std::vector<EVec3f>& cps);
  void UpdateSharedStroke(const int _shared_idx, const std::vector<EVec3f>& cps);
  void UpdateSharedStroke(const int _shared_idx, const std::vector<EVec3f>& cps1, const std::vector<EVec3f>& cps2, const float rate);

  void  UnlockAllStrokes(); // copy from prevしたときの shared strokeの扱いが不明
  bool bSelStrokeShared();


  std::string OutputAsText() const;
  void LoadState(const std::vector<int>& _vec_shared_idxs, const std::vector<std::vector<EVec3f>>& _vec_cps);
};


class DeformationStrokes::Stroke
{
private:

  //plane info
  int   m_plane_xyz; //0: yz, 1:zx, 2:xy
  float m_plane_pos; //pposition 
  EVec3f m_plane_normal;
  
  //cp and stroke info 
  std::vector<EVec3f> m_stroke;
  std::vector<EVec3f> m_cps;
  bool m_normal_side;
  int m_selected_cpid;

public:
  bool m_is_shared;
  int  m_shared_idx; //shared stroke idx (全体の何番目かを入れておく)
  bool m_is_locked;

  Stroke();

  bool operator==(const Stroke& _src) const;
  bool operator!=(const Stroke& _src) const;

  //Getters / Setters 
  int   GetNumCPs  () const{ return static_cast<int>(m_cps.size()); }
  int   GetPlaneXYZ() const{ return m_plane_xyz; }
  float GetPlanePos() const{ return m_plane_pos; }
  void  SetPlaneXYZ(int plane_xyz) { m_plane_xyz = plane_xyz; }
  void  SetPlanePos(float plane_pos) { m_plane_pos = plane_pos; }
  std::vector<EVec3f> GetStroke() const { return m_stroke; }
  std::vector<EVec3f> GetCPs() const { return m_cps; }

  int  GetSelectedCpIdx() const { return m_selected_cpid; }
  void SetSelectedCpIdx(const int _idx){ 
    m_selected_cpid = (_idx < 0 || m_cps.size() <= _idx) ?  -1 : _idx;
  }
  void SetCPs(const std::vector<EVec3f>& _cps){
    m_cps = _cps;
    UpdateStroke();
  }
  
  //CP manipulation
  bool AddNewCP(const EVec3f _pos);
  void MoveSelectedCP(const EVec3f& _pos);
  void DeleteSelectedCP();

  int  PickCPs(const EVec3f& _ray_pos, const EVec3f& _ray_dir, const float& _cp_radius, const bool& _change_selected_idx);

  void ChangeSide();
  void UpdateStroke();
  void DrawStroke(const bool& _is_selected) const;
  void DrawControlPoints(const float& _cp_radius, const bool& _is_selected) const;

  std::string OutputAsText() const;
  void LoadState(const std::vector<EVec3f>& _cps);
};





