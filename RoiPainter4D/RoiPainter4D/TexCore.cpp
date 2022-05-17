#include "TexCore.h"

#include "tmarchingcubes.h"

#include "DlgSelectMskId.h"
#include "DlgSetIntValue.h"
#include "RoiPaintView.h"
#include "Mode\ModeSegGCut.h"
#include "DlgSetStackOrder.h"


#include <random>

//#include "./3rdParty/dcmtk/tdcmtk.h"




TexCore::TexCore()
{
	fprintf(stderr, "TexCore constractor ...\n");


	fprintf(stderr, "TexCore Dlg Gen");
	m_dlg.Create(IDD_DIALOG_VOLINFO);
	m_dlg.ShowWindow(SW_SHOW);
	fprintf(stderr, "Done\n");

	m_volOrig = 0;

	LoadVolume("init","init");

	m_imgTf.Allocate(TRANS_FUNC_SIZE);
	for (int i = 0; i < TRANS_FUNC_SIZE; ++i) m_imgTf[4 * i] = m_imgTf[4 * i + 1] = i;

	m_imgPsu   .AllocateHeuImg(256);
	m_imgMskCol.Allocate(256);

	fprintf(stderr, "TexCore constractor DONE! \n");

}


TexCore::~TexCore()
{
	if( m_volOrig   ) delete[] m_volOrig  ;
	if( m_volOrigGM ) delete[] m_volOrigGM;
}








static bool t_LoadDefoultVolume
	(
	EVec3i &reso,
	EVec3f &pitch,
	float* &vol
	)
{
	const int N = 64, R = N/5;
	reso  << N, N, N;
	pitch << 1, 1, 1;
	vol = new float[reso[0] * reso[1] * reso[2]];

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
				vol[x + y * reso[0] + z * reso[1] * reso[0]] = v - 1500.0f;
			}
		}
	}
	return true;
}




static bool t_LoadTRawSS(
	const char* fname,
	EVec3i &reso ,
	EVec3f &pitch,
	float* &vol
	)
{
	FILE *fp = fopen(fname, "rb");
	if (fp == 0) return false;

	fread(&reso [0], sizeof(int   ), 1, fp);
	fread(&reso [1], sizeof(int   ), 1, fp);
	fread(&reso [2], sizeof(int   ), 1, fp);
	
	double px, py, pz;
	fread(&px, sizeof(double), 1, fp);
	fread(&py, sizeof(double), 1, fp);
	fread(&pz, sizeof(double), 1, fp);
	pitch << (float) px, (float) py, (float) pz;

	const int WHD = reso[0] * reso[1] * reso[2];


	if (WHD> 1024 * 1024 * 1024 )
	{
		AfxMessageBox("volume size is too big (2Gbyte voxels)");
		return false;
	}

	//read signed short array
	short *tmp = new short[WHD];
	if (fread(tmp, sizeof(short), WHD, fp) == WHD)
	{
		vol = new float[WHD];
		for (int i = 0; i < WHD; ++i) vol[i] = tmp[i];
		delete[] tmp;
		fclose(fp);
		return true;
	}
	else
	{
		delete[] tmp;
		fclose(fp);
		return false;
	}
}



bool t_LoadMatLabMat(
	const char* fname,
	EVec3i &reso ,
	EVec3f &pitch,
	float* &vol
	)
{
	AfxMessageBox("TODO この機能は未実装です。");
	//todo
	return false;
}




static bool t_LoadTxt(
	const char* fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	float* &vol )
{
	reso  << 600, 300, 110;
	pitch << 1,1,1;

	vol = new float[600 * 300 * 110];

	double d;

	FILE *fp = fopen(fname, "r");

	for (int r = 0; r < 600; ++r)
	{
		for (int p = 0; p < 110; ++p)
		{
			for (int t = 0; t < 300; ++t)
			{
				fscanf( fp, "%lf", &d );
				vol[ p * 300 * 600 + t * 600 + r] = (float)d;
				fscanf( fp, ",");
			}
		}
	}

	return true;
}






