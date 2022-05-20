#pragma once

#include <iostream>
#include <vector>
#include <list>


//////////////////////////////////////////////////////////////////////////////////
//このコードは井尻が理研在籍中に、CGGemsJP用にOpen Sourceとして公開したコードに基づく
//
//Implemented by TakashiIjiri @ Riken based on 
//"The water shed transform definitions algorithms and parallelization strategies"
//Jos B.T.M et al
//algorithm 4.1
//
//////////////////////////////////////////////////////////////////////////////////

#define TWS_WSHED       0
#define TWS_INIT       -1
#define TWS_MASK       -2
#define TWS_FICTITIOUS -3

// gradient magnitude のレンジは [0, 1024] としておく
#define WSD_HMIN   0
#define WSD_HMAX 255


class TWsPixelEx
{
  unsigned short m_val   ;//pixel intensity 
  int            m_label ;//pixel label INIT
  int            m_dist  ;//mask時のbasin/watershed pixelからの距離

  TWsPixelEx    *m_neighborPtr[26];
  int            m_num_neighbors  ;
  //std::vector<TWsPixelEx*> m_neighbours;//reference to neighboring pixels

public:
	~TWsPixelEx(){}
  
  TWsPixelEx(){
    m_num_neighbors = 0;
		//m_value = 0 ;
		//m_dist  = 0;
		m_label = TWS_FICTITIOUS;
	}

	TWsPixelEx( const TWsPixelEx &p)
	{
		m_val   = p.m_val  ;//pixelの輝度値
		m_label = p.m_label;//pixelにつくラベル INIT
		m_dist  = p.m_dist ;//mask時のbasin/watershed pixelからの距離
    m_num_neighbors = p.m_num_neighbors;
		for(int i=0; i < m_num_neighbors; ++i) m_neighborPtr[i] = p.m_neighborPtr[i] ;
	}

	//setter//
	inline void Set(unsigned short val)
  {
		m_val   = std::min( (unsigned short)WSD_HMAX, std::max( val, (unsigned short)WSD_HMIN) );
		m_dist  = 0       ;
		m_label = TWS_INIT;
  }
	inline void setLabel(int label){ m_label = label;     }
	inline void setLabelToINIT()   { m_label = TWS_INIT ; }
	inline void setLabelToMASK()   { m_label = TWS_MASK ; }
	inline void setLabelToWSHED()  { m_label = TWS_WSHED; }
	inline void setDistance(int d) { m_dist  = d    ; }

	inline void addNeighbour(TWsPixelEx *n)
  {
    m_neighborPtr[m_num_neighbors] = n;
    ++m_num_neighbors;
  }

  //getter
  //   inline byte getValue   () const { return m_value      ; }
  inline unsigned short getIntValue() const { return (int) m_val; } 
  inline int            getLabel   () const { return m_label; }
  inline int            getDistance() const { return m_dist ; }

	inline bool isLabelINIT()  const {return m_label == TWS_INIT ;      }
	inline bool isLabelMASK()  const {return m_label == TWS_MASK ;      } 
	inline bool isLabelWSHED() const {return m_label == TWS_WSHED;      }
	inline bool isFICTITIOUS() const {return m_label == TWS_FICTITIOUS; }

  inline int         GetNumNeighbor(){ return m_num_neighbors; }
  inline TWsPixelEx* GetNeighbor(int idx){ return m_neighborPtr[idx];}
};




class TWsFIFO
{
	std::list<TWsPixelEx*> m_queue;
public:
    TWsFIFO(){}
    ~TWsFIFO(){}

    inline bool fifo_empty(             ) { return m_queue.empty();}
	inline void fifo_add  (TWsPixelEx *p) { m_queue.push_front(p); }

    inline TWsPixelEx *fifo_remove()
	{
		TWsPixelEx *r = m_queue.back();
		m_queue.pop_back();
		return r;
    }
};




