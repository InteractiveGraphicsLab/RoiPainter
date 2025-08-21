#include "DeformationStrokes.h"
#include "OglForCLI.h"
#include "TMesh.h"
#include "kcurves.h"
#include <fstream>


DeformationStrokes::DeformationStrokes()
{
  m_strokes.clear();
  m_selected_stroke_idx = -1;
}


bool DeformationStrokes::operator==(const DeformationStrokes& _src) const
{
  if (this->m_selected_stroke_idx != _src.m_selected_stroke_idx) return false;
  if (this->m_strokes != _src.m_strokes) return false;
  return true;
}


bool DeformationStrokes::operator!=(const DeformationStrokes& _src) const
{
  return !(*this == _src);
}


void DeformationStrokes::AddNewStroke()
{
  m_strokes.push_back(Stroke());
  m_selected_stroke_idx = static_cast<int>(m_strokes.size()) - 1;
}


void DeformationStrokes::ClearAllStrokes()
{
  m_strokes.clear();
  m_selected_stroke_idx = -1;
}


void DeformationStrokes::Delete_SelStroke()
{
  if (m_selected_stroke_idx == -1) return;
  m_strokes.erase(m_strokes.begin() + m_selected_stroke_idx);
  m_selected_stroke_idx = -1;
}


void DeformationStrokes::DeleteSelectedCP()
{
  if (m_selected_stroke_idx == -1) return;

  m_strokes[m_selected_stroke_idx].DeleteSelectedCP();

  if (m_strokes[m_selected_stroke_idx].GetNumCPs() == 0)
    Delete_SelStroke();
}

void DeformationStrokes::MoveSelectedCP(const EVec3f& _pos)
{
  if (m_selected_stroke_idx == -1) return;
  m_strokes[m_selected_stroke_idx].MoveSelectedCP(_pos);
}




std::vector<std::vector<EVec3f>> DeformationStrokes::GetAllStrokeCurves() const
{
  std::vector<std::vector<EVec3f>> curves;

  for (const auto& stroke : m_strokes)
  {
    std::vector<EVec3f> vec_i = stroke.GetStroke();
    if (vec_i.size() > 0) curves.push_back(vec_i);
  }
  return curves;
}




int DeformationStrokes::GetStrokePlaneXYZ(const int _idx)
{
  if (_idx < 0 || _idx >= m_strokes.size()) return 0;
  return m_strokes[_idx].GetPlaneXYZ();
}


bool DeformationStrokes::AddCP_SelStroke(const EVec3f& _pos)
{
  if (m_selected_stroke_idx == -1)
  {
    AddNewStroke();
  }

  if (m_strokes[m_selected_stroke_idx].m_shared_idx != -1) return false;

  return m_strokes[m_selected_stroke_idx].AddNewCP(_pos);
}



int DeformationStrokes::PickCPs(
    const EVec3f& _ray_pos, 
    const EVec3f& _ray_dir, 
    const float _cp_radius, 
    const bool _change_selected_idx, 
    const bool _only_selected)
{
  const int size = static_cast<int>(m_strokes.size());
  const int tmp = std::max(m_selected_stroke_idx, 0);

  for (int i = tmp; i < size + tmp; ++i)
  {
    const int n = i % size;
    if (_only_selected && m_selected_stroke_idx != i) continue;

    if (m_strokes[n].PickCPs(_ray_pos, _ray_dir, _cp_radius, _change_selected_idx) != -1)
    {
      if (_change_selected_idx) m_selected_stroke_idx = n;
      return n;
    }
  }

  if (_change_selected_idx) m_selected_stroke_idx = -1;
  return -1;
}



void DeformationStrokes::UnselectStroke()
{
  m_selected_stroke_idx = -1;
}

int DeformationStrokes::GetPlaneXyz_SelStroke() const
{
  if (m_selected_stroke_idx == -1) return -1;
  return m_strokes[m_selected_stroke_idx].GetPlaneXYZ();
}

float DeformationStrokes::GetPlanePos_SelStroke() const
{
  if (m_selected_stroke_idx == -1) return -1.0f;
  return m_strokes[m_selected_stroke_idx].GetPlanePos();
}

