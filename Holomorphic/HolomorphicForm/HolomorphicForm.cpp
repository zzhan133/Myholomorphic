#include "HolomorphicForm.h"
#include "../LinearAlgebra/SparseMatrix.h"

using namespace MeshLib;


//-----------------------------------------------------------------------------

double CWedgeOperator::wedge_product()
{

	double p = 0;
	for( MeshFaceIterator fiter( m_pMesh[0] ); !fiter.end(); ++ fiter )
	{
		CFace * f0 = *fiter;
		CFace * f1 = m_pMesh[1]->idFace( f0->id() );
		p += ( f_normal(f0)*( f_du(f0)^f_du(f1)) ) * f_area(f0);
	}
	return p;
};



double CWedgeOperator::wedge_star_product()
{

	double p = 0;
	for( MeshFaceIterator fiter( m_pMesh[0] ); !fiter.end(); ++ fiter )
	{
		CFace * f0 = *fiter;
		CFace * f1 = m_pMesh[1]->idFace( f0->id() );
		CPoint du = f_du(f1);
		CPoint n  = f_normal(f1);
		du = n^du;

		p += ( f_normal(f0)*( f_du(f0)^du) ) * f_area( f0 );
	}
	return p;
};

CWedgeOperator::CWedgeOperator(CMesh * mesh0, CMesh * mesh1)
{
	m_pMesh[0] = mesh0;
	m_pMesh[1] = mesh1;
};

CWedgeOperator::~CWedgeOperator()
{

};


void CHolomorphicForm::convert( CFace * f )
{
	CHalfEdge * he[3];
	CPoint      p[3];
	double      u[3];
	double      du[3];

	int        i = 0;

	for( FaceHalfedgeIterator fhiter( f ); !fhiter.end(); ++fhiter )
	{
		CHalfEdge * h = *fhiter;
		he[i] = h;
		 p[i] = h->source()->point();
		du[i] = ( h == h->edge()->halfedge(0) )? e_w(h->edge()):-e_w( h->edge() );
		i ++;
	}

	CPoint n = f_normal(f);
	double area = f_area( f );
	

	assert( fabs( du[0] + du[1] + du[2] ) < 1e-5 );

	u[0] = 0;
	u[1] = u[0] + du[0];
	u[2] = u[1] + du[1];

	CPoint df = (p[2] - p[0] ) * ( ((p[0]-p[1])*(p[1]-p[2])) * u[0] );
	df += (p[1] - p[0] ) * ( ((p[0]-p[2])*(p[2]-p[1])) * u[0] );

	df += (p[2] - p[1] ) * ( ((p[1]-p[0])*(p[0]-p[2])) * u[1] );
	df += (p[0] - p[1] ) * ( ((p[1]-p[2])*(p[2]-p[0])) * u[1] );

	df += (p[0] - p[2] ) * ( ((p[2]-p[1])*(p[1]-p[0])) * u[2] );
	df += (p[1] - p[2] ) * ( ((p[2]-p[0])*(p[0]-p[1])) * u[2] );

	df /=(4*area*area);
	f_du( f ) = df;

};

CHolomorphicForm::CHolomorphicForm( std::list<CMesh*> & meshes )
{
	for( std::list<CMesh*>::iterator miter = meshes.begin(); miter != meshes.end(); miter ++ )
	{
		CMesh * pM = *miter;
		m_meshes.push_back( pM );
	}

	for( int i = 0; i < m_meshes.size(); i ++ )
	{
		for( MeshEdgeIterator eiter( m_meshes[i] ); !eiter.end(); ++ eiter )
		{
			CEdge * edge = *eiter;
			e_string( edge ) = edge->string();
			CEdgeTrait * pT = (CEdgeTrait*) edge->trait();
			pT->read();
		}

	}	

};

CHolomorphicForm::~CHolomorphicForm()
{
};




