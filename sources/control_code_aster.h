
#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include "LMT/include/containers/vec.h"
#include "dic/correlation/ImgInterp.h"
#include "LMT/include/mesh/mesh.h"
#include "dic/correlation/mesh_carac_correlation.h"
#include <iostream>
#include <boost/graph/graph_concepts.hpp>
#include "FieldSet.h"
#include "commandes_generales.h"


void Write_msh_file (TM &m, std::string racine_fic, Vec<double> constrained_nodes, double pix2m, Vec < Vec < std::string > > Prop_Mat, double thickness){

     std::string nom_fic = ( racine_fic + ".msh" );
     if (exists(nom_fic)) remove(nom_fic.c_str());
     int vide = system(("touch " + nom_fic).c_str());
     std::string computation_type = Prop_Mat[Prop_Mat.size()-1][0]; 
     std::ofstream msh( nom_fic.c_str() , std::ios::app);
     
     PRINT(nom_fic);
     
     int type_elem;
     if (m.elem_list[0]->nb_nodes_virtual() == 3){
	  if (computation_type == "2DPS") type_elem = 2;
	  else if (computation_type == "2DPE") type_elem = 2;
	  else if (computation_type == "3Dex") type_elem = 4;
     }
     else if (m.elem_list[0]->nb_nodes_virtual() == 4){
	  if (computation_type == "2DPS") type_elem = 3;
	  else if (computation_type == "2DPE") type_elem = 3;
	  else if (computation_type == "3Dex") type_elem = 5;
     }
     
     msh << "$MeshFormat\n";
     msh << "2.2 0 8\n";
     msh << "$EndMeshFormat\n";
     msh << "$Nodes\n";
     msh << m.node_list.size() << "\n";
     for (int i = 0; i < m.node_list.size() ; ++i){//liste des noeuds avec leur position dans le plan
	  msh << i + 1 << " " << m.node_list[i].pos[ 0 ]*pix2m << " " << m.node_list[i].pos[ 1 ]*pix2m<< " 0\n";
     }
     msh << "$EndNodes\n";
     msh << "$Elements\n";
     msh << m.elem_list.size() << "\n";
     for (int i = 0; i < m.elem_list.size(); ++i){//liste des noeuds avec leur position dans l'espace
	  msh << i + 1 ;
	  msh << " " << type_elem << " 2 0 1";
	  for (int jj=0; jj<m.elem_list[0]->nb_nodes_virtual(); jj++)
		msh   << " "  << m.elem_list[i]->node_virtual(jj)->number + 1 ;
	  msh << "\n";
     }
     msh << "$EndElements\n";
  
}

void calc_code_aster_into_LMTppMesh(Vec<TM> &Vecteur_de_maillages_output, Vec<TM> &m_ref, Vec<double> constrained_nodes, double pix2m, Vec < Vec < std::string > > Prop_Mat , double thickness){
     
   
    char* HomeDir;
    HomeDir = getenv ("HOME");
    std::string racine_fic = std::string(HomeDir) + "/aaa_test";
    std::string thelaw = Prop_Mat[0][1];
    
    Write_msh_file(m_ref[0], racine_fic, constrained_nodes, pix2m, Prop_Mat, thickness);

    //Vec<TM> Vecteur_de_maillages_output = load_abq_res_odb(nom_fic, m_ref);
    
    
}
