#ifndef  _FORM_TRAIT_H_
#define  _FORM_TRAIT_H_

#include <map>
#include <vector>

#include "../Mesh/mesh.h"
#include "../Trait/Trait.h"
#include "../Geometry/Point.h"
#include "../Geometry/Point2.h"
#include "../Parser/Parser.h"

namespace MeshLib
{
  class CWedge;

  class CFaceTrait: public CTrait
  {
  public:
    bool m_touched;
    CFaceTrait() { m_touched = false; };
    ~CFaceTrait() {};
  };

  class CVertexTrait : public  CTrait
  {
  public:
    int     m_index;
    int     m_father;
	  CPoint2 m_uv;
    bool      m_touched;
    std::string m_string;
    CWedge * m_wedge;
    double      m_u;
  
    CVertexTrait() { m_index = 0; m_touched = false; m_wedge = NULL; m_u = 0; };
    ~CVertexTrait() {};

    std::string & string() 
    {
        return m_string; 
    }
   void read()
	{
	  CParser parser( m_string );

    std::list<CToken*> & tokens = parser.tokens();
    for( std::list<CToken*>::iterator titer = tokens.begin(); titer != tokens.end(); titer ++ )
    {
        CToken * pT = *titer;
		    if( pT->m_key == "father" )
		    {
            std::string str = pT->m_value;
            int s = (int)str.find("(");
			      int t = (int)str.find(")");
			      std::string value = str.substr(s+1,t-s-1);
            m_father = atoi( value.c_str() );
		    }
    }
  }

  };

class CEdgeTrait : public  CTrait
  {
  public:
    double m_length;
    double m_weight;
    double m_du;
	  bool     m_sharp;
    std::string m_string;

    CEdgeTrait() { m_length = 0.0; m_du = 0.0; m_sharp = false; m_weight = 0; };
    ~CEdgeTrait(){};

   void read()
	{
	  CParser parser( m_string );

    std::list<CToken*> & tokens = parser.tokens();
    for( std::list<CToken*>::iterator titer = tokens.begin(); titer != tokens.end(); titer ++ )
    {
        CToken * pT = *titer;
		    if( pT->m_key == "sharp" )
		    {
			    m_sharp = true;
		    }
    }
  }
  };

class CHalfEdgeTrait : public  CTrait
{
public:
  double m_angle;
  CPoint m_s;
  CWedge * m_wedge;
  CHalfEdgeTrait() { m_angle = 0.0; };
  ~CHalfEdgeTrait() {};
};

inline double & v_u( CVertex * v )
{
    CVertexTrait * pVT = (CVertexTrait*) v->trait();
    return pVT->m_u;
}

inline std::string & v_string( CVertex* v ) 
{
    CVertexTrait * pVT = (CVertexTrait*) v->trait();
    return pVT->m_string;
};

inline bool & v_touched( CVertex* v ) 
{
    CVertexTrait * pVT = (CVertexTrait*) v->trait();
    return pVT->m_touched;
};

inline int & v_father( CVertex* v ) 
{
    CVertexTrait * pVT = (CVertexTrait*) v->trait();
    return pVT->m_father;
};

inline int & v_idx( CVertex* v ) 
{
    CVertexTrait * pVT = (CVertexTrait*) v->trait();
    return pVT->m_index;
};

inline double & e_l( CEdge* e ) 
{
    CEdgeTrait * pET = (CEdgeTrait*) e->trait();
    return pET->m_length;
};

inline double & e_w( CEdge* e ) 
{
    CEdgeTrait * pET = (CEdgeTrait*) e->trait();
    return pET->m_weight;
};


inline double & e_du( CEdge* e ) 
{
    CEdgeTrait * pET = (CEdgeTrait*) e->trait();
    return pET->m_du;
};

inline std::string & e_string( CEdge* e ) 
{
    CEdgeTrait * pET = (CEdgeTrait*) e->trait();
    return pET->m_string;
};

inline bool & e_sharp( CEdge* e ) 
{
    CEdgeTrait * pET = (CEdgeTrait*) e->trait();
    return pET->m_sharp;
};

inline double & c_a( CHalfEdge * c )
{
    CHalfEdgeTrait * pHT = (CHalfEdgeTrait*) c->trait();
    return pHT->m_angle;
};

inline CWedge * & c_wedge( CHalfEdge * c )
{
    CHalfEdgeTrait * pHT = (CHalfEdgeTrait*) c->trait();
    return pHT->m_wedge;
};

inline CPoint2 & v_uv( CVertex* v ) 
{
    CVertexTrait * pVT = (CVertexTrait*) v->trait();
    return pVT->m_uv;
};

inline bool & f_touched( CFace* f ) 
{
    CFaceTrait * pFT = (CFaceTrait*) f->trait();
    return pFT->m_touched;
};

class CHarmonicFormTrait
{
public:
	CHarmonicFormTrait( CMesh * pMesh );
	~CHarmonicFormTrait();

protected:
    CMesh * m_pMesh;

    CVertexTrait        * m_vertex_traits;
    CEdgeTrait          * m_edge_traits;
    CHalfEdgeTrait   * m_halfedge_traits;
    CFaceTrait           *m_face_traits;

  private:
    //allocate and dellocate traits

    void _allocate_vertex_trait();
    void _allocate_edge_trait();
    void _allocate_halfedge_trait();
    void _allocate_face_trait();
 
    void _dellocate_vertex_trait();
    void _dellocate_edge_trait();
    void _dellocate_halfedge_trait();
    void _dellocate_face_trait();

};

}
#endif  _FORM_TRAIT_H_