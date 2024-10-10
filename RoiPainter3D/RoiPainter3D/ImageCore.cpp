#pragma unmanaged
#include "ImageCore.h"
#include "tmath.h"
#include "tmarchingcubes.h"
#include "tdcmtk.h"
#include "ddsbase.h"
#include "./Mode/ModeSegGCut.h"
#include <iostream>

#pragma managed
#include "FormStackOrder.h"
#include "FormIntegerSelection.h"
#include "climessagebox.h"

#pragma unmanaged

using namespace RoiPainter3D;

ImageCore::ImageCore()
{
  std::cout << "CONSTRUCTOR ImageCore ...\n";

	m_vol_orig = 0;
	LoadVolume("init","init");
	m_img_maskcolor.Allocate(256);
  m_active_maskid = -1;
 
  std::cout << "CONSTRUCTOR ImageCore ...DONE\n";
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//VOLUME LOADER///////////////////////////////////////////////////////////////////////////////////////////

static bool t_LoadDefoultVolume(
	EVec3i &reso,
	EVec3f &pitch,
	short* &vol
	)
{
	const int N = 64, R = N/5;
	reso  << N, N+3, N -3;
	pitch << 1, 1, 1;
	vol = new short[reso[0] * reso[1] * reso[2]];

	EVec3f c1(2*R, 2*R, 2*R), c2(3*R, 3 * R, 3 * R);

	for (int z = 0; z < reso[2]; ++z)
	{
		for (int y = 0; y < reso[1]; ++y)
		{
			for (int x = 0; x < reso[0]; ++x)
			{
				EVec3f p((float)x, (float)y, (float)z);
				float d1 = (p - c1).norm();
				float d2 = (p - c2).norm();
				float v = 3000 * exp(-(d1-R)*(d1-R) / 10 ) + 3000 * exp(-(d2-R)*(d2-R) / 10);
				vol[x + y * reso[0] + z * reso[1] * reso[0]] = (short)( v - 1500 );
			}
		}
	}
	return true;
}




static bool t_LoadTRawSS(
	const char* fname,
	EVec3i &reso ,
	EVec3f &pitch,
	short* &vol
	)
{
	FILE *fp = fopen(fname, "rb");
	if (fp == 0) return false;

	fread(&reso [0], sizeof(int), 1, fp);
	fread(&reso [1], sizeof(int), 1, fp);
	fread(&reso [2], sizeof(int), 1, fp);
	
	double px, py, pz;
	fread(&px, sizeof(double), 1, fp);
	fread(&py, sizeof(double), 1, fp);
	fread(&pz, sizeof(double), 1, fp);
	pitch << (float) px, (float) py, (float) pz;

	const int WHD = reso[0] * reso[1] * reso[2];


	if (WHD> 1024 * 1024 * 1024 )
	{
		CLI_MessageBox_OK_Show( "volume size is too big (2Gbyte voxels)", "caution");
		return false;
	}

	//read signed short array
	vol = new short[WHD];
  size_t num = fread(vol, sizeof(short), WHD, fp);
	fclose(fp);

	if (num != WHD){
		delete[] vol;
    vol = 0;
		return false;
	}

	return true;
}



//気象データ用のレガシーコード
static bool t_LoadTxt(
	const char* fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol )
{
	reso  << 600, 300, 110;
	pitch << 1,1,1;

	vol = new short[600 * 300 * 110];

	double d;

	FILE *fp = fopen(fname, "r");

	for (int r = 0; r < 600; ++r)
	{
		for (int p = 0; p < 110; ++p)
		{
			for (int t = 0; t < 300; ++t)
			{
				fscanf( fp, "%lf", &d );
				vol[ p * 300 * 600 + t * 600 + r] = (short)d;
				fscanf( fp, ",");
			}
		}
	}

	return true;
}


/*
////////////////////////////////////////////////////////////////////////////////////
static bool t_LoadFAV(
	const  string fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol)
{

	std::cout << "t_LoadFAV";
	
	FavLibrary::Fav fav;
	fav.read( fname.c_str() );

	if (fav.getNumObjects() == 0) return false;
	
	int id = fav.getObjects().begin()->first;
	FavLibrary::Object     &obj       = fav.getObjects().begin()->second;
	FavLibrary::Structure  &structure = obj.structure;		;
	FavLibrary::Grid       &grid      = obj.grid     ;
	FavLibrary::BitPerVoxel bitPerVox = structure.getBitPerVoxel();
	std::cout << fav.getObjects().size() << " " << id << " " << bitPerVox << "\n";

	FavLibrary::ColorMode cMode = structure.getColorMode();
	std::cout << "mode " << structure.getColorMode() << "\n";

	const int W = (int)structure.getDimensionX();
	const int H = (int)structure.getDimensionY();
	const int D = (int)structure.getDimensionZ();
	
	
	reso  << W, H, D;
	pitch << 1, 1, 1;
	vol = new short[W*H*D];

	std::cout << "dimension << W << " " << H << " " << D << "\n";
	std::cout << structure.get_voxel_map()      ->size() << "\n";
	std::cout << structure.get_voxel_map_16bit()->size() << "\n";
	std::cout << structure.get_alpha_map()      ->size() << "\n";
	std::cout << structure.get_color_map()      ->size() << "\n";
	std::cout << structure.get_color_map_16bit()->size() << "\n";

  
	for( int z = 0; z < D; ++z)
  {
	  for( int y = 0; y < H; ++y)
    {
	    for( int x = 0; x < W; ++x)
	    {
		    //int r = structure.getColorRed  (x, y, z);
		    //int g = structure.getColorGreen(x, y, z);
		    //int b = structure.getColorBlue (x, y, z);
		    //std::cout << r << " " << g << " " << b << "--" ;
		    //vol[x + y * W + z * W*H] = r+g+b;
	    }
    }
  }


    return true;
}
*/


static bool t_LoadBMP_TIFs
(
	const vector<string> &fnames,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol 
)
{
	OglImage2D<CH_RGBA> img;
	img.Allocate( fnames.front().c_str() );

	const int W = reso[0] = img.GetW();
	const int H = reso[1] = img.GetH();
	const int D = reso[2] = (int)fnames.size();
	const int WH = W*H;

	if( W == 0 || H == 0 || D == 0) return false;

	vol = new short[W * H* D ];

	for(int z=0; z < (int)fnames.size(); ++z)
	{
		OglImage2D<CH_RGBA> slice;
		slice.Allocate( fnames[z].c_str() );

		if( slice.GetW() != W || slice.GetH() != H )
		{
			delete[] vol;
			vol = 0;
			return false;
		}

		//copy
		for(int y=0; y < H; ++y)
		{
			for(int x=0; x < W; ++x)
			{
				const int I = 4 * (x + y * W);
				vol[ x + y * W + z * WH ] = (short) ( (slice[ I ] + slice[ I+1 ] + slice[ I+2 ]) / 3) ;
			}
		}
		if( z % 50 ==0 ) std::cout << z << " / " << fnames.size() << "done\n";
	}

	pitch << 1,1,1;	
	return true;
}



static bool t_LoadDCMs
(
	const vector<string> &fnames,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol 
)
{

	//read initial frame to get size info
	Tdcmtk tdcm(fnames.front().c_str());
	int fNum;
	tdcm.getSize(reso[0], reso[1], fNum);

	const int W = reso[0];
	const int H = reso[1];
	const int D = reso[2] = (int)fnames.size();

	pitch[0] = (float) tdcm.getPitchX();
	pitch[1] = (float) tdcm.getPitchY();
	double zPos0 = tdcm.getZPos();

	vol = new short[W*H*D];

	//load all dicom file
	for (int z = 0; z < D; ++z)
	{
		Tdcmtk tdcm( fnames[z].c_str() );
		std::cout << tdcm.getZPos() << "\n";

		int tW, tH, tfNum;
		tdcm.getSize(tW, tH, tfNum);

		if (tfNum > 1 || W != tW || H != tH )
		{
			CLI_MessageBox_OK_Show("error strange input", "error");
      delete[] vol;
      vol = 0;
      return false;
		}

		if (z == 1) pitch[2] = (float)( tdcm.getZPos() - zPos0 );

		tdcm.getPixelsAs<short>( &vol[z*W*H] );

		std::cout << "(" << z << "/" << D << ")\n";
	}

	if( pitch[2] < 0)
	{
		std::cout << "flip in z\n";
		pitch[2] *= -1.0;
    //t_FlipVolumeInZ<short>(W,H,D, vol);
    FlipVolumeInZ<short>(W,H,D, vol);
	}

	return true;
}





static bool t_LoadDCM3D(
	const  string fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol)
{
	Tdcmtk tdcm( fname.c_str() );

	tdcm.getSize(reso[0], reso[1], reso[2]);
	if( reso[2] <= 1 ) return false;
	
	pitch[0] = 1;
	pitch[1] = 1;
	pitch[2] = 1;
  CLI_MessageBox_OK_Show( "pitch情報は読み込んでいません。正しい値をダイアログより指定してしてください", "message");

	std::cout << "resolution " << reso[0] << " " << reso[1]  << " " << reso[2] << "\n";

	vol = new short[ reso[0] * reso[1] * reso[2] ];
	tdcm.getPixels3DAs<short>( vol );

	return true;
}




static bool t_LoadPVM3D(
	const  string fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol)
{
	unsigned int W,H,D,components;
	float  px,py,pz;

	byte *buf = readPVMvolume(fname.c_str(), &W, &H, &D,&components, &px,&py,&pz );
  std::cout << "load pvm " << W << " " << H << " " << D << " " << components << "\n";

	reso  << W , H , D ;
	pitch << px, py, pz;
	vol = new short[W*H*D];

	for ( int i=0, s = W*H*D; i < s; ++i)
	{
		if ( components == 1)
    {
      vol[i] = buf[i];
    }
    else if ( components == 2) 
    {
			vol[i] = (short) ( buf[2*i] * 256 + buf[2*i+1] );
		}
	}
	return true;
}



bool ImageCore::LoadVolume(string fname, string fext)
{
	vector<string> fnames;
	fnames.push_back( fname );
	return LoadVolume( fnames, fext );
}


bool ImageCore::LoadVolume(vector<string> fnames, string fext)
{
	if (fnames.size() == 0) return false;
	
	const int L = (int)fext.length();

	int trgtId = 
    (fnames.size() == 1 && ( fext == "traw3D_ss"                     ) ) ? 0 : 
		(fnames.size() == 1 && ( fext == "mat" || fext == "MAT"          ) ) ? 1 :
		(fnames.size() == 1 && ( fext == "txt" || fext == "TXT"          ) ) ? 2 :
		(fnames.size() == 1 && ( fext == "dcm" || fext == "DCM" || L == 0) ) ? 3 :
		(fnames.size() == 1 && ( fext == "pvm" || fext == "PVM"          ) ) ? 4 :
		(fnames.size() == 1 && ( fext == "fav" || fext == "FAV"          ) ) ? 5 :
		(fnames.size() == 1 && ( fext == "sph" || fext == "SPH"          ) ) ? 6 :
		(fnames.size() >  1 && ( fext == "dcm" || fext == "DCM" || L == 0) ) ? 10 :	
		(fnames.size() >  1 && ( fext == "bmp" || fext == "BMP"	         ) ) ? 11 :
		(fnames.size() >  1 && ( fext == "tif" || fext == "TIF"	         ) ) ? 11 : -1;
	
	if ( m_vol_orig  ) delete[] m_vol_orig  ;
	if ( m_vol_origgm) delete[] m_vol_origgm;
	m_vol_orig   = 0;
	m_vol_origgm = 0;

	bool success = false;
	//3D
  if ( trgtId == 0 && t_LoadTRawSS   (fnames[0].c_str(), m_resolution, m_pitch, m_vol_orig) ) success = true;
	if ( trgtId == 2 && t_LoadTxt      (fnames[0].c_str(), m_resolution, m_pitch, m_vol_orig) ) success = true;
	if ( trgtId == 3 && t_LoadDCM3D    (fnames[0].c_str(), m_resolution, m_pitch, m_vol_orig) ) success = true;
	if ( trgtId == 4 && t_LoadPVM3D    (fnames[0].c_str(), m_resolution, m_pitch, m_vol_orig) ) success = true;
	
  //if ( trgtId == 5 && t_LoadFAV      (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
  if ( trgtId == 5 ) CLI_MessageBox_OK_Show("fav loader is under construction.", "message");
  if ( trgtId == 6 ) CLI_MessageBox_OK_Show("sph loader is under construction.", "message");

  //2D slices
	if ( trgtId ==10 && t_LoadDCMs     (fnames           , m_resolution, m_pitch, m_vol_orig) ) success = true;
	if ( trgtId ==11 && t_LoadBMP_TIFs (fnames           , m_resolution, m_pitch, m_vol_orig) ) success = true;


	bool strangePitch = m_pitch[0] <= 0 || m_pitch[1] <= 0 || m_pitch[2] <= 0;

	if( success && strangePitch )
  {
		m_pitch[0] = m_pitch[1] = m_pitch[2] = 1;
		CLI_MessageBox_OK_Show("Pitch情報を読み込めませんでした。\n正しい値をダイアログより指定してください.", "message");
	}

	if (success)
	{
		m_filepath = fnames[0];

		if( trgtId == 10 && !strangePitch)
		{
			CLI_MessageBox_OK_Show("Dicom Sliceを読み込みました。\n dcm属性値より画像のスタック方向を自動決定しました。\n（念のため左右反転の有無を確認してください。）", "message");
		}
		else
		{
      int flg = RoiPainter3D::formStackOrder_showModalDialog();
      //if (flg == 1) t_FlipVolumeInZ<short>(m_resolution[0], m_resolution[1], m_resolution[2], m_vol_orig);
      if (flg == 1) FlipVolumeInZ<short>( m_resolution[0], m_resolution[1], m_resolution[2], m_vol_orig);
		}
	}
	else 
	{
		m_filepath = "";
		t_LoadDefoultVolume(m_resolution, m_pitch, m_vol_orig);
	}

	// post loading 
	short minV, maxV;
	GetMaxMinOfArray<short>( m_resolution[0]* m_resolution[1] * m_resolution[2], m_vol_orig, minV, maxV);

	m_vol_origgm = new float[m_resolution[0]* m_resolution[1] * m_resolution[2]];
	m_vol      .Allocate( m_resolution );
	m_vol_flag .Allocate( m_resolution );
	m_vol_mask .Allocate( m_resolution );
	m_vol_gm   .Allocate( m_resolution );

	m_vol     .SetValue(m_vol_orig, minV, maxV);
	m_vol_flag.SetAllZero();
	m_vol_mask.SetAllZero();

	m_mask_data.clear();
	m_mask_data.push_back( MaskData("bckGrnd", EVec3i(0,0,0), 0, 0 ) );

  m_vol_minmax  << minV, maxV;

	UpdateGradMagnituteVolume();

	ModeSegGCut::getInst()->NewVolLoaded();

  return true;
}


void ImageCore::UpdateGradMagnituteVolume()
{
	const int N = m_resolution[0] * m_resolution[1] * m_resolution[2];

	memset( m_vol_origgm, 0, sizeof(float) * N);
	Sobel3D<short, float>( m_resolution[0], m_resolution[1], m_resolution[2], m_vol_orig, m_vol_origgm);

	float minV, maxV;
	GetMaxMinOfArray<float>( N, m_vol_origgm, minV, maxV);

	const float c = 255.0f;
	for (int i = 0; i < N; ++i)
	{
		m_vol_origgm[i] = (m_vol_origgm[i] - minV) / (maxV - minV);
		m_vol_gm[i] = (byte)( 255 * m_vol_origgm[i] ) ;
	}

	m_vol_gm.SetUpdated();
}



void ImageCore::UpdateOGLVolume(short windowlv_min,  short windowlv_max)
{
  m_vol.SetValue( m_vol_orig, (short)windowlv_min, (short)windowlv_max);
}



void ImageCore::UpdateOGLMaskColorImg()
{
  //gen mask color id
  m_img_maskcolor.SetZero();
  for (int i = 0; i < (int)m_mask_data.size(); ++i)
  {
    m_img_maskcolor[4 * i + 0] = m_mask_data[i].m_color[0];
    m_img_maskcolor[4 * i + 1] = m_mask_data[i].m_color[1];
    m_img_maskcolor[4 * i + 2] = m_mask_data[i].m_color[2];
    m_img_maskcolor[4 * i + 3] = (int)(m_mask_data[i].m_alpha * 255);

    if (m_mask_data[i].m_b_drawsurface) ImageCore::GetInst()->m_img_maskcolor[4 * i + 3] = 0;
  }
  m_img_maskcolor.SetUpdated();
}



void  ImageCore::DrawMaskSurfaces()
{

  for (int i = 0; i < (int)ImageCore::GetInst()->m_mask_data.size(); ++i)
  {
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_LIGHT1 );
    glEnable( GL_LIGHT2 );
    glDisable(GL_CULL_FACE);
    glEnable( GL_BLEND );

    if( ImageCore::GetInst()->m_mask_data[i].m_b_drawsurface )
    {
      float  a  = (float)ImageCore::GetInst()->m_mask_data[i].m_alpha;
      EVec3i cd = ImageCore::GetInst()->m_mask_data[i].m_color;
      EVec4f c( cd[0]/500.0f, cd[1]/500.0f, cd[2]/500.0f, a);
      static float spec[4] = {0.5f,0.5f,0.5f, 1};
      static float s[1]    = {64.0f};

      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , c.data() );
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , c.data() );
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , spec     );
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, s        );
      ImageCore::GetInst()->m_mask_data[i].m_surface.Draw();
    }
    glDisable( GL_BLEND );
  }
}



