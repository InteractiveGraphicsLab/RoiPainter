//理研在籍中、井尻がCG Gems JP用にOpenSourceとして公開したコード

/*------------------------------------------------------------------------------------
本ソフトウエアは NYSLライセンス (Version 0.9982) にて公開します

A. 本ソフトウェアは Everyone'sWare です。このソフトを手にした一人一人が、
   ご自分の作ったものを扱うのと同じように、自由に利用することが出来ます。

  A-1. フリーウェアです。作者からは使用料等を要求しません。
  A-2. 有料無料や媒体の如何を問わず、自由に転載・再配布できます。
  A-3. いかなる種類の 改変・他プログラムでの利用 を行っても構いません。
  A-4. 変更したものや部分的に使用したものは、あなたのものになります。
       公開する場合は、あなたの名前の下で行って下さい。

B. このソフトを利用することによって生じた損害等について、作者は
   責任を負わないものとします。各自の責任においてご利用下さい。

C. 著作者人格権は 井尻敬(理化学研究所)に帰属します。著作権は放棄します。

D. 以上の３項は、ソース・実行バイナリの双方に適用されます。
----------------------------------------------------------------------------------*/


//----Boykov and Kolmogorov algorithm (dual search tree)----------------------------
//
//残余グラフ上に，次の2種の探索木を構築し，Souce --> Sink パス効果的に検索する
//始点をルートとする探索木 「S」　　
//終点をルートとする探索木 「T」　
// ※残余グラフのnodeは, 探索木S, 探索木T, 木に含まれない のいずれかの状態を持つ. フラグ　f_inST　で管理)
//
//アルゴリズム: 探索木 S T を最初に一度構築し，その後 1-4を繰り返す
// 1. GROWTH       : 探索木 S T の成長させ増加可能経路を探索
// 2. AUGMENTATION : 増加可能経路にフローを流す
// 3. ADOPTION_S   : 探索木Sのedgeが切れ，木構造が壊れている可能性があるので修復する adoptionは養子縁組の意味
// 4. ADOPTION_T   : 探索木Tのedgeが切れ，木構造が壊れている可能性があるので修復する 
//
// 成長予定の探索木頂点 を持つキュー を linked listで実装 
//『 TFlowNetwork_BK4::m_actHead 』キュー
//『 TFlowNetwork_BK4::m_actTail 』キュー
//『 TFlowNetwork_BK4::m_actHead 』キュー
//----------------------------------------------------------------------------------





#pragma once

#include "tqueue.h"


//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


//この実装は、グラフカット法の作るフローネットワークに特化しているため、通常のフローネットワークには使えない

//Node & Edge data structure//
class BK4_Edge;
class BK4_Node;


class BK4_Node
{
public:
	BK4_Edge  *e_first ;//全ての辺を巡る際の 最初の辺
	BK4_Edge  *e_parent;//探索木の親edgeへのポインタ．0なら親なし(ルートか探索木のノードでない) 
	int        flgST   ;//0:探索木のノードでない   1:探索木Sのノード  -1:探索木Tのノード  

	BK4_Node  *n_actF;//active node 次へのポインタ(FIFOキューをここで実装)
	BK4_Node  *n_actB;//active node 前へのポインタ(FIFOキューをここで実装)

	int ts  ;//time stamp
	int dist;//dist from source

	BK4_Node()
	{
		e_first   = e_parent = 0;
		flgST     = 0           ;
		n_actF    = n_actB   = 0;
		ts = dist = 0;
	}
};


class BK4_Edge
{
public:
	double   capa; // 容量
	BK4_Node *n_from; // 辺の元の 頂点 
	BK4_Node *n_to  ; // 辺の先の 頂点 
	BK4_Edge *e_rev ; // 逆並行辺
	BK4_Edge *e_next; // 頂点から出る全ての辺を巡る際の 次の辺 (nullなら終わり)

	BK4_Edge(){
		n_from = n_to  = 0;
		e_next = e_rev = 0;
		capa = 0;
	}
};


 



