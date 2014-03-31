
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

typedef LMT::Mesh<Mesh_carac_pb_correlation_basic<double,2> > TM;
typedef TM::Pvec Pvec;

bool exists( const std::string & file )
{
     std::ifstream fichier( file.c_str() );
    return !fichier.fail();
}

bool create_dir( const std::string & dir_path )
{
    int b = system(("mkdir -p " + dir_path).c_str());
    return exists(dir_path);
}

double calc_dist_segment( Vec <double > point, Vec < Vec < double > > segment){
  
    Vec <double> distances;
    double dist_P0 = pow( pow((point[0] - segment[0][0]),2) + pow((point[1] - segment[0][1]),2) , 0.5);
    distances << dist_P0 ;
    double dist_P1 = pow( pow((point[0] - segment[1][0]),2) + pow((point[1] - segment[1][1]),2) , 0.5);
    distances << dist_P1 ;
    
    Vec <double> P0M = point - segment[0];
    Vec <double> P0P1 = segment[1] - segment[0]; 
    double norme_P0P1 =  pow( pow( P0P1[0],2) + pow( P0P1[1],2) , 0.5);
    
    double norme_P0H = dot(P0M,P0P1)/norme_P0P1;
    double norme_P0M = pow( pow( P0M[0],2) + pow( P0M[1],2) , 0.5);
    double norme_MH = pow( pow(norme_P0M, 2) - pow(norme_P0H, 2) , 0.5);
    
    distances << norme_MH ;
     
    double mini = min(distances);
    
    return mini;
}

Vec <double> select_cn (Vec<TM> maillages, MP ch, std::string CL, int nbs, Vec <double> &constrained_nodes){

	    //Vec <double> constrained_nodes;
	    if (CL=="nope"){
		for( int i = 0; i < maillages[0].skin.node_list.size(); ++i )
		    constrained_nodes << maillages[0].skin.node_list[ i ].number;
	    }
	    else if (CL=="picked"){
	      
		MP bs = ch[ nbs ];
		MP bs2 = bs[ 2 ]; // va chercher la liste dans le bsitem
		Vec < Vec < Vec < double > > >  coor_nds_bc;
		
		for (int bc = 0; bc < bs2.size(); bc++){                  //  Récupération des coordonnées des segments choisis
		    MP morceau = bs2 [ bc ];
		    if ( morceau.type() == "PickedZoneItem"){
			MP pel_bc = morceau["picked_element[0]"];
			MP el_bc = pel_bc["element"];
			MP indice_el_bc = el_bc["indices"];
			Vec <int> indices;
			for (int nds = 0; nds < indice_el_bc.size(); nds++)
			    indices << indice_el_bc[nds];
			PRINT(indices);
			Vec < Vec < double > > coor_nds;
			MP mesh_bc = pel_bc["mesh"];
			MP pts_bc = mesh_bc["points"];
			for (int nds = 0; nds < indices.size(); nds++){
			    Vec < double > coor_nd;
			    MP noeuds = pts_bc[indices[nds]];
			    MP pos = noeuds["pos"];
			    for (int coor = 0; coor < pos.size(); coor++)
			      coor_nd << pos[coor];
			    coor_nds.push_back(coor_nd);
			}
			coor_nds_bc.push_back(coor_nds);
		    }
		}
		
		maillages[0].update_skin();
		for( int nds = 0; nds < maillages[0].skin.node_list.size(); ++nds ){ //  Sélection des noeuds contraints
		      bool close_to_the_border = 0 ;
		      for (int nbc = 0; nbc < coor_nds_bc.size(); nbc++){
			      Vec < double > pos_nd = maillages[0].skin.node_list[ nds ].pos;
			      double dist = calc_dist_segment( pos_nd, coor_nds_bc[nbc]);
			      if (dist < 1)
				  close_to_the_border = 1 ;
		      }
		      if (close_to_the_border)
			  constrained_nodes << maillages[0].skin.node_list[ nds ].number;
		}
	     }
	     return constrained_nodes;
}