/*
watershed algorithm
参照 1) Digital image processing
     2) "The watershed transform: definitions, algorithms and parallelization strategies"

実装は, 2)のalgorithm 4.1にあるもの．
ただし、36-52に間違いがあるっぽいので訂正した．

内容
preprocess (1 - 13)
  pixelの色に応じてソート
  pixelの近傍をセット

flooding process (15 - 70)
  
maskとfifoキュー初期化(17 - 24)
    値hを持つpixelにmaskフラグを立てる
    現在のbasinとwatershed pixelに接しているものをfifoキューに入れる
  
mask領域内のpixelのラベルを確定する(25 - 52)
    キューをうまいこと使うことで、既存のbasinからの離散距離に応じて，labeling領域を広げていく

local minima detection(53 - 68)
    local minimaを見つけて,各local minima内のpixelに一貫したlabelを打つ

*注意* flatな領域があるため、上のような大変なalgorithmになったっぽいですね。

25-52では、maskフラグが立った各ピクセルを,　既存のbasin pixelやwatershed　pixelからの距離順に
ラべリングしていく.

あるpixel p (dist = d) のすべての近傍 qを見たときに、qの状態としてあり得るものは
A) basin、watershed、maskフラグのいずれでもない  dist = 0       && label = INIT
B) basin内部のpixel                              dist = 0 / d-1 && label = LABEL 
C) watershed pixel                               dist = 0 / d-1 && label = WATERSHED
D) maskフラグ かつ dist = 0 まだキューに入っていない　　　　　　　 label = MASK
E) maskフラグ かつ dist = d pixel pと同列に扱われている            label = MASK                        

pixel pのすべての近傍 qをloopして、
　　1, 状態Dのqをキューに入れる　q.dist = d + 1とする
  　2, 状態Bのpixelあり、すべてのラベルがおんなじ --> p.label = LABEL
  　3, 状態Bのpixelあり、ラベルの種類が異なる     --> p.label = WATERSHED
    4, 状態Cのpixelしかない                       --> p.label = WATERSHED

	labelsは、water shedが0, それ以外のpixelには1から始まる連続のラベルが貼られる
*/

inline void runWatershedAlgorithmEx(int size, TWsPixelEx **sortedPixPtr, void (*progressFunc)(double) = 0 )
{
	//run watershed algorithm////////////////////////////////////////////////////
	int curlab       = 0;
	int heightIndex1 = 0;
	int heightIndex2 = 0;
	TWsFIFO queue;

	for(int h = WSD_HMIN; h <= WSD_HMAX; ++h) //hを持つ物すべてにmaskフラグを立て、既存basin/watershed pixelの隣接pixelをqueueに入れる
	{	
		for(int pIdx = heightIndex1 ; pIdx < size; ++pIdx) 
		{
			TWsPixelEx &p = *sortedPixPtr[pIdx];
			if( p.getIntValue() != h)
      { 
        heightIndex1 = pIdx; 
        break; 
      }

			p.setLabelToMASK();

      const int num_neighbors = p.GetNumNeighbor();
			for(int i=0 ; i< num_neighbors; ++i) if( p.GetNeighbor(i)->getLabel() >= 0 ) //basin or watershed
			{		    
				p.setDistance(1);
				queue.fifo_add(&p);
				break;
			}
		} 

		queue.fifo_add( new TWsPixelEx() );//add fictitious pixel
	  //今のキューの中身 -->  mark pix pix pix ... pix でmarkはfictitiousで、pixは、既存のbasin/watershedに隣接した領域

		int curdist = 1;
	  while(true) //extend basins 
		{
			TWsPixelEx *p = queue.fifo_remove();

			if(p->isFICTITIOUS())
			{
				delete p;
				if(queue.fifo_empty()) break;
				else{
					queue.fifo_add(new TWsPixelEx());//add fictitious pixel
					++curdist;
					p = queue.fifo_remove();
				}
			}

			//neighborsの状況によりpをラべリング * 以下に説明あり
			bool hasNeighboringWsPix = false;
      const int num_neighbors  = p->GetNumNeighbor();

			for(int i = 0; i < num_neighbors; ++i) 
			{
				TWsPixelEx &q = *p->GetNeighbor(i);

				if( q.isLabelMASK() && (q.getDistance() == 0) )
        {
					q.setDistance( curdist+1 );
					queue.fifo_add( &q       );
				}	    
				else if( (q.getDistance() <= curdist) && q.isLabelWSHED() )
        {
          hasNeighboringWsPix = true;
        }
				else if( (q.getDistance() <= curdist) && q.getLabel() > 0 ) 
				{ 
          //qは既存のbasinに入っている
					if     ( p->isLabelMASK()                  ) p->setLabel(q.getLabel());
					else if( p->getLabel()    != q.getLabel()  ) p->setLabelToWSHED();
				}
			}
			if( p->isLabelMASK() && hasNeighboringWsPix ) p->setLabelToWSHED();//ここが突出したwspixelを作っているができる
		}

	  //Detect and process new minima at level h 
	  for(int pIdx = heightIndex2 ; pIdx < size; pIdx++) 
		{
			TWsPixelEx &p = *sortedPixPtr[ pIdx ];
			if(p.getIntValue() != h) { heightIndex2 = pIdx; break; } // This pixel is at level h+1	

			p.setDistance( 0 ); // Reset distance to zero
			
			if(p.isLabelMASK()) { //the pixel is inside a new minimum
				curlab++;
				p.setLabel(curlab);		    
				queue.fifo_add(&p);
			    
			  //pから同じ値(MASKラベル)のpixelにfloodfill
				while(!queue.fifo_empty()) 
				{
					TWsPixelEx &q = *queue.fifo_remove();

          const int num_neighbors = q.GetNumNeighbor();

					for(int i=0 ; i < num_neighbors; ++i) if( q.GetNeighbor(i)->isLabelMASK() ) 
					{
						q.GetNeighbor(i)->setLabel(curlab);
						queue.fifo_add( q.GetNeighbor(i) );
					}
				}
			} 
	  } 
		if( h%30==0 && progressFunc != 0) progressFunc( h / (double)WSD_HMAX);
	} 
}


