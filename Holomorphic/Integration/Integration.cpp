#include "Integration.h"

using namespace MeshLib;

CIntegration::CIntegration( CMesh * pForm, CMesh * pDomain )
{
	m_pForm = pForm;
	m_pDomain = pDomain;
}

CIntegration::~CIntegration()
{
}

void CIntegration::_integrate()
{
	for( MeshEdgeIterator eiter( m_pForm ); !eiter.end(); eiter ++ )
	{
		CEdge * e = *eiter;
		Ine_string( e ) = e->string();
		CInEdgeTrait * pET = (CInEdgeTrait*) e->trait();
		pET->read();
	}

	CVertex * head = NULL;

	for( MeshVertexIterator viter( m_pDomain ); !viter.end(); viter ++ )
	{
		CVertex * v = *viter;
		Inv_touched( v ) = false;
		Inv_string( v ) = v->string();
		CInVertexTrait * pVT = (CInVertexTrait*) v->trait();
		pVT->read();
		head = v;
	}

	std::queue<CVertex*> vqueue;
	Inv_uv( head ) = CPoint2(0,0);
	Inv_touched( head )  = true;

	vqueue.push( head );

	while( !vqueue.empty() )
	{
		head = vqueue.front();
		vqueue.pop();

		for( VertexEdgeIterator veiter( head ); !veiter.end(); veiter ++ )
		{
			CEdge * e = *veiter;
			CVertex * v1 = m_pDomain->edgeVertex1( e );
			CVertex * v2 = m_pDomain->edgeVertex2( e );


			CVertex * tail = ( head != v1 )?v1:v2;
			if( Inv_touched( tail ) ) continue;
			Inv_touched( tail ) = true;
			vqueue.push( tail );

			int id1 = Inv_father( v1 );
			int id2 = Inv_father( v2 );

			CVertex * w1 = m_pForm->idVertex( id1 );
			CVertex * w2 = m_pForm->idVertex( id2 );

			CEdge * we = m_pForm->vertexEdge( w1, w2 );

			if( m_pForm->edgeVertex1( we ) == w1 )
			{
				Ine_duv( e ) = Ine_duv( we );
			}
			else
			{
				Ine_duv( e ) = CPoint2(0,0)-Ine_duv( we );
			}

			if( tail == v2 )
			{
				Inv_uv( tail )  = Inv_uv( head ) + Ine_duv( e );
			}
			else
			{
				Inv_uv( tail )  = Inv_uv( head ) - Ine_duv( e );
			}
		}
	}

	for( MeshVertexIterator viter( m_pDomain ); !viter.end(); ++viter )
	{
		CVertex * v = *viter;
		CPoint2 uv = Inv_uv( v );
		char line[1024];
		sprintf(line,"uv=(%g %g)", uv[0], uv[1]);
		v->string() = line;
	}
}