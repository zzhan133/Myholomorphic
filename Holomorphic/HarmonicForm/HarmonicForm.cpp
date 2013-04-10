#include "HarmonicForm.h"

using namespace MeshLib;

static double _cot( CPoint & p0, CPoint & p1, CPoint & p2)
{
	double a = (p0 - p2).norm();
	double b = (p1 - p2).norm();
	double c = (p0 - p1).norm();
	double cos_ = (a * a + b * b - c * c) / (2 * a * b);
	double sin_ = 1 - cos_ * cos_;
	if (sin_ < 0)
		sin_ = 0;
	else if (sin_ > 1.0)
		sin_ = 1.0;
	sin_ = sqrt(sin_);

	//assert(_finite(cos_ / sin_));

	return cos_ / sin_;
}


CHarmonicForm::CHarmonicForm( CMesh * pMesh, CMesh * pWMesh ):m_boundary( pWMesh )
{
  //read "sharp" edge attributes
  m_pMesh  = pMesh;
  for( MeshEdgeIterator eiter( m_pMesh ); !eiter.end(); eiter ++ )
  {
      CEdge * e = *eiter;
      CEdgeTrait * pT = (CEdgeTrait *) e->trait();
      pT->read();
  }

  //read "father" vertex attributes
  m_pWMesh = pWMesh;
  for( MeshVertexIterator viter( m_pWMesh ); !viter.end(); viter ++ )
  {
      CVertex * pV = *viter;
      CVertexTrait * pCT = (CVertexTrait *) pV->trait();
      pCT->string() = pV->string();
      pCT->read();
  }


};

CHarmonicForm::~CHarmonicForm()
{
};

void CHarmonicForm::_closed_1_form()
{
  for( MeshVertexIterator viter( m_pWMesh ); !viter.end(); viter ++ )
  {
      CVertex * pV = *viter;
      v_u( pV ) = 0;
  }
  
  std::list<CLoop*> & loops = m_boundary.loops();
  assert( loops.size() == 2 );
  
  for( std::list<CLoop*>::iterator liter = loops.begin(); liter != loops.end(); liter ++ )
  {
    CLoop * pL = *liter;
    for( std::list<CHalfEdge*>::iterator hiter = pL->halfedges().begin(); hiter != pL->halfedges().end(); hiter ++ )
    {
      CHalfEdge * pH = *hiter;
      CVertex      * pV = pH->target();
      v_u( pV ) = 1.0;
    }
    break;
  }

  for( MeshEdgeIterator eiter( m_pWMesh ); !eiter.end(); eiter ++ )
  {
    CEdge * we = *eiter;
    CVertex * w1 = m_pWMesh->edgeVertex1( we );
    CVertex * w2 = m_pWMesh->edgeVertex2( we );
    e_du( we ) = v_u( w2 ) - v_u( w1 );

    int id1 =  v_father( w1 );
    int id2 =  v_father( w2 );

    CVertex * v1 = m_pMesh->idVertex( id1 );
    CVertex * v2 = m_pMesh->idVertex( id2 );

    CEdge * e = m_pMesh->vertexEdge( v1, v2 );

    if( m_pMesh->edgeVertex1( we ) == v1 )
    {
        e_du( e ) = e_du( we );
    }
    else
    {
        e_du( e ) = -e_du( we );
    }
  }
}

void CHarmonicForm::_edge_weight()
{

  for( MeshEdgeIterator eiter( m_pMesh ); !eiter.end();  eiter ++ )
  {
      CEdge * e = *eiter;
      CHalfEdge * he = e->halfedge(0);

      CVertex * pv = he->source();
			CVertex * tv =  he->target();
			CVertex * nv = he->he_next()->target();
				
			double cot0 = _cot(pv->point(), tv->point(), nv->point() );

			CHalfEdge * sh = he->he_sym();
      assert( sh != NULL );
			nv = sh->he_next()->target();
			double cot1 = _cot(tv->point(), pv->point(), nv->point() );

			double wt = cot0 + cot1;

      e_w( e ) = wt;
  }
}

