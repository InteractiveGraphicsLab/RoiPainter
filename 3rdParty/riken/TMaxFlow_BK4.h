//�����ݐВ��A��K��CG Gems JP�p��OpenSource�Ƃ��Č��J�����R�[�h

/*------------------------------------------------------------------------------------
�{�\�t�g�E�G�A�� NYSL���C�Z���X (Version 0.9982) �ɂČ��J���܂�

A. �{�\�t�g�E�F�A�� Everyone'sWare �ł��B���̃\�t�g����ɂ�����l��l���A
   �������̍�������̂������̂Ɠ����悤�ɁA���R�ɗ��p���邱�Ƃ��o���܂��B

  A-1. �t���[�E�F�A�ł��B��҂���͎g�p������v�����܂���B
  A-2. �L��������}�̂̔@�����킸�A���R�ɓ]�ځE�Ĕz�z�ł��܂��B
  A-3. �����Ȃ��ނ� ���ρE���v���O�����ł̗��p ���s���Ă��\���܂���B
  A-4. �ύX�������̂╔���I�Ɏg�p�������̂́A���Ȃ��̂��̂ɂȂ�܂��B
       ���J����ꍇ�́A���Ȃ��̖��O�̉��ōs���ĉ������B

B. ���̃\�t�g�𗘗p���邱�Ƃɂ���Đ��������Q���ɂ��āA��҂�
   �ӔC�𕉂�Ȃ����̂Ƃ��܂��B�e���̐ӔC�ɂ����Ă����p�������B

C. ����Ґl�i���� ��K�h(�����w������)�ɋA�����܂��B���쌠�͕������܂��B

D. �ȏ�̂R���́A�\�[�X�E���s�o�C�i���̑o���ɓK�p����܂��B
----------------------------------------------------------------------------------*/


//----Boykov and Kolmogorov algorithm (dual search tree)----------------------------
//
//�c�]�O���t��ɁC����2��̒T���؂��\�z���CSouce --> Sink �p�X���ʓI�Ɍ�������
//�n�_�����[�g�Ƃ���T���� �uS�v�@�@
//�I�_�����[�g�Ƃ���T���� �uT�v�@
// ���c�]�O���t��node��, �T����S, �T����T, �؂Ɋ܂܂�Ȃ� �̂����ꂩ�̏�Ԃ�����. �t���O�@f_inST�@�ŊǗ�)
//
//�A���S���Y��: �T���� S T ���ŏ��Ɉ�x�\�z���C���̌� 1-4���J��Ԃ�
// 1. GROWTH       : �T���� S T �̐������������\�o�H��T��
// 2. AUGMENTATION : �����\�o�H�Ƀt���[�𗬂�
// 3. ADOPTION_S   : �T����S��edge���؂�C�؍\�������Ă���\��������̂ŏC������ adoption�͗{�q���g�̈Ӗ�
// 4. ADOPTION_T   : �T����T��edge���؂�C�؍\�������Ă���\��������̂ŏC������ 
//
// �����\��̒T���ؒ��_ �����L���[ �� linked list�Ŏ��� 
//�w TFlowNetwork_BK4::m_actHead �x�L���[
//�w TFlowNetwork_BK4::m_actTail �x�L���[
//�w TFlowNetwork_BK4::m_actHead �x�L���[
//----------------------------------------------------------------------------------





#pragma once

#include "tqueue.h"


//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


//���̎����́A�O���t�J�b�g�@�̍��t���[�l�b�g���[�N�ɓ������Ă��邽�߁A�ʏ�̃t���[�l�b�g���[�N�ɂ͎g���Ȃ�

//Node & Edge data structure//
class BK4_Edge;
class BK4_Node;


class BK4_Node
{
public:
	BK4_Edge  *e_first ;//�S�Ă̕ӂ�����ۂ� �ŏ��̕�
	BK4_Edge  *e_parent;//�T���؂̐eedge�ւ̃|�C���^�D0�Ȃ�e�Ȃ�(���[�g���T���؂̃m�[�h�łȂ�) 
	int        flgST   ;//0:�T���؂̃m�[�h�łȂ�   1:�T����S�̃m�[�h  -1:�T����T�̃m�[�h  

	BK4_Node  *n_actF;//active node ���ւ̃|�C���^(FIFO�L���[�������Ŏ���)
	BK4_Node  *n_actB;//active node �O�ւ̃|�C���^(FIFO�L���[�������Ŏ���)

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
	double   capa; // �e��
	BK4_Node *n_from; // �ӂ̌��� ���_ 
	BK4_Node *n_to  ; // �ӂ̐�� ���_ 
	BK4_Edge *e_rev ; // �t���s��
	BK4_Edge *e_next; // ���_����o��S�Ă̕ӂ�����ۂ� ���̕� (null�Ȃ�I���)

	BK4_Edge(){
		n_from = n_to  = 0;
		e_next = e_rev = 0;
		capa = 0;
	}
};


 



class TFlowNetwork_BK4
{
	const int m_nNum     ;//���_�̐�     (���������Ɍ���)
	const int m_eNumMax  ;//�ő�̕ӂ̐� (���������Ɍ���) 
	int       m_eNum     ;//���ۂ̕ӂ̐� (addEdge���ĂԂ���2�{������)

	BK4_Edge *m_e        ;//��  �̔z��
	BK4_Node *m_n        ;//���_�̔z��

	BK4_Node *m_actHead  ;//active node�̃L���[��head
	BK4_Node *m_actTail  ;//active node�̃L���[��tail

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

