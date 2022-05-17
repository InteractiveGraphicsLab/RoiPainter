#pragma once


/* ------------------------------------
class TexCore

date 2016/08/20 
written by Takashi Ijiri

This class manages following images,
+ volumetric image
+ maskId image
+ transfer functions (as images)
 -------------------------------------*/






#define TRANS_FUNC_SIZE 256
#define HIST_BIN_SIZE   256


#include "OglForCLI.h"
#include "OglImage.h"
#include "tmesh.h"

#include <vector>
#include <string>

using namespace std;




class MaskData
{

public:
	string name     ;
	TMesh  surf     ;
	EVec3i color    ;
	double alpha    ;
	bool   bRendSurf;
	bool   lock     ;

	MaskData(string _name, EVec3i _color, double _alpha, bool _bRendSurf, bool _lock = false) 
	{
		name      = _name;
		color     = _color;
		alpha     = _alpha;
		bRendSurf = _bRendSurf;
		lock      = _lock;
	}


	void Set(const MaskData& v)
	{
		name     = v.name;
		surf     = v.surf;
		color    = v.color;
		alpha    = v.alpha;
		bRendSurf= v.bRendSurf;
		lock     = v.lock;
	}

	MaskData( const MaskData& v)
	{
		fprintf( stderr, "for debug, MaskData copy const\n");
		Set(v);
	}
	
	MaskData& operator=(const MaskData& v)
	{
		fprintf( stderr, "for debug, MaskData operator =\n");
		Set(v);
		return *this;
	}

};




//todo gen Dlg for ManageVolInfo
//以下は一時的に作ったダイアログの代わりをするクラスで、
// resolution, pitchを管理する

class TmpVolInfoDlg
{
public:


};






class TexCore
{
private:
	//volume info 
	EVec3i  m_Reso ;
	EVec3f  m_Pitch;
	float   m_winLvMin, m_winLvMax;

	string  m_filePath;

public:

	TmpVolInfoDlg m_dlg;

	//volume 
	float              *m_volOrig  ; // original image          [W,H,D]
	float              *m_volOrigGM; // original image grad mag [W,H,D]
	OglImage3D          m_vol      ; // original image with tone mapping (byte) 
	OglImage3D          m_volFlg   ; // Flg 
	OglImage3D          m_volMsk   ; // mask 
	OglImage3D          m_volGmag  ; // gradient magnitude 
	OglImage1D<CH_RGBA> m_imgTf    ; // 1st ch(intensity-->trans), 2nd (grad mag-->trans) 
	OglImage1D<CH_RGBA> m_imgPsu   ; // func: intensity --> psude color 
	OglImage1D<CH_RGBA> m_imgMskCol; // func: maskID    --> color

	int                 m_maskActiveId;
	vector<MaskData>    m_maskData    ;

	float m_histVol [ HIST_BIN_SIZE ];
	float m_histGmag[ HIST_BIN_SIZE ];

private:
	TexCore();

public:
	~TexCore();
	static TexCore* getInst(){ static TexCore p; return &p;}



	void UpdateWindowLevel(float minV, float maxV);

	//Loaders volume 
	bool LoadVolume(vector<string> fnames, string fext);
	bool LoadVolume(string fname         , string fext) ;



	//getter & setter for resolution and pitch
	EVec3f getCuboidF() { return EVec3f((float)(m_Reso[0] * m_Pitch[0]), (float)(m_Reso[1] * m_Pitch[1]), (float)(m_Reso[2] * m_Pitch[2])); }
	EVec3i getResolution() { return m_Reso; }

	//pitch
	EVec3f getPitch()  { return m_Pitch; }
	void   setPitchW(const float &pW) { m_Pitch[0] = pW; }
	void   setPitchH(const float &pH) { m_Pitch[1] = pH; }
	void   setPitchD(const float &pD) { m_Pitch[2] = pD; }

	double getPitchW() { return m_Pitch[0]; }
	double getPitchH() { return m_Pitch[1]; }
	double getPitchD() { return m_Pitch[2]; }

	//volume min/mac
	float getVolMin() { return m_winLvMin; }
	float getVolMax() { return m_winLvMax; }

	//get Value 
	float getVolVal(const EVec3f &pos);


	//mask manipuration
	void mask_storeCurrentForeGround();

	void ActvMsk_SetLock    (const bool   tf    );
	void ActvMsk_SetRendSurf(const bool   tf    );
	void ActvMsk_SetAlpha   (const double alpha );
	void ActvMsk_SetColor   (const EVec3i &c    );
	void ActvMsk_erode      ();
	void ActvMsk_dilate     ();
	void ActvMsk_fillHole   ();
	void ActvMsk_delete     ();
	void ActvMsk_margeTo    ();
	void ActvMsk_fillPsuedoHoleAsNewRegion();
	void ActvMsk_ExportObj(const char *fname);
	void ActvMsk_ExportStl(const char *fname);
	void ActvMsk_ExportBmp(const char *fname);

	//mask I/O
	void loadMask(const char *fname);
	void saveMask(const char *fname);

	string getVolFilePath(){ return m_filePath; }


private:
	void updateHistogram();
	void updateGradVolume();

};