static bool t_LoadBMP_TIFs(
	const vector<string> &fnames,
	EVec3i &reso     ,
	EVec3f &pitch    ,

	float* &vol )
{
	bool bFlip;
	OglImage2D<CH_RGBA> img;
	img.Allocate( fnames.front().c_str(), bFlip );

	const int W = reso[0] = img.getWidth ();
	const int H = reso[1] = img.getHeight();
	const int D = reso[2] = (int)fnames.size();
	const int WH = W*H;

	if( W == 0 || H == 0 || D == 0) return false;

	vol = new float[W * H* D ];

	for(int z=0; z < (int)fnames.size(); ++z)
	{
		OglImage2D<CH_RGBA> slice;
		slice.Allocate( fnames[z].c_str(), bFlip );
		if( bFlip ) {}//do nothing (左上が原点でOK)

		if( slice.getWidth() != W || slice.getHeight() != H )
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
				vol[ x + y * W + z * WH ] = (slice[ I ] + slice[ I+1 ] + slice[ I+2 ]) * 0.3333333f;
			}
		}
		if( z%20 ==0 ) fprintf( stderr, "%d/%d done\n", z, (int)fnames.size());
	}

	pitch << 1,1,1;	
	return true;
}



static bool t_LoadDCMs(
	const vector<string> &fnames,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	float* &vol )
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

	vol = new float[W*H*D];


	//load all dicom file
	for (int z = 0; z < D; ++z)
	{
		Tdcmtk tdcm( fnames[z].c_str() );
		fprintf( stderr, "%f\n", tdcm.getZPos());

		int tW, tH, tfNum;
		tdcm.getSize(tW, tH, tfNum);

		if (tfNum > 1 || W != tW || H != tH )
		{
			AfxMessageBox("error strange input");
			exit(0);
		}

		if (z == 1) pitch[2] = tdcm.getZPos() - zPos0;

		tdcm.getPixelsToFlt( &vol[z*W*H] );

		fprintf( stderr, "(%d/%d)", z, D);
	}


	if( pitch[2] < 0)
	{
		fprintf( stderr, "flip in z\n");
		pitch[2] *= -1.0;
		t_flipVolumeInZ(W,H,D, vol);
	}

	return true;
}





static bool t_LoadDCM3D(
	const  string fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	float* &vol)
{
	Tdcmtk tdcm( fname.c_str() );

	tdcm.getSize(reso[0], reso[1], reso[2]);
	if( reso[2] <= 1 ) return false;
	
	pitch[0] = 1;//(float) tdcm.getPitchX();
	pitch[1] = 1;//(float) tdcm.getPitchY();
	pitch[2] = 1;//pitch[0];
	AfxMessageBox("pitch情報は読み込んでいません。正しい値をダイアログより指定してしてください");

	fprintf( stderr, "resolution %d %d %d\n", reso[0], reso[1], reso[2]);


	vol = new float[ reso[0] * reso[1] * reso[2] ];
	tdcm.getPixelsToFlt3D( vol );



	return true;
}


static bool t_LoadPVM3D(
	const  string fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	float* &vol)
{
	unsigned int W,H,D,components;
	float  px,py,pz;

	byte *buf = readPVMvolume(fname.c_str(), &W, &H, &D,&components, &px,&py,&pz );

	fprintf( stderr, "load pvm %d %d %d %d\n", W,H,D,components);

	reso  << W, H, D;
	pitch << px, py, pz;
	vol = new float[W*H*D];

	for( int i=0,s = W*H*D; i < s; ++i)
	{
		if( components == 1) vol[i] = buf[i];
		else if( components == 2) {
			vol[i] = (int) buf[2*i] * 256 + buf[2*i+1];
		}
	}
	return true;
}








////////////////////////////////////////////////////////////////////////////////////