int DeformationStrokes::GetShareIdx_SelStroke()
{
  if (m_selected_stroke_idx == -1) return -1;
  return m_strokes[m_selected_stroke_idx].m_shared_idx;
}

std::vector<EVec3f> DeformationStrokes::GetCPs_SelStroke()
{
  if (m_selected_stroke_idx == -1) return std::vector<EVec3f>();
  return m_strokes[m_selected_stroke_idx].GetCPs();
}


void DeformationStrokes::Lock_SelStroke()
{
  if (m_selected_stroke_idx == -1) return;
  m_strokes[m_selected_stroke_idx].m_is_locked = true;
}

void DeformationStrokes::Unlock_SelStroke()
{
  if (m_selected_stroke_idx == -1) return;
  m_strokes[m_selected_stroke_idx].m_is_locked = false;
}

void DeformationStrokes::UnlockAllStrokes()
{
  for (auto& stroke : m_strokes)
  {
    if (stroke.m_shared_idx != -1)
    {
      stroke.m_is_locked = false;
    }
  }
}



void DeformationStrokes::MakeShare_SelStroke(const int new_shared_idx)
{
  if (m_selected_stroke_idx == -1) return ;
  m_strokes[m_selected_stroke_idx].m_is_shared = true;
  m_strokes[m_selected_stroke_idx].m_shared_idx = new_shared_idx;
  m_strokes[m_selected_stroke_idx].m_is_locked = true;
}


void DeformationStrokes::AddNewSharedStroke(
    const int new_shared_idx,
    const int   plane_xyz, 
    const float plane_pos,
    const std::vector<EVec3f>& cps, 
    bool locked )
{
  m_strokes.push_back(Stroke());
  int idx = (int)m_strokes.size()-1;
  m_strokes[idx].m_is_shared = true;
  m_strokes[idx].m_shared_idx = new_shared_idx;
  m_strokes[idx].m_is_locked = locked;
  m_strokes[idx].SetPlaneXYZ(plane_xyz);
  m_strokes[idx].SetPlanePos(plane_pos);
  m_strokes[idx].SetCPs(cps);
}





void DeformationStrokes::UnsharedStroke(const int _shared_idx)
{
  //search the target stroke index
  int stroke_idx = -1;
  for (int i = 0; i < (int)m_strokes.size(); ++i)
  {
    if (m_strokes[i].m_shared_idx == _shared_idx)
      stroke_idx = i;
  }
  if (stroke_idx == -1) return;

  // lockedなら sharedでなくす。
  // lockedでないなら削除
  if (m_strokes[stroke_idx].m_is_locked == true)
  {
    m_strokes[stroke_idx].m_is_shared = false;
    m_strokes[stroke_idx].m_shared_idx = -1;
    m_strokes[stroke_idx].m_is_locked = true;
  }
  else
  {
    m_strokes.erase(m_strokes.begin() + stroke_idx);
  }
}


void DeformationStrokes::UpdateSharedStroke(
  const int _shared_idx,
  const std::vector<EVec3f>& cps)
{
  //search the target stroke index
  int stroke_idx = -1;
  for (int i = 0; i < (int)m_strokes.size(); ++i)
  {
    if (m_strokes[i].m_shared_idx == _shared_idx)
      stroke_idx = i;
  }
  if (stroke_idx == -1) return;

  Stroke& stroke = m_strokes[stroke_idx];
  if (stroke.m_is_locked) return; //skip 編集中のstrokeは  補間しない
  stroke.SetCPs(cps);
}


void DeformationStrokes::UpdateSharedStroke(
  const int _shared_idx,
  const std::vector<EVec3f>& cps1,
  const std::vector<EVec3f>& cps2,
  const float rate)
{
  //search the target stroke index
  int stroke_idx = -1;
  for (int i = 0; i < (int)m_strokes.size(); ++i)
  {
    if (m_strokes[i].m_shared_idx == _shared_idx)
      stroke_idx = i;
  }
  if (stroke_idx == -1) return;

  Stroke& stroke = m_strokes[stroke_idx];
  if (stroke.m_is_locked) return; //skip 編集中のstrokeは  補間しない

  if (cps1.size() != cps2.size()) return;

  std::vector<EVec3f> vec;
  for (int j = 0; j < (int)cps1.size(); ++j) vec.push_back(cps1[j] + (cps2[j] - cps1[j]) * rate);
  stroke.SetCPs(vec);
}