class TFlowNetwork_BK4
{
	const int m_nNum     ;//頂点の数     (初期化時に決定)
	const int m_eNumMax  ;//最大の辺の数 (初期化時に決定) 
	int       m_eNum     ;//実際の辺の数 (addEdgeを呼ぶたび2本増える)

	BK4_Edge *m_e        ;//辺  の配列
	BK4_Node *m_n        ;//頂点の配列

	BK4_Node *m_actHead  ;//active nodeのキューのhead
	BK4_Node *m_actTail  ;//active nodeのキューのtail

	int m_time ;
	double m_trimed_tLink_capa;
public:
	~TFlowNetwork_BK4()
	{
		delete[] m_e;
		delete[] m_n;
	}

	TFlowNetwork_BK4( const int nodeNum, const int edgeNumMax, const int SourceID, const int SinkID) : m_nNum( nodeNum ), m_eNumMax( edgeNumMax )
	{
		m_eNum = 0;
		m_e   = new BK4_Edge[ m_eNumMax  ];
		m_n   = new BK4_Node[ m_nNum     ];
		m_actHead = m_actTail = 0;
		m_trimed_tLink_capa = 0;

		//ここで探索木を初期化 
		m_n[ SourceID ].flgST =  1;   
		m_n[ SinkID   ].flgST = -1;   

 	}

	//////////////////////////////////////////////////////////////////////////////////
	//辺の追加 :: 辺(from,to).容量=capa1 と　辺(to,from).容量=capa2  の2辺が追加される
	void addEdge(const int &from, const int &to, const double &capa1, const double &capa2)
	{
		BK4_Node *nF = &m_n[ from   ], *nT   = &m_n[ to ];
		BK4_Edge *e  = &m_e[ m_eNum ], *eRev = &m_e[ m_eNum + 1 ];

		//↓辺(from,to)            //↓辺(to, from)
		e->n_from = nF         ;   eRev->n_from = nT    ;
		e->n_to   = nT         ;   eRev->n_to   = nF    ;
		e->e_rev  = eRev       ;   eRev->e_rev  = e     ;
		e->capa   = capa1      ;   eRev->capa   = capa2 ;

		e->e_next = nF->e_first;   eRev->e_next = nT->e_first; // 頂点から出る全ての辺検索のための 辺ポインタの付け替え
		nF->e_first = e        ;   nT->e_first  = eRev;
		
		m_eNum += 2;//辺の数更新
	}

	void add_nLink( const int &pIdx, const int &qIdx, const double &capa)
	{
		addEdge( pIdx, qIdx, capa, capa );
	}