static bool t_LoadFAV(
	const  string fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	float* &vol)
{

	
	FavLibrary::Fav fav;
	fav.read( fname.c_str() );
	/*
	fprintf( stderr, "voxels %d\n", fav.getNumVoxels() );
	fprintf( stderr, "objects %d\n", fav.getNumObjects());

	for( int i=1; i <= fav.getNumObjects(); ++i){
		fprintf( stderr, "%f %f %f\n",  fav.getObject(i).grid.getDimensionX(),
										fav.getObject(i).grid.getDimensionY(),
										fav.getObject(i).grid.getDimensionZ() );	
	}

	for (int i = 0; i < fav.getNumVoxels(); ++i) 
	{
		int geoId = fav.getVoxel(i).getGeometryInfo().getId();
		fprintf(stderr, "voxel %d\n", geoId );

		fav.palette.getGeometryById(geoId).getShape();
		fprintf(stderr, "geom %f %f %f\n", 
			fav.palette.getGeometryById(geoId).getScaleX(),
			fav.palette.getGeometryById(geoId).getScaleY(),
			fav.palette.getGeometryById(geoId).getScaleZ());
	}
	*/


	if (fav.getNumObjects() == 0) return false;

	FavLibrary::Structure  &structure = fav.getObjects().begin()->second.structure;
	FavLibrary::Grid       &grid      = fav.getObjects().begin()->second.grid     ;
	FavLibrary::BitPerVoxel bitPerVox = structure.getBitPerVoxel();
	fprintf(stderr, "%d!!, %d\n", fav.getObjects().size(), bitPerVox ) ;


	const int W = (int)structure.getDimensionX();
	const int H = (int)structure.getDimensionY();
	const int D = (int)structure.getDimensionZ();
	
	reso  << W, H, D;
	pitch << 1, 1, 1;
	vol = new float[W*H*D];

	fprintf(stderr, "dimension %d %d %d\n", W, H, D);
	for( int z = 0; z < D; ++z)
	for( int y = 0; y < H; ++y)
	for( int x = 0; x < W; ++x)
	{
		fprintf(stderr, "(%d %d %d)", x, y, z);
		int v = structure.getVoxel(x + y * W + z * W*H);
		fprintf(stderr, "(%d %d %d %d)", x, y, z, v);
	}
	

    return true;
}




/*









	byte *buf = readPVMvolume(fname.c_str(), &W, &H, &D,&components, &px,&py,&pz );

	fprintf( stderr, "load pvm %d %d %d %d\n", W,H,D,components);

	reso  << W, H, D;
	pitch << px, py, pz;
	vol = new float[W*H*D];

	for( int i=0,s = W*H*D; i < s; ++i)
	{
		if( components == 1) vol[i] = buf[i];
		else if( components == 2) {
			vol[i] = (int) buf[2*i] * 256 + buf[2*i+1];
		}
	}
	return true;
}

*/

bool TexCore::LoadVolume(string fname, string fext)
{
	vector<string> fnames;
	fnames.push_back( fname );
	return LoadVolume( fnames, fext );
}

