#ifndef TDCMTK_H
#define TDCMTK_H

#pragma unmanaged

// DCMTK

#ifndef _DEBUG
#include "./include/dcmtk/dcmdata/dctk.h"

#pragma comment(lib, "dcmdata.lib")
#pragma comment(lib, "oflog.lib")
#pragma comment(lib, "ofstd.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "ws2_32.lib"  )
#pragma comment(lib, "wsock32.lib" )
class Tdcmtk
{
  DcmFileFormat* m_FileFormat;
  DcmDataset*	   m_dataSet;

  int m_W, m_H, m_fNum   ; // note fnum represent the number of slices

  int m_bitsStored;
  int m_bitsAlloc ;
  int m_highBit   ;
  int m_smplPerPix; //1:gray 3 rgb
  int m_plnConfig ;//0:normal (rgba rgba), > 1 (rrrr... gggg... bbb...)
  int m_pixRepres ;
  double m_RescaleIntercept;
  double m_RescaleSlope    ;


  double m_pitch[2]; //pixel pitch (x,y)
  double m_pos[3]  ; //slice pos (x,y,z)

  double m_slice_thickness;

public:
  Tdcmtk( const char *fname );
  bool isOpened(){ return m_dataSet != 0; }

  void getSize  ( int &W, int &H, int &fNum );


  // if failed return -1
  double getPitchX ( ){ return m_pitch[0]; }
  double getPitchY ( ){ return m_pitch[1]; }
  double getZPos   ( ){ return m_pos  [2]; }
  double getSliceThickness(){ return m_slice_thickness; }

  //void getFormat( int &chNum, int &bitNum, int &bSigned );


	//data should be allocated
	//蜿りドttp://radroom.web.fc2.com/dicom/image.html
	template<class T> 
	bool getPixelsAs(T* data)
	{
		if (!m_dataSet)
		{
			fprintf(stderr, "fails to open the file");
			return false;
		}

		if (m_bitsAlloc == 16 && m_pixRepres) 
		{
			//fprintf(stderr, "READ PIXELS 16bit - Signded! findAndGetSint16Array\n"); 
			const Sint16 *v = 0;
			unsigned long psize = 0;
			if ( m_dataSet->findAndGetUint16Array( DCM_PixelData, (const Uint16*&) v, &psize,false ).bad() ) return false;

			for (int i = 0, s = m_W * m_H; i < s; ++i) data[i] = (T)( m_RescaleSlope * v[i] + m_RescaleIntercept);
			delete[] v;
		}
		else if (m_bitsAlloc == 16 && !m_pixRepres)
		{
			//fprintf(stderr, "READ PIXELS 16bit - UnSigned!  findAndGetUint16Array\n"); 
			const Uint16 *v = 0;
			unsigned long psize = 0;
			if ( m_dataSet->findAndGetUint16Array( DCM_PixelData, (const Uint16*&) v, &psize,false ).bad() ) return false;

			for (int i = 0, s = m_W * m_H; i < s; ++i) data[i] = (T)( m_RescaleSlope * v[i] + m_RescaleIntercept);
			delete[] v;
		}
		else
		{
			fprintf(stderr, "this file type is not implemeted yet\n");
			return false;
		}

		return true;
	}

	
	//data should be allocated
	template<class T>
	bool getPixels3DAs(T* data)
	{
		if (!m_dataSet)
		{
			fprintf(stderr, "fails to open the file");
			return false;
		}

		if (m_bitsAlloc == 16 && m_pixRepres) 
		{
			fprintf(stderr, "READ PIXELS 16bit - Signded! findAndGetSint16Array \n"); 
			const Sint16 *v = 0;
			unsigned long psize = 0;
			if ( m_dataSet->findAndGetUint16Array( DCM_PixelData, (const Uint16*&) v, &psize,false ).bad() ) return false;

      const int num = m_W * m_H * m_fNum;
#pragma omp parallel for 
			for (int i = 0; i < num; ++i)  data[i] = (T)( m_RescaleSlope * v[i] + m_RescaleIntercept);
			delete[] v;
		}
		else if (m_bitsAlloc == 16 && !m_pixRepres)
		{
			fprintf(stderr, "READ PIXELS 16bit - Signded! findAndGetUint16Array\n"); 
			const Uint16 *v = 0;
			unsigned long psize = 0;
			if ( m_dataSet->findAndGetUint16Array( DCM_PixelData, (const Uint16*&) v, &psize,false ).bad() ) return false;

      const int num = m_W * m_H * m_fNum;
#pragma omp parallel for 
			for (int i = 0; i < num; ++i)  data[i] = (T)( m_RescaleSlope * v[i] + m_RescaleIntercept);
			delete[] v;
		}
		else
		{
			fprintf(stderr, "this file type is not implemeted yet\n");
			return false;
		}
		return true;
	}





private:

};

#else

class Tdcmtk
{
public:
	Tdcmtk( const char *fname ){}
    bool isOpened(){ return false; }
	void getSize  ( int &W, int &H, int &fNum ){}
    double getPitchX ( ){ return 0; }
    double getPitchY ( ){ return 0; }
    double getZPos   ( ){ return 0; }
    double getSliceThickness(){ return 0; }

	void getFormat( int &chNum, int &bitNum, int &bSigned ){}
	bool getPixelsToFlt(float* data){return false;}
	bool getPixelsToFlt3D(float* data){return false;}

	template<class T>
	bool getPixelsAs(T* data) { return false; }
	template<class T>
	bool getPixels3DAs(T* data) { return false; }
};


#endif



/*

class Tdcmtk3D
{
    DcmFileFormat* m_FileFormat;
    DcmDataset*	   m_dataSet;

	//reso
    int m_W, m_H, m_fNum;
    
	//format
	int m_nBits ;
    int m_bSined;
    int m_bChNum    ;//1:gray 3 rgb
    int m_bChArrange;//0:normal (rgba rgba), > 1 (rrrr... gggg... bbb...)

	//pitch
    double m_pitch[3]; //pixel pitch (x,y, z)

public:
    Tdcmtk3D( const char *fname );
    bool isOpened(){ return m_dataSet != 0; }

    void getSize  ( int &W, int &H, int &fNum );


    // if failed return -1
    double getPitchX ( ){ return m_pitch[0]; }
    double getPitchY ( ){ return m_pitch[1]; }
    double getZPos   ( ){ return m_pos  [2]; }

    void getFormat( int &chNum, int &bitNum, int &bSigned );


	//data should be allocated
	bool getPixelsToFlt(float* data);



private:
    //DcmElement* getTagElem(DcmTagKey tagKey);

	//data should not be allocated
	bool getPixels_SInt16(const Sint16* &data);
	bool getPixels_UInt16(const Uint16* &data);

};

*/


#pragma managed


#endif // TDCMTK_H
