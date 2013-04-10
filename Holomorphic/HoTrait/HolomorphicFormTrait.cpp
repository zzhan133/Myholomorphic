#include "HolomorphicFormTrait.h"

using namespace MeshLib;

CHolomorphicFormTrait::CHolomorphicFormTrait( CMesh * pMesh )
{
    m_pMesh = pMesh;

    _allocate_edge_trait();
    _allocate_face_trait();

};

CHolomorphicFormTrait::~CHolomorphicFormTrait( )
{
    _dellocate_edge_trait();
    _dellocate_face_trait();
};



void CHolomorphicFormTrait::_allocate_edge_trait()
{
    m_edge_traits = new ChoEdgeTrait[m_pMesh->numEdges()];
    assert( m_edge_traits );

    int id = 0;
    for( std::list<CEdge*>::iterator eiter = m_pMesh->edges().begin(); eiter != m_pMesh->edges().end(); eiter ++ )
    {
      CEdge * e = *eiter;
       e->trait() = (CTrait*)  &m_edge_traits[id++];
    }
}
    

void CHolomorphicFormTrait::_allocate_face_trait()
{
    int num = m_pMesh->numFaces() ;

    m_face_traits = new ChoFaceTrait[num];
    assert( m_face_traits );
    
    int id = 0;
    for( std::list<CFace*>::iterator fiter = m_pMesh->faces().begin(); fiter != m_pMesh->faces().end(); fiter ++ )
    {
      CFace * f = *fiter;
      
        f->trait() = (CTrait*)&m_face_traits[id++];
    }
}


void CHolomorphicFormTrait::_dellocate_edge_trait()
{
    for( std::list<CEdge*>::iterator eiter = m_pMesh->edges().begin(); eiter != m_pMesh->edges().end(); eiter ++ )
    {
      CEdge * e = *eiter;
       e->trait() = NULL;
    }
    delete []m_edge_traits;
}
    


void CHolomorphicFormTrait::_dellocate_face_trait()
{

    for( std::list<CFace*>::iterator fiter = m_pMesh->faces().begin(); fiter != m_pMesh->faces().end(); fiter ++ )
    {
      CFace * f = *fiter;
      f->trait() = NULL;
    }
    delete []m_face_traits;
}