void CHolomorphicForm::conjugate()
{
	int i = 0;
	for(; i < m_meshes.size(); i ++ )
	{

		for( MeshFaceIterator fiter( m_meshes[i] ); !fiter.end(); ++ fiter )
		{
			CFace * face = *fiter;

			CPoint p[3];
			int   k = 0;
			for( FaceVertexIterator fviter( face ); !fviter.end(); ++ fviter )
			{
				CVertex * v = *fviter;
				p[k++] = v->point();
			}

			CPoint n = ( p[1] - p[0])^( p[2] - p[0] );
			f_area( face ) = n.norm()/2.0;
			f_normal( face ) = n/n.norm();

			convert( face );
		}

	}	


	int n = m_meshes.size();

	CSparseMatrix * A = new CSparseMatrix(n,n,n*7);
	assert( A );
	double *  x = new double[n];
	double *  b = new double[n];

	assert( b );
	assert( x );


	for(i = 0;  i < n; i ++ )
	{
		for( int j = 0;  j < n; j ++ )
		{
				CWedgeOperator wo( m_meshes[i], m_meshes[j] );
				A->AddElementTail(i,j, wo.wedge_product());
		}
	}


	for( i = 0; i < n ; i ++ )
	{
		for( int j = 0; j < n ; j ++ )
		{
			CWedgeOperator wo( m_meshes[i], m_meshes[j] );
			b[j] = wo.wedge_star_product();
		}

		A->CGSolver2Matlab(b, x, 1e-8, n);

		for( MeshEdgeIterator eiter( m_meshes[i] ); !eiter.end(); ++ eiter )
		{
			CEdge * e = *eiter;
			e_duv( e )[0] = e_w( e );
			e_duv( e )[1] = 0;
		
			int id1 = m_meshes[i]->edgeVertex1(e)->id();
			int id2 = m_meshes[i]->edgeVertex2(e)->id();

			for( int k = 0; k < n ; k ++ )
			{
				CVertex * w1 = m_meshes[k]->idVertex( id1 );
				CVertex * w2 = m_meshes[k]->idVertex( id2 );

				CEdge * edge = m_meshes[k]->vertexEdge( w1, w2 );
				e_duv( e )[1] += e_w(edge) * x[k];
			}

			char line[1024];
			sprintf(line,"duv=(%g %g)", e_duv(e)[0], e_duv(e)[1]);
			//sprintf(line," duv=(%g %g)", e_duv(e)[0], 0.0);
			e->string() += line;
		}

	}

	delete A;
	delete []x;
	delete []b;

};


/*
void Conjugate::save( const char * head ) 
{

	for( int i = 0; i < m_solids.GetSize(); i ++ )
	{
		std::string name( head );
		
		char buffer[128];

		sprintf(buffer,"%d.m",i);

		std::string postfix(buffer);
		
		name+= postfix;

		FILE * _os = fopen(name.c_str(), "w");
		assert( _os );


		MeshLib::Solid & solid = *m_solids[i];

		// vertices

		for (MeshLib::SolidVertexIterator iter(&solid); !iter.end(); ++iter )
		{
			MeshLib::Solid::tVertex v = *iter;
			MeshLib::Point p = v->point();

			fprintf(_os,"Vertex %d %g %g %g\r\n", solid.vertexId(v),p[0],p[1],p[2]);
		}

		for (MeshLib::SolidFaceIterator  fiter(&solid); !fiter.end(); ++fiter )
		{
			MeshLib::Solid::tFace f = *fiter;
			fprintf(_os,"Face %d ",f->id() );

			for( MeshLib::FaceVertexIterator  fiter( f ); !fiter.end(); ++fiter )
			{
				MeshLib::Solid::tVertex v = *fiter;
				fprintf(_os,"%d ",v->id());
			}

			fprintf(_os,"\r\n");
		}

		for (MeshLib::SolidEdgeIterator eiter(&solid); !eiter.end(); ++eiter )
		{
			MeshLib::Solid::tEdge e = *eiter;
			fprintf(_os,"Edge %d %d ",solid.edgeVertex1(e)->id(),solid.edgeVertex2(e)->id());
			Point duv = trait<WedgeOperatorEdgeTrait,Edge>( e ).duv();
			fprintf(_os,"{duv=(%g %g)}\r\n", duv[0],duv[1]);
		}
	}
};
*/