bool TexCore::LoadVolume(vector<string> fnames, string fext)
{
	if (fnames.size() == 0) return false;
	

	const int L = (int)fext.length();

	int trgtId = (fnames.size() == 1 && ( fext == "traw3D_ss"					  ) ) ? 0 : 
				 (fnames.size() == 1 && ( fext == "mat" || fext == "MAT"		  ) ) ? 1 :
				 (fnames.size() == 1 && ( fext == "txt" || fext == "TXT"	      ) ) ? 2 :
				 (fnames.size() == 1 && ( fext == "dcm" || fext == "DCM" || L == 0) ) ? 3 :
				 (fnames.size() == 1 && ( fext == "pvm" || fext == "PVM"          ) ) ? 4 :
				 (fnames.size() == 1 && ( fext == "fav" || fext == "FAV"          ) ) ? 5 :

				 (fnames.size() >  1 && ( fext == "dcm" || fext == "DCM" || L == 0) ) ? 10 :	
				 (fnames.size() >  1 && ( fext == "bmp" || fext == "BMP"		  ) ) ? 11 :
				 (fnames.size() >  1 && ( fext == "tif" || fext == "TIF"		  ) ) ? 11 : -1;
	


	if ( m_volOrig  ) delete[] m_volOrig  ;
	if ( m_volOrigGM) delete[] m_volOrigGM;
	m_volOrig   = 0;
	m_volOrigGM = 0;

	bool success = false;
	if ( trgtId == 0 && t_LoadTRawSS   (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId == 1 && t_LoadMatLabMat(fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId == 2 && t_LoadTxt      (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId == 3 && t_LoadDCM3D    (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId == 4 && t_LoadPVM3D    (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId == 5 && t_LoadFAV      (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;

	if ( trgtId ==10 && t_LoadDCMs     (fnames           , m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId ==11 && t_LoadBMP_TIFs (fnames           , m_Reso, m_Pitch, m_volOrig) ) success = true;


	bool strangePitch = m_Pitch[0] <= 0 || m_Pitch[1] <= 0 || m_Pitch[2] <= 0;
	if( success && strangePitch ){
		AfxMessageBox( "Pitch情報を読み込めませんでした。\n正しい値をダイアログより指定してください.");
		m_Pitch[0] = m_Pitch[1] = m_Pitch[2] = 1;
	}



	if (success)
	{
		m_filePath = fnames[0];

		if( trgtId == 10 && !strangePitch)
		{
			AfxMessageBox("Dicom Sliceを読み込みました。\n dcm属性値より画像のスタック方向を自動決定しました。\n（念のため左右反転の有無を確認してください。）");
		}
		else
		{
			DlgSetStackOrder dlg;
			if (IDOK != dlg.DoModal()) t_flipVolumeInZ<float>( m_Reso[0], m_Reso[1], m_Reso[2], m_volOrig);
		}
	}
	else 
	{
		m_filePath = "";
		t_LoadDefoultVolume(m_Reso, m_Pitch, m_volOrig);
	}

	

	// post loading 
	float minV, maxV;
	t_getMaxMinOfArray<float>( m_Reso[0]* m_Reso[1] * m_Reso[2], m_volOrig, minV, maxV);

	m_volOrigGM = new float[m_Reso[0]* m_Reso[1] * m_Reso[2]];
	m_vol    .Allocate( m_Reso );
	m_volFlg .Allocate( m_Reso );
	m_volMsk .Allocate( m_Reso );
	m_volGmag.Allocate( m_Reso );
	
	m_vol   .SetValue(m_volOrig, minV, maxV);
	m_volFlg.SetAllZero();
	m_volMsk.SetAllZero();

	m_maskData.clear();
	m_maskData.push_back( MaskData("bckGrnd", EVec3i(0,0,0), 0, 0 ) );

	m_winLvMin = minV;
	m_winLvMax = maxV;


	updateGradVolume();
	updateHistogram ();
	m_dlg.NewVolumeLoaded(m_Reso, m_Pitch, EVec2d(minV, maxV));
	m_dlg.HistogramUpdated(HIST_BIN_SIZE, m_histVol, m_histGmag);
	CRoiPaintView::getInst()->initCameraPos( getCuboidF() );
	ModeSegGCut::getInst()->newVolLoaded();
	return true;
}





void TexCore::UpdateWindowLevel(float minV, float maxV)
{
	m_vol.SetValue( m_volOrig, minV, maxV);
	updateHistogram();
	m_dlg.HistogramUpdated(HIST_BIN_SIZE, m_histVol, m_histGmag);
}



void TexCore::updateHistogram()
{
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

	for (int i = 0; i < HIST_BIN_SIZE; ++i) m_histGmag[i] = 0;
	for (int i = 0; i < HIST_BIN_SIZE; ++i) m_histVol [i] = 0;
	
	for (int i = 0; i < N; ++i) 
	{
		m_histGmag[ m_volGmag[i] ] += 1;
		m_histVol [ m_vol    [i] ] += 1;
	}

	for (int i = 0; i < HIST_BIN_SIZE  ; ++i) 
	{
		m_histGmag[i] = (float) log( 1.0 + m_histGmag[i] );
	}

	float maxV = 0, maxG = 0;
	for (int i = 5; i < HIST_BIN_SIZE-5; ++i) maxV = max(maxV , m_histVol [i]);
	for (int i = 5; i < HIST_BIN_SIZE-5; ++i) maxG = max(maxG , m_histGmag[i]);
	for (int i = 0; i < HIST_BIN_SIZE  ; ++i) m_histVol [i] /= maxV;
	for (int i = 0; i < HIST_BIN_SIZE  ; ++i) m_histGmag[i] /= maxG;

}


void TexCore::updateGradVolume()
{
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

	memset( m_volOrigGM, 0, sizeof(float) * N);
	t_sobel3D<float>( m_Reso[0], m_Reso[1], m_Reso[2], m_volOrig, m_volOrigGM);

	float minV, maxV;
	t_getMaxMinOfArray<float>( N, m_volOrigGM, minV, maxV);

	const float c = 255.0f;
	for (int i = 0; i < N; ++i)
	{
		m_volOrigGM[i] = (m_volOrigGM[i] - minV) / (maxV - minV);
		m_volGmag[i] = (byte)( 255 * m_volOrigGM[i] ) ;
	}

	m_volGmag.setUpdated();
}






static const int ColPalletN = 14;
static const EVec3i ColPallet[ColPalletN] = {
	EVec3i(255,0  ,0) , EVec3i(0  ,0,255), 
	EVec3i(0,255,255 ), EVec3i(255,0,255), EVec3i(255,255,0 ),
	EVec3i(0, 128,128), EVec3i(128,0,128), EVec3i(128,128, 0), 
	EVec3i(255,128,0) , EVec3i(0,255,128), EVec3i(128,0,255 ), 
	EVec3i(128,255,0) , EVec3i(0,128,255), EVec3i(255, 0, 128) , 
	
};



/*----------------------------------------------
mask_storeCurrentForeGround
generate new region by using all voxels with (m_volFlg[i] == 255) 
-----------------------------------------------*/
void TexCore::mask_storeCurrentForeGround()
{
	const int id = (int) m_maskData.size();
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

	for (int i = 0; i < N; ++i) if( m_volFlg[i] == 255 )  m_volMsk[i] = id;
	m_volMsk.setUpdated();

	//initial color
	static int c = 1;
	c++;
	//store new region
	m_maskData.push_back( MaskData("region", ColPallet[c%ColPalletN], 0.1, false, true) );
}

void TexCore::ActvMsk_SetLock    (const bool   tf    )
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;
	m_maskData[m_maskActiveId].lock = tf;
}




void TexCore::ActvMsk_SetRendSurf(const bool   tf    )
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;

	MaskData &trgtMsk = m_maskData[m_maskActiveId];

	trgtMsk.bRendSurf = tf;
	

	if( tf == true && trgtMsk.surf.m_vSize == 0)
	{
		const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

		byte *v = new byte[N];

		for( int i=0; i < N; ++i )
			v[i] = ( m_volMsk[i] == m_maskActiveId ) ? 255 : 0;

		t_MarchingCubes<byte>(m_Reso, m_Pitch, v,128, 0, 0, trgtMsk.surf);
		trgtMsk.surf.smoothing(2);

		delete[] v;
	}
}



void TexCore::ActvMsk_SetAlpha   (const double alpha )
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;
	m_maskData[m_maskActiveId].alpha = alpha;
}



void TexCore::ActvMsk_SetColor   (const EVec3i &c     )
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;
	m_maskData[m_maskActiveId].color = c;
}




void TexCore::ActvMsk_erode      ()
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;

	OglImage3D v(m_volMsk);
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];
	
	for( int i = 0; i < N; ++i ) 
	{
		v[i] = (m_volMsk[i] == m_maskActiveId ) ? 255 : 
		       (m_maskData[ m_volMsk[i] ].lock) ? 0   : 1; 
	}

	t_morpho3D_erode( v );

	for( int i = 0; i < N; ++i )
	{
		if( m_volMsk[i] == m_maskActiveId && v[i] != 255 ) m_volMsk[i] = 0;
	}

	m_maskData[m_maskActiveId].surf.clear();
	m_maskData[m_maskActiveId].bRendSurf = false;
	m_volMsk.setUpdated();
}





void TexCore::ActvMsk_dilate     ()
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;

	OglImage3D v(m_volMsk);
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];
	
	for( int i = 0; i < N; ++i ) 
	{
		v[i] = (m_volMsk[i] == m_maskActiveId ) ? 255 : 
		       (m_maskData[ m_volMsk[i] ].lock) ? 0   : 1; 
	}

	t_morpho3D_dilate( v );

	for( int i = 0; i < N; ++i )
	{
		if( v[i] == 255 ) m_volMsk[i] = m_maskActiveId;
	}

	m_maskData[m_maskActiveId].surf.clear();
	m_maskData[m_maskActiveId].bRendSurf = false;
	m_volMsk.setUpdated();
}