	//本文中の t-linkのトリミング参照、さらにこのタイミングで探索木を成長させる
	void add_tLink( const int &sIdx, const int &tIdx, const int &pIdx, const double &capaSP, const double &capaPT)
	{
		if     ( capaSP > capaPT ) 
		{ 
			addEdge(sIdx, pIdx, capaSP-capaPT, 0); 
			m_trimed_tLink_capa += capaPT;

			m_n[ pIdx].flgST    = 1;
			m_n[ pIdx].e_parent = &m_e[ m_eNum -2];
			actNode_push( &m_n[ pIdx] );

		}
		else if( capaSP < capaPT ) { 
			addEdge(pIdx, tIdx, capaPT-capaSP, 0); 
			m_trimed_tLink_capa += capaSP; 

			m_n[ pIdx ].flgST    = -1;
			m_n[ pIdx ].e_parent = &m_e[ m_eNum -2];
			actNode_push( &m_n[ pIdx] );
		}
		else m_trimed_tLink_capa += capaSP;            
	}
	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////////////
	////////////////////active 頂点 の fifo queue/////////////////////////////////////
	inline bool actNode_empty(){ return m_actHead == 0; }
	inline void actNode_push(BK4_Node *n)
	{
		if( n->n_actB || n->n_actF ) return;
		if( m_actHead == 0 ){
			m_actHead = m_actTail = n;
		}
		else
		{
			m_actTail->n_actB = n;
			n->n_actF = m_actTail;
			m_actTail = n;
		}
	}
	inline void actNode_pop()
	{
		//m_actHead must not be 0
		BK4_Node *top = m_actHead;
		m_actHead = m_actHead->n_actB; top->n_actB = 0;
		
		if( m_actHead != 0 ) m_actHead->n_actF = 0;
		//m_actHead == 0ならqueueが空になった
	}
	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	


/*-----------------------------------------------------------------------------------------
// Boykov and Kolmogorov algorithm (dual search tree)									　//
-----------------------------------------------------------------------------------------*/
	double calcMaxFlow(const int FROM, const int TO, byte *minCut)
	{
		const BK4_Node *N_FROM = &m_n[FROM];
		const BK4_Node *N_TO   = &m_n[ TO ];
		m_time = 0;

		double maxFlow = 0;
		TQueue< BK4_Node* > orphanNodes_S(80,40), orphanNodes_T(80,40);
		TQueue< BK4_Edge* > aug_pathS    (80,40), aug_pathT    (80,40);


		//0...探索木は初期化済み 

		BK4_Edge *pathE, *eS, *eT;
		while( true )
		{
			//1...GROWTH 探索木SとTが接触する辺 pathEを返す-------------------
			pathE = GROWTH( );
			if( pathE == 0 ) break;
		
			//2-1 AUGMENTATION 増加可能な最大容量検索-------------------------
			double pathMaxFlow = pathE->capa;

			aug_pathS.clear();
			aug_pathT.clear();

			eS = pathE->n_from->e_parent; //木Sをさかのぼる
			while( 1 ) 
			{
				if( eS->capa < pathMaxFlow) pathMaxFlow = eS->capa; 
				if( eS->n_from == N_FROM ) break;
				aug_pathS.push_back (eS);
				eS = eS->n_from->e_parent;
			}

			eT = pathE->n_to  ->e_parent; //木Tをすすめる
			while( 1 ) 
			{
				if( eT->capa < pathMaxFlow) pathMaxFlow = eT->capa; 
				if( eT->n_to == N_TO ) break;
				aug_pathT.push_back (eT);
				eT = eT->n_to  ->e_parent;
			}

			//2-2 AUGMENTATION フロー追加-------------------------------------
			pathE->capa         -=  pathMaxFlow;
			pathE->e_rev->capa  +=  pathMaxFlow;

			//経路の最後 辺 (s,p) (p,t) の容量を変化 (逆並行辺は無視)
			eS->capa        -=  pathMaxFlow; 
			if( eS->capa == 0 ){ orphanNodes_S.push_back( eS->n_to   ); eS->n_to  ->e_parent = 0; }

			eT->capa        -=  pathMaxFlow; 
			if( eT->capa == 0 ){ orphanNodes_T.push_back( eT->n_from ); eT->n_from->e_parent = 0; } 

			while( aug_pathS.hasElement() )
			{
				eS = aug_pathS.front(); aug_pathS.pop_front();
				eS->capa         -=  pathMaxFlow;
				eS->e_rev->capa  +=  pathMaxFlow;
				if( eS->capa == 0 ){ orphanNodes_S.push_front( eS->n_to   ); eS->n_to  ->e_parent = 0; }
			}
			while( aug_pathT.hasElement() )
			{
				eT = aug_pathT.front(); aug_pathT.pop_front();
				eT->capa         -=  pathMaxFlow;
				eT->e_rev->capa  +=  pathMaxFlow;
				if( eT->capa == 0 ){ orphanNodes_T.push_front( eT->n_from ); eT->n_from->e_parent = 0; } 
			}

			//3 ADOPTION-----------
			ADOPTION_S( N_FROM, orphanNodes_S ); 
			ADOPTION_T( N_TO  , orphanNodes_T );
			
			maxFlow += pathMaxFlow;
			m_time++;
		}
		for( int i=0; i<m_nNum; ++i) minCut[i] = m_n[i].flgST > 0 ? 1 : 0;

		return maxFlow + m_trimed_tLink_capa;
	}


private:

