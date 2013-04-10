#ifndef  _FORM_TRAIT_H_
#define  _FORM_TRAIT_H_

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
  class CFaceTrait: public CTrait
  {
  public:

    double  m_area;
    CPoint  m_normal;
    CPoint  m_du;

    CFaceTrait() { m_area = 0; };
    ~CFaceTrait() {};
  };

  
class CEdgeTrait : public  CTrait
  {
  public:
    double m_length;
    double m_w;
    CPoint m_duv;
    std::string m_string;

    CEdgeTrait() { m_length = 0.0; m_w = 0;  };
    ~CEdgeTrait(){};

	  void read()
	  {
		  CParser parser( m_string );
		
      for( std::list<CToken*>::iterator iter = parser.tokens().begin() ; iter != parser.tokens().end(); ++ iter )
		  {
			  CToken * token = *iter;
			  if( token->m_key == "w" )
			  {
				  float w;
				  sscanf( token->m_value.c_str(), "(%f)",&w);
			  	m_w = w;
			  }
		  }
	  }
 };

inline double & e_l( CEdge* e ) 
{
    CEdgeTrait * pET = (CEdgeTrait*) e->trait();
    return pET->m_length;
};

inline double & e_w( CEdge* e ) 
{
    CEdgeTrait * pET = (CEdgeTrait*) e->trait();
    return pET->m_w;
};


inline CPoint & e_duv( CEdge* e ) 
{
    CEdgeTrait * pET = (CEdgeTrait*) e->trait();
    return pET->m_duv;
};

inline std::string & e_string( CEdge* e ) 
{
    CEdgeTrait * pET = (CEdgeTrait*) e->trait();
    return pET->m_string;
};

inline double & f_area( CFace* f ) 
{
    CFaceTrait * pFT = (CFaceTrait*) f->trait();
    return pFT->m_area;
};

inline CPoint & f_normal( CFace * f )
{
    CFaceTrait * pFT = (CFaceTrait*) f->trait();
    return pFT->m_normal;
};

inline CPoint & f_du( CFace * f )
{
    CFaceTrait * pFT = (CFaceTrait*) f->trait();
    return pFT->m_du;
};

class CHolomorphicFormTrait
{
public:
	CHolomorphicFormTrait( CMesh * pMesh );
	~CHolomorphicFormTrait();

protected:
    CMesh * m_pMesh;

    CEdgeTrait          * m_edge_traits;
    CFaceTrait           *m_face_traits;

  private:
    //allocate and dellocate traits

    void _allocate_edge_trait();
    void _allocate_face_trait();
 
    void _dellocate_edge_trait();
    void _dellocate_face_trait();
};

}
#endif  _FORM_TRAIT_H_