/////////////////////////////////////////////////////////////
//MASK IO////////////////////////////////////////////////////
void ImageCore::LoadMask(const char *fname)
{
	FILE* fp = fopen(fname, "rb");
	
	//save mask image
	int version, W,H,D;
	fread(&version, sizeof(int), 1, fp);
	fread(&W      , sizeof(int), 1, fp);
	fread(&H      , sizeof(int), 1, fp);
	fread(&D      , sizeof(int), 1, fp);

	if(W != m_resolution[0] || H != m_resolution[1] || D != m_resolution[2] )
	{
		RoiPainter3D::CLI_MessageBox_OK_Show( "strange volume size\n", "caution");
		fclose( fp );
		return;
	}

  //read mask voxels
	fread( &m_vol_mask[0], sizeof(byte), W * H * D, fp);


  int flg = RoiPainter3D::formStackOrder_showModalDialog();
  if (flg == 1) m_vol_mask.FlipInZ();

	m_vol_mask.SetUpdated();

  //read mask status
	int maskN;
	fread(&maskN, sizeof(int), 1, fp);

	m_mask_data.clear();

	for (int i=0; i < maskN; ++i)
	{
		int lock, nLen;
		int col[3];
		double alpha;
		fread(&alpha, sizeof(double), 1, fp);
		fread(col   , sizeof(int   ), 3, fp);
		fread(&lock , sizeof(int   ), 1, fp);

		fread(&nLen , sizeof(int   ), 1, fp);
		char *name = new char[nLen + 1];
		fread(name, sizeof(char), nLen + 1, fp);

		std::cout << nLen << " " << name << "\n";

		m_mask_data.push_back( MaskData(string(name), EVec3i(col[0],col[1],col[2]), alpha, 0, lock?true:false) );

		delete[] name; 
	}
	fclose(fp);
}



