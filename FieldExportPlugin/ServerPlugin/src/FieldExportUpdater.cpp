#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include "FieldExportUpdater.h"
#include "sources/commandes_generales.h"
#include "mesh/write_mesh_vtk.h"
                        

bool FieldExportUpdater::run( MP mp ) {
    
    MP c = mp[ "_children[ 0 ]" ];
    Vec<TM> Vecteur_de_maillages = load_FieldSetItem_into_LMTpp_Mesh(c);
    int format = mp["Format.num"];
    QString adresse = mp["Adresse"];
    std::string stdadresse = adresse.toStdString();
    PRINT(stdadresse);
    PRINT(stdadresse[stdadresse.size()-1]);
    std::string racine_fic;
    if (stdadresse[stdadresse.size()-1] == '/')
        racine_fic = stdadresse + "resultat_";
    else
        racine_fic = stdadresse + "/resultat_";
    
    if (format == 0){// TXT
        //
    }
    else if (format == 1){// VTU
       for (int num_mesh = 0; num_mesh < Vecteur_de_maillages.size(); num_mesh++){
          write_mesh_vtk( racine_fic + to_string(num_mesh) + ".vtu",Vecteur_de_maillages[ num_mesh ]);
       }
    }
    else if (format == 2){
        
    }
    
    mp.flush();
}



