
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


//template<unsigned dim>
bool AbaqusComputationUpdater::run( MP mp ) {
    
    Vec<TM> Vecteur_de_maillages_input; 
    Vec<double> constrained_nodes; 
    double pix2m = mp[ "pix2m" ];
    FieldSet fs_output;
    Vec < Vec < std::string > > Prop_Mat; // FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])
    
    extract_computation_parameters( mp, Vecteur_de_maillages_input, constrained_nodes, Prop_Mat, fs_output);
    
    add_message( mp, ET_Info, "Lancement du calcul" ); mp.flush();
    Vec<TM> Vecteur_de_maillages_output = calc_abq_into_LMTppMesh(Vecteur_de_maillages_input, constrained_nodes, pix2m, Prop_Mat); // CALCUL
    add_message( mp, ET_Info, "Calcul terminé" ); mp.flush();
    
    
    put_result_in_MP(Vecteur_de_maillages_output, mp, fs_output); // SORTIE DANS UN FieldSet "calcul"
    add_message( mp, ET_Info, "Résultat renvoyé" ); mp.flush();
    
}