inline int wsd_cmpindexEx(const void *a, const void *b)
{
	const TWsPixelEx *pa = *(const TWsPixelEx **)a;
	const TWsPixelEx *pb = *(const TWsPixelEx **)b;
	if( pb->getIntValue() < pa->getIntValue() ) return  1;
	if( pb->getIntValue() > pa->getIntValue() ) return -1;
	return 0;
}



inline void TWatershed2DEx(int W, int H, const byte *img, int *pixel_labels)
{
	//construct pixels and sort it///////////////////////////////////////////////
	const int WH = W * H;
	TWsPixelEx *pixels        = new TWsPixelEx [WH];//
	TWsPixelEx **sortedPixPtr = new TWsPixelEx*[WH];//sortしたもの
	for(int i = 0; i < WH; ++i) sortedPixPtr[i] = &pixels[i];

	//輝度値とneighborsをセット
	int idx = 0;
	for(int y = 0; y < H; ++y) 
	for(int x = 0; x < W; ++x, ++idx) 
	{
		pixels[ idx ].Set( img[idx] );
		for( int dy = -1; dy <=1 ; ++dy )
		for( int dx = -1; dx <=1 ; ++dx ) if( dx != 0 || dy != 0 )
		{
			if( y + dy >= 0 && y + dy < H && x + dx >= 0 && x + dx < W )		
				pixels[idx].addNeighbour( &pixels[idx + dx + dy * W] );
		}	
	}
	qsort( sortedPixPtr, WH, sizeof(TWsPixelEx*), wsd_cmpindexEx );

	//run watershed algorithm////////////////////////////////////////////////////
	runWatershedAlgorithmEx(WH, sortedPixPtr );
	for(int pIdx = 0 ; pIdx < WH ; ++pIdx) pixel_labels[pIdx] = pixels[pIdx].getLabel();

	delete[] sortedPixPtr;
	delete[] pixels;
}



