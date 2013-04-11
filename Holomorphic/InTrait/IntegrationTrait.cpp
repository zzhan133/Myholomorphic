#include "IntegrationTrait.h"

using namespace MeshLib;

CIntegrationTrait::CIntegrationTrait( CMesh * pMesh )
{
    m_pMesh = pMesh;

    _allocate_edge_trait();
    _allocate_vertex_trait();

};

CIntegrationTrait::~CIntegrationTrait( )
{
    _dellocate_edge_trait();
    _dellocate_vertex_trait();
};



void CIntegrationTrait::_allocate_edge_trait()
{
    m_edge_traits = new CInEdgeTrait[m_pMesh->numEdges()];
    assert( m_edge_traits );

    int id = 0;
    for( std::list<CEdge*>::iterator eiter = m_pMesh->edges().begin(); eiter != m_pMesh->edges().end(); eiter ++ )
    {
      CEdge * e = *eiter;
       e->trait() = (CTrait*)  &m_edge_traits[id++];
    }
}
    

void CIntegrationTrait::_allocate_vertex_trait()
{
    int num = m_pMesh->numVertices() ;

    m_vertex_traits = new CInVertexTrait[num];
    assert( m_vertex_traits );
    
    int id = 0;
    for( std::list<CVertex*>::iterator viter = m_pMesh->vertices().begin(); 
		viter != m_pMesh->vertices().end(); viter ++ )
    {
      CVertex * v = *viter;
      
        v->trait() = (CTrait*)&m_vertex_traits[id++];
    }
}


void CIntegrationTrait::_dellocate_edge_trait()
{
    for( std::list<CEdge*>::iterator eiter = m_pMesh->edges().begin(); eiter != m_pMesh->edges().end(); eiter ++ )
    {
      CEdge * e = *eiter;
       e->trait() = NULL;
    }
    delete []m_edge_traits;
}
    


void CIntegrationTrait::_dellocate_vertex_trait()
{

    for( std::list<CVertex*>::iterator viter = m_pMesh->vertices().begin(); 
		viter != m_pMesh->vertices().end(); viter ++ )
    {
      CVertex * v = *viter;
      v->trait() = NULL;
    }
    delete []m_vertex_traits;
}
