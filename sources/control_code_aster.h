
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

template<class n >
struct N;

void Write_msh_file (TM &m, std::string root_file, double pix2m, std::string computation_type){

     std::string name_file = ( root_file + ".msh" );
     if (exists(name_file)) remove(name_file.c_str());
     int vide = system(("touch " + name_file).c_str());
     std::ofstream msh( name_file.c_str() , std::ios::app);
     
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

void Write_export_file (std::string root_file){
    
    std::string name_file = ( root_file + ".export" );
    if (exists(name_file)) remove(name_file.c_str());
    int vide = system(("touch " + name_file).c_str());
    std::ofstream exp( name_file.c_str() , std::ios::app);
    
    exp << "P actions make_etude\n";
    exp << "P aster_root /aster\n";
    exp << "P consbtc oui\n";
    exp << "P corefilesize unlimited\n";
    exp << "P cpresok RESNOOK\n";
    exp << "P debug nodebug\n";
    exp << "P display marsannay\n";
    exp << "P distrib non\n";
    exp << "P facmtps 1\n";
    exp << "P lang fr\n";
    exp << "P mclient marsannay\n";
    exp << "P mem_aster 100.0\n";
    exp << "P memjob 5144000\n";
    exp << "P mode batch\n";
    exp << "P mpi_nbcpu 1\n";
    exp << "P mpi_nbnoeud 1\n";
    exp << "P nbmaxnook 5\n";
    exp << "P ncpus 1\n";
    exp << "P noeud frontal1\n";
    exp << "P nomjob CM_CLDIC\n";
    exp << "P origine ASTK 1.11.0\n";
    exp << "P platform LINUX64\n";
    exp << "P protocol_copyfrom asrun.plugins.server.SCPServer\n";
    exp << "P protocol_copyto asrun.plugins.server.SCPServer\n";
    exp << "P protocol_exec asrun.plugins.server.SSHServer\n";
    exp << "P proxy_dir /scratch/tmp_astout\n";
    exp << "P rep_trav /tmp/mathieu-marsannay-batch.12746\n";
    exp << "P soumbtc oui\n";
    exp << "P tpsjob 2640\n";
    exp << "P version 11.5\n";
    exp << "A args \n";
    exp << "A memjeveux 750.0\n";
    exp << "A tpmax 158400\n";
    exp << "F comm " << root_file << ".comm D  1\n";
    exp << "F msh " << root_file << ".msh D  19\n";
    exp << "F libr " << root_file << "_DX.resu R  61\n";
    exp << "F libr " << root_file << "_DY.resu R  62\n";
    exp << "F libr " << root_file << "_FX.resu R  63\n";
    exp << "F libr " << root_file << "_FY.resu R  64\n";
  
}

void Write_comm_file (std::string root_file, Vec<TM> mesh, Vec < Vec < std::string > > Prop_Mat, Vec<double> constrained_nodes, double pix2m){
    
    std::string name_file = ( root_file + ".comm" );
    if (exists(name_file)) remove(name_file.c_str());
    int vide = system(("touch " + name_file).c_str());
    std::ofstream comm( name_file.c_str() , std::ios::app);
    
    std::string computation_type = Prop_Mat[Prop_Mat.size()-1][0];
    std::string thelaw = Prop_Mat[0][1];

    
    comm << "  \n";
    comm << "DEBUT(PAR_LOT = 'NON', IMPR_MACRO = 'NON', IGNORE_ALARM = ('SUPERVIS_1','MODELISA4_6','ALGORITH7_60',),);\n";
    comm << "\n";
    comm << "# librairies python\n";
    comm << "import numpy as NP\n";
    comm << "\n";
    comm << "#================================================================================\n";
    comm << "#=====  1  - DEFINITION OF MESH AND MODEL======= ================================\n";
    comm << "#================================================================================\n";
    comm << "\n";
    comm << "# Read the mesh by defaut\n";
    comm << "PRE_GMSH();\n";
    comm << "MAIL = LIRE_MAILLAGE(UNITE = 20,\n";
    comm << "                      FORMAT = 'ASTER',);\n";
    comm << "\n";
    comm << "# Calculate and print the global dimensions  \n";                 
    comm << "TABG = RECU_TABLE(CO = MAIL,\n";
    comm << "                  NOM_TABLE = 'CARA_GEOM',);\n";
    comm << "\n";
    comm << "precis = 0.001;\n";
    comm << "\n";
    comm << "# Definition of the mecanical model \n";
    comm << "\n";
    comm << "MO = AFFE_MODELE(MAILLAGE = MAIL,\n";
    comm << "                 AFFE = (_F(TOUT = 'OUI',\n";
    comm << "                            PHENOMENE = 'MECANIQUE',\n";
    comm << "                            MODELISATION = ";
    if (computation_type == "2DPS")
      comm << "'C_PLAN',),\n";
    else if (computation_type == "2DPE")
      comm << "'D_PLAN',),\n";
    comm << "                        ),\n";
    comm << "                );\n";
    comm << "\n";
    comm << "#================================================================================\n";
    comm << "#=====  2  - DEFINITION OF THE MATERIAL BEHAVIOR=================================\n";
    comm << "#================================================================================\n";
    comm << "# Units : m, MPa\n";
    comm << "\n";
    
    if (thelaw == "Elas_iso"){
	comm << "NU__     = " << Prop_Mat[2][1] << ";\n";
	comm << "E__      = " << Prop_Mat[1][1] << ";\n";
        comm << "\n";
	comm << "MAT = DEFI_MATERIAU(ELAS = _F(E = E__,\n";
	comm << "				      NU = NU__,\n";
	comm << "				      ALPHA = 0.), \n";
	comm << "				);\n";
	comm << "    \n";
    }
    else if (thelaw == "Powerlaw"){
	comm << "E__      = " << Prop_Mat[1][1] << ";\n";
	comm << "NU__     = " << Prop_Mat[2][1] << ";\n";
	comm << "A_PUIS__ = " << Prop_Mat[3][1] << ";\n";
	comm << "N_PUIS__ = " << Prop_Mat[4][1] << ";\n";
	comm << "SY__     = " << Prop_Mat[5][1] << ";\n";
        comm << "\n";
	comm << "MAT = DEFI_MATERIAU(ELAS = _F(E = E__,\n";
	comm << "				      NU = NU__,\n";
	comm << "				      ALPHA = 0.), \n";
	comm << "			 ECRO_PUIS = _F(SY = SY__,\n";
	comm << "				       A_PUIS = A_PUIS__, \n";
	comm << "				       N_PUIS = N_PUIS__), \n";
	comm << "				);\n";
	comm << "    \n";
    }
    comm << "CHMAT = AFFE_MATERIAU(MAILLAGE = MAIL,\n";
    comm << "			  AFFE=_F(TOUT = 'OUI',\n";
    comm << "				  MATER = MAT,),\n";
    comm << "			 );\n";
    comm << "\n";
    comm << "#================================================================================\n";
    comm << "#=====  3  - DEFINITION OF BOUNDARY CONDITIONS / LOADING ========== =============\n";
    comm << "#================================================================================\n";
    comm << "\n";
    comm << "LTPS = [0";
    for (int num_mesh =0; num_mesh < mesh.size(); num_mesh++)
	comm << ", " << num_mesh+1 ;
    comm << "]\n";
    comm << "TFIN = max(LTPS)\n";
    comm << "NB_T = len(LTPS)\n";
    comm << "LTPS2 = []\n";
    comm << "Decoup = 1\n";
    comm << "for kk in range(NB_T-1):\n";
    comm << "    pas = (LTPS[kk+1]-LTPS[kk])/Decoup\n";
    comm << "    for tt in range(Decoup):\n";
    comm << "        LTPS2.append(LTPS[kk]+tt*pas)\n";
    comm << "LTPS2.append(LTPS[-1])\n";
    comm << "\n";
    comm << "# INSTANTS ARCHIVAGE\n";
    comm << "\n";
    comm << "LINST = DEFI_LIST_REEL(VALE = LTPS,);\n";
    comm << "\n";
    comm << "# INSTANTS CALCUL\n";
    comm << "\n";
    comm << "LINST2 = DEFI_LIST_REEL(VALE = LTPS2,);                 \n";
    comm << "LINSTC = DEFI_LIST_INST(DEFI_LIST = _F(METHODE = 'AUTO',\n";
    comm << "                                       LIST_INST = LINST2,\n";
    comm << "                                       PAS_MINI = 1.e-8,\n";
    comm << "                                      ),\n";
    comm << "                       );\n";
    comm << "\n";
    // MEASURED BOUNDARY CONDITIONS
    for (int nn=0; nn<constrained_nodes.size(); nn++){
	for (int nc=0; nc<TM::dim; nc++){
	    comm << "dep_" << constrained_nodes[nn]+1 << "_" << "XYZ"[nc] << " = DEFI_FONCTION(NOM_PARA='INST' ,\n";
	    comm << "				VALE = ( 0. , 0.,";
	    for (int num_mesh = 0; num_mesh < mesh.size(); num_mesh++)
		comm << "\n					 " << num_mesh+1  <<  ". , " << mesh[num_mesh].node_list[constrained_nodes[nn]].dep[nc]*pix2m << " ,";
	    comm << "),) ;\n";
	    comm << "\n";
	 }
    }
    comm << "    # DEPLACEMENTS PARAMETRIQUES SUR LES BORDS\n";
    comm << "\n";
    comm << "TRAC1 = AFFE_CHAR_CINE_F( MODELE = MO, \n";
    comm << "			MECA_IMPO=(";
    if (constrained_nodes.size() == 0)
	PRINT("NO BOUNDARY CONDITION SELECTED");
    for (int nn=0; nn<constrained_nodes.size(); nn++){
	comm << "\n				_F( NOEUD='N" << constrained_nodes[nn]+1 << "', ";
	for (int nc=0; nc<TM::dim; nc++)
	    comm << "D" << "XYZ"[nc] << " = dep_" << constrained_nodes[nn]+1 << "_" << "XYZ"[nc] << ", ";
	comm << "),";				
    }
    comm << "),);\n";
    comm << "   \n";
    comm << "#================================================================================\n";
    comm << "#=====  4  - RESOLUTION =========================================================\n";
    comm << "#================================================================================\n";
    comm << "\n";
    comm << "SIG = STAT_NON_LINE(MODELE = MO,\n";
    comm << "                    CHAM_MATER = CHMAT,\n";
    comm << "                    EXCIT = (\n";
    comm << "                             _F(CHARGE = TRAC1,),),\n";
    if (thelaw == "Elas_iso")
	comm << "		    COMP_ELAS=_F(RELATION='ELAS', DEFORMATION='GROT_GDEP',),\n";
    else if (thelaw == "Powerlaw")
	comm << "		    COMP_INCR=_F(RELATION='VMIS_ISOT_PUIS', DEFORMATION='GROT_GDEP',),\n";
    comm << "                    INCREMENT = _F(LIST_INST = LINSTC,\n";
    comm << "                                   INST_FIN  = LTPS[NB_T-1],\n";
    comm << "                                  ),\n";
    comm << "                    NEWTON = _F(PREDICTION = 'EXTRAPOLE',\n";
    comm << "                                REAC_ITER = 1,),\n";
    comm << "                    CONVERGENCE = _F(RESI_GLOB_RELA = 1.E-4,),\n";
    comm << "                    ARCHIVAGE = _F(LIST_INST = LINST,),\n";
    comm << "                   );\n";
    comm << "\n";
    comm << "SIG = CALC_CHAMP(reuse=SIG,\n";
    comm << "		 RESULTAT = SIG,\n";
    comm << "		 FORCE = 'FORC_NODA', );\n";
    comm << "\n";
    comm << "#================================================================================\n";
    comm << "#=====  5  - OUTPUT =============================================================\n";
    comm << "#================================================================================\n";
    comm << "\n";
    comm << "############################################\n";
    comm << "####  COMPOSANTES DU CHAMP DE DEPLACEMENT\n";
    comm << "############################################\n";
    comm << "\n";
    comm << "MATABLE = CREA_TABLE(RESU = _F(RESULTAT = SIG,\n";
    comm << "                               NOM_CHAM ='DEPL',\n";
    comm << "                               TOUT_CMP = 'OUI',\n";
    comm << "                               TOUT = 'OUI',\n";
    comm << "                               LIST_INST = LINST,),\n";
    comm << "                     TITRE = '',\n";
    comm << "                    );\n";
    comm << "\n";
    comm << "lnoeud = ['N1";
    for (int nn=1; nn<mesh[0].node_list.size(); nn++)
	comm << "', 'N" << nn+1 ;
    comm << "']\n";
    comm << "\n";
    comm << "LeTitre = 'Table reduite - p='+str(1.);\n";
    comm << "\n";
    comm << "TabRed = CALC_TABLE(TABLE = MATABLE,\n";
    comm << "                    TITRE = LeTitre,\n";
    comm << "                    ACTION = (_F(OPERATION = 'EXTR',\n";
    comm << "                                 NOM_PARA = ('NOEUD', 'INST', 'DX', 'DY'),),\n";
    comm << "                              _F(OPERATION = 'FILTRE',\n";
    comm << "                                 NOM_PARA = 'NOEUD',\n";
    comm << "                                 VALE_K = lnoeud,),),)\n";
    comm << "\n";
    comm << "IMPR_TABLE(TABLE = TabRed,\n";
    comm << "           FORMAT = 'TABLEAU_CROISE',\n";
    comm << "           NOM_PARA = ('NOEUD', 'INST', 'DX'),\n";
    comm << "           UNITE = 61,)\n";
    comm << "\n";
    comm << "IMPR_TABLE(TABLE = TabRed,\n";
    comm << "           FORMAT = 'TABLEAU_CROISE',\n";
    comm << "           NOM_PARA = ('NOEUD', 'INST', 'DY'),\n";
    comm << "           UNITE = 62,)\n";
    comm << "\n";
    comm << "MATABLEF = CREA_TABLE(RESU = _F(RESULTAT = SIG,\n";
    comm << "                               NOM_CHAM ='FORC_NODA',\n";
    comm << "                               TOUT_CMP = 'OUI',\n";
    comm << "                               TOUT = 'OUI',\n";
    comm << "                               LIST_INST = LINST,),\n";
    comm << "                     TITRE = '',\n";
    comm << "                    );\n";
    comm << "\n";
    comm << "TabRedf = CALC_TABLE(TABLE = MATABLEF,\n";
    comm << "                    TITRE = LeTitre,\n";
    comm << "                    ACTION = (_F(OPERATION = 'EXTR',\n";
    comm << "                                 NOM_PARA = ('NOEUD', 'INST', 'DX', 'DY'),),\n";
    comm << "                              _F(OPERATION = 'FILTRE',\n";
    comm << "                                 NOM_PARA = 'NOEUD',\n";
    comm << "                                 VALE_K = lnoeud,),),)\n";
    comm << "\n";
    comm << "IMPR_TABLE(TABLE = TabRedf,\n";
    comm << "           FORMAT = 'TABLEAU_CROISE',\n";
    comm << "           NOM_PARA = ('NOEUD', 'INST', 'DX'),\n";
    comm << "           UNITE = 63,)\n";
    comm << "\n";
    comm << "IMPR_TABLE(TABLE = TabRedf,\n";
    comm << "           FORMAT = 'TABLEAU_CROISE',\n";
    comm << "           NOM_PARA = ('NOEUD', 'INST', 'DY'),\n";
    comm << "           UNITE = 64,)\n";
    comm << "\n";
    comm << "FIN();\n";
}

Vec<int> clear_result_file(std::string filename){
  
    std::ifstream strim(filename.c_str());
    std::string ligne;
    
    int numlines=count_lines(filename);
    int nl=0;
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
	std::string fc = ligne.substr(0,1);
	if (fc == "#")
	   nl++;
	else break;
    }
    
    Vec<int> num_nodes;
    Vec<std::size_t> nplaces = find_str_in_str(ligne, "N");
    
    for (int nnds=2; nnds<nplaces.size(); nnds++){
	Vec<std::size_t> vec_spaces = find_str_in_str(ligne.substr(nplaces[nnds]+1, ligne.size()-nplaces[nnds]+1), " ");
	num_nodes << atoi(ligne.substr(nplaces[nnds]+1,vec_spaces[0]).c_str());
    }
    
    int res_sys = system(("sed -i -e '1," + to_string(nl+2) + "d' " + filename).c_str());
    return num_nodes;
}

