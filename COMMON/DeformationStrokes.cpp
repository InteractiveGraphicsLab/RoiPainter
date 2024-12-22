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
  if (this->m_set_shared_idx != _src.m_set_shared_idx) return false;
  return true;
}


bool DeformationStrokes::operator!=(const DeformationStrokes& _src) const
{
  return !(*this == _src);
}


int DeformationStrokes::GetNumStrokes() const
{
  return static_cast<int>(m_strokes.size());
}


int DeformationStrokes::GetSelectedStrokeIdx() const
{
  return m_selected_stroke_idx;
}


void DeformationStrokes::SetSelectedStrokeIdx(const int& _idx)
{
  if (_idx < 0 || m_strokes.size() <= _idx)
  {
    m_selected_stroke_idx = -1;
  }
  else
  {
    m_selected_stroke_idx = _idx;
  }
}


void DeformationStrokes::AddStroke()
{
  m_strokes.push_back(Stroke());
  m_selected_stroke_idx = static_cast<int>(m_strokes.size()) - 1;

  if (m_debug)
  {
    std::cout << "Add stroke: " << m_strokes.size() << " strokes.\n";
  }
}


void DeformationStrokes::DeleteSelectedStroke()
{
  if (m_selected_stroke_idx == -1) return;

  m_strokes.erase(m_strokes.begin() + m_selected_stroke_idx);
  m_selected_stroke_idx = -1;

  if (m_debug)
  {
    std::cout << "Delete stroke: " << m_strokes.size() << " strokes.\n";
  }
}


void DeformationStrokes::ClearAllStrokes()
{
  m_strokes.clear();
  m_selected_stroke_idx = -1;

  if (m_debug)
  {
    std::cout << "Clear all strokes.\n";
  }
}


std::vector<std::vector<EVec3f>> DeformationStrokes::GetStroke() const
{
  std::vector<std::vector<EVec3f>> vec;
  for (const auto& stroke : m_strokes)
  {
    std::vector<EVec3f> vec_i = stroke.GetStroke();
    if (vec_i.size() > 0)
    {
      vec.push_back(vec_i);
    }
  }

  return vec;
}


DeformationStrokes::Stroke* DeformationStrokes::GetStrokeInst(const int _idx)
{
  if (_idx < 0 || _idx >= m_strokes.size())
  {
    return nullptr;
  }

  return &m_strokes[_idx];
}


bool DeformationStrokes::AddControlPoint(const EVec3f& _pos)
{
  if (m_selected_stroke_idx == -1)
  {
    AddStroke();
  }

  if (m_strokes[m_selected_stroke_idx].m_shared_idx != -1) return false;

  return m_strokes[m_selected_stroke_idx].AddControlPoint(_pos);
}


int DeformationStrokes::PickControlPoint(const EVec3f& _ray_pos, const EVec3f& _ray_dir, const float& _cp_radius, const bool& _change_selected_idx, const bool& _only_selected)
{
  const int size = static_cast<int>(m_strokes.size());

  const int tmp = std::max(m_selected_stroke_idx, 0);
  for (int i = tmp; i < size + tmp; ++i)
  {
    const int n = i % size;
    if (_only_selected && (m_selected_stroke_idx != -1) && (m_selected_stroke_idx != i)) continue;
    if (m_strokes[n].PickControlPoint(_ray_pos, _ray_dir, _cp_radius, _change_selected_idx) != -1)
    {
      if (_change_selected_idx)
      {
        m_selected_stroke_idx = n;
      }
      return n;
    }
  }

  if (_change_selected_idx)
  {
    m_selected_stroke_idx = -1;
  }
  return -1;
}


void DeformationStrokes::MoveSelectedControlPoint(const EVec3f& _pos)
{
  if (m_selected_stroke_idx == -1) return;
  m_strokes[m_selected_stroke_idx].MoveSelectedControlPoint(_pos);
}