void TexCore::ActvMsk_fillHole   ()
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;

	OglImage3D v(m_volMsk);
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];
	
	for( int i = 0; i < N; ++i ) 
	{
		v[i] = (m_volMsk[i] == m_maskActiveId ) ? 255 : 0;
	}

	t_morpho3D_FillHole( v );

	for( int i = 0; i < N; ++i )
	{
		if( v[i] == 255 && !m_maskData[ m_volMsk[i] ].lock ) m_volMsk[i] = m_maskActiveId;
	}

	m_maskData[m_maskActiveId].surf.clear();
	m_maskData[m_maskActiveId].bRendSurf = false;
	m_volMsk.setUpdated();
}


void TexCore::ActvMsk_delete()
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;

	if (m_maskActiveId == 0)
	{
		AfxMessageBox("0th region (background) cannot be removed");
		return;
	}


	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];
	
	for( int i = 0; i < N; ++i ) 
	{
		if     ( m_volMsk[i] >  m_maskActiveId ) m_volMsk[i] -- ;
		else if( m_volMsk[i] == m_maskActiveId ) m_volMsk[i] = 0;
	}


	m_maskData.erase( m_maskData.begin() + m_maskActiveId );
	m_volMsk.setUpdated();
	m_maskActiveId = 0;
}