		//�����ŒT���؂������� 
		m_n[ SourceID ].flgST =  1;   
		m_n[ SinkID   ].flgST = -1;   

 	}

	//////////////////////////////////////////////////////////////////////////////////
	//�ӂ̒ǉ� :: ��(from,to).�e��=capa1 �Ɓ@��(to,from).�e��=capa2  ��2�ӂ��ǉ������
	void addEdge(const int &from, const int &to, const double &capa1, const double &capa2)
	{
		BK4_Node *nF = &m_n[ from   ], *nT   = &m_n[ to ];
		BK4_Edge *e  = &m_e[ m_eNum ], *eRev = &m_e[ m_eNum + 1 ];

		//����(from,to)            //����(to, from)
		e->n_from = nF         ;   eRev->n_from = nT    ;
		e->n_to   = nT         ;   eRev->n_to   = nF    ;
		e->e_rev  = eRev       ;   eRev->e_rev  = e     ;
		e->capa   = capa1      ;   eRev->capa   = capa2 ;

		e->e_next = nF->e_first;   eRev->e_next = nT->e_first; // ���_����o��S�Ă̕ӌ����̂��߂� �Ӄ|�C���^�̕t���ւ�
		nF->e_first = e        ;   nT->e_first  = eRev;
		
		m_eNum += 2;//�ӂ̐��X�V
	}

	void add_nLink( const int &pIdx, const int &qIdx, const double &capa)
	{
		addEdge( pIdx, qIdx, capa, capa );
	}

	//�{������ t-link�̃g���~���O�Q�ƁA����ɂ��̃^�C�~���O�ŒT���؂𐬒�������
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
	////////////////////active ���_ �� fifo queue/////////////////////////////////////
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
		//m_actHead == 0�Ȃ�queue����ɂȂ���
	}
	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	


/*-----------------------------------------------------------------------------------------
// Boykov and Kolmogorov algorithm (dual search tree)									�@//
-----------------------------------------------------------------------------------------*/
	double calcMaxFlow(const int FROM, const int TO, byte *minCut)
	{
		const BK4_Node *N_FROM = &m_n[FROM];
		const BK4_Node *N_TO   = &m_n[ TO ];
		m_time = 0;

		double maxFlow = 0;
		TQueue< BK4_Node* > orphanNodes_S(80,40), orphanNodes_T(80,40);
		TQueue< BK4_Edge* > aug_pathS    (80,40), aug_pathT    (80,40);


		//0...�T���؂͏������ς� 

		BK4_Edge *pathE, *eS, *eT;
		while( true )
		{
			//1...GROWTH �T����S��T���ڐG����� pathE��Ԃ�-------------------
			pathE = GROWTH( );
			if( pathE == 0 ) break;
		
			//2-1 AUGMENTATION �����\�ȍő�e�ʌ���-------------------------
			double pathMaxFlow = pathE->capa;

			aug_pathS.clear();
			aug_pathT.clear();

			eS = pathE->n_from->e_parent; //��S�������̂ڂ�
			while( 1 ) 
			{
				if( eS->capa < pathMaxFlow) pathMaxFlow = eS->capa; 
				if( eS->n_from == N_FROM ) break;
				aug_pathS.push_back (eS);
				eS = eS->n_from->e_parent;
			}

			eT = pathE->n_to  ->e_parent; //��T�������߂�
			while( 1 ) 
			{
				if( eT->capa < pathMaxFlow) pathMaxFlow = eT->capa; 
				if( eT->n_to == N_TO ) break;
				aug_pathT.push_back (eT);
				eT = eT->n_to  ->e_parent;
			}

			//2-2 AUGMENTATION �t���[�ǉ�-------------------------------------
			pathE->capa         -=  pathMaxFlow;
			pathE->e_rev->capa  +=  pathMaxFlow;

			//�o�H�̍Ō� �� (s,p) (p,t) �̗e�ʂ�ω� (�t���s�ӂ͖���)
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

	//�T����S��T���ڐG����ӂ�Ԃ� (�����ƍ���������Ȃ� if(e->capa)�̕�����Ȃ����H�v���K�v
	BK4_Edge* GROWTH( )
	{
		BK4_Node *pivN;
		BK4_Edge *e   ;
		while( 1 )
		{
			if( actNode_empty() ) break;

			switch( m_actHead->flgST )//m_actHead�����ꂪ queue A ��front�v�f
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
			actNode_pop();//���ׂĂ̎q�m�[�h�𒲂׏I����Ă���pop����
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
				continue; //�e����!
			}

			//�e���Ȃ�
			orphN->flgST = 0; //orphN��؂���폜
			for( e = orphN->e_first; e ; e = e->e_next) 
			{
				v = e->n_to;
				if( v->e_parent == e ){ orphanNodes.push_back( v ); v->e_parent = 0; }//orphN�̂��ׂĂ̎q���Ǘ�������
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
				continue; //�e����!
			}

			orphN->flgST = 0; //orphN��؂���폜��
			for( BK4_Edge *e = orphN->e_first; e ; e = e->e_next) 
			{
				v = e->n_to;
				if( v->e_parent && v->e_parent->n_to == orphN) { orphanNodes.push_back( v ); v->e_parent = 0; } //orphN�̂��ׂĂ̎q���Ǘ�������
				if( v->flgST < 0 && e->capa ) actNode_push( v ); 
			}
		}
	}
	//���ւ̃p�X������΁A���܂ł̋���+1�@�������0��Ԃ�
	inline int isValidParent_S( const BK4_Node *FROM, const BK4_Node *n)
	{
		if( n->flgST  != 1       ) return 0; //�T����S�̃m�[�h�łȂ���� 0
		//if( n         == FROM  ) return 1; //�T����S�̃��[�g�Ȃ�       1
		
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
		if( n->flgST != -1   ) return 0; //�T����T�̃m�[�h�łȂ���� 0 
		//if( n        == TO ) return 1; //�T����T�̃��[�g�Ȃ�       1
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