void CHarmonicForm::_harmonic_1_form()
{
    _edge_weight();
    
    int vid = 0;

    for( MeshVertexIterator viter( m_pMesh ); !viter.end(); viter ++ )
    {
        CVertex * v = *viter;
        v_idx( v ) = vid ++;
    }


   int num = m_pMesh->numVertices();

	CSparseMatrix * pMatrix = new CSparseMatrix(num, num, num * 7);
	assert( pMatrix );
	double* b = new double[num];
	assert( b );
  memset(b, 0, sizeof(double) * num);
	
	double* x = new double[num];
	assert( x );
	memset(x, 0, sizeof(double) * num);

  for( MeshEdgeIterator eiter( m_pMesh ); !eiter.end(); eiter ++ )
  {
      CEdge * e = *eiter;
      CVertex * v1 = m_pMesh->edgeVertex1( e );
      CVertex * v2 = m_pMesh->edgeVertex2( e );

      int id1 = v_idx( v1 );
      int id2 = v_idx( v2 );

      double w = e_w( e );
		
      pMatrix->AddElementTail( id1,  id2,  -w );
      pMatrix->AddElementTail( id2,  id1,  -w );

  }

  for( MeshVertexIterator viter( m_pMesh ); !viter.end(); viter ++ )
  {
      CVertex * v = *viter;
      int  id = v_idx( v );

      double sum_w = 0;
      double sum_b  = 0;

      for( VertexOutHalfedgeIterator vhiter( m_pMesh, v ); !vhiter.end();  ++vhiter  )
      {
          CHalfEdge * he = *vhiter;
          assert( he->source() == v );
          CEdge * e = he->edge();
          sum_w += e_w( e );
          
          if( m_pMesh->edgeVertex1( e ) != v )
          {
              sum_b += -( e_w( e ) * e_du( e ) );
          }
          else
          {
            sum_b += e_w( e )  * e_du( e );
          }
          

      }
      pMatrix->AddElementTail( id,  id,  sum_w );
      b[id] = sum_b;
      printf("%f ",  sum_b );
  }

  int itrs = num/4;
  pMatrix->CGSolver2Matlab(b, x, 1e-8, itrs);

for( MeshVertexIterator viter( m_pMesh ); !viter.end(); viter ++ )
  {
      CVertex * v = *viter;
      int  id = v_idx( v );
      v_u( v ) = x[id];
      printf("%f ", x[id]);
  }

for( MeshEdgeIterator eiter( m_pMesh ); !eiter.end(); eiter ++ )
  {
      CEdge * e = *eiter;
      CVertex * v1 = m_pMesh->edgeVertex1( e );
      CVertex * v2 = m_pMesh->edgeVertex2( e );
      e_du( e ) += v_u( v2 ) - v_u( v1 );

	  char line[1024];
	  sprintf(line,"w=(%f)",e_du(e) );
	  e->string()=line;
  }

}

void CHarmonicForm::_integrate()
{
  for( MeshEdgeIterator eiter( m_pWMesh ); !eiter.end(); eiter ++ )
  {
      CEdge * we = *eiter;

      CVertex * w1 = m_pWMesh->edgeVertex1( we );
      CVertex * w2 = m_pWMesh->edgeVertex2( we );

      int id1 = v_father( w1 );
      int id2 = v_father( w2 );

      CVertex * v1 = m_pMesh->idVertex( id1 );
      CVertex * v2 = m_pMesh->idVertex( id2 );

      CEdge * e = m_pMesh->vertexEdge( v1, v2 );

      if( m_pMesh->edgeVertex1( e ) == v1 )
      {
          e_du( we ) = e_du( e );
      }
      else
      {  
        e_du( we ) = - e_du( e );
      }
  }

  CVertex * head = NULL;
  for( MeshVertexIterator viter( m_pWMesh ); !viter.end(); viter ++ )
  {
      CVertex * v = *viter;
      v_touched( v ) = false;
      head = v;
  }

  std::queue<CVertex*> vqueue;
  v_touched( head ) = true;
  vqueue.push( head );
  v_uv( head ) = CPoint2(0,0.25);

  while( !vqueue.empty() )
  {
    CVertex * head = vqueue.front();
    vqueue.pop();

    for( VertexEdgeIterator veiter(  head ); !veiter.end(); veiter ++ )
    {
      CEdge * e = *veiter;
      double du = e_du( e );

      CVertex * v1 = m_pWMesh->edgeVertex1( e );
      CVertex * v2 = m_pWMesh->edgeVertex2( e );

      if( v1 == head )
      {
        if( v_touched(v2) ) continue;
        v_touched( v2 ) = true;
        vqueue.push( v2 );
        v_uv( v2 ) = v_uv( v1 ) + CPoint2( du,0 );
      }
      else
      {
        assert( v2 == head );
        if( v_touched( v1 ) ) continue;
        v_touched( v1 ) = true;
        vqueue.push( v1 );
        v_uv( v1 ) = v_uv( v2 ) - CPoint2( du,0 );
      }
    }
  }
  
  double umin = 1e+10;
  double umax = -1e+10;

 for( MeshVertexIterator viter( m_pWMesh );  !viter.end(); viter ++ )
  {
      CVertex* v = *viter;
  	  CPoint2 p = v_uv( v );
      umax = (umax > p[0] )? umax: p[0];
      umin = (umin < p[0] )? umin: p[0];
  }

 for( MeshVertexIterator viter( m_pWMesh );  !viter.end(); viter ++ )
  {
      CVertex* v = *viter;
  	  CPoint2 p = v_uv( v );
      p[0] = ( p[0] - umin )/(umax-umin);
      v_uv( v ) = p;
  }

  for( MeshVertexIterator viter( m_pWMesh );  !viter.end(); viter ++ )
  {
      CVertex* v = *viter;
      char uv_line[1024];
		  CPoint2 p = v_uv( v );

      if( v->string().size() > 0 )
      {
  		  sprintf(uv_line,"uv=(%g %g)", p[0], p[1] ); 
      }
      else
      {
  		  sprintf(uv_line,"uv=(%g %g)", p[0], p[1] ); 
      }

		  v->string() = uv_line;
  }
}

void CHarmonicForm::calculate_harmonic_form()
{
    _closed_1_form();
    _harmonic_1_form();
    _integrate();
}