void ImageCore::SaveMask( const char* fname)
{
  FILE* fp = fopen(fname, "wb");

  //save mask image
  int version = 0;
  fwrite(&version  , sizeof(int), 1, fp);
  fwrite(&m_resolution[0], sizeof(int), 1, fp);
  fwrite(&m_resolution[1], sizeof(int), 1, fp);
  fwrite(&m_resolution[2], sizeof(int), 1, fp);
  fwrite(&m_vol_mask[0], sizeof(byte), m_resolution[0] * m_resolution[1] * m_resolution[2], fp);

	int maskN = (int)m_mask_data.size();
	fwrite(&maskN, sizeof(int), 1, fp);

	for( const auto &it : m_mask_data )
	{
		int iLock = it.m_b_locked;
		fwrite(&it.m_alpha      , sizeof(double), 1, fp);
		fwrite(it.m_color.data(), sizeof(int)   , 3, fp);
		fwrite(&iLock, sizeof(int), 1, fp);

		int nLen = (int)it.m_name.length();
		fwrite(&nLen          , sizeof(int ),  1    , fp);
		fwrite(it.m_name.c_str(), sizeof(char), nLen+1, fp);

		std::cout << nLen << " " << it.m_name.c_str() << "\n";
	}
	fclose(fp);
}





