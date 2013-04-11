#ifndef _INTEGRATION_H_
#define _INTEGRATION_H_

#include  <math.h>
#include <queue>
#include <list>
#include <vector>
#include "../Mesh/mesh.h"
#include "../Mesh/boundary.h"
#include "../Mesh/iterators.h"
#include "../InTrait/IntegrationTrait.h"

namespace MeshLib
{

class CIntegration
{
public:

	CIntegration( CMesh * pForm, CMesh * pDomain );
	~CIntegration();
	void _integrate();

private:
	CMesh * m_pForm;
	CMesh * m_pDomain;
};

}
#endif