TM load_into_2DLMTpp_Mesh(MP mesh){ 
    TM dic_mesh;
    TypedArray<int> *indices_elem = dynamic_cast<TypedArray<int> *>( mesh[ "_elements[0].indices" ].model() );
    const unsigned nb_elems = indices_elem->size(1);
    
    qDebug() << "Nb de noeuds : " << mesh[ "points" ].size() << ", Nb d'éléments : " << nb_elems;
    
    for( int i = 0, n = mesh[ "points" ].size(); i < n; ++i ) {
        MP pos = mesh[ "points" ][ i ][ "pos" ];
        dic_mesh.add_node( Pvec( pos[ 0 ], pos[ 1 ], pos[ 2 ] ) );
    }
    for( int nel = 0, mel = nb_elems; nel < mel; ++nel ) {
        MP el = mesh[ "_elements" ][ nel ];
        // if ( el.type() == "Element_TriangleList" and dim == 2 ) {
        if ( el.type() == "Element_TriangleList"  ) {
            if ( TypedArray<int> *indices = dynamic_cast<TypedArray<int> *>( el[ "indices" ].model() ) ) {
                for( int nt = 0, ct = 0; nt < indices->size( 1 ); ++nt ) {
                    unsigned o[ 3 ];
                    for( int j = 0; j < 3; ++j ){
                        o[ j ] = qMin( indices->operator[]( ct++ ), (int)dic_mesh.node_list.size() - 1 );
                    }
                    dic_mesh.add_element( Triangle(), DefaultBehavior(), o );
                }
            }
        }
    }
    dic_mesh.remove_unused_nodes();
    return dic_mesh;
}


Vec<TM> load_FieldSetItem_into_LMTpp_Mesh(FieldSet fs_input){
    Mesh_vecs maillage = fs_input.mesh;
    MP maillage_transfert = maillage.save();
    TM dic_mesh = load_into_2DLMTpp_Mesh(maillage_transfert);
    Vec<TM> Vecteur_de_maillages_input;
    Vecteur_de_maillages_input.resize(fs_input.fields[0].values.size()); 
    for (int num_mesh = 0; num_mesh < Vecteur_de_maillages_input.size(); num_mesh++){
        Vecteur_de_maillages_input[num_mesh]=dic_mesh;
        for (int no = 0; no < Vecteur_de_maillages_input[num_mesh].node_list.size(); no++ ) {
            Vecteur_de_maillages_input[num_mesh].node_list[no].dep[0] = fs_input.fields[0].values[num_mesh].data[no];
            Vecteur_de_maillages_input[num_mesh].node_list[no].dep[1] = fs_input.fields[1].values[num_mesh].data[no];
        }
        Vecteur_de_maillages_input[num_mesh].update_skin();        
    }
    return Vecteur_de_maillages_input;
}


void extract_computation_parameters( MP mp, Vec<TM> &Vecteur_de_maillages_input, Vec<double> &constrained_nodes,  Vec < Vec < std::string > > &Prop_Mat, FieldSet &fs_input_bckp){

    
    MP ch = mp[ "_children" ]; 
    double Young, Poisson, loi, rapport, sigma_0, n;
    
    // CHARGEMENT DU CHAMP D'ENTREE ET DES PROPRIETES MATERIAU
    std::string CL = "nope";
    int nbs;
    for( int ii = 0; ii < ch.size(); ++ii ) {
	MP c = ch[ ii ];
	if (( c.type() == "BoundariesSelectionItem" )){
	    CL = "picked";
	    nbs = ii;
	}
    }
    for( int ii = 0; ii < ch.size(); ++ii ) {
	MP c = ch[ ii ];
	if (( c.type() == "FieldSetItem" ) or ( c.type() == "FieldSetCorreliItem" )){
	    FieldSet fs_input(c); fs_input_bckp = fs_input;
	    Vecteur_de_maillages_input = load_FieldSetItem_into_LMTpp_Mesh(fs_input);
	    select_cn (Vecteur_de_maillages_input,  ch,  CL, nbs, constrained_nodes);
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
    
}


void put_result_in_MP (Vec<TM> &maillages, MP &mp, FieldSet &fs_output){// SORTIE DANS UN FieldSet "calcul"
  
    double pix2m = mp[ "pix2m" ];
    for (int num_mesh = 0; num_mesh < maillages.size(); num_mesh++){
	for (int no = 0; no < maillages[num_mesh].node_list.size(); no++ ) {
	    fs_output.fields[0].values[num_mesh].data[no] = maillages[num_mesh].node_list[no].dep[0]/pix2m;
	    fs_output.fields[1].values[num_mesh].data[no] = maillages[num_mesh].node_list[no].dep[1]/pix2m;
	}
    }
    fs_output.save(mp["_output[0]"]);
  
}