//////////////////////////////////////////////////////////
//Mask Manipulation///////////////////////////////////////

//todo unique pointerを利用する
//
// 0: locked mask id, 1: unlocked non-target, 255:target mask id
void ImageCore::GetFlgVolByMask_0_1_255(
    const int trgt_maskid,
    byte* flgvol //allocated
  )
{
  const int N = GetNumVoxels();

  byte *mask_locked = new byte[m_mask_data.size()];
  for( int i=0; i < m_mask_data.size(); ++i )
  {
    mask_locked[i] = m_mask_data[i].m_b_locked ? 1 : 0;
  }

#pragma omp parallel for 
  for (int i = 0; i < N; ++i) 
  {
    flgvol[i] = (m_vol_mask[i] == trgt_maskid) ? 255 :
                (mask_locked[m_vol_mask[i]]  ) ? 0 : 1;
  }


  delete[] mask_locked;
}



void ImageCore::ActiveMask_Delete  ()
{
  if (m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid) return;

  if (m_active_maskid == 0)
  {
    CLI_MessageBox_OK_Show("0th region (background) cannot be removed", "caution");
    return;
  }

  if (CLI_MessageBox_YESNO_Show("Do you want to DELETE the selected mask?", "caution") == false) return;


  const int N = m_resolution[0] * m_resolution[1] * m_resolution[2];

  byte* mask3d = m_vol_mask.GetVolumePtr();
#pragma omp parallel for
  for (int i = 0; i < N; ++i)
  {
    if (     mask3d[i] >  m_active_maskid) mask3d[i]--;
    else if (mask3d[i] == m_active_maskid) mask3d[i] = 0;
  }

  m_mask_data.erase( m_mask_data.begin() + m_active_maskid );
  m_active_maskid = 0;
  m_vol_mask.SetUpdated();

}

