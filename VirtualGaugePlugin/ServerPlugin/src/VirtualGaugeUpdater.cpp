#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include "VirtualGaugeUpdater.h"
#include "../../../sources/commandes_generales.h"
#include "mesh/interpolation.h"
#include "mesh/write_mesh_vtk.h"



bool VirtualGaugeUpdater::run( MP mp ) {
    
    MP ch = mp[ "_children" ];
    Vec<TM> Mesh_Vector_input;  
    TM GaugeMesh;
    DicCPU<double, TM::dim> dic; // For the computation of the strain
    Vec<Vec<double> > eps_result; eps_result.resize(3);
    Vec<Vec<double> > dep_result; dep_result.resize(TM::dim);
    
    for( int ii = 0; ii < ch.size(); ++ii ){
	MP c = ch[ ii ];
	qDebug() << c.type();
	if (( c.type() == "FieldSetItem" ) or ( c.type() == "FieldSetCorreliItem" )){
	    FieldSet fs_input(c); 
	    Mesh_Vector_input = load_FieldSetItem_into_LMTpp_Mesh(fs_input);
	}
	else if (( c.type() == "MeshItem" )){
	    Mesh_vecs maillage; maillage.load(c["_mesh"]);
	    MP maillage_transfert = maillage.save();
	    GaugeMesh = load_MeshMP_into_2DLMTpp_Mesh(maillage_transfert);
	}
    }
    
    int format = mp["Format.num"];
    QString adresse = mp["Adresse"];std::string stdadresse = adresse.toStdString();
    QString nom = mp["Nom"];std::string stdnom = nom.toStdString();
    std::string racine_fic;
    if (stdadresse[stdadresse.size()-1] == '/')
        racine_fic = stdadresse + stdnom + "_";
    else
        racine_fic = stdadresse + "/" + stdnom + "_";
    if ( exists(stdadresse) == 0 )
        create_dir(stdadresse);
    
    for (int num_mesh=0; num_mesh < Mesh_Vector_input.size(); num_mesh++){
	Vec<Vec<double> > dep_tmp; dep_tmp.resize(TM::dim);
	GetEpsInVecs ge;
	for (int no = 0; no < GaugeMesh.node_list.size(); no++){
            GaugeMesh.node_list[ no ].dep = interpolation( Mesh_Vector_input[num_mesh], dep_DM(), GaugeMesh.node_list[ no ].pos );
	    for (int d = 0; d < TM::dim; d++)
	       dep_tmp[d] << GaugeMesh.node_list[ no ].dep[d];
	}
	dic.get_epsilon( GaugeMesh );
	write_mesh_vtk( racine_fic + to_string(num_mesh) + ".vtu",GaugeMesh);
	apply( GaugeMesh.elem_list, ge );
	
	Vec<Vec<double> > eps_tmp; eps_tmp.resize(eps_result.size());
	for (int d = 0; d<eps_result.size(); d++){
	    eps_tmp[d].resize(ge.e.size());
	    for (int ne = 0; ne < ge.e.size(); ne++){
		eps_tmp[d][ne] = ge.e[ne][d];
	    }
	    eps_result[d].resize(Mesh_Vector_input.size());
	    eps_result[d][num_mesh] = mean(eps_tmp[d]);
	}
	for (int d = 0; d<dep_result.size(); d++){
	    dep_result[d].resize(Mesh_Vector_input.size());
	    dep_result[d][num_mesh] = mean(dep_tmp[d]);
	}
	
	
    }
    
    PRINT(eps_result);
    
    write_vec(eps_result[0], racine_fic + "_epsx.txt");
    write_vec(eps_result[1], racine_fic + "_epsy.txt");
    write_vec(eps_result[2], racine_fic + "_epsxy.txt");
    write_vec(dep_result[0], racine_fic + "_depx.txt");
    write_vec(dep_result[1], racine_fic + "_depy.txt");
    
    
}