inline void TWatershed3DEx
( 
  int W,  int H,  int D, 
  const float* gMagImg, 
  const float gMagCoef, 
  std::vector<int> &labels, 
  void (*progressFunc)(double) = 0 
)
{
  time_t t0 = clock();

  const int WH  = W * H;
  const int WHD = W*H*D;

	//construct pixels and sort it///////////////////////////////////////////////
	TWsPixelEx *pixels        = new TWsPixelEx [WHD];//
	TWsPixelEx **sortedPixPtr = new TWsPixelEx*[WHD];//sortしたもの
	for(int i = 0; i < WHD; ++i) sortedPixPtr[i] = &pixels[i];

	// 9 + 8 + 9近傍
#pragma omp parallel for 
	for ( int z = 0; z < D; ++z)
  {
	  for ( int y = 0; y < H; ++y)
    {
	    for ( int x = 0; x < W; ++x)
      {
        const int idx = x + y * W + z * WH;
        pixels[ idx ].Set( (unsigned short)( gMagCoef * gMagImg[idx] ) );
        
        //add 26 近傍
        if ( x>0  &&  y>0 && z>0  ) pixels[idx].addNeighbour( &pixels[ idx-1-W-WH] );
        if (          y>0 && z>0  ) pixels[idx].addNeighbour( &pixels[ idx  -W-WH] );
        if ( x<W-1&&  y>0 && z>0  ) pixels[idx].addNeighbour( &pixels[ idx+1-W-WH] );
        if ( x>0          && z>0  ) pixels[idx].addNeighbour( &pixels[ idx-1  -WH] );
        if (                 z>0  ) pixels[idx].addNeighbour( &pixels[ idx    -WH] );
        if ( x<W-1        && z>0  ) pixels[idx].addNeighbour( &pixels[ idx+1  -WH] );
        if ( x>0  && y<H-1&& z>0  ) pixels[idx].addNeighbour( &pixels[ idx-1+W-WH] );
        if (         y<H-1&& z>0  ) pixels[idx].addNeighbour( &pixels[ idx  +W-WH] );
        if ( x<W-1&& y<H-1&& z>0  ) pixels[idx].addNeighbour( &pixels[ idx+1+W-WH] );

        if ( x>0  &&  y>0         ) pixels[idx].addNeighbour( &pixels[ idx-1-W   ] );
        if (          y>0         ) pixels[idx].addNeighbour( &pixels[ idx  -W   ] );
        if ( x<W-1&&  y>0         ) pixels[idx].addNeighbour( &pixels[ idx+1-W   ] );
        if ( x>0                  ) pixels[idx].addNeighbour( &pixels[ idx-1     ] );
        //if (                    ) pixels[idx].addNeighbour( &pixels[ idx       ] );
        if ( x<W-1                ) pixels[idx].addNeighbour( &pixels[ idx+1     ] );
        if ( x>0  && y<H-1        ) pixels[idx].addNeighbour( &pixels[ idx-1+W   ] );
        if (         y<H-1        ) pixels[idx].addNeighbour( &pixels[ idx  +W   ] );
        if ( x<W-1&& y<H-1        ) pixels[idx].addNeighbour( &pixels[ idx+1+W   ] );

        if ( x>0  &&  y>0 && z<D-1) pixels[idx].addNeighbour( &pixels[ idx-1-W+WH] );
        if (          y>0 && z<D-1) pixels[idx].addNeighbour( &pixels[ idx  -W+WH] );
        if ( x<W-1&&  y>0 && z<D-1) pixels[idx].addNeighbour( &pixels[ idx+1-W+WH] );
        if ( x>0          && z<D-1) pixels[idx].addNeighbour( &pixels[ idx-1  +WH] );
        if (                 z<D-1) pixels[idx].addNeighbour( &pixels[ idx    +WH] );
        if ( x<W-1        && z<D-1) pixels[idx].addNeighbour( &pixels[ idx+1  +WH] );
        if ( x>0  && y<H-1&& z<D-1) pixels[idx].addNeighbour( &pixels[ idx-1+W+WH] );
        if (         y<H-1&& z<D-1) pixels[idx].addNeighbour( &pixels[ idx  +W+WH] );
        if ( x<W-1&& y<H-1&& z<D-1) pixels[idx].addNeighbour( &pixels[ idx+1+W+WH] );
	    }
    }
  }

  time_t t1 = clock();

	qsort( sortedPixPtr, WHD, sizeof(TWsPixelEx*), wsd_cmpindexEx );
  
  time_t t2 = clock();

	//run watershed algorithm////////////////////////////////////////////////////
	runWatershedAlgorithmEx( WHD, sortedPixPtr, progressFunc );
	
  time_t t3 = clock();

  labels.resize( WHD);
	for(int pIdx = 0 ; pIdx < WHD ; ++pIdx)  labels[pIdx] = pixels[pIdx].getLabel();

  time_t t4 = clock();

  std::cout << "TIME WSD  " 
    << (t1-t0)/(float)CLOCKS_PER_SEC << " " 
    << (t2-t1)/(float)CLOCKS_PER_SEC << " " 
    << (t3-t2)/(float)CLOCKS_PER_SEC << " " 
    << (t4-t3)/(float)CLOCKS_PER_SEC << "\n"; 

	delete[] sortedPixPtr;
	delete[] pixels; 
}








//以下の関数は井尻@立命が追加
//20160826 