// map_old2new_id は 旧IDから新IDへの対応
// example 旧ID  0,1,2,3,4,5,6,7   (2,3,5がマージ対象)
//         新ID  0,1,x,x,2,x,3,4  x=5
//
//
void ImageCore::MargeMaskIDs (std::set<int> &ids)
{
  m_active_maskid = 0;

  //check zero 
  bool b_zero_exist = false;
  for ( auto i : ids ) 
    if( i == 0 ) b_zero_exist = true;

  if (b_zero_exist)
  {
    const char *message = "0th region (background) cannot be marged";
    CLI_MessageBox_OK_Show( message, "CAUTION");
    return;
  }

  const int num_mask_id = (int)m_mask_data.size();
  
  //calc mapping and new_mask_data
  int *map_old2new_id = new int[ num_mask_id ];

  int tmp_id = 0;
  for ( int i = 0; i < num_mask_id; ++i)
  {
    if( ids.find(i) != ids.end() )
    { 
      map_old2new_id[i] = num_mask_id - (int) ids.size();
    }
    else
    {
      map_old2new_id[i] = tmp_id;
      ++tmp_id;
    }
  }

  //modify vol mask
  const int N = GetNumVoxels();

#pragma omp parallel for
  for (int i = 0; i < N; ++i)
  {
    m_vol_mask[i] = map_old2new_id[m_vol_mask[i]];
  }
  m_vol_mask.SetUpdated();

  //modify mask data
  MaskData marge_maskdata = m_mask_data[ *ids.begin() ];
  for ( auto it = ids.rbegin(); it != ids.rend(); ++it)
  {
    m_mask_data.erase(m_mask_data.begin() + *it);
  }

  marge_maskdata.m_surface.Clear();
  marge_maskdata.m_b_drawsurface = false;
  m_mask_data.push_back( marge_maskdata );

  delete[] map_old2new_id;
}


//
//smart fillhole 
//
void ImageCore::SmartFillHole( std::set<int> &ids, int dilation_size )
{
  bool b_zero_exist = false;
  for ( auto i : ids ) 
    if( i == 0 ) b_zero_exist = true;

  if (b_zero_exist)
  {
    const char *message = 
      "0th region (background) cannot be used"
      "0番領域はSmart fillhole似利用できません";
    CLI_MessageBox_OK_Show( message, "CAUTION");
    return;
  }

  //1. gen volume 
  const int num_masks  = (int)m_mask_data.size();
  const int num_voxels = GetNumVoxels();
  byte *is_trgt    = new byte[num_masks ];
  byte *vol_orig   = new byte[num_voxels];

  memset( is_trgt, 0, sizeof(byte) * num_masks );
  for ( auto it : ids) is_trgt[it] = 1;

#pragma omp parallel for 
  for ( int i=0; i < num_voxels; ++i )
  {
    vol_orig[i] = is_trgt[m_vol_mask[i]] ? 255 : 1;
  }


  //2. calc dilation
  byte *vol_dilate = new byte[num_voxels];
  memcpy( vol_dilate, vol_orig, sizeof(byte)*num_voxels);

  for( int i=0; i < dilation_size; ++i)
    Dilate3D(m_resolution, vol_dilate);
  
  //3. calc hole filling --> extract hole area (前景に変化した画素を抽出)
  byte *vol_fill   = new byte[num_voxels];
  

#pragma omp parallel for 
  for ( int i=0; i < num_voxels; ++i )
  {
    vol_fill[i] = (vol_dilate[i] == 255) ? 255 : 0;
  }

  FillHole3D(m_resolution[0], m_resolution[1], m_resolution[2], vol_fill);

#pragma omp parallel for 
  for ( int i=0; i < num_voxels; ++i )
  {
    vol_fill[i] = (vol_fill[i]==255 && vol_dilate[i] != 255) ? 255 : 1;
  }

  
  //store hole area
  bool b_foreexist = false;
  for ( int i=0; i < num_voxels && !b_foreexist; ++i ) 
  {
    if ( vol_fill[i] == 255) b_foreexist = true;
  }
  
  if( b_foreexist )
  {
    for ( int i=0; i < dilation_size; ++i )
      Dilate3D(m_resolution, vol_fill);

    m_vol_flag.SetAllZero();

#pragma omp parallel for     
    for ( int i=0; i < num_voxels; ++i)
    {
      int maskid = m_vol_mask[i]; 
      bool tf = (vol_fill[i] == 255) && 
                ( maskid == 0 || !m_mask_data[maskid].m_b_locked ) ;
      if ( tf ) 
      {
        m_vol_flag[i] = 255;
      }
    }

    StoreForegroundAsNewMask();
  } 

  delete[] is_trgt;
  delete[] vol_orig;
  delete[] vol_dilate;
  delete[] vol_fill;
}