void DeformationStrokes::DeleteSelectedControlPoint()
{
  if (m_selected_stroke_idx == -1) return;

  m_strokes[m_selected_stroke_idx].DeleteSelectedControlPoint();

  if (m_strokes[m_selected_stroke_idx].GetNumControlPoints() == 0)
  {
    DeleteSelectedStroke();
  }
}


void DeformationStrokes::ReleaseStroke()
{
  m_selected_stroke_idx = -1;
}


int DeformationStrokes::GetCommonXYZ() const
{
  if (m_selected_stroke_idx == -1) return -1;

  return m_strokes[m_selected_stroke_idx].GetCommonXYZ();
}


float DeformationStrokes::GetCommonCoord() const
{
  if (m_selected_stroke_idx == -1) return -1.0f;

  return m_strokes[m_selected_stroke_idx].GetCommonCoord();
}


DeformationStrokes::Stroke* DeformationStrokes::ShareSelectedStroke(const int& _shared_idx)
{
  if (m_selected_stroke_idx == -1) return nullptr;
  Stroke* stroke = &m_strokes[m_selected_stroke_idx];
  stroke->m_is_shared = true;
  stroke->m_shared_idx = _shared_idx;
  stroke->m_is_locked = true;
  std::cout << "Shared stroke (id = " << _shared_idx << ")\n";
  return stroke;
}


int DeformationStrokes::UnshareSelectedStroke()
{
  if (m_selected_stroke_idx == -1) return -1;

  Stroke& stroke = m_strokes[m_selected_stroke_idx];
  const int shared_idx = stroke.m_shared_idx;
  stroke.m_is_shared = false;
  stroke.m_shared_idx = -1;
  stroke.m_is_locked = true;
  std::cout << "Unshared stroke (id = " << shared_idx << ")\n";
  return shared_idx;
}


void DeformationStrokes::LockSelectedStroke()
{
  if (m_selected_stroke_idx == -1) return;

  Stroke& stroke = m_strokes[m_selected_stroke_idx];
  stroke.m_is_locked = true;
  std::cout << "Locked stroke (id = " << stroke.m_shared_idx << ")\n";
}


