#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include "FieldExportUpdater.h"
#include "sources/commandes_generales.h"
#include "mesh/write_mesh_vtk.h"
                        
typedef LMT::Mesh<Mesh_carac_pb_correlation_basic<double,2> > TM;

bool FieldExportUpdater::run( MP mp ) {
    
    MP c = mp[ "_children[ 0 ]" ];
    Vec<TM> Vecteur_de_maillages = load_FieldSetItem_into_LMTpp_Mesh(c);
    
    int format = mp["Format.num"];
    QString adresse = mp["Adresse"];std::string stdadresse = adresse.toStdString();
    QString nom = mp["Nom"];std::string stdnom = nom.toStdString();
    
    std::string racine_fic;
    if (stdadresse[stdadresse.size()-1] == '/')
        racine_fic = stdadresse + stdnom + "_";
    else
        racine_fic = stdadresse + "/" + stdnom + "_";
    
    if ( exists(stdadresse) == 0)
        create_dir(stdadresse);
        
    
    if (format == 0){// TXT
        // ELEMENTS
        std::string name_els = racine_fic + "elements.txt";
        if (exists(name_els)) remove(name_els.c_str());
        PRINT(name_els);
        std::ofstream els (name_els.c_str());
        for (int i = 0; i < Vecteur_de_maillages[0].elem_list.size(); i++){ 
            els << i + 1 ;
            for (int elsize =0; elsize < Vecteur_de_maillages[0].elem_list[i]->nb_nodes_virtual(); elsize ++)
                els << ", " << Vecteur_de_maillages[0].elem_list[i]->node_virtual(elsize)->number + 1 ;
            els << "\n";
        }
        //NOEUDS
        std::string name_nodes = racine_fic + "nodes.txt";
        if (exists(name_nodes)) remove(name_nodes.c_str());
        PRINT(name_nodes);
        std::ofstream nodes (name_nodes.c_str());
        PRINT(Vecteur_de_maillages[0].node_list[0].dim);
        for (int i = 0; i < Vecteur_de_maillages[0].node_list.size(); i++){
            nodes << i + 1 ;
            for (int numnodes = 0; numnodes < Vecteur_de_maillages[0].node_list[0].dim; numnodes++)
                nodes << ", " << Vecteur_de_maillages[0].node_list[i].pos[ numnodes ];
        nodes << "\n";
        }
        // DEPLACEMENTS
        std::string rac_disp = racine_fic + "disp";
        for (int i = 0; i < Vecteur_de_maillages.size(); i++){
            for (int j = 0; j < Vecteur_de_maillages[0].node_list[0].dim; j++){
                std::string name_disp = rac_disp + "XYZ"[ j ] + "_im" + to_string(i+1) + ".txt";
                if (exists(name_disp)) remove(name_disp.c_str());
                std::ofstream disp (name_disp.c_str());
                for (int k = 0; k < Vecteur_de_maillages[0].node_list.size(); k++)
                    disp << Vecteur_de_maillages[i].node_list[k].dep[j] << "\n";
            }
        }
    }
    else if (format == 1){// VTU
       for (int num_mesh = 0; num_mesh < Vecteur_de_maillages.size(); num_mesh++){
          write_mesh_vtk( racine_fic + "im" + to_string(num_mesh+1) + ".vtu",Vecteur_de_maillages[ num_mesh ]);
       }
    }
    else if (format == 2){// IMAGE
        
    }
    add_message( mp, ET_Info, "Résultat écrit" );
    mp.flush();
}