void ImageCore::ActiveMask_Erode()
{
  std::cout << "mask erode...\n";
  if ( m_active_maskid <= 0 || m_mask_data.size() <= m_active_maskid) return;

  const int N = m_resolution[0] * m_resolution[1] * m_resolution[2];
  
  byte* flgs = new byte[N];
  GetFlgVolByMask_0_1_255( m_active_maskid, flgs);

  Erode3D( m_resolution, flgs);

#pragma omp parallel for 
  for (int i = 0; i < N; ++i) 
  {
    if (m_vol_mask[i] == m_active_maskid && flgs[i] == 1) 
    {
      m_vol_mask[i] = 0;
    }
  }

  m_vol_mask.SetUpdated();
  delete[] flgs;

  std::cout << "mask erode...DONE\n";
}



void ImageCore::ActiveMask_Dilate  ()
{
  if ( m_active_maskid <= 0 || m_mask_data.size() <= m_active_maskid) return;

  std::cout << "mask dilate...\n";
  const int N = m_resolution[0] * m_resolution[1] * m_resolution[2];

  byte* flgs = new byte[N];
  GetFlgVolByMask_0_1_255( m_active_maskid, flgs);
  
  Dilate3D( m_resolution, flgs);

#pragma omp parallel for 
  for (int i = 0; i < N; ++i) 
  {
    if (flgs[i] == 255) 
    {
      m_vol_mask[i] = m_active_maskid;
    }
  }
  m_vol_mask.SetUpdated();

  delete[] flgs;
  std::cout << "mask dilate...DONE\n";
}



void ImageCore::FillHole( std::set<int> &ids )
{
  std::cout << "mask fillhole...\n";

  bool b_zero_exist = false;
  for ( auto i : ids ) 
    if( i == 0 ) b_zero_exist = true;

  if (b_zero_exist)
  {
    const char *message = 
      "0th region (background) cannot be used"
      "0番領域はSmart fillhole似利用できません";
    CLI_MessageBox_OK_Show( message, "CAUTION");
    return;
  }

  //1. gen flg volume 
  const int num_masks  = (int)m_mask_data.size();
  const int num_voxels = GetNumVoxels();
  byte *is_trgt  = new byte[num_masks ];
  byte *vol_flg  = new byte[num_voxels];
  byte *vol_hole = new byte[num_voxels];

  memset( is_trgt, 0, sizeof(byte) * num_masks );
  for ( auto it : ids) is_trgt[it] = 1;

#pragma omp parallel for 
  for ( int i=0; i < num_voxels; ++i )
  {
    vol_flg[i] = is_trgt[m_vol_mask[i]] ? 255 : 0;
  }

  memcpy( vol_hole, vol_flg, sizeof(byte) * num_voxels );
  FillHole3D(m_resolution[0], m_resolution[1], m_resolution[2], vol_hole);


  m_vol_flag.SetAllZero();
#pragma omp parallel for     
  for ( int i=0; i < num_voxels; ++i)
  {
    if( vol_hole[i] == 255 && 
        vol_flg [i] != 255 && 
       !m_mask_data[m_vol_mask[i]].m_b_locked)
    {
      m_vol_flag[i] = 255;
    }
  }

  StoreForegroundAsNewMask();

  delete[] is_trgt;
  delete[] vol_flg;
  delete[] vol_hole;

  std::cout << "fillhole...DONE\n";
}




void ImageCore::ActiveMask_ExportObj  (const string &fname)
{
  if ( m_active_maskid <= 0 || m_mask_data.size() <= m_active_maskid) return;

  const char* message = "Smoothing Time? (平滑化回数を指定)";
  int smoothing_n;
  FormIntegerSelection_doModal( 2, 0, 100, message, smoothing_n );


  const int N = GetNumVoxels();

  short *v = new short[N];
  for (int i = 0; i < N; ++i) 
  {
    v[i] = (m_vol_mask[i] == m_active_maskid) ? 255 : 0;
  }

  TMesh mesh;
  marchingcubes::MarchingCubes(m_resolution, m_pitch, v, 128, 0, 0, mesh);
  mesh.Smoothing(smoothing_n);
  mesh.ExportObjNoTexCd(fname.c_str());

  delete[] v;
}