void TexCore::ActvMsk_margeTo()
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;

	if (m_maskActiveId == 0)
	{
		AfxMessageBox("0th region (background) cannot be marged");
		return;
	}


	int trgtMaskID;
	DlgSelectMskId dlg;
	if ( dlg.MyDoModal( trgtMaskID ) != IDOK) return;

	
	if (trgtMaskID == 0)
	{
		AfxMessageBox("0th region (background) cannot marged");
		return;
	}
	if (trgtMaskID == m_maskActiveId )
	{
		AfxMessageBox("Cannot marge to it self");
		return;
	}


	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];
	fprintf( stderr, "active %d, trgt, %d\n", m_maskActiveId, trgtMaskID);

	/*古い実装（バグ有り）
	
	// region, 0,1,2 があるときに
	act = 2, trgt=1とすると
	0 1 2 --> 0,2,1ってなるのでだめ
	0 1 2 --> 0 2 2 としてから --> 0 1 1とすればおｋ

	for( int i = 0; i < N; ++i ) 
	{
		if     ( m_volMsk[i] >  trgtMaskID ) m_volMsk[i] -= 1 ;
		else if( m_volMsk[i] == trgtMaskID ) m_volMsk[i] = m_maskActiveId;
	}

	m_maskData.erase( m_maskData.begin() + trgtMaskID );
	m_maskData[ m_maskActiveId ].surf.clear();
	m_maskData[ m_maskActiveId ].bRendSurf = false;
	m_volMsk.setUpdated();
	*/
	for( int i = 0; i < N; ++i ) if( m_volMsk[i] == trgtMaskID ) m_volMsk[i] = m_maskActiveId;

	for( int i = 0; i < N; ++i ) if( m_volMsk[i] >  trgtMaskID ) --m_volMsk[i];

	if( m_maskActiveId > trgtMaskID ) --m_maskActiveId;

	m_maskData.erase( m_maskData.begin() + trgtMaskID );

	m_maskData[m_maskActiveId].surf.clear();
	m_maskData[m_maskActiveId].bRendSurf = false;
	m_volMsk.setUpdated();
}