void DeformationStrokes::UnlockSelectedStroke()
{
  if (m_selected_stroke_idx == -1) return;

  Stroke& stroke = m_strokes[m_selected_stroke_idx];
  stroke.m_is_locked = false;
  std::cout << "Unlocked stroke (id = " << stroke.m_shared_idx << ")\n";
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


void DeformationStrokes::UpdateSharedStrokes(const int& _shared_idx, Stroke* _left, Stroke* _right, const float& _rate)
{
  if (_shared_idx < 0) return;

  const int size = static_cast<int>(m_strokes.size());
  int stroke_idx = -1;
  for (int i = 0; i < size; ++i)
  {
    Stroke& stroke = m_strokes[i];
    if (stroke.m_shared_idx == _shared_idx)
    {
      stroke_idx = i;
      break;
    }
  }
  if (stroke_idx == -1)
  {
    if ((_left == nullptr) && (_right == nullptr))
    {
      return;
    }
    else
    {
      stroke_idx = size;
      m_strokes.push_back((_left != nullptr) ? *_left : *_right);
      m_strokes[stroke_idx].m_is_shared = true;
      m_strokes[stroke_idx].m_shared_idx = _shared_idx;
      m_strokes[stroke_idx].m_is_locked = false;
    }
  }

  // update stroke
  Stroke& stroke = m_strokes[stroke_idx];
  if (&stroke == _left)
  {
    //std::cout << "[UpdateSharedStrokes] Skipped." << std::endl; // debug
  }
  else if ((_left == nullptr) && (_right == nullptr))
  {
    if (stroke.m_is_locked == true)
    {
      stroke.m_is_shared = false;
      stroke.m_shared_idx = -1;
      //std::cout << "[UpdateSharedStrokes] Unlocked." << std::endl; // debug
    }
    else
    {
      m_strokes.erase(m_strokes.begin() + stroke_idx);
      //std::cout << "[UpdateSharedStrokes] Erased." << std::endl; // debug
    }
  }
  else if (stroke.m_is_locked == false)
  {
    if ((_left == nullptr) && (_right != nullptr))
    {
      // Copy from right
      stroke = *_right;
      stroke.m_is_locked = false;
      //std::cout << "[UpdateSharedStrokes] Copied from right." << std::endl; // debug
    }
    else if ((_left != nullptr) && (_right == nullptr))
    {
      // Copy from left
      stroke = *_left;
      stroke.m_is_locked = false;
      //std::cout << "[UpdateSharedStrokes] Copied from left." << std::endl; // debug
    }
    else
    {
      // Interpolate
      stroke = *_left;
      stroke.m_is_locked = false;
      const int num_cps = stroke.GetNumControlPoints();
      const std::vector<EVec3f>& left_cps = _left->GetControlPoints();
      const std::vector<EVec3f>& right_cps = _right->GetControlPoints();
      std::vector<EVec3f> vec;
      for (int j = 0; j < num_cps; ++j)
      {
        EVec3f cp = left_cps[j] + (right_cps[j] - left_cps[j]) * _rate;
        vec.push_back(cp);
      }
      stroke.SetControlPoints(vec);
      //std::cout << "[UpdateSharedStrokes] Interpolated." << std::endl; // debug
    }
    m_selected_stroke_idx = -1;
  }
  else
  {
    //std::cout << "[UpdateSharedStrokes] Skipped." << std::endl; // debug
  }
}


DeformationStrokes::Stroke* DeformationStrokes::GetSharedStroke(const int& _shared_idx)
{
  const int size = static_cast<int>(m_strokes.size());
  for (int i = 0; i < size; ++i)
  {
    if (m_strokes[i].m_shared_idx == _shared_idx)
    {
      if (m_strokes[i].m_is_locked == true)
      {
        return &m_strokes[i];
      }
      else
      {
        break;
      }
    }
  }

  return nullptr;
}


bool DeformationStrokes::IsSelectedStrokeShared()
{
  if (m_selected_stroke_idx == -1) return false;

  return (m_strokes[m_selected_stroke_idx].m_shared_idx != -1);
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
  m_selected_cp_idx = -1;
  m_common_xyz = -1;
  m_common_coord = -1.0f;
  m_is_shared = false;
  m_shared_idx = -1;
  m_is_locked = true;
}


bool DeformationStrokes::Stroke::operator==(const Stroke& _src) const
{
  if (this->m_selected_cp_idx != _src.m_selected_cp_idx) return false;
  if (this->m_common_xyz != _src.m_common_xyz) return false;
  if (this->m_common_coord != _src.m_common_coord) return false;
  if (this->m_is_shared != _src.m_is_shared) return false;
  if (this->m_shared_idx != _src.m_shared_idx) return false;
  if (this->m_is_locked != _src.m_is_locked) return false;
  //if (this->m_stroke != _src.m_stroke) return false;
  return true;
}


bool DeformationStrokes::Stroke::operator!=(const Stroke& _src) const
{
  return !(*this == _src);
}


int DeformationStrokes::Stroke::GetNumControlPoints() const
{
  return static_cast<int>(m_cps.size());
}


int DeformationStrokes::Stroke::GetSelectedControlPointIdx() const
{
  return m_selected_cp_idx;
}


void DeformationStrokes::Stroke::SetSelectedControlPointIdx(const int& _idx)
{
  if (_idx < 0 || m_cps.size() <= _idx)
  {
    m_selected_cp_idx = -1;
  }
  else
  {
    m_selected_cp_idx = _idx;
  }
}


int DeformationStrokes::Stroke::GetCommonXYZ() const
{
  //if (m_selected_cp_idx == -1) return -1;

  return m_common_xyz;
}


float DeformationStrokes::Stroke::GetCommonCoord() const
{
  //if (m_selected_cp_idx == -1) return -1.0f;

  return m_common_coord;
}


bool DeformationStrokes::Stroke::AddControlPoint(const EVec3f& _pos, const bool& _is_not_loading)
{
  if ((m_shared_idx != -1) && _is_not_loading) return false;

  const int num_cps = static_cast<int>(m_cps.size());
  bool is_added = false;
  if (num_cps == 0)
  {
    m_cps.push_back(_pos);
    m_selected_cp_idx = num_cps;
    is_added = true;
  }
  else if (num_cps == 1)
  {
    for (int i = 0; i < 3; ++i)
    {
      if (fabsf(m_cps[0][i] - _pos[i]) < 0.001f)
      {
        m_cps.push_back(_pos);
        m_selected_cp_idx = num_cps;
        is_added = true;
        break;
      }
    }
  }
  else if (num_cps == 2)
  {
    for (int i = 0; i < 3; ++i)
    {
      const float common_coord = m_cps[0][i];
      if ((fabsf(common_coord - m_cps[1][i]) < 0.001f) && (fabsf(common_coord - _pos[i]) < 0.001f))
      {
        m_common_xyz = i;
        m_common_coord = common_coord;
        m_cps.push_back(_pos);
        m_selected_cp_idx = num_cps;
        is_added = true;
        break;
      }
    }
  }
  else
  {
    if (fabsf(m_common_coord - _pos[m_common_xyz]) < 0.001f)
    {
      if (_is_not_loading)
      {
        float min_dist = INFINITY;
        int min_idx = -1;

        const float dist_begin = (_pos - m_cps[0]).norm();
        if (dist_begin < min_dist)
        {
          min_dist = dist_begin;
          min_idx = 0;
        }
        for (int i = 0; i < num_cps - 1; ++i)
        {
          const EVec3f cent = (m_cps[i] + m_cps[i + 1]) / 2;
          const float dist = (_pos - cent).norm();
          const float rad = (m_cps[i] - m_cps[i + 1]).norm() / 2;
          if ((dist < rad) && (dist < min_dist))
          {
            min_dist = dist;
            min_idx = i + 1;
          }
        }
        const float dist_end = (_pos - m_cps[num_cps - 1]).norm();
        if (dist_end < min_dist)
        {
          min_dist = dist_end;
          min_idx = num_cps;
        }


        if (min_idx != -1)
        {
          m_cps.insert(m_cps.begin() + min_idx, _pos);
          m_selected_cp_idx = min_idx;
          is_added = true;
        }
        else
        {
          float dist_begin = (_pos - m_cps[0]).norm();
          float dist_end = (_pos - m_cps[num_cps - 1]).norm();
          if (dist_begin < dist_end)
          {
            m_cps.insert(m_cps.begin(), _pos);
            m_selected_cp_idx = 0;
            is_added = true;
          }
          else
          {
            m_cps.push_back(_pos);
            m_selected_cp_idx = num_cps;
            is_added = true;
          }
        }
      }
      else
      {
        m_cps.push_back(_pos);
        m_selected_cp_idx = num_cps;
        is_added = true;
      }
    }
  }

  UpdateStroke();

  if (m_debug)
  {
    std::cout << "Add cp: " << m_cps.size() << " cps.\n";
    std::cout << _pos << "\n";
  }

  return is_added;
}


void DeformationStrokes::Stroke::MoveSelectedControlPoint(const EVec3f& _pos)
{
  if (m_selected_cp_idx == -1) return;

  if (m_is_locked == false)
  {
    m_is_locked = true;
    std::cout << "Locked stroke (id = " << m_shared_idx << ")\n";
  }

  if (fabsf(m_common_coord - _pos[m_common_xyz]) < 0.001f)
  {
    m_cps[m_selected_cp_idx] = _pos;
  }

  UpdateStroke();
}


void DeformationStrokes::Stroke::DeleteSelectedControlPoint()
{
  if (m_selected_cp_idx == -1) return;
  if (m_shared_idx != -1) return;

  m_cps.erase(m_cps.begin() + m_selected_cp_idx);
  m_selected_cp_idx = -1;

  UpdateStroke();

  if (m_debug)
  {
    std::cout << "Delete cp: " << m_cps.size() << " cps.\n";
  }
}


void DeformationStrokes::Stroke::ClearAllControlPoints()
{
  m_cps.clear();
  m_selected_cp_idx = -1;

  UpdateStroke();

  if (m_debug)
  {
    std::cout << "Clear all control points.\n";
  }
}


int DeformationStrokes::Stroke::PickControlPoint(const EVec3f& _ray_pos, const EVec3f& _ray_dir, const float& _cp_radius, const bool& _change_selected_idx)
{
  const int size = static_cast<int>(m_cps.size());
  for (int i = 0; i < size; ++i)
  {
    if (DistRayAndPoint(_ray_pos, _ray_dir, m_cps[i]) < _cp_radius)
    {
      if (_change_selected_idx)
      {
        m_selected_cp_idx = i;
      }
      return i;
    }
  }

  if (_change_selected_idx)
  {
    m_selected_cp_idx = -1;
  }
  return -1;
}


void DeformationStrokes::Stroke::UpdateStroke()
{
  std::vector<EVec3f> prev_stroke = m_stroke;
  m_stroke.clear();
  if (m_cps.size() < 3) return;

  // convert cps: EVec3f to EVec2f
  std::vector<EVec2f> cps_2f;
  for (const auto& cp : m_cps)
  {
    EVec2f vec;
    int idx = 0;
    for (int i = 0; i < 3; ++i)
    {
      if (i != m_common_xyz)
      {
        vec[idx++] = cp[i];
      }
    }
    cps_2f.push_back(vec);
  }

  std::vector<EVec2f> stroke_2f;
  stroke_2f = KCurves::CalcKCurvesOpen(cps_2f);

  // do not update when calclations fail
  if (stroke_2f.size() == 0)
  {
    //std::cout << "not update." << std::endl; // debug
    m_stroke = prev_stroke;
    return;
  }

  // convert stroke: EVec2f to EVec3f
  for (const auto& p : stroke_2f)
  {
    EVec3f vec;
    int idx = 0;
    for (int i = 0; i < 3; ++i)
    {
      if (i == m_common_xyz)
      {
        vec[i] = m_common_coord;
      }
      else
      {
        vec[i] = p[idx++];
      }
    }
    m_stroke.push_back(vec);
  }
}


std::vector<EVec3f> DeformationStrokes::Stroke::GetStroke() const
{
  return m_stroke;
}


std::vector<EVec3f> DeformationStrokes::Stroke::GetControlPoints() const
{
  return m_cps;
}


void DeformationStrokes::Stroke::SetControlPoints(const std::vector<EVec3f>& _cps)
{
  m_cps = _cps;
  UpdateStroke();
}


void DeformationStrokes::Stroke::DrawStroke(const bool& _is_selected) const
{
  if (m_stroke.size() == 0) return;

  // for testing
  const EVec3f COLOR_R = { 1.0f, 0.0f, 0.0f };
  const EVec3f COLOR_Y = { 1.0f, 1.0f, 0.0f };
  const EVec3f COLOR_G = { 0.0f, 1.0f, 0.0f };
  const EVec3f COLOR_A = { 0.0f, 1.0f, 1.0f };

  //const EVec3f color = EVec3f(1.0f, _is_selected ? 0.0f : 1.0f, 0.0f);
  const EVec3f color = _is_selected ? COLOR_R : (m_shared_idx == -1 ? COLOR_Y : (m_is_locked ? COLOR_G : COLOR_A));

  DrawPolyLine(color, _is_selected ? 1.5f * 6 : 1.0f * 6, m_stroke, false);
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
    const bool flag = (_is_selected && (m_selected_cp_idx == i));
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
  {
    AddControlPoint(cp, false);
  }
}
