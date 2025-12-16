#pragma once

#include <iostream>
#include <vector>
#include <set>
#include "tmath.h"


class PlanarCurve
{
private:

  //plane info
  int   m_plane_xyz = 0; //0: yz, 1:zx, 2:xy
  float m_plane_pos = 0; //pposition 

  //cp and curve info 
  std::vector<EVec3f> m_curve = {};
  std::vector<EVec3f> m_cps   = {};
  bool m_normal_side = true;

public:
  PlanarCurve() {}

  inline int GetNumCPs() const { return static_cast<int>(m_cps.size()); }
  inline int   GetPlaneXYZ() const { return m_plane_xyz; }
  inline float GetPlanePos() const { return m_plane_pos; }
  inline void  SetPlaneXYZ(int plane_xyz) { m_plane_xyz = plane_xyz; }
  inline void  SetPlanePos(float plane_pos) { m_plane_pos = plane_pos; }
  inline std::vector<EVec3f> GetCurve() const { return m_curve; }
  inline std::vector<EVec3f> GetCPs  () const { return m_cps;   }

  inline bool GetNormalSide() const { return m_normal_side; }
  inline void FlipNormal() { m_normal_side = !m_normal_side; }

  //CP manipulation
  bool AddCP(const EVec3f& pos, int& inserted_idx); // return inserted cp idx
  void MoveCP(int cpidx, const EVec3f& _pos);
  void DeleteCP(int cpidx);
  int  PickCPs(const EVec3f& ray_pos, const EVec3f& ray_dir, const float cp_radius);


  void Draw   (const EVec4f& color, const float thickness) const;
  void DrawCPs(const EVec4f& color, float radius, int select_cp_idx) const;

  std::string OutputAsText() const;
  void InitByCPs(const std::vector<EVec3f>& cps);

private:
  void UpdateCurve();

};




class SharedCurves
{
  
private:
  std::vector<PlanarCurve> m_curves; // frame分のcurve
  std::vector<bool> m_manip; //Userがf-frameを編集したら m_manip[f] = true
  
public:
  SharedCurves(const int num_frame, const int init_frame, const PlanarCurve &src)
  {
    m_curves = std::vector<PlanarCurve>(num_frame, src);
    m_manip = std::vector<bool>(num_frame, false);
    m_manip[init_frame] = true;
  }

  void MoveCP(const int frame_idx, const int cp_idx, const EVec3f &pos) 
  {
    if (frame_idx < 0 || m_curves.size() <= frame_idx) return;
    m_curves[frame_idx].MoveCP(cp_idx, pos);
  }
  void Draw(const int frame_idx, const bool is_on_manip, const float cp_radius, const int select_cp_idx);
};







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
  bool GetStrokeNormalsSide(const int _idx);
  void UnselectStroke();
  int PickCPs(const EVec3f& ray_pos, const EVec3f& ray_dir, const float cp_radius, 
              const bool change_selected_idx = true, const bool only_selected = false);
  std::vector<std::vector<EVec3f>> GetAllStrokeCurves() const;

  //Manipulation for selected stroke
  void  Delete_SelStroke();
  bool  AddCP_SelStroke(const EVec3f& _pos);
  bool  AddCP_SelStroke(const EVec3f& _pos, const EVec3f& _nearest_vertex_normal);
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
  
  // flip normal
  void FlipSelNormals();

  std::string OutputAsText() const;
  void LoadState(const std::vector<int>& _vec_shared_idxs, const std::vector<std::vector<EVec3f>>& _vec_cps);
};


class DeformationStrokes::Stroke
{
private:

  //plane info
  int   m_plane_xyz; //0: yz, 1:zx, 2:xy
  float m_plane_pos; //pposition 
  
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
  bool GetNormalSide() const { return m_normal_side; }
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
  bool AddNewCP(const EVec3f _pos, const EVec3f& _nearest_vertex_normal);
  void MoveSelectedCP(const EVec3f& _pos);
  void DeleteSelectedCP();

  int  PickCPs(const EVec3f& _ray_pos, const EVec3f& _ray_dir, const float& _cp_radius, const bool& _change_selected_idx);

  void FlipNormal();
  void UpdateStroke();
  void DrawStroke(const bool& _is_selected) const;
  void DrawControlPoints(const float& _cp_radius, const bool& _is_selected) const;

  std::string OutputAsText() const;
  void LoadState(const std::vector<EVec3f>& _cps);
};