	//探索木SとTが接触する辺を返す (もっと高速化するなら if(e->capa)の分岐をなくす工夫が必要
	BK4_Edge* GROWTH( )
	{
		BK4_Node *pivN;
		BK4_Edge *e   ;
		while( 1 )
		{
			if( actNode_empty() ) break;

			switch( m_actHead->flgST )//m_actHead←これが queue A のfront要素
			{
			case 1 :
				e = m_actHead->e_first;
				while( e ){
					if( e->capa )
					{
						pivN = e->n_to;
						switch( pivN->flgST )
						{
						case 0:
							pivN->flgST    = 1  ;
							pivN->e_parent = e  ;
							actNode_push( pivN );
							break;
						case -1:
							return e;
						}
					}
					e = e->e_next;
				}
				break;
			case -1:
				e = m_actHead->e_first;
				while( e ){
					if( e->e_rev->capa )
					{
						pivN = e->n_to;
						switch( pivN->flgST )
						{
						case 0:
							pivN->flgST    =  -1     ;
							pivN->e_parent = e->e_rev;
							actNode_push( pivN )     ;
							break;
						case 1:
							return e->e_rev;
						}
					}
					e = e->e_next;
				}
				break;
			}
			actNode_pop();//すべての子ノードを調べ終わってからpopする
		}
		return 0; // not found!!	
	}


	//tree S adoption
	void ADOPTION_S( const BK4_Node *FROM, TQueue<BK4_Node*> &orphanNodes)
	{
		BK4_Node *v, *orphN ;
		BK4_Edge *e, *parent_candidate;

		while( orphanNodes.hasElement() )
		{
			orphN = orphanNodes.front(); orphanNodes.pop_front();

			int minDist = INT_MAX;
			parent_candidate = 0 ;

			for( e = orphN->e_first; e ; e = e->e_next ) if( e->e_rev->capa && e->n_to->e_parent != e )
			{
				v = e->n_to;
				int d = isValidParent_S( FROM, v);
				if( d )
				{
					v->ts   = m_time;
					v->dist = d     ;
					if( d < minDist ) 
					{
						parent_candidate = e->e_rev;
						minDist = d ;
					}
				}
			}
			if( parent_candidate != 0 ){
				orphN->e_parent = parent_candidate;
				continue; //親発見!
			}

			//親いない
			orphN->flgST = 0; //orphNを木から削除
			for( e = orphN->e_first; e ; e = e->e_next) 
			{
				v = e->n_to;
				if( v->e_parent == e ){ orphanNodes.push_back( v ); v->e_parent = 0; }//orphNのすべての子を孤立させる
				if( v->flgST > 0 && e->e_rev->capa ) actNode_push( v );   
			}
		}
	}

	void ADOPTION_T( const BK4_Node *TO, TQueue<BK4_Node*> &orphanNodes)
	{
		BK4_Node *v, *orphN;
		BK4_Edge *e, *parent_candidate;

		while( orphanNodes.hasElement() )
		{
			orphN = orphanNodes.front(); orphanNodes.pop_front();

			int minDist = INT_MAX;
			parent_candidate = 0;

			for( e = orphN->e_first;  e ;  e = e->e_next ) if( e->capa && e->n_to->e_parent != e->e_rev )
			{
				v = e->n_to;
				int d = isValidParent_T( TO, v );
				if( d ){
					v->ts   = m_time;
					v->dist = d     ;

					if( d < minDist ) 
					{
						parent_candidate = e;
						minDist = d;
					}
				}
			}
			if( parent_candidate != 0 ) {
				orphN->e_parent = parent_candidate;
				continue; //親発見!
			}

			orphN->flgST = 0; //orphNを木から削除し
			for( BK4_Edge *e = orphN->e_first; e ; e = e->e_next) 
			{
				v = e->n_to;
				if( v->e_parent && v->e_parent->n_to == orphN) { orphanNodes.push_back( v ); v->e_parent = 0; } //orphNのすべての子を孤立させる
				if( v->flgST < 0 && e->capa ) actNode_push( v ); 
			}
		}
	}
	//根へのパスがあれば、根までの距離+1　無ければ0を返す
	inline int isValidParent_S( const BK4_Node *FROM, const BK4_Node *n)
	{
		if( n->flgST  != 1       ) return 0; //探索木Sのノードでなければ 0
		//if( n         == FROM  ) return 1; //探索木Sのルートなら       1
		
		BK4_Node *v;
		int d = 2;
		for( BK4_Edge *e = n->e_parent; e; )
		{
			v = e->n_from;
			if( v->ts == m_time ) return d + v->dist - 1;
			if( v     == FROM   ) return d;
			e = v->e_parent;
			++d;
		}
		return 0;
	}

