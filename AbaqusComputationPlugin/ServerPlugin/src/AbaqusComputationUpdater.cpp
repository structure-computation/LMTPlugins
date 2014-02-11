
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
    MP ch = mp[ "_children" ]; 
    double Young, Poisson, loi, rapport, sigma_0, n;
    FieldSet fs_input_bckp;
    
    // CHARGEMENT DU CHAMP D'ENTREE ET DES PROPRIETES MATERIAU
    for( int ii = 0; ii < ch.size(); ++ii ) {
        MP c = ch[ ii ];
        if (( c.type() == "FieldSetItem" ) or ( c.type() == "FieldSetCorreliItem" )){
            FieldSet fs_input(c); fs_input_bckp = fs_input;
            Vecteur_de_maillages_input = load_FieldSetItem_into_LMTpp_Mesh(c);
            for( int i = 0; i < Vecteur_de_maillages_input[0].skin.node_list.size(); ++i )
                constrained_nodes << Vecteur_de_maillages_input[0].skin.node_list[ i ].number;
        }
        
        if ( c.type() == "MaterialABQItem" ) {
            loi = c["Comportement.num"];
            std::cout << loi << std::endl;
            Young = c["Young[0]"];
            Poisson = c["Poisson[0]"];
            if (loi == 1)
                rapport = c["rapport[0]"];
            if (loi == 2){
                sigma_0 = c["sigma_0[0]"];
                n = c["n[0]"];
            }
        }
    }
    
    
    // REECRITURE DES PROPRIETES MATERIAU DANS UN FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])
    Vec < Vec < std::string > > Prop_Mat ; 
    if (loi == 0)
        Prop_Mat.resize(3);
    if (loi == 1)
        Prop_Mat.resize(4);
    if (loi == 2)
        Prop_Mat.resize(5);
    
    Prop_Mat[0] << "thelaw";
    if (loi == 0)
        Prop_Mat[0] << "Elas_iso";
    else if (loi == 1)
        Prop_Mat[0] << "Elas_ortho";
    else if (loi == 2)
        Prop_Mat[0] << "RO";
    std::string thelaw = Prop_Mat[0][1];
    
    Prop_Mat[1] << "Young";
    Prop_Mat[1] << LMT::to_string(Young*1e9); 
    Prop_Mat[2] << "Poisson";
    Prop_Mat[2] << LMT::to_string(Poisson); 
    if (loi == 1){
        Prop_Mat[3] << "rapport";
        Prop_Mat[3] << LMT::to_string(rapport);
    }
    else if (loi == 2){
        Prop_Mat[3] << "sigma_0";
        Prop_Mat[3] << LMT::to_string(sigma_0*1e6);
        Prop_Mat[4] << "n";
        Prop_Mat[4] << LMT::to_string(n);
    }
    
    add_message( mp, ET_Info, "Calcul lancé" );
    mp.flush();
    
    // CALCUL
    Vec<TM> Vecteur_de_maillages_output = calc_abq_into_LMTppMesh(Vecteur_de_maillages_input, constrained_nodes, pix2m, Prop_Mat);

    
    add_message( mp, ET_Info, "Calcul terminé" );
    mp.flush();
    
    // SORTIE DANS UN FieldSet
    FieldSet fs_output = fs_input_bckp;
    for (int num_mesh = 0; num_mesh < Vecteur_de_maillages_output.size(); num_mesh++){
        for (int no = 0; no < Vecteur_de_maillages_output[num_mesh].node_list.size(); no++ ) {
            fs_output.fields[0].values[num_mesh].data[no] = Vecteur_de_maillages_output[num_mesh].node_list[no].dep[0]/pix2m;
            fs_output.fields[1].values[num_mesh].data[no] = Vecteur_de_maillages_output[num_mesh].node_list[no].dep[1]/pix2m;
        }
    }
    fs_output.save(mp["_output[0]"]);
    PRINT("coucou");
    add_message( mp, ET_Info, "Résultat renvoyé" );
    mp.flush();
    PRINT("coucou");
    
    
}