// computer watershed segmentation
// labelは, 
void t_wsd_CalcLabelFromGMag
( 
	const int W, 
	const int H, 
	const int D, 
	const float* gMagVol, 
	const float  volCoef, 

	      int* labels //should be allocated
)
{
	if( W * H > 1024*1024*4 ) 
	{ 
		std::cout << "データサイズが大きすぎます\n" ; 
		return;
	}

	//calc layer size 
  const int stepZ = 2 * 1024 * 1024 / ( W * H) ;
	const int iterN = ( D % stepZ == 0 ) ? D / stepZ  :  D / stepZ + 1;
	std::cout <<"watershed (" 
            << W << " " << H << " " << D << " " 
            << "), stepZ:" << stepZ << " iterN: " << iterN << "\n";

	std::vector< std::vector<int> > tmpLabels( iterN, std::vector<int>() );
	

	//calc water shed for each layers (メモリに乗らない場合を考慮して複数レイヤに分割して処理)
	for( int i = 0; i < iterN; ++i)
	{
    std::cout << "wsd " << i << "/" << iterN << "\n";
		int z0 = i * stepZ;
		int tmpD = ( z0 + stepZ <= D ) ? stepZ : D - z0;
		TWatershed3DEx( W, H, tmpD, &gMagVol[ W * H * z0 ], volCoef, tmpLabels[i], 0 );
	}


	//assemble  multiple label ID lists  into one
	int labelOffset = 0;
	int idxOffset   = 0;

	for( const auto &tmpLbl : tmpLabels )
	{
		int maxLabelVal = 0;
		for( int i = 0, s = (int) tmpLbl.size(); i < s; ++i) 
		{
			labels[ i + idxOffset ] = ( tmpLbl[i] == 0 ) ? 0 : tmpLbl[i] + labelOffset;
			maxLabelVal = std::max( maxLabelVal, tmpLbl[i] );
		}
		labelOffset += maxLabelVal;
		idxOffset   += (int) tmpLbl.size();
	}
}






// watershedの境界ピクセル（labels[i] == 0)を近傍の領域にマージする
//なるべく色の近い領域にマージする
void t_wsd_CollapseWsdPixels3D(
	const int W, 
	const int H, 
	const int D, 
	const short* vol, 
	
  int* labels)
{
	const int WH = W*H, WHD = W*H*D;

	byte *visFlg = new byte[WHD];

	while( true )
	{
		memset( visFlg, 0, sizeof(byte) * WHD);

		int idx = 0;
		for( int z = 0; z < D; ++z        )	
		for( int y = 0; y < H; ++y        )	
		for( int x = 0; x < W; ++x, ++idx ) if( labels[ idx ] <= 0 )
		{	
			//左、右、上、下
			float colDiff = FLT_MAX;

			for( int c = 0; c<6; ++c)
			{
				int neiIdx = 0;
				if     ( c == 0 && x >   0 ) neiIdx = idx - 1 ;//左
				else if( c == 1 && x < W-1 ) neiIdx = idx + 1 ;//右
				else if( c == 2 && y >   0 ) neiIdx = idx - W ;//下
				else if( c == 3 && y < H-1 ) neiIdx = idx + W ;//上
				else if( c == 4 && z >   0 ) neiIdx = idx - WH;
				else if( c == 5 && z < D-1 ) neiIdx = idx + WH;
				else continue;

				if( labels[ neiIdx ] <= 0 || visFlg[ neiIdx ] != 0) continue;
				
				float d = (vol[idx] - vol[neiIdx])*(vol[idx] - (float)vol[neiIdx]);

				if( d < colDiff )
				{ 
					colDiff = d; 
					labels[idx] = labels[ neiIdx ];
					visFlg[idx] = 1;
				}
			}
		}

		bool boundExist = false;
		for( int i=0; i < WHD; ++i) if( labels[i] <= 0) 
		{
			boundExist = true;
			break;
		}
		if( !boundExist ) break;

		//std::cout << "t_wsd_CollapseWsdPixels3D loop end \n";
	}

	delete[] visFlg;
}




//　要素数が１の領域 (labelID) をwatershed境界領域にする（高効率化のため）
// labelの振りなおしの必要が生じる
void t_wsd_RemoveOneVoxWsdLabel( const int num_voxels, int *labels)
{
	//max label 
	int label_max = 0;
  for( int i=0; i < num_voxels; ++i) 
  {
    label_max = std::max( label_max, labels[i]);
  }

  //各領域のvoxel数をカウント
  int *label_num_voxel = new int[label_max + 1];
  memset( label_num_voxel, 0, sizeof(int) * (label_max + 1) );

  for ( int i=0; i < num_voxels; ++i) 
  {
    ++label_num_voxel[ labels[i] ];
  }

	//1 voxelしか含まない領域を 境界要素へ
	for ( int i=0; i < num_voxels; ++i) if( label_num_voxel[ labels[i] ] == 1 )
	{
		label_num_voxel[ labels[i] ] = -1;
		labels[i] = 0;
	}

	//labelを連続に振り直す
	int offset = 0;
	int *new_label = new int[label_max + 1];

	for( int i = 0; i < label_max + 1; ++i)
	{
		if( label_num_voxel[i] == -1 ) offset++;
		else new_label[i] = i - offset;
	}

  for ( int i=0; i < num_voxels; ++i)  labels[i] = new_label[ labels[i] ];


  delete[] label_num_voxel;
  delete[] new_label;
}


