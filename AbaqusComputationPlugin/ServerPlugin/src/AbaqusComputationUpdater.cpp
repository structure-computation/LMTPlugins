#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <containers/vec.h>
#include "dic/correlation/ImgInterp.h"
#include "mesh/mesh.h"
#include "../../../sources/control_abq.h"
#include "mesh/write_mesh_vtk.h"
#include "dic/correlation/mesh_carac_correlation.h"
#include <iostream>
#include <boost/graph/graph_concepts.hpp>
#include "AbaqusComputationUpdater.h"


bool AbaqusComputationUpdater::run( MP mp ) {
    
    //// Variables 
    Vec<TM> Mesh_Vector_input;  
    Vec<int> constrained_nodes; 
    Vec<int> indices_bc_cn;
    MP param = mp["_children[0]"];
    double pix2m = param[ "pix2m" ];
    double thickness = param[ "thickness" ];
    FieldSet fs_output;
    int ex_field =0;
    Vec < Vec < std::string > > Prop_Mat; // FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])
    Vec<Vec< std::string> > force_files;
    Vec<Vec< double > > calc_force;
    //////////////
    
    extract_computation_parameters( mp, Mesh_Vector_input, constrained_nodes, indices_bc_cn,  Prop_Mat, fs_output, force_files, ex_field); // Lecture des paramètres du calcul
    
    if (ex_field == 0){
	add_message( mp, ET_Info, "No input field or mesh !" );    mp.flush(); 
	PRINT("No input field or mesh !");
    }
    else if (ex_field > 1){
	add_message( mp, ET_Info, "More than one input field or mesh. Possible confusion ! Doing nothing." );    mp.flush(); 
	PRINT("MORE THAN 1 INPUT FIELD, POSSIBLE CONFUSION");
	Mesh_Vector_input.resize(0);
    }
    else if (ex_field == -1){
	add_message( mp, ET_Info, "Not enough information (steps) in the txt files for the boundary conditions. Doing nothing." );    mp.flush(); 
	Mesh_Vector_input.resize(0);
    }
    else if ( indices_bc_cn.size() == 0){
	add_message( mp, ET_Info, "No boundary condition defined, by field or text file. Doing nothing." );    mp.flush(); 
	Mesh_Vector_input.resize(0);
    }
    if (Mesh_Vector_input.size() == 0) {
	add_message( mp, ET_Info, "There was a problem with the model edition. Check your computation conditions." );    mp.flush(); 
    }
    else {    
      
	add_message( mp, ET_Info, "Lancement du calcul" );    mp.flush();
	Vec<TM> Mesh_Vector_output = calc_abq_into_LMTppMesh(Mesh_Vector_input, constrained_nodes, pix2m, Prop_Mat, thickness); // Ref field computation for a given set of parameter
	add_message( mp, ET_Info, "Calcul terminé" );    mp.flush();
	
	extract_f_from_LMTppMesh (Mesh_Vector_output, constrained_nodes, indices_bc_cn, calc_force);
	
	PRINT(calc_force);
	
	put_result_in_MP(Mesh_Vector_output, mp, fs_output); mp.flush();   // Sortie dans un FieldSet "calcul" 
	add_message( mp, ET_Info, "Résultat renvoyé" );    mp.flush(); 
	
	sleep(1);
    }
    
}