	inline int isValidParent_T( const BK4_Node *TO, const BK4_Node *n)
	{
		if( n->flgST != -1   ) return 0; //探索木Tのノードでなければ 0 
		//if( n        == TO ) return 1; //探索木Tのルートなら       1
		BK4_Node *v;

		int d = 2; 
		for( BK4_Edge *e = n->e_parent; e; )
		{
			v = e->n_to;
			if( v->ts == m_time ) return d + v->dist - 1;
			if( v     == TO     ) return d;
			e = v->e_parent;
			++d;
		}
		return 0;
	}



public:
	static void test()
	{

/*------------------------------------------------
		
		source:0

		2  3  4     bright: 2,3,4,5,6,  dark:7,8,9,10 
pixels  5  6  7     2:fixed as fore
		8  9 10    10:fixex as back

		sink  :1 
		
--------------------------------------------------*/

		fprintf( stderr, "BK algorithm  start! \n");


		const int N         = 9 + 2;
		const int sourceIdx = 0;
		const int sinkIdx   = 1;
		TFlowNetwork_BK4 network( N, 100, 0, 1 );

		network.add_tLink( sourceIdx, sinkIdx,  2, 10000.0, 0.0    ); 
		network.add_tLink( sourceIdx, sinkIdx,  3,    10.0, 1.0    ); 
		network.add_tLink( sourceIdx, sinkIdx,  4,    10.0, 1.0    ); 
		network.add_tLink( sourceIdx, sinkIdx,  5,    10.0, 1.0    ); 
		network.add_tLink( sourceIdx, sinkIdx,  6,    10.0, 1.0    ); 
		network.add_tLink( sourceIdx, sinkIdx,  7,     1.0, 10.0   ); 
		network.add_tLink( sourceIdx, sinkIdx,  8,     1.0, 10.0   ); 
		network.add_tLink( sourceIdx, sinkIdx,  9,     1.0, 10.0   ); 
		network.add_tLink( sourceIdx, sinkIdx, 10,     0.0, 10000.0); 



		network.add_nLink( 2,  3,  10.0); 
		network.add_nLink( 2,  5,  10.0);
		network.add_nLink( 3,  4,  10.0); 
		network.add_nLink( 3,  6,  10.0);
		network.add_nLink( 4,  7, 1.0);
		network.add_nLink( 5,  6,  10.0); 
		network.add_nLink( 5,  8, 1.0); 
		network.add_nLink( 6,  7, 1.0); 
		network.add_nLink( 6,  9, 1.0); 
		network.add_nLink( 7, 10,  10.0); 
		network.add_nLink( 8,  9,  10.0); 
		network.add_nLink( 9, 10,  10.0); 


		byte *minCut = new byte[ N ];
		double flow  = network.calcMaxFlow( sourceIdx, sinkIdx, minCut );



		fprintf( stderr, "BK algorithm  flow = %f minCut:\n", flow );
		for( int i=0; i<N; ++i) fprintf( stderr, "%d-%d\n", i, minCut[i] );
		fprintf( stderr, "\n" );
		delete[] minCut;
	}

};