// _shared_idxを持つstrokeが sharedでかつlockedならそのCPsを取得する
bool DeformationStrokes::GetCpsOfLockedSharedStroke(const int _shared_idx, std::vector<EVec3f>& cps)
{
  for (int i = 0; i < (int)m_strokes.size(); ++i)
  {
    if (m_strokes[i].m_shared_idx == _shared_idx)
    {
      if (m_strokes[i].m_is_locked == true)
      {
        cps = m_strokes[i].GetCPs();
        return true;
      }
    }
  }
  return false;
}





bool DeformationStrokes::bSelStrokeShared()
{
  if (m_selected_stroke_idx == -1) return false;
  return m_strokes[m_selected_stroke_idx].m_shared_idx != -1;
}




void DeformationStrokes::FlipSelNormals()
{
  if (m_selected_stroke_idx == -1) return;
  m_strokes[m_selected_stroke_idx].FlipNormal();
}




void DeformationStrokes::DrawStrokes(const bool& _only_selected_stroke) const
{
  const int size = static_cast<int>(m_strokes.size());
  for (int i = 0; i < size; ++i)
  {
    if (_only_selected_stroke && (m_selected_stroke_idx != -1) && (m_selected_stroke_idx != i)) continue;

    const bool flag = (m_selected_stroke_idx == i);
    m_strokes[i].DrawStroke(flag);
  }
}


void DeformationStrokes::DrawControlPoints(const float& _cp_radius, const bool& _only_selected_stroke) const
{
  const int strokes_size = static_cast<int>(m_strokes.size());
  for (int i = 0; i < strokes_size; ++i)
  {
    if (_only_selected_stroke && (m_selected_stroke_idx != -1) && (m_selected_stroke_idx != i)) continue;

    const bool flag = (m_selected_stroke_idx == i);
    m_strokes[i].DrawControlPoints(_cp_radius, flag);
  }
}


std::string DeformationStrokes::OutputAsText() const
{
  std::string text = "";
  for (const auto& stroke : m_strokes)
  {
    if (stroke.m_is_locked == false) continue;
    text += stroke.OutputAsText() + "\n";
  }
  return text;
}


void DeformationStrokes::LoadState(const std::vector<int>& _vec_shared_idxs, const std::vector<std::vector<EVec3f>>& _vec_cps)
{
  ClearAllStrokes();
  const int size = static_cast<int>(_vec_shared_idxs.size());
  for (int i = 0; i < size; ++i)
  {
    if (_vec_cps[i].size() >= 3)
    {
      Stroke stroke;
      stroke.m_shared_idx = _vec_shared_idxs[i];
      stroke.m_is_locked = true;
      stroke.LoadState(_vec_cps[i]);
      m_strokes.push_back(stroke);
    }
  }
}





DeformationStrokes::Stroke::Stroke()
{
  m_stroke.clear();
  m_cps.clear();
  m_normal_side = true;
  m_selected_cpid = -1;
  m_plane_xyz = -1;
  m_plane_pos = -1.0f;
  m_plane_normal = EVec3f(0.0f, 0.0f, 0.0f);
  m_is_shared = false;
  m_shared_idx = -1;
  m_is_locked = true;
}



bool DeformationStrokes::Stroke::operator==(const Stroke& src) const
{
  if (this->m_selected_cpid != src.m_selected_cpid) return false;
  if (this->m_plane_xyz  != src.m_plane_xyz ) return false;
  if (this->m_plane_pos  != src.m_plane_pos ) return false;
  if (this->m_is_shared  != src.m_is_shared ) return false;
  if (this->m_shared_idx != src.m_shared_idx) return false;
  if (this->m_is_locked  != src.m_is_locked ) return false;
  return true;
}

bool DeformationStrokes::Stroke::operator!=(const Stroke& _src) const
{
  return !(*this == _src);
}