void ImageCore::ExportMaskIDsAsOneMesh   ( std::set<int> mask_ids, const char *fname )
{
  const char* message = "Smoothing Time? (平滑化回数を指定)";
  int smoothing_n;
  FormIntegerSelection_doModal( 2, 0, 100, message, smoothing_n );

  //vector[]のアクセスが遅いので配列を作成
  byte mask_b_trgt[256] = {0};
  for( auto id : mask_ids ) mask_b_trgt[id] = 1;
  
  //generate binary volume
  const int N = GetNumVoxels();
  short *v = new short[N];

  for (int i = 0; i < N; ++i) 
  {
    v[i] = ( mask_b_trgt[ m_vol_mask[i] ] ) ? 255 : 0;
  }

  TMesh mesh;
  marchingcubes::MarchingCubes( m_resolution, m_pitch, v, 128, 0, 0, mesh);
  mesh.Smoothing(smoothing_n);
  mesh.ExportObjNoTexCd( fname );

  delete[] v; 
}


static std::string GenBmpSliceName(std::string base, int idx )
{
  string fname = string(base);                     
  if      ( idx < 10  ) fname += string("000");
  else if ( idx < 100 ) fname += string("00") ;
  else if ( idx < 1000) fname += string("0")  ;
  fname += std::to_string( idx ) + string(".bmp");
  return fname;
}


void ImageCore::ExportAllMaskIdAsBmpSlice( const char *fname )
{ 
  int W, H, D, WH, WHD;
  std::tie(W,H,D,WH,WHD) = GetResolution5();

  OGLImage2D4 img;
  img.Allocate(W,H);

  for ( int z = 0; z < D; ++z )
  {
    for ( int y = 0; y < H; ++y )
    {
      for ( int x = 0; x < W; ++x )
      {
        EVec3i &c = m_mask_data[ m_vol_mask[x + y*W + z*WH] ].m_color;
        img[4*(x + y*W) + 0] = c[0];
        img[4*(x + y*W) + 1] = c[1];
        img[4*(x + y*W) + 2] = c[2];
      }
    }
    img.SaveAs( GenBmpSliceName( fname, z).c_str() );
  }
}



void ImageCore::ExportVolumeAsBmpSlice( 
    const char* fname, 
    int xy_yz_zx )
{
  int W, H, D, WH, WHD;
  std::tie(W,H,D,WH,WHD) = GetResolution5();

  OGLImage2D4 img;

  if ( xy_yz_zx == 0 )
  {
    img.Allocate(W,H);
    for ( int z = 0; z < D; ++z )
    {
      for ( int y = 0; y < H; ++y )
      {
        for ( int x = 0; x < W; ++x )
        {
          img.SetIntensity(x,y, m_vol[x + y*W + z*WH]);
        }
      }
      img.SaveAs( GenBmpSliceName( fname, z).c_str() );
    }
  }
  else if ( xy_yz_zx == 1 )
  {
    img.Allocate(D,H);
    for ( int x = 0; x < W; ++x )
    {
      for ( int z = 0; z < D; ++z )
      {
        for ( int y = 0; y < H; ++y )
        {
          img.SetIntensity(z,y, m_vol[x + y*W + z*WH]);
        }
      }
      img.SaveAs( GenBmpSliceName( fname, x).c_str() );
    }
  }
  else
  {
    img.Allocate(D,W);
    for ( int y = 0; y < H; ++y )
    {
      for ( int z = 0; z < D; ++z )
      {
        for ( int x = 0; x < W; ++x )
        {
          img.SetIntensity(z,x, m_vol[x + y*W + z*WH]);
        }
      }
      img.SaveAs( GenBmpSliceName( fname, y).c_str() );
    }

  }
}








void ImageCore::ExportOneMaskIdAsBmpSlice( int mask_id, const char *fname)
{
  int W, H, D, WH, WHD;
  std::tie(W,H,D,WH,WHD) = GetResolution5();

  OGLImage2D1 img;
  img.Allocate(W,H);

  for ( int z = 0; z < D; ++z )
  {
    for ( int y = 0; y < H; ++y )
    {
      for ( int x = 0; x < W; ++x )
      {
        if ( m_vol_mask[x + y*W + z*WH] == mask_id ) 
          img[x + y*W ] = 255;
        else 
          img[x + y*W ] = 0;
      }
    }
    string bmp_name = string(fname);                     

    if      ( z < 10  ) bmp_name += string("000");
    else if ( z < 100 ) bmp_name += string("00") ;
    else if ( z < 1000) bmp_name += string("0")  ;
    bmp_name += std::to_string(z) + string(".bmp");
    
    img.SaveAs( bmp_name.c_str() );
  }
}






static const int ColPalletN = 14;
static const EVec3i ColPallet[ColPalletN] = {
	EVec3i(255,0  ,0) ,
  EVec3i(0  ,0,255), 
	EVec3i(0,255,255 ), 
  EVec3i(255,0,255), 
  EVec3i(255,255,0 ),
	EVec3i(0, 100,100), 
  EVec3i(100,0,100), 
  EVec3i(100,100, 0), 
	EVec3i(255,100,0) ,
  EVec3i(0,255,100), 
  EVec3i(100,0,255 ), 
	EVec3i(100,255,0) , 
  EVec3i(0,100,255),
  EVec3i(255, 0, 100), 
};



