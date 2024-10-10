#ifndef MODE_MODESEGBRONCHI_H_
#define MODE_MODESEGBRONCHI_H_
#pragma unmanaged

#include "ModeInterface.h"
#include "GlslShader.h"
#include "tmesh.h"
#include <vector>
#include <set>
#include <unordered_set>


class TreeObj
{
private:
	static float m_radius;
	static float m_path_width;
	int      m_age;
	int      m_frame_idx;
	EVec3f   m_pos;
	TreeObj *m_parent;
	std::vector<TreeObj*> m_children;
	std::vector<std::vector<EVec3f>> m_pathToChildren;

public:
	TreeObj(int frame_idx, TreeObj* parent, EVec3f pos = EVec3f(100,-100,-100))
	{
		m_frame_idx = frame_idx;
		m_pos    = pos;
		m_parent = parent;
		m_children.clear();
		m_pathToChildren.clear();
	}

	~TreeObj()
	{
		m_parent = nullptr;
		for (auto& child : m_children) delete child;
		m_children.clear();
		m_pathToChildren.clear();
	}

	void Copy(const TreeObj& src) {
		m_frame_idx = src.m_frame_idx;
		m_pos = src.m_pos;
		m_age = src.m_age;
		m_parent   = src.m_parent;
		m_children = src.m_children;
		m_pathToChildren           = src.m_pathToChildren;
	}

	TreeObj(const TreeObj& src) { std::cout << "コピーコンストラクタ\n"; Copy(src); }
	TreeObj& TreeObj::operator=(const TreeObj& src) { std::cout << "代入演算子オーバーロード\n"; Copy(src); return *this; }
	
	static float GetRadius()    { return m_radius; }
	static float GetPathWidth() { return m_path_width; }
	TreeObj* GetParent() const { return m_parent; }
	int    GetFrameIdx() const { return m_frame_idx; }
	int    GetAge()      const { return m_age; }
	EVec3f GetPos()      const { return m_pos; }
	std::vector<TreeObj*> GetChildren() const { return m_children; }
	std::vector<std::vector<EVec3f>> GetPathsToChildren(){ return m_pathToChildren; }

	
	static void SetRadius(float radius)        { m_radius = radius; }
	static void SetPathWidth(float path_width) { m_path_width = path_width; }
	void SetAge(int    age) { m_age = age; }
	void SetPos(EVec3f pos) { m_pos = pos; }

	void AddChild(TreeObj* child){ 
		m_children.push_back(child); 
		m_pathToChildren.push_back(std::vector<EVec3f>());
	}

	void SetPathToChild(const TreeObj* child, const std::vector<EVec3f> path)
	{
		for (int i = 0; i < (int)m_children.size(); ++i)
		{
			if (m_children[i] == child)
			{
				m_pathToChildren[i] = path;
			}
		}
	}
	
	//TODO m_treeStructPathToChildrenのpathもいじる
	void DeleteChild(TreeObj *ptr) 
	{
		std::vector<TreeObj*> tmp_chs;
		std::vector<std::vector<EVec3f>> tmp_paths;


		for( int i = 0; i < (int) m_children.size(); ++i)
		{
			if (m_children[i] == ptr) {
				delete m_children[i];
			} else {
				tmp_chs.push_back(m_children[i]);
				tmp_paths.push_back(m_pathToChildren[i]);
			}
		}
		m_children = tmp_chs;
		m_pathToChildren = tmp_paths;
	}
};




class ModeSegBronchi : public ModeInterface 
{
private:
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;

  bool                m_b_draw_cutstroke;
  std::vector<EVec3f> m_stroke;

  //control points
  //float     m_cp_radius;
  TMesh     m_cp_mesh;
	TreeObj*  m_selected_cp;
	TreeObj*  m_pick_cp;

	std::vector<TreeObj*> m_roots;

	//to memorize the origin image before pre-filter
	std::vector< short* > t_dst4d;
	//std::vector< short* > m_max;
	std::vector< short* > m_min;
	std::vector< short* > m_max_m_min;


  ModeSegBronchi();
public:
  ~ModeSegBronchi();

  static ModeSegBronchi* GetInst(){
    static ModeSegBronchi p;
    return &p;
  }


  /////////////////////////////////////////////////////////////////
  virtual MODE_ID GetModeID() { return MODE_SEG_BRONCHI; }

	virtual void StartMode();
	virtual bool CanEndMode();
	virtual void FinishSegmentation();
	virtual void DrawScene(const EVec3f& cuboid, const EVec3f& camP, const EVec3f& camF);
	

	virtual void LBtnDown   (const EVec2i& p, OglForCLI* ogl);
	virtual void LBtnUp     (const EVec2i& p, OglForCLI* ogl);
	virtual void RBtnDown   (const EVec2i& p, OglForCLI* ogl);
	virtual void RBtnUp     (const EVec2i& p, OglForCLI* ogl);
	virtual void MBtnDown   (const EVec2i& p, OglForCLI* ogl);
	virtual void MBtnUp		  (const EVec2i &p, OglForCLI *ogl);
	virtual void LBtnDclk	  (const EVec2i &p, OglForCLI *ogl);
	virtual void RBtnDclk	  (const EVec2i &p, OglForCLI *ogl);
	virtual void MBtnDclk	  (const EVec2i &p, OglForCLI *ogl);
	virtual void MouseMove	(const EVec2i &p, OglForCLI *ogl);
	virtual void MouseWheel	(const EVec2i &p, short zDelta, OglForCLI *ogl);
	virtual void KeyDown    (int nChar) ;
	virtual void KeyUp      (int nChar) ;
	

	void UpControlPointSize();
	void DownControlPointSize();
	void UpPathWidth();
	void DownPathWidth();

	void IntializeVarState(const int num_frames, const int num_voxels);
	void InitCenteredness(const int num_frames, const int num_voxels, const EVec3i reso);
	void LoGFilter(const int ksize = 5, const double sigma = 0.1);
	void ChangeImg(const int num_frames, const int num_voxels);
	void ClearSelectedCp() { m_selected_cp = nullptr; }

	//Tree Algorithm
	TreeObj* PickControlPoints(const EVec3f& ray_pos, const EVec3f& ray_dir);
	void AddNewCp  (const int frame_idx, const EVec3f pos);
	void DeleteCp  (const int frame_idx, TreeObj* cp     );
	void UpdatePath(const int frame_idx, TreeObj* trgt_cp, bool path_to_parent = true);


	void SaveCp(std::string fname);
	void LoadCp(std::string fname);
	void CopyPrevFrameCp();


	//Region Growing + Shortest Path Algorithm
	void ComputePathOneFrameAllCp(const int frame_idx);
	void ComputePathAllFrameAllCp();
	void RunOneFrameRegionGrow(const EVec3i& thresh_minmax, const int frame_idx, int former_age, int latter_age, int latter_layerA, int latter_layerB);
};

#endif