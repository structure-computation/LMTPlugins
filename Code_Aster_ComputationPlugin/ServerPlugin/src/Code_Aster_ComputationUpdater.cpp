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
    Vec<TM> Vecteur_de_maillages_input;  
    Vec<int> constrained_nodes; 
    Vec<int> indices_bc_cn;
    MP param = mp["_children[0]"];
    double pix2m = param[ "pix2m" ];
    double thickness = param[ "thickness" ];
    FieldSet fs_output;
    Vec < Vec < std::string > > Prop_Mat; // FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])
    Vec<Vec< std::string> > force_files;
    //////////////
    
    extract_computation_parameters( param, Vecteur_de_maillages_input, constrained_nodes, indices_bc_cn,  Prop_Mat, fs_output, force_files); // Lecture des paramètres du calcul
    
    add_message( mp, ET_Info, "Lancement du calcul" );    mp.flush();
    Vec<TM> Vecteur_de_maillages_output = Vecteur_de_maillages_input;
    
    calc_code_aster_into_LMTppMesh(Vecteur_de_maillages_output, Vecteur_de_maillages_input, constrained_nodes, pix2m, Prop_Mat, thickness); // Ref field computation for a given set of parameter
    
   
    
    //calc_abq_into_LMTppMesh(Vecteur_de_maillages_output, Vecteur_de_maillages_input, constrained_nodes, pix2m, Prop_Mat, thickness); // Ref field computation for a given set of parameter
   
    
    
    add_message( mp, ET_Info, "Calcul terminé" );    mp.flush();
    
    //put_result_in_MP(Vecteur_de_maillages_output, mp, fs_output); // Sortie dans un FieldSet "calcul"
    //add_message( mp, ET_Info, "Résultat renvoyé" );    mp.flush();  
}