void TexCore::ActvMsk_ExportObj(const char *fname)
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;

	
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

	byte *v = new byte[N];
	for( int i=0; i < N; ++i ) v[i] = ( m_volMsk[i] == m_maskActiveId ) ? 255 : 0;

	TMesh mesh;
	t_MarchingCubes<byte>(m_Reso, m_Pitch, v,128, 0, 0, mesh);
	mesh.smoothing(2);
	mesh.exportObjNoTexCd(fname);

	delete[] v;
}


void TexCore::ActvMsk_ExportStl(const char *fname)
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;

	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

	byte *v = new byte[N];
	for( int i=0; i < N; ++i ) v[i] = ( m_volMsk[i] == m_maskActiveId ) ? 255 : 0;

	TMesh mesh;
	t_MarchingCubes<byte>(m_Reso, m_Pitch, v, 128, 0, 0, mesh);
	mesh.smoothing(2);
	mesh.exportStl(fname);

	delete[] v;
}



void TexCore::ActvMsk_ExportBmp(const char *fname)
{
	if( m_maskActiveId < 0 || m_maskData.size() <= m_maskActiveId ) return;

	const int W = m_Reso[0];
	const int H = m_Reso[1];
	const int D = m_Reso[2];

	CString str;

	for( int z = 0; z < D; ++z)
	{
		if(      z < 10    ) str.Format( "%s000%d.bmp", fname, z);
		else if( z < 100   ) str.Format( "%s00%d.bmp" , fname, z);
		else if( z < 1000  ) str.Format( "%s0%d.bmp"  , fname, z);
		else if( z < 10000 ) str.Format( "%s%d.bmp"   , fname, z);

		OGLImage2D4 img;
		img.Allocate(W,H);

		for( int y = 0; y < H; ++y)
		for( int x = 0; x < W; ++x)
		{
			const int i = 4 * (x + W * y);
			const int I = x + y*W + z * W*H;
			img[ i + 0 ] = img[ i + 1 ] = img[ i + 2 ] = img[ i + 3 ] = (m_volMsk[I] == m_maskActiveId) ? 255 : 0;
		}
		img.SaveAs( str, 0);
	}

}









