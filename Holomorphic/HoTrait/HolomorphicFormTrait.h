#ifndef  _FORM_HOTRAIT_H_
#define  _FORM_HOTRAIT_H_

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
  class ChoFaceTrait: public CTrait
  {
  public:

    double  m_area;
    CPoint  m_normal;
    CPoint  m_du;

    ChoFaceTrait() { m_area = 0; };
    ~ChoFaceTrait() {};
  };

  
class ChoEdgeTrait : public  CTrait
  {
  public:
    double m_length;
    double m_w;
    CPoint m_duv;
    std::string m_string;

    ChoEdgeTrait() { m_length = 0.0; m_w = 0;  };
    ~ChoEdgeTrait(){};

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

inline double & hoe_l( CEdge* e ) 
{
    ChoEdgeTrait * pET = (ChoEdgeTrait*) e->trait();
    return pET->m_length;
};

inline double & hoe_w( CEdge* e ) 
{
    ChoEdgeTrait * pET = (ChoEdgeTrait*) e->trait();
    return pET->m_w;
};


inline CPoint & e_duv( CEdge* e ) 
{
    ChoEdgeTrait * pET = (ChoEdgeTrait*) e->trait();
    return pET->m_duv;
};

inline std::string & hoe_string( CEdge* e ) 
{
    ChoEdgeTrait * pET = (ChoEdgeTrait*) e->trait();
    return pET->m_string;
};

inline double & f_area( CFace* f ) 
{
    ChoFaceTrait * pFT = (ChoFaceTrait*) f->trait();
    return pFT->m_area;
};

inline CPoint & f_normal( CFace * f )
{
    ChoFaceTrait * pFT = (ChoFaceTrait*) f->trait();
    return pFT->m_normal;
};

inline CPoint & f_du( CFace * f )
{
    ChoFaceTrait * pFT = (ChoFaceTrait*) f->trait();
    return pFT->m_du;
};

class CHolomorphicFormTrait
{
public:
	CHolomorphicFormTrait( CMesh * pMesh );
	~CHolomorphicFormTrait();

protected:
    CMesh * m_pMesh;

    ChoEdgeTrait          * m_edge_traits;
    ChoFaceTrait           *m_face_traits;

  private:
    //allocate and dellocate traits

    void _allocate_edge_trait();
    void _allocate_face_trait();
 
    void _dellocate_edge_trait();
    void _dellocate_face_trait();
};

}
#endif  _FORM_HOTRAIT_H_