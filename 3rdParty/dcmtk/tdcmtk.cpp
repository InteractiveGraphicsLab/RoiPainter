#include "./tdcmtk.h"

#ifndef _DEBUG

//ref: http://wiki.nakaguchi.org/index.php?DCMTK

extern void t_info  (const char* fmt, ...);
extern void t_debug (const char* fmt, ...);


bool t_getTagValInt(DcmDataset *DataSet, DcmTagKey tagKey, int &value)
{
    DcmElement *elem = NULL;
    DcmTag tag(tagKey);

    if ( DataSet->findAndGetElement(tag, elem).bad() || elem->getLength() == 0) return false;

    Sint16 rvS16;
    Sint32 rvS32;
    Uint8  rvU8 ;
    Uint16 rvU16;
    Uint32 rvU32;
    if      ( elem->getSint16( rvS16 ).good() ) { value = (int)rvS16; }
    else if ( elem->getSint32( rvS32 ).good() ) { value = (int)rvS32; }
    else if ( elem->getUint8 ( rvU8  ).good() ) { value = (int)rvU8 ; }
    else if ( elem->getUint16( rvU16 ).good() ) { value = (int)rvU16; }
    else if ( elem->getUint32( rvU32 ).good() ) { value = (int)rvU32; }
    else {
        t_info("\n%s is not integer value\n", tag.getTagName());
        return false;
    }
    return true;
}






double getValueInsideKakko(const std::string s)
{
//以下のようなstringにおいて、[ ]内の、1つめの数字を返す。（2つ目の数字は無視）
//例1
//        (0018,0050) DS [0.5]                                    #   4, 1 SliceThickness
//例2
//        (0028,0030) DS [0.468\0.468]                            #  12, 2 PixelSpacing
  const int startPos  = (int) s.find("[") +1;
  const int kakkoToji = (int) s.find("]");
  if( startPos == std::string::npos || kakkoToji == std::string::npos ) return -1.0;
  const int endPos = (s.find("\\") == std::string::npos)? kakkoToji : (int) s.find("\\");
  const std::string strVal = s.substr(startPos, endPos-startPos);
  return std::stof(strVal);
}



Tdcmtk::Tdcmtk( const char *fname )
{    
  m_FileFormat = new DcmFileFormat;
  OFCondition status = m_FileFormat->loadFile(fname);

  if ( status.bad() )
  {
    t_info( "cannot open the file!!!");
    if ( m_FileFormat )
    {
      delete m_FileFormat;
      m_FileFormat = 0;
    }
    return;
  }

  m_dataSet = m_FileFormat->getDataset();

  m_W = m_H = m_fNum = 0;

  //size
  t_getTagValInt(m_dataSet, DCM_Columns            , m_W    );
  t_getTagValInt(m_dataSet, DCM_Rows               , m_H    );
  t_getTagValInt(m_dataSet, DCM_NumberOfFrames     , m_fNum );

  //pixel configureation
  t_getTagValInt(m_dataSet, DCM_BitsStored           , m_bitsStored ); // bits for 1 pixel (actually used)
  t_getTagValInt(m_dataSet, DCM_BitsAllocated        , m_bitsAlloc  ); // bits for 1 pixel (allocated)
  t_getTagValInt(m_dataSet, DCM_HighBit              , m_highBit    );
  t_getTagValInt(m_dataSet, DCM_SamplesPerPixel      , m_smplPerPix ); // 1:gray, 3:rgb
  t_getTagValInt(m_dataSet, DCM_PlanarConfiguration  , m_plnConfig  );
  t_getTagValInt(m_dataSet, DCM_PixelRepresentation  , m_pixRepres  ); //unsigned (0) or 2nd complement (1)
  //t_getTagValInt(m_dataSet, DCM_LossyImageCompression, cm  ); //unsigned (0) or signed (1)

  if( m_dataSet->findAndGetFloat64( DCM_RescaleIntercept, m_RescaleIntercept, 0 ).bad() ) m_RescaleIntercept = 0;
  if( m_dataSet->findAndGetFloat64( DCM_RescaleSlope    , m_RescaleSlope    , 0 ).bad() ) m_RescaleSlope     = 1;

  if( m_dataSet->findAndGetFloat64( DCM_PixelSpacing        , m_pitch[0], 0 ).bad() ) m_pitch[0] = -1;
  if( m_dataSet->findAndGetFloat64( DCM_PixelSpacing        , m_pitch[1], 1 ).bad() ) m_pitch[1] = -1;
  if( m_dataSet->findAndGetFloat64( DCM_ImagePositionPatient, m_pos[0]  , 0 ).bad() ) m_pos  [0] = -1;
  if( m_dataSet->findAndGetFloat64( DCM_ImagePositionPatient, m_pos[1]  , 1 ).bad() ) m_pos  [1] = -1;
  if( m_dataSet->findAndGetFloat64( DCM_ImagePositionPatient, m_pos[2]  , 2 ).bad() ) m_pos  [2] = -1;


  //double ori1, ori2, ori3;
  //m_dataSet->findAndGetFloat64( DCM_ImageOrientationPatient, ori1, 0 ) ;
  //m_dataSet->findAndGetFloat64( DCM_ImageOrientationPatient, ori2, 1 ) ;
  //m_dataSet->findAndGetFloat64( DCM_ImageOrientationPatient, ori3, 2 ) ;
  //std::cout << "orientation" << ori1 << " " << ori2 << " " << ori3 << "\n";


  std::stringstream ss;
  m_dataSet->print(ss);
  //std::ofstream outputfile("C:\\Users\\xxxx\\Desktop\\txt2.txt");
  while(!ss.eof()){
    std::string tmpLine;
    std::getline(ss, tmpLine);
    if( tmpLine.find("(0018,0050)") != std::string::npos )//SliceThickness
    {
      m_slice_thickness = getValueInsideKakko ( tmpLine );
    }
    else if( tmpLine.find("(0028,0030)") != std::string::npos )//PixelSpacing
    {
      m_pitch[0] = m_pitch[1] = getValueInsideKakko ( tmpLine );
    }
    //outputfile << tmpLine << std::endl;
  }
  //outputfile.close();

	/*
	t_info( "DCM_BitsStored %d\n"         , m_bitsStored);
	t_info( "DCM_BitsAllocated %d\n"      , m_bitsAlloc);
	t_info( "DCM_HighBit %d\n"            , m_highBit);
	t_info( "DCM_SamplesPerPixel %d\n"    , m_smplPerPix);
	t_info( "DCM_PlanarConfiguration %d\n", m_plnConfig);
	t_info( "DCM_PixelRepresentation %d\n", m_pixRepres);
	*/
	//t_info( "DCM_Rescale Intercept %f Slope %f\n", m_RescaleIntercept, m_RescaleSlope);
}

void Tdcmtk::getSize  ( int &W, int &H, int &fNum )
{
    W    = m_W   ;
    H    = m_H   ;
    fNum = m_fNum;
}

/*
void Tdcmtk::getFormat( int &chNum, int &bitNum, int &bSigned  )
{
    chNum   = m_bChNum;
    bitNum  = m_nBits ;
    bSigned = m_bSined;
}
*/









#endif