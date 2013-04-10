#include <math.h>
#include "../mesh/mesh.h"

#include "../Trait/HarmonicFormTrait.h"
#include "../HarmonicForm/HarmonicForm.h"
#include "../HoTrait/HolomorphicFormTrait.h"
#include "../HolomorphicForm/HolomorphicForm.h"

using namespace MeshLib;

int main( int argc, char * argv[] )
{
	// compute harmonic_form [21:40/4/9/2013 Zhe]
	CMesh cmesh;
	cmesh.read_m( argv[1] );
	CMesh wmesh;
	wmesh.read_m( argv[2] );

	CHarmonicFormTrait  mtraits( &cmesh );
	CHarmonicFormTrait  wtraits( &wmesh );

	CHarmonicForm harmonic( & cmesh, &wmesh );
	harmonic.calculate_harmonic_form();

	cmesh.write_m( argv[3] );
	wmesh.write_m( argv[4] );
	//end

	std::list<CMesh*> meshes;
	std::list<CHolomorphicFormTrait*> traits;

	for( int i = 1; i < argc; i ++ )
	{
		CMesh * pMesh = new CMesh;
		assert( pMesh );
		pMesh->read_m( argv[i] );
		meshes.push_back( pMesh );
		CHolomorphicFormTrait * pTrait = new CHolomorphicFormTrait(pMesh);
		assert( pTrait );
		traits.push_back( pTrait );
	}

	CHolomorphicForm form( meshes );
	form.conjugate();

	int id = 0;
	for( std::list<CMesh*>::iterator miter = meshes.begin(); miter != meshes.end(); miter++)
	{
		CMesh * pM = *miter;
		char line[1024];
		sprintf(line,"test_%d.m", id++ );
		pM->write_m( line );
	}

	for( std::list<CHolomorphicFormTrait*>::iterator titer = traits.begin(); titer != traits.end(); titer++)
	{
		CHolomorphicFormTrait * pT = *titer;
		delete pT;
	}

	for( std::list<CMesh*>::iterator miter = meshes.begin(); miter != meshes.end(); miter++)
	{
		CMesh * pM = *miter;
		delete pM;
	}


	return 0;
}