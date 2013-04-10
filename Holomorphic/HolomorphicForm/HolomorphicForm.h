#ifndef _HARMONIC_FORM_H_
#define _HARMONIC_FORM_H_

#include  <math.h>
#include <queue>
#include <list>
#include <vector>
#include "../Mesh/mesh.h"
#include "../Mesh/boundary.h"
#include "../Mesh/iterators.h"
#include "../Trait/HolomorphicFormTrait.h"
#include "../LinearAlgebra/SparseMatrix.h"

namespace MeshLib
{

class CWedgeOperator  
{
public:
	CWedgeOperator(CMesh * solid0, CMesh * solid1);
	~CWedgeOperator();
	double wedge_product();
	double wedge_star_product();
private:
	CMesh * m_pMesh[2];
};

class CHolomorphicForm
{
public:
	CHolomorphicForm( std::list<CMesh*> & meshes );
	~CHolomorphicForm();
	std::vector<CMesh*> & meshes() { return m_meshes; };

	void conjugate();

	//void save( const char * head );

private:
	std::vector<CMesh*> m_meshes;
	void convert( CFace * f );
};

}
#endif