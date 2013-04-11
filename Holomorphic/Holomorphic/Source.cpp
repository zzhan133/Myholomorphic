#include <math.h>
#include "../mesh/mesh.h"

#include "../Trait/HarmonicFormTrait.h"
#include "../HarmonicForm/HarmonicForm.h"
#include "../HoTrait/HolomorphicFormTrait.h"
#include "../HolomorphicForm/HolomorphicForm.h"
#include "../InTrait/IntegrationTrait.h"
#include "../Integration/Integration.h"

using namespace MeshLib;

int main( int argc, char * argv[] )
{
	const char* wout[2] = {"kitten_w_0.m","kitten_w_1.m"};
	const char* uout[2] = {"kitten_u_0.m","kitten_u_1.m"};

	std::list<CMesh*> meshes;
	std::list<CHolomorphicFormTrait*> traits;
	
	// compute harmonic_form [21:40/4/9/2013 Zhe]
	for (int j = 0;j < 2; j++)
	{
		CMesh * pWMesh = new CMesh;
		assert( pWMesh );
		pWMesh->read_m( argv[1] );
		CMesh wmesh;
		wmesh.read_m( argv[j+2] );

		CHarmonicFormTrait  mtraits( pWMesh );
		CHarmonicFormTrait  wtraits( &wmesh );

		CHarmonicForm harmonic( pWMesh, &wmesh );
		harmonic.calculate_harmonic_form();

		//pWMesh = &cmesh;
		meshes.push_back(pWMesh);
		CHolomorphicFormTrait * pTrait = new CHolomorphicFormTrait(pWMesh);
		assert( pTrait );
		traits.push_back( pTrait );
//		cmesh.write_m( wout[j] );
//		wmesh.write_m( uout[j] );
	}	
	//end

	// computer the holomorphic 1-from [1:40/4/10/2013 Zhe]


	//for( int i = 0; i < 2; i ++ )
	//{
	//	CMesh * pMesh = new CMesh;
	//	assert( pMesh );
	//	pMesh->read_m( wout[i] );
	//	meshes.push_back( pMesh );
	//	CHolomorphicFormTrait * pTrait = new CHolomorphicFormTrait(pMesh);
	//	assert( pTrait );
	//	traits.push_back( pTrait );
	//}

	CHolomorphicForm form( meshes );
	form.conjugate();

	//int id = 0;
	//for( std::list<CMesh*>::iterator miter = meshes.begin(); miter != meshes.end(); miter++)
	//{
	//	CMesh * pM = *miter;
	//	char line[1024];
	//	sprintf(line,"kitten.duv_%d.m", id++ );
	//	pM->write_m( line );
	//}

	for( std::list<CHolomorphicFormTrait*>::iterator titer = traits.begin(); titer != traits.end(); titer++)
	{
		CHolomorphicFormTrait * pT = *titer;
		delete pT;
	}


	// Integration [3:21/4/10/2013 Zhe]
	const char* uvout[2] = {"kitten0_uv.m","kitten1_uv.m"};
	const char* wIn[2] = {"kitten.duv_0.m","kitten.duv_1.m"};
	const char* uvflat[2] = {"kitten0_flat_uv.m","kitten1_flat_uv.m"};
	int i = 0;
	for( std::list<CMesh*>::iterator miter = meshes.begin(); miter != meshes.end(); miter++)
	{
		CMesh* holo_mesh = *miter;
		CMesh fund_mesh;
		fund_mesh.read_m(argv[4]);
		CIntegrationTrait holo_trait(holo_mesh);
		CIntegrationTrait fund_trait( &fund_mesh );
		CIntegration integrator( holo_mesh, &fund_mesh );
		integrator._integrate();

		fund_mesh.write_m( uvout[i] );
		for( MeshVertexIterator viter( &fund_mesh); !viter.end(); ++viter )
		{
			CVertex * v = *viter;
			CPoint2 p = Inv_uv( v );
			CPoint map(p[0],p[1],0);
			v->point() = map;
		}
		fund_mesh.write_m(uvflat[i]);
		i++;
	}


	for( std::list<CMesh*>::iterator miter = meshes.begin(); miter != meshes.end(); miter++)
	{
		CMesh * pM = *miter;
		delete pM;
	}
	return 0;
}