void load_aster_res_into_LMTppMesh(std::string root_file, Vec<TM> &mesh, double thickness){
    
    Vec<int> num_nodes;
    for (int nc=0; nc<TM::dim; nc++){
	num_nodes = clear_result_file(root_file + "_D" + "XYZ"[nc] + ".resu");
	Vec<Vec<double> > result_file = load_vecvec(root_file + "_D" + "XYZ"[nc] + ".resu");
	for (int num_mesh=0; num_mesh<mesh.size(); num_mesh++){
	    for (int nn=0; nn<num_nodes.size(); nn++){
		mesh[num_mesh].node_list[num_nodes[nn]-1].dep[nc] = result_file[num_mesh][nn+1];
	    }
	}
    }
    for (int nc=0; nc<TM::dim; nc++){
	num_nodes = clear_result_file(root_file + "_F" + "XYZ"[nc] + ".resu");
	Vec<Vec<double> > result_file = load_vecvec(root_file + "_F" + "XYZ"[nc] + ".resu");
	for (int num_mesh=0; num_mesh<mesh.size(); num_mesh++){
	    for (int nn=0; nn<num_nodes.size(); nn++){
		mesh[num_mesh].node_list[num_nodes[nn]-1].f_nodal[nc] = thickness*result_file[num_mesh][nn+1];
	    }
	}
    }
    
}
    
Vec<TM> calc_code_aster_into_LMTppMesh(Vec<TM> &m_ref, Vec<double> constrained_nodes, double pix2m, Vec < Vec < std::string > > Prop_Mat , double thickness){
    
    Vec<TM> Mesh_vector_output = m_ref;
    char* HomeDir;
    HomeDir = getenv ("HOME");
    std::string root_dir = std::string(HomeDir) + "/scratch";
    std::string root_file = std::string(HomeDir) + "/scratch/aaa_test";
    create_dir(root_dir);
    std::string thelaw = Prop_Mat[0][1];
    
    Write_msh_file(m_ref[0], root_file, pix2m, Prop_Mat[Prop_Mat.size()-1][0]);
    Write_export_file(root_file);
    Write_comm_file(root_file, m_ref, Prop_Mat, constrained_nodes, pix2m);
    
    put_void_file_in(root_file + "result.txt");
    int res_sys = system(("/opt/aster/bin/as_run " + root_file + ".export > " + root_file + "result.txt").c_str()); // COMPUTATION

    load_aster_res_into_LMTppMesh(root_file, Mesh_vector_output, thickness);
    return Mesh_vector_output;
    
}
