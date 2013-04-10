#ifndef _HARMONIC_FORM_H_
#define _HARMONIC_FORM_H_

#include  <math.h>
#include <queue>
#include "../Mesh/mesh.h"
#include "../Mesh/boundary.h"
#include "../Mesh/iterators.h"
#include "../Trait/HarmonicFormTrait.h"
#include "../LinearAlgebra/SparseMatrix.h"

namespace MeshLib
{
  class CHarmonicForm
  {
  public:
    CHarmonicForm( CMesh * pMesh, CMesh * pWMesh );
    ~CHarmonicForm();

    void calculate_harmonic_form();

  protected:
    CMesh * m_pMesh;
    CMesh * m_pWMesh;
    CBoundary m_boundary;

    void _closed_1_form();
    void _edge_weight();
    void _harmonic_1_form();
    void _integrate();
  };
}
#endif