bool DeformationStrokes::Stroke::AddNewCP(const EVec3f _pos)
{
  //can not add to shared curve
  if (m_shared_idx != -1) return false; 
  const int num_cps = static_cast<int>(m_cps.size());

  if (num_cps == 0)
  {
    m_cps.push_back(_pos);
    m_selected_cpid = num_cps;
    UpdateStroke();
    return true;
  }
  
  if (num_cps == 1)
  {
    int xyz = (fabsf(m_cps[0][0] - _pos[0]) < 0.001f) ? 0:
              (fabsf(m_cps[0][1] - _pos[1]) < 0.001f) ? 1 : 
              (fabsf(m_cps[0][2] - _pos[2]) < 0.001f) ? 2 : -1;
    if (xyz == -1) return false;

    m_cps.push_back(_pos);
    m_selected_cpid = num_cps;
    m_plane_xyz = xyz;
    m_plane_pos = m_cps[0][xyz];
    m_plane_normal = (m_plane_xyz == 0) ? EVec3f(1.0f, 0.0f, 0.0f) : (m_plane_xyz == 1) ? EVec3f(0.0f, 1.0f, 0.0f) : (m_plane_xyz == 2) ? EVec3f(0.0f, 0.0f, 1.0f) : EVec3f(0.0f, 0.0f, 0.0f);
    UpdateStroke();
    return true;
  }

  // cp size >= 2
  if (fabsf(m_plane_pos - _pos[m_plane_xyz]) > 0.001f) return false;

  if (num_cps == 2)
  {
    m_cps.push_back(_pos);
    m_selected_cpid = num_cps;
    UpdateStroke();
    return true;
  }

  // cp size >= 3
  //compute distance from the center of polyline 
  float min_dist   = (_pos - m_cps[0]).norm();
  int   insert_idx = 0;

  for (int i = 0; i < num_cps - 1; ++i)
  {
    const EVec3f cent = (m_cps[i] + m_cps[i + 1]) / 2;
    const float  dist = (_pos - cent).norm();

    if ( dist < min_dist)
    {
      min_dist = dist;
      insert_idx = i + 1;
    }
  }

  const float dist_end = (_pos - m_cps[num_cps - 1]).norm();
  if (dist_end < min_dist)
  {
    min_dist = dist_end;
    insert_idx = num_cps;
  }
  m_cps.insert(m_cps.begin() + insert_idx, _pos);
  m_selected_cpid = insert_idx;

  UpdateStroke();

  if (m_debug)
  {
    std::cout << "Add cp: " << m_cps.size() << " cps.\n";
    std::cout << _pos << "\n";
  }

  return true;
}


void DeformationStrokes::Stroke::MoveSelectedCP(const EVec3f& _pos)
{
  if (m_selected_cpid == -1) return;

  if ( fabsf(m_plane_pos - _pos[m_plane_xyz]) < 0.001f)
    m_cps[m_selected_cpid] = _pos;
  m_is_locked = true;

  UpdateStroke();
}


void DeformationStrokes::Stroke::DeleteSelectedCP()
{
  //shared strokeの制御点は消せない
  if (m_selected_cpid == -1 || m_shared_idx != -1) return;
  m_cps.erase(m_cps.begin() + m_selected_cpid);
  m_selected_cpid = -1;
  UpdateStroke();
}



int DeformationStrokes::Stroke::PickCPs(
    const EVec3f& _ray_pos, 
    const EVec3f& _ray_dir, 
    const float& _cp_radius, 
    const bool& _change_selected_idx)
{
  const int size = static_cast<int>(m_cps.size());

  for (int i = 0; i < size; ++i)
  {
    if (DistRayAndPoint(_ray_pos, _ray_dir, m_cps[i]) < _cp_radius)
    {
      if (_change_selected_idx) m_selected_cpid = i;
      return i;
    }
  }

  if (_change_selected_idx) m_selected_cpid = -1;

  return -1;
}



void DeformationStrokes::Stroke::FlipNormal()
{
  m_normal_side = m_normal_side ? false : true;
}



void DeformationStrokes::Stroke::UpdateStroke()
{
  if (m_cps.size() < 3) return;

  // convert cps: EVec3f to EVec2f
  std::vector<EVec2f> cps_2f;
  for (const auto& cp : m_cps)
  {
    if (m_plane_xyz == 0) cps_2f.push_back(EVec2f(cp[1], cp[2]));
    if (m_plane_xyz == 1) cps_2f.push_back(EVec2f(cp[0], cp[2]));
    if (m_plane_xyz == 2) cps_2f.push_back(EVec2f(cp[0], cp[1]));
  }

  std::vector<EVec2f> stroke_2f;
  stroke_2f = KCurves::CalcKCurvesOpen(cps_2f);

  //failed
  if (stroke_2f.size() == 0) return;

  // convert stroke: EVec2f to EVec3f
  m_stroke.clear();
  for (const auto& p : stroke_2f)
  {

    EVec3f vec;
    if (m_plane_xyz == 0) vec << m_plane_pos, p[0], p[1];
    if (m_plane_xyz == 1) vec << p[0], m_plane_pos, p[1];
    if (m_plane_xyz == 2) vec << p[0], p[1], m_plane_pos;
    m_stroke.push_back(vec);
  }
}



static const EVec3f COLOR_R = { 1.0f, 0.0f, 0.0f };
static const EVec3f COLOR_Y = { 1.0f, 1.0f, 0.0f };
static const EVec3f COLOR_G = { 0.0f, 1.0f, 0.0f };
static const EVec3f COLOR_A = { 0.0f, 1.0f, 1.0f };

void DeformationStrokes::Stroke::DrawStroke(const bool& _is_selected) const
{
  if (m_stroke.size() == 0) return;

  const EVec3f color = _is_selected       ? COLOR_R : 
                       m_shared_idx == -1 ? COLOR_Y : 
                       m_is_locked        ? COLOR_G : COLOR_A;

  DrawPolyLine(color, _is_selected ? 1.5f * 6 : 1.0f * 6, m_stroke, false);

  glDisable(GL_LIGHTING);
  glColor3f(0.0f, 1.0f, 1.0f);
  glLineWidth(_is_selected ? 1.5f * 6 : 1.0f * 6);

  const float normal_length = 1.0f;

  EVec3f previous_draw_normal = EVec3f(0.0f, 0.0f, 0.0f);

  glBegin(GL_LINES);
  for (size_t i = 0; i < m_stroke.size(); ++i)
  {
    EVec3f tangent;
    if (i == 0) tangent = m_stroke[i + 1] - m_stroke[i];
    else if (i == m_stroke.size() - 1) tangent = m_stroke[i] - m_stroke[i - 1];
    else tangent = m_stroke[i + 1] - m_stroke[i - 1];
    tangent.normalize();

    EVec3f draw_normal = m_plane_normal.cross(tangent);
    draw_normal.normalize();

    if (i == 0)
    {
      previous_draw_normal = draw_normal;
    }
    else
    {
      if (previous_draw_normal.dot(draw_normal) < 0.0f)
      {
        draw_normal = -draw_normal;
      }
      previous_draw_normal = draw_normal;
    }

    glVertex3fv(m_stroke[i].data());

    const EVec3f normal_end = m_stroke[i] + draw_normal * normal_length * (m_normal_side ? 1.0f : -1.0f);
    glVertex3fv(normal_end.data());
  }
  glEnd();
}


void DeformationStrokes::Stroke::DrawControlPoints(const float& _cp_radius, const bool& _is_selected) const
{
  if (m_cps.size() == 0) return;

  // color
  const float COLOR_W[4] = { 1.0f, 1.0f, 1.0f, 0.5f };
  const float COLOR_SHIN64[1] = { 64 };
  const float COLOR_R[4] = { 1.0f, 0.0f, 0.0f, 0.5f };
  const float COLOR_Y[4] = { 1.0f, 1.0f, 0.0f, 0.5f };
  const float COLOR_G[4] = { 0.0f, 1.0f, 0.0f, 0.5f };
  const float COLOR_A[4] = { 0.0f, 1.0f, 1.0f, 0.5f };

  const int size = static_cast<int>(m_cps.size());
  for (int i = 0; i < size; ++i)
  {
    const bool flag = (_is_selected && (m_selected_cpid == i));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, COLOR_W);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, COLOR_SHIN64);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, flag ? COLOR_R : (m_shared_idx == -1 ? COLOR_Y : (m_is_locked ? COLOR_G : COLOR_A)));
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, flag ? COLOR_R : (m_shared_idx == -1 ? COLOR_Y : (m_is_locked ? COLOR_G : COLOR_A)));

    glEnable(GL_LIGHTING);
    TMesh::DrawSphere(m_cps[i], _cp_radius);
    glDisable(GL_LIGHTING);
  }
}


