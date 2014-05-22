#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <containers/vec.h>
#include "dic/correlation/ImgInterp.h"
#include "mesh/mesh.h"
#include "../../../sources/control_code_aster.h"
#include "mesh/write_mesh_vtk.h"
#include "dic/correlation/mesh_carac_correlation.h"
#include <iostream>
#include <boost/graph/graph_concepts.hpp>
#include "Code_Aster_ComputationUpdater.h"


bool Code_Aster_ComputationUpdater::run( MP mp ) {
    
       //// Variables 
    Vec<TM> Mesh_Vector_input;  
    Vec<int> constrained_nodes; 
    Vec<int> indices_bc_cn;
    MP param = mp["_children[0]"];
    double pix2m = param[ "pix2m" ];
    double thickness = param[ "thickness" ];
    FieldSet fs_output;
    Vec < Vec < std::string > > Prop_Mat; // FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])
    Vec<Vec< std::string> > force_files;
    //////////////
    
    extract_computation_parameters( param, Mesh_Vector_input, constrained_nodes, indices_bc_cn,  Prop_Mat, fs_output, force_files); // Lecture des paramètres du calcul
	    
    add_message( mp, ET_Info, "Lancement du calcul" );    mp.flush();
    Vec<TM> Mesh_Vector_output = calc_code_aster_into_LMTppMesh(Mesh_Vector_input, constrained_nodes, pix2m, Prop_Mat, thickness); // Computation for a given set of parameter
    add_message( mp, ET_Info, "Calcul terminé" );    mp.flush();
	    
	    for (int num_mesh = 0; num_mesh < Mesh_Vector_output.size(); num_mesh++)
		write_mesh_vtk( "/home/mathieu/aaa_test/aaa_test" + to_string(num_mesh) + ".vtu",Mesh_Vector_output[ num_mesh ]);
	    
    put_result_in_MP(Mesh_Vector_output, mp, fs_output); // Sortie dans un FieldSet "calcul"
    add_message( mp, ET_Info, "Résultat renvoyé" );    mp.flush(); 
 
    sleep(1);
    
}



