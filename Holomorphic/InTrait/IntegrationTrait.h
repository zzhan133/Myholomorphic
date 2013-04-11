#ifndef  _INTEGRATION_TRAIT_H_
#define  _INTEGRATION_TRAIT_H_

#include <map>
#include <vector>
#include <list>

#include "../Mesh/mesh.h"
#include "../Trait/Trait.h"
#include "../Geometry/Point.h"
#include "../Geometry/Point2.h"
#include "../Parser/Parser.h"

namespace MeshLib
{

  class CInVertexTrait : public  CTrait
  {
  public:

    bool        m_touched;
    CPoint2		m_uv;
    std::string m_string;
	int         m_father;

    CInVertexTrait() { m_father = 0; m_touched = false;};
    ~CInVertexTrait(){};

	  void read()
	  {
		  CParser parser( m_string );
		
		  for( std::list<CToken*>::iterator iter = parser.tokens().begin() ; iter != parser.tokens().end(); ++ iter )
		  {
			  CToken * token = *iter;
			  if( token->m_key == "father" )
			  {
				  int f = 0;
				  sscanf( token->m_value.c_str(), "(%d)",&f);
			  	  m_father = f;
			  }
		  }
	  }
 };

class CInEdgeTrait : public  CTrait
  {
  public:

    CPoint2		m_duv;
    std::string m_string;

    CInEdgeTrait() {};
    ~CInEdgeTrait(){};

	  void read()
	  {
		  CParser parser( m_string );
		
		  for( std::list<CToken*>::iterator iter = parser.tokens().begin() ; iter != parser.tokens().end(); ++ iter )
		  {
			  CToken * token = *iter;
			  if( token->m_key == "duv" )
			  {
				  float w[2];
				  sscanf( token->m_value.c_str(), "(%g %g)",&w[0], &w[1]);
			  	  m_duv = CPoint2(w[0],w[1]);
			  }
		  }
	  }
 };

inline CPoint2 & Ine_duv( CEdge* e ) 
{
    CInEdgeTrait * pET = (CInEdgeTrait*) e->trait();
    return pET->m_duv;
};

inline std::string & Ine_string( CEdge* e ) 
{
    CInEdgeTrait * pET = (CInEdgeTrait*) e->trait();
    return pET->m_string;
};

inline int & Inv_father( CVertex * v ) 
{
    CInVertexTrait * pVT = (CInVertexTrait*) v->trait();
    return pVT->m_father;
};

inline CPoint2 & Inv_uv( CVertex * v ) 
{
    CInVertexTrait * pVT = (CInVertexTrait*) v->trait();
    return pVT->m_uv;
};

inline bool & Inv_touched( CVertex * v ) 
{
    CInVertexTrait * pVT = (CInVertexTrait*) v->trait();
    return pVT->m_touched;
};

inline std::string & Inv_string( CVertex * v ) 
{
    CInVertexTrait * pVT = (CInVertexTrait*) v->trait();
    return pVT->m_string;
};

class CIntegrationTrait
{
public:
	CIntegrationTrait( CMesh * pMesh );
	~CIntegrationTrait();

protected:
    CMesh * m_pMesh;

    CInEdgeTrait			 * m_edge_traits;
    CInVertexTrait         *m_vertex_traits;

  private:
    //allocate and dellocate traits

    void _allocate_edge_trait();
    void _allocate_vertex_trait();
 
    void _dellocate_edge_trait();
    void _dellocate_vertex_trait();
};

}
#endif  _INTEGRATION_TRAIT_H_