void ImageCore::StoreForegroundAsNewMask()
{
	const int id = (int) m_mask_data.size();
	const int N = m_resolution[0] * m_resolution[1] * m_resolution[2];

#pragma omp parallel for
	for (int i = 0; i < N; ++i)
  {
    if( m_vol_flag[i] == 255 )  
      m_vol_mask[i] = id;
  }
	m_vol_mask.SetUpdated();

	//initial color
	static int c = 1;
	c++;
	//store new region
	m_mask_data.push_back( 
    MaskData("region", ColPallet[c%ColPalletN], 0.1, false, true)
  );
}


void ImageCore::ActiveMask_SetLocked(const bool tf)
{
	if( m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid ) return;
	m_mask_data[m_active_maskid].m_b_locked = tf;
}


void ImageCore::ActiveMask_SetAlpha(const double alpha)
{
	if( m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid ) return;
	m_mask_data[m_active_maskid].m_alpha = alpha;
}


void ImageCore::ActiveMask_SetColor(const EVec3i &c)
{
	if( m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid ) return;
	m_mask_data[m_active_maskid].m_color = c;
}



void ImageCore::ActiveMask_SetRendSurf(const bool tf)
{
	if( m_active_maskid < 0 || m_mask_data.size() <= m_active_maskid ) return;
	MaskData &trgtMsk = m_mask_data[m_active_maskid];

	trgtMsk.m_b_drawsurface = tf;
	
	if( tf == true && trgtMsk.m_surface.m_vSize == 0)
	{
		const int N = GetNumVoxels();

		short *v = new short[N];

		for( int i=0; i < N; ++i )
    {
      v[i] = ( m_vol_mask[i] == m_active_maskid ) ? 255 : 0;
    }

    marchingcubes::MarchingCubes(m_resolution, m_pitch, v, 128, 0, 0, trgtMsk.m_surface);
		trgtMsk.m_surface.Smoothing(2);

		delete[] v;
	}
}



void  ImageCore::ClearMaskSurface(int trgtid) 
{
  if( trgtid < 0 || m_mask_data.size() <= trgtid ) return;
  MaskData &trgtMsk = m_mask_data[trgtid];

  trgtMsk.m_b_drawsurface = false;
  trgtMsk.m_surface.Clear();
}





void ImageCore::SaveVolumeAsTraw3dss(const char *fname)
{
  string f = string(fname);
  if(f.length() < 9 || f.substr(f.length()-9, 9) != "traw3D_ss" ){
    f += ".traw3D_ss";
  }
    
  EVec3i res = m_resolution;
  EVec3f pit = m_pitch;
  short *vol = m_vol_orig;

	const int     W = res[0];
	const int     H = res[1];
	const int     D = res[2];
	const int WHD = W*H*D;
	const double px = pit[0];
	const double py = pit[1];
	const double pz = pit[2];

  FILE *fp = fopen( fname, "wb" );
  if( fp == 0 ) return;

  fwrite( &W , sizeof(int   ), 1, fp ); 
  fwrite( &H , sizeof(int   ), 1, fp ); 
  fwrite( &D , sizeof(int   ), 1, fp );
  fwrite( &px, sizeof(double), 1, fp ); 
  fwrite( &py, sizeof(double), 1, fp ); 
  fwrite( &pz, sizeof(double), 1, fp );
  for (int i = 0; i < WHD; ++i)
  {
    short v = (short)vol[i];
    fwrite( &v, sizeof(short), 1, fp );
  }
  fclose(fp);

  /*
  //test code just for debug
  float *imgZX = new float[D*W];
  float *imgZY = new float[D*H];
  memset( imgZX, 0, sizeof(float) * D*W);
  memset( imgZY, 0, sizeof(float) * D*H);

  for( int x = 0; x < W; ++x)
  for( int y = 0; y < H; ++y)
  for( int z = 0; z < D; ++z)
  {
    imgZX[x + z*W] += vol[x + y*W + z *W*H];
    imgZY[y + z*H] += vol[x + y*W + z *W*H];
  }

  OGLImage2D4 img;
  img.Allocate(W,D);
  img.setGrayValue_normalize(imgZX);
  img.SaveAs("zx.bmp", 0);
  
  img.Allocate(H,D);
  img.setGrayValue_normalize(imgZY);
  img.SaveAs("zy.bmp", 0);
  */
}



// m_vol_flagを初期化
// voxel at locked mask --> 0 
// otherwise --> 1
void ImageCore::InitializeVolFlgByLockedMask(int fore_maskid )
{
  const int num_voxels = GetNumVoxels();

  //vector[]のアクセスが遅いので配列を作成
  byte masklocked[256] = {};
  for( int i=0; i < (int)m_mask_data.size(); ++i ) 
    masklocked[i] = m_mask_data[i].m_b_locked ? 255 : 0;

#pragma omp parallel for
  for (int i = 0; i < num_voxels; ++i) 
  {
    m_vol_flag[i] = ( m_vol_mask[i]==fore_maskid ) ? 255 :
                    ( masklocked[m_vol_mask[i]]  ) ? 0   : 1;
  }

	m_vol_flag.SetUpdated();
}



bool ImageCore::bForeVoxelsExistInVolFlg()
{
	const int num_voxels = GetNumVoxels();

	bool bForeExist = false;
	for (int i = 0; i < num_voxels; ++i)
	{
		if ( ImageCore::GetInst()->m_vol_flag[i] == 255)
		{
			bForeExist = true;
			break;
		}
	}
  return bForeExist;

}