std::string DeformationStrokes::Stroke::OutputAsText() const
{
  std::string text = "";
  text += std::to_string(m_shared_idx) + "\n";
  for (const auto& cp : m_cps)
  {
    for (int i = 0; i < 3; ++i)
    {
      text += std::to_string(cp[i]) + " ";
    }
    text += "\n";
  }
  return text;
}


void DeformationStrokes::Stroke::LoadState(const std::vector<EVec3f>& _cps)
{
  for (const auto& cp : _cps)
    m_cps.push_back(cp);
 

  if (m_cps.size() >= 2)
  {
    m_plane_xyz = (fabsf(m_cps[0][0] - m_cps[1][0]) < 0.001f) ?  0 : 
                  (fabsf(m_cps[0][1] - m_cps[1][1]) < 0.001f) ?  1 : 2;
    m_plane_pos = m_cps[0][m_plane_xyz];
  }
  m_selected_cpid = (int) m_cps.size() - 1 ;
  UpdateStroke();
}





//
////木村くんのコード（一つの関数で色々やるのでわかりにくいが、、）
//// 2 (left, right) = (exist,  null)  --> _shared_idxに leftをコピー
//// 3 (left, right) = (exist, exist)  --> _shared_idxに left/rightの補間をコピー
//void DeformationStrokes::UpdateSharedStrokes(
//    const int& _shared_idx, 
//    Stroke* _left, 
//    Stroke* _right, 
//    const float& _rate)
//{
//  if (_shared_idx < 0) return;
//
//  //search the target stroke index
//  int stroke_idx = -1;
//  for (int i = 0; i < (int)m_strokes.size(); ++i)
//  {
//    if (m_strokes[i].m_shared_idx == _shared_idx)
//      stroke_idx = i;
//  }
//   
//  if (stroke_idx == -1) return;
//
//
//  // update stroke
//  Stroke& stroke = m_strokes[stroke_idx];
//  if (stroke.m_is_locked) return; //skip 編集中のstrokeは  補間しない
//  if (&stroke == _left  ) return; //skip leftが自身の場合も補間しない
//
//  if (     _left == nullptr && _right != nullptr) //case 2 copy from right
//  {
//    stroke = *_right;
//    stroke.m_is_locked = false;
//  }
//  else if (_left != nullptr && _right == nullptr) //case 2 copy from left
//  {
//    stroke = *_left;
//    stroke.m_is_locked = false;
//  }
//  else if(_left != nullptr && _right != nullptr)
//  {
//    //case 3 Interpolate
//    stroke.m_is_locked = false;
//    const int num_cps = stroke.GetNumCPs();
//    const std::vector<EVec3f>& left_cps  = _left->GetCPs();
//    const std::vector<EVec3f>& right_cps = _right->GetCPs();
//    std::vector<EVec3f> vec;
//    for (int j = 0; j < num_cps; ++j)
//    {
//      vec.push_back(left_cps[j] + (right_cps[j] - left_cps[j]) * _rate);
//    }
//    stroke.SetCPs(vec);
//  }
//
//  m_selected_stroke_idx = -1;
//}
//
//
//DeformationStrokes::Stroke* DeformationStrokes::GetSharedStroke(const int& _shared_idx)
//{
//  for (int i = 0; i < (int)m_strokes.size(); ++i)
//  {
//    if (m_strokes[i].m_shared_idx == _shared_idx)
//    {
//      if (m_strokes[i].m_is_locked == true) 
//        return &m_strokes[i];
//    }
//  }
//
//  return nullptr;
//}