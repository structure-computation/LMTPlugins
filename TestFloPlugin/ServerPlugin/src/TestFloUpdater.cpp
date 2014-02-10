
#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include <containers/vec.h>
#include "dic/correlation/ImgInterp.h"
#include "mesh/mesh.h"
#include "sources/control_abq.h"
#include "mesh/write_mesh_vtk.h"
#include "dic/correlation/mesh_carac_correlation.h"
#include <iostream>
#include <boost/graph/graph_concepts.hpp>
#include "TestFloUpdater.h"



//template<unsigned dim>
bool TestFloUpdater::run( MP mp ) {
    
    TM dic_mesh;
    Vec<TM> Vecteur_de_maillages_input; 
    int nb_images =1;
    
    Vec<double> constrained_nodes; 
    
    double pix2m = mp[ "pix2m" ];
    
    MP ch = mp[ "_children" ]; 
    FieldSet fs_input_bckp;
    double Young, Poisson, loi, rapport, sigma_0, n;
    
    for( int ii = 0; ii < ch.size(); ++ii ) {
        MP c = ch[ ii ];
        if (( c.type() == "FieldSetItem" ) or ( c.type() == "FieldSetCorreliItem" )){
            FieldSet fs_input(c);fs_input_bckp=fs_input;
            Mesh_vecs maillage = fs_input.mesh;
            MP maillage_transfert = maillage.save();
            dic_mesh = load_into_2DLMTpp_Mesh(maillage_transfert);
            
            Vecteur_de_maillages_input.resize(fs_input.fields[0].values.size()); 
            for (int num_mesh = 0; num_mesh < Vecteur_de_maillages_input.size(); num_mesh++){
                Vecteur_de_maillages_input[num_mesh]=dic_mesh;
                fs_input.load_current_time_step(num_mesh);
                for (int no = 0; no < Vecteur_de_maillages_input[num_mesh].node_list.size(); no++ ) {
                    Vecteur_de_maillages_input[num_mesh].node_list[no].dep[0] = fs_input.fields[0].values[0].data[no];
                    Vecteur_de_maillages_input[num_mesh].node_list[no].dep[1] = fs_input.fields[1].values[0].data[no];
                }
                Vecteur_de_maillages_input[num_mesh].update_skin();
                for( int i = 0; i < Vecteur_de_maillages_input[num_mesh].skin.node_list.size(); ++i )
                    constrained_nodes << Vecteur_de_maillages_input[num_mesh].skin.node_list[ i ].number;
                //write_mesh_vtk( "/media/mathieu/Data/test_input_" + to_string(num_mesh) + ".vtu", Vecteur_de_maillages_input[num_mesh]);
            }
        }
        //         else if ( c.type() == "MeshItem" ) {
        //             MP mesh = c["_mesh" ]; 
        //             dic_mesh = load_into_2DLMTpp_Mesh(mesh);
        //             add_message( mp, ET_Info, "Maillage récupéré" );
        //             mp.flush();
        //             //write_mesh_vtk( "/media/mathieu/Data/test.vtu",dic_mesh);
        //             Vecteur_de_maillages_input.resize(1); Vecteur_de_maillages_input[0]=dic_mesh;
        //             constrained_nodes << 0; constrained_nodes << 10;
        //         }
        
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
    
    std::string racine_fic = ("/media/mathieu/Data/aaa_test_" + LMT::to_string( nb_images ) );
    Write2DINP (Vecteur_de_maillages_input, racine_fic, constrained_nodes, pix2m, Prop_Mat);
    add_message( mp, ET_Info, "Ecriture du fichier .inp terminée" );
    mp.flush();
    
    std::string nom_fic_lck = racine_fic + ".lck";if (exists(nom_fic_lck)) remove(nom_fic_lck.c_str());
    std::string nom_fic_odb = racine_fic + ".odb";if (exists(nom_fic_odb)) remove(nom_fic_odb.c_str());
    
    if (thelaw == "UMAT"){
        // std::string umatname = load_param_str(nom_fic_param,"umatname");
        //system(("abaqus interactive job=" + racine_fic + ".inp user=" + umatname + ".f double > res_s.txt").c_str() );
        // system(("abaqus interactive job=" + racine_fic + ".inp user=" + umatname + ".f double").c_str() );
    }
    else if (thelaw == "dpc"){
        //system(("abaqus interactive job=" + racine_fic + ".inp user=elasticity_variation_epsvol.f double > res_s.txt").c_str() );
        system(("abaqus interactive job=" + racine_fic + ".inp user=elasticity_variation_epsvol.f double").c_str() );
    }
    else{
        //system(("abaqus interactive job=" + racine_fic + ".inp cpus=6 double > res_s.txt").c_str() );
        system(("/media/mathieu/Data/Abaqus/exec/abq6112.exe interactive job=" + racine_fic + ".inp cpus=8 ").c_str() );
        std::cout << "/media/mathieu/Data/Abaqus/exec/abq6112.exe interactive job=" << racine_fic << ".inp cpus=8 " << std::endl;
    }
    
    add_message( mp, ET_Info, "Calcul terminé" );
    mp.flush();
    
    
    std::string nom_fic = racine_fic + ".odb";
    Vec<TM> Vecteur_de_maillages_output = load_abq_res_odb(nom_fic, Vecteur_de_maillages_input);
    
    for (int num_mesh = 0; num_mesh < Vecteur_de_maillages_output.size(); num_mesh++){
        write_mesh_vtk( "/media/mathieu/Data/test_output_" + to_string(num_mesh) + ".vtu", Vecteur_de_maillages_output[num_mesh]);
        for (int no = 0; no < Vecteur_de_maillages_output[num_mesh].node_list.size(); no++ ) {
            //PRINT(Vecteur_de_maillages_output[num_mesh].node_list[no].dep[0]);
          //  PRINT(no);
        }
    }
    
    add_message( mp, ET_Info, "Résultat récupéré" );
    mp.flush();
    
    FieldSet fs_output = fs_input_bckp;
    
    for (int num_mesh = 0; num_mesh < Vecteur_de_maillages_output.size(); num_mesh++){
        fs_output.load_current_time_step(num_mesh);
        for (int no = 0; no < Vecteur_de_maillages_input[num_mesh].node_list.size(); no++ ) {
            fs_output.fields[0].values[0].data[no] = Vecteur_de_maillages_output[num_mesh].node_list[no].dep[0]/pix2m;
            fs_output.fields[1].values[0].data[no] = Vecteur_de_maillages_output[num_mesh].node_list[no].dep[1]/pix2m;
        }
    }
    
    fs_output.save(mp["_output[0]"]);
    add_message( mp, ET_Info, "Résultat renvoyé" );
    mp.flush();
    
    
    
}