void TexCore::ActvMsk_fillPsuedoHoleAsNewRegion()
{
	DlgSetIntValue dlg;
	int size;
	if( IDOK != dlg.MyDoModal( "select open hole size[pixel]", 1, 20, size) ) return; 

	fprintf( stderr, "ActvMsk_fillPsuedoHoleAsNewRegion size:%d\n", size);

	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2]; 
	
	
	OglImage3D flg;
	flg.Allocate(m_Reso[0], m_Reso[1], m_Reso[2]);
	for( int i= 0; i < N; ++i) flg[i] = (m_volMsk[i] == m_maskActiveId) ? 255 : 1;

	//dilation (size)
	OglImage3D flgDlt(flg);
	for( int i = 0; i < size; ++i) t_morpho3D_dilate( flgDlt );

	//fill hole
	OglImage3D flgFill( flgDlt );
	for( int i= 0; i < N; ++i) flgFill[i] = (flgFill[i] == 255) ? 255 : 0;
	t_morpho3D_FillHole( flgFill );
	
	//extract ony hole 
	for( int i= 0; i < N; ++i) flgFill[i] = (flg[i] == 255) ? 0 : 
		                                    (flgFill[i] == 255 && flgDlt[i] != 255) ? 255 : 1;


	//opening opearation
	 t_morpho3D_erode ( flgFill );
	 t_morpho3D_dilate( flgFill );

	//dilation the hole 
	for( int i = 0; i < size; ++i) t_morpho3D_dilate( flgFill );


	// store as new region
	for (int i = 0; i < N; ++i) 
	{
		m_volFlg[i] = (!m_maskData[ m_volMsk[i] ].lock && flgFill[i] == 255) ? 255 : 0;
	}

	mask_storeCurrentForeGround();
}






void TexCore::loadMask(const char *fname)
{
	FILE* fp = fopen(fname, "rb");
	
	//save mask image
	int version, W,H,D;
	fread(&version, sizeof(int), 1, fp);
	fread(&W      , sizeof(int), 1, fp);
	fread(&H      , sizeof(int), 1, fp);
	fread(&D      , sizeof(int), 1, fp);

	if(W != m_Reso[0] || H != m_Reso[1] || D != m_Reso[2] )
	{
		AfxMessageBox( "strange volume size\n");
		fclose( fp );
		return;
	}

	fread( &m_volMsk[0], sizeof(byte), W * H * D, fp);


	
	DlgSetStackOrder dlg;
	if (IDOK != dlg.DoModal()) m_volMsk.flipVolumeInZ();

	m_volMsk.setUpdated();

	int maskN;
	fread(&maskN, sizeof(int), 1, fp);

	m_maskData.clear();

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

		fprintf(stderr, "%d %s\n", nLen, name);

		m_maskData.push_back( MaskData(string(name), EVec3i(col[0],col[1],col[2]), alpha, 0, lock?true:false) );

		delete[] name; 
	}
	fclose(fp);
}



void TexCore::saveMask(const char *fname)
{
	FILE* fp = fopen(fname, "wb");

	//save mask image
	int version = 0;
	fwrite(&version  , sizeof(int), 1, fp);
	fwrite(&m_Reso[0], sizeof(int), 1, fp);
	fwrite(&m_Reso[1], sizeof(int), 1, fp);
	fwrite(&m_Reso[2], sizeof(int), 1, fp);
	fwrite(&m_volMsk[0], sizeof(byte), m_Reso[0] * m_Reso[1] * m_Reso[2], fp);

	int maskN = (int)m_maskData.size();
	fwrite(&maskN, sizeof(int), 1, fp);

	for( const auto &it : m_maskData )
	{
		int iLock = it.lock;
		fwrite(&it.alpha      , sizeof(double), 1, fp);
		fwrite(it.color.data(), sizeof(int)   , 3, fp);
		fwrite(&iLock, sizeof(int), 1, fp);

		int nLen = (int)it.name.length();
		fwrite(&nLen          , sizeof(int ),  1    , fp);
		fwrite(it.name.c_str(), sizeof(char), nLen+1, fp);

		fprintf(stderr, "%d %s\n", nLen, it.name.c_str());
	}
	fclose(fp);
}





float TexCore::getVolVal(const EVec3f &pos)
{
	const EVec3f cuboid = getCuboidF();
	const EVec3f pitch  = getPitch();
	const EVec3i res    = getResolution();

	const int x = (int)(pos[0] / pitch[0]);
	const int y = (int)(pos[1] / pitch[1]);
	const int z = (int)(pos[2] / pitch[2]);

	if( x < 0 || y < 0 || z < 0 || res[0] <= x || res[1] <= y|| res[2] <= z) return 0;

	const int I = x + y*res[0] + z * res[0] * res[1];

	return m_volOrig[I];	
}


