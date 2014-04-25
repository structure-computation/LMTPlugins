
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
#include <time.h>

typedef LMT::Mesh<Mesh_carac_pb_correlation_basic<double,2> > TM;
typedef TM::Pvec Pvec;
typedef ImgInterp<double,2/*,ImgInterpOrder3_Kernel*/> I2;
LMT::Vec<I2> images;

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

Vec<std::size_t> find_str_in_str(std::string str, std::string sub){
    std::size_t idx ;
    Vec<std::size_t> solution;
    for(std::size_t pos = 0;pos < str.size();++pos){
        idx = str.find(sub,pos);
        pos=idx;
        if (idx>str.size())
            break;
        else
            solution << idx;
    }
    return solution;
}

double count_lines(std::string nom_fic){
    std::string numlines;
    system("touch res_system.txt");
    system(("wc -l " + nom_fic + "> res_system.txt").c_str());
    std::ifstream fichier("res_system.txt");
    std::string res;
    std::getline(fichier, res);
    for (int i = 0; i<(res.size()-nom_fic.size()-1);i++) numlines+=res[i];
    int numlines_int=atoi(numlines.c_str());
    return numlines_int;
}

double load_param(std::string nom_fic, std::string nom_param){
    std::ifstream strim(nom_fic.c_str());
    std::string ligne;
    double param;
    int numlines=count_lines(nom_fic);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
        if (ligne==nom_param){
            std::getline(strim, ligne);
            param = atof(ligne.c_str());
            break;
        }
    }
    return param;
}

std::string load_param_str(std::string nom_fic, std::string nom_param){
    std::ifstream strim(nom_fic.c_str());
    std::string ligne;
    std::string param;
    int numlines=count_lines(nom_fic);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
        if (ligne==nom_param){
            std::getline(strim, ligne);
            param = ligne;
            break;
        }
    }
    return param;
}

Vec<double> load_res(std::string nom_fic){
    std::ifstream strim(nom_fic.c_str());
    std::string ligne;
    Vec<double> resultat;
    
    int numlines=count_lines(nom_fic);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
	double a;
	std::istringstream convert(ligne);
	convert >> a;
        resultat.push_back(a);
    }
    return resultat;
}

void write_mat (Mat<double> M, std::string name){
    std::ofstream mat (name.c_str());
    for (int ii = 0; ii < M.col(0).size(); ii++){
        for (int jj = 0; jj < M.row(0).size(); jj++){
            mat << double(M(ii,jj)) << " " ;
        }
        mat << "\n";
    }
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

Vec <double> select_cn (Vec<TM> maillages, MP ch, std::string CL, int nbs, Vec <int> &constrained_nodes){

	    Vec<int> indices_bc_cn;
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
		      int indice;
		      for (int nbc = 0; nbc < coor_nds_bc.size(); nbc++){
			      Vec < double > pos_nd = maillages[0].skin.node_list[ nds ].pos;
			      double dist = calc_dist_segment( pos_nd, coor_nds_bc[nbc]);
			      if (dist < 1){
				  close_to_the_border = 1 ;
				  indice = nbc;
			      }
		      }
		      if (close_to_the_border){
			  constrained_nodes << maillages[0].skin.node_list[ nds ].number;
			  indices_bc_cn << indice;
		      }
		}
	     }
	     return indices_bc_cn;
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

void  extract_dep_from_LMTppMesh ( Vec<TM> res, Vec < Vec <Vec < double > > > &dep ){

    Vec <Vec <double> > dep_ref ;
    Vec <double> depU;Vec <double> depV;
    for (int num_mesh = 0; num_mesh < res.size(); ++num_mesh){
        for (int n = 0; n < res[ num_mesh ].node_list.size(); ++n){
            depU << res[ num_mesh ].node_list[ n ].dep[ 0 ];
            depV << res[ num_mesh ].node_list[ n ].dep[ 1 ];
        }
    }
    dep_ref.push_back(depU);dep_ref.push_back(depV);dep.push_back(dep_ref);
}


void extract_fnod_from_LMTppMesh ( Vec<TM> res_ref, std::string senstrac , Vec < Vec <Vec<double> > > &calc_force , Vec<int> constrained_nodes){


    Vec <Vec<double> > fnodaltot;
    for( int num_mesh = 0; num_mesh < res_ref.size(); ++num_mesh ) {
	Vec<double> fnodal;
        for( int n = 0; n < constrained_nodes.size(); ++n ){
	    if (senstrac == "hor")
		fnodal << res_ref[num_mesh].node_list[constrained_nodes[n]].f_nodal[0];
	    else if (senstrac == "ver")
		fnodal << res_ref[num_mesh].node_list[constrained_nodes[n]].f_nodal[1];
        }
        fnodaltot.push_back(fnodal);
    } 
    
    calc_force.push_back(fnodaltot);
}

void extract_id_properties(Vec < Vec < std::string > > Prop_Mat, Vec<int> &prop2id){
  
    std::string thelaw = Prop_Mat[0][1];
    
    if (thelaw == "Elas_iso"){
	prop2id << 2; // nu
    }
    else if (thelaw == "Elas_ortho"){
	prop2id << 2;// nu
	prop2id << 3;// E1/E2
    }
    else if (thelaw == "RO"){
	prop2id << 3;// sigma_0
	prop2id << 4;// n
    }
    else if (thelaw == "UMAT"){
	std::istringstream iss(Prop_Mat[1][1]);
	int umat_nparam;
	iss >> umat_nparam;
	std::string texte2 = Prop_Mat[1+1+umat_nparam+1][1];
	std::istringstream iss2(texte2);
	int umat_nparamid;
	iss2 >> umat_nparamid;
	for (int npa = 0; npa < umat_nparamid; npa++){
	    std::string stri = "paramid" + LMT::to_string(npa);
	    std::string texte = Prop_Mat[1+1+umat_nparam+1+1+npa][1];
	    std::istringstream is(Prop_Mat[1+1+umat_nparam+1+1+npa][1]);
	    int prop;
	    is >> prop;
	    prop2id << prop + 1 ;
	}
    }
    else if (thelaw == "Equation"){
	std::istringstream iss(Prop_Mat[3][1]);
	int nparam;
	iss >> nparam;
	std::string texte2 = Prop_Mat[2+2+2*nparam][1];
	std::istringstream iss2(texte2);
	int nparamid; 
	iss2 >> nparamid;
	for (int npa = 0; npa < nparamid; npa++){
	    std::istringstream is(Prop_Mat[2+2+2*nparam+1+npa][1]);
	    int prop;
	    is >> prop;
	    prop2id << prop + 3 ;
	}
    }
    else if (thelaw == "UMAT_Lem_diccit"){
	std::istringstream iss(Prop_Mat[1][1]);
	int umat_nparam;
	iss >> umat_nparam;
	std::string texte2 = Prop_Mat[1+1+umat_nparam+1][1];
	std::istringstream iss2(texte2);
	int umat_nparamid;
	iss2 >> umat_nparamid;
	for (int npa = 0; npa < umat_nparamid; npa++){
	    std::string stri = "paramid" + LMT::to_string(npa);
	    std::string texte = Prop_Mat[1+1+umat_nparam+1+1+npa][1];
	    std::istringstream is(Prop_Mat[1+1+umat_nparam+1+1+npa][1]);
	    int prop;
	    is >> prop;
	    prop2id << prop + 1 ;
	}
    }
  
  
}

void extract_images(MP mp, LMT::Vec<I2> &images){
 
   MP ch = mp[ "_children" ]; 
    for( int ii = 0; ii < ch.size(); ++ii ) {
	MP c = ch[ ii ];
	if ( c.type() == "ImgSetItem" ) {
            for( int j = 0; j < c[ "_children" ].size(); ++j ) {
                MP im = c[ "_children" ][ j ];
                if ( im.type() == "ImgItem" ) {
                    I2 *img = images.new_elem();
                    QString name = im[ "img.src" ];
                    if ( im[ "img.src" ].type() != "Path" )
                        name = "../CorreliOnline/html/" + name;

                    try {
                        img->load( name.toAscii().data() );
                        img->reverse_y();
                    } catch ( std::string msg ) {
                        //add_message( mp, ET_Error, "Img " + name + " does not exist" );
			PRINT("Images does not exist");
                        return false;
                    }
                } /*else if ( im.type() == "RawVolume" ) {
                    I2 *img = images.new_elem();
                    MP ms( im[ "img_size" ] );
                    Vec<int,3> S( ms[ 0 ], ms[ 1 ], ms[ 2 ] );
                    PRINT( S );

                    MP volume = im[ "_children" ][ 0 ];
                    qDebug() << volume;
                    MP data = updater->sc->load_ptr( volume[ "_ptr" ] );
                    qDebug() << data;

	            QString name = data;
                    PRINT( name.toAscii().data() );

                    try {
                        typedef unsigned char PI8;
                        img->template load_binary<PI8>( name.toAscii().data(), S );
                    } catch ( std::string msg ) {
                        add_message( mp, Updater::ET_Error, "Img " + name + " does not exist" );
                        return false;
                    }
		    PRINT( name.toAscii().data() );
                }*/
            }
        }
  
    }
}


void extract_computation_parameters( MP mp, Vec<TM> &Vecteur_de_maillages_input, Vec<int> &constrained_nodes,  Vec<int> &indices_bc_cn, Vec < Vec < std::string > > &Prop_Mat, FieldSet &fs_input_bckp, Vec<Vec<std::string> > &force_files){

    
    MP ch = mp[ "_children" ]; 
    double Young, Poisson, loi, rapport, sigma_0, n, ct, sign;
    std::string param_file, umatname, computation_type;
    int umat_ndepvar, umat_nparam, umat_nparamid, nparam, nparamid;
    MP mat;
    ct = mp["computation_type.num"];
    bool ex_field =0; 
    
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
	    ex_field =1;
	    FieldSet fs_input(c); fs_input_bckp = fs_input;
	    Vecteur_de_maillages_input = load_FieldSetItem_into_LMTpp_Mesh(fs_input);
	    indices_bc_cn = select_cn (Vecteur_de_maillages_input,  ch,  CL, nbs, constrained_nodes);
	}
	
	if ( c.type() == "MaterialABQItem" ) {
	    mat=c;
	    loi = c["Comportement.num"];
	    Young = c["Young[0]"];
	    Poisson = c["Poisson[0]"];
	    if (loi == 1) // Elas_ortho
		rapport = c["rapport[0]"];
	    else if (loi == 2){ // Ramberg_Osgood
		sigma_0 = c["sigma_0[0]"];
		n = c["n[0]"];
	    }
	    else if (loi == 3){ // UMAT
		QString Qparam_file = c["param_file"];
	        param_file = Qparam_file.toStdString();
		QString Qumatname = c["umat_file"];
	        umatname = Qumatname.toStdString();
	    }
	    else if (loi == 4){ // Equation
		QString Qparam_file = c["param_file"];
	        param_file = Qparam_file.toStdString();
	    }
	    else if (loi == 5){ // Equation + UMAT
		QString Qparam_file = c["param_file"];
	        param_file = Qparam_file.toStdString();
		QString Qumatname = c["umat_file"];
	        umatname = Qumatname.toStdString();
	    }
	    if (ct == 0)
		computation_type = "2DPS";
	    else if (ct == 1)
		computation_type = "2DPE";
	    else if (ct == 2)
		computation_type = "3Dex";
	}
    }
    
    
    // REECRITURE DES PROPRIETES MATERIAU DANS UN FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])
   
    if ((loi == 3) or (loi==5)){ // UMAT
	umat_nparam = load_param(param_file,"umat_nparam");
	umat_ndepvar = load_param(param_file,"umat_ndepvar");
	umat_nparamid = load_param(param_file,"umat_nparamid");
	Prop_Mat.resize(1 + 1 + umat_nparam + 1 + 1 + umat_nparamid +1 ); // thelaw + umat_nparam + number of parameters of the UMAT + umat_ndepvar + umat_nparamid + line for umat_nparamid + nparamid lines + umatname
    }
    if (loi == 4){ // Equation
	nparam = load_param(param_file,"nparam");
	nparamid = load_param(param_file,"nparamid");
	Prop_Mat.resize(1 +2 + 1 + nparam + nparam  + 1 + nparamid +1); // thelaw + Young + Poisson + nparam + number of parameters of the equation + names of parameters of the equation + nparamid + nparamid lines + equation
    }
    if (loi == 5){ // Equation+UMAT
	umat_nparam = load_param(param_file,"umat_nparam");
	umat_ndepvar = load_param(param_file,"umat_ndepvar");
	umat_nparamid = load_param(param_file,"umat_nparamid");
	Prop_Mat.resize(1 + 1 + umat_nparam + 1 + 1 + umat_nparamid +1 ); // thelaw + umat_nparam + number of parameters of the UMAT + umat_ndepvar + umat_nparamid + line for umat_nparamid + nparamid lines + umatname
    }
    
    
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
    else if (loi == 3)
	Prop_Mat[0] << "UMAT";
    else if (loi == 4)
	Prop_Mat[0] << "Equation";
    else if (loi == 5)
	Prop_Mat[0] << "UMAT_Lem_diccit";
    
    if ((loi == 3) or (loi == 5)){ // UMAT
	Prop_Mat[1] << "umat_nparam";
	Prop_Mat[1] << LMT::to_string(umat_nparam);
	for (int npa = 0; npa < umat_nparam; npa++){
	    std::string stri;
	    stri = "param" + LMT::to_string(npa+1);
	    Prop_Mat[2+npa] << stri;
	    std::string par = load_param_str(param_file,stri);
	    Prop_Mat[2+npa] << par;
	}
	Prop_Mat[2+umat_nparam] << "umat_ndepvar";
	Prop_Mat[2+umat_nparam] << LMT::to_string(umat_ndepvar);
	Prop_Mat[2+umat_nparam+1] << "umat_nparamid";
	Prop_Mat[2+umat_nparam+1] << LMT::to_string(umat_nparamid);
	
	for (int npa = 0; npa < umat_nparamid; npa++){
	    std::string stri;
	    stri = "paramid" + LMT::to_string(npa+1);
	    Prop_Mat[2+umat_nparam+1+1+npa] << stri;///
	    std::string par = load_param_str(param_file,stri);
	    Prop_Mat[2+umat_nparam+1+1+npa] << par;
	}
	Prop_Mat[2+umat_nparam+1+1+umat_nparamid] << "umatname";
	Prop_Mat[2+umat_nparam+1+1+umat_nparamid] << umatname;
    }
    else if (loi == 4){ // Equation // thelaw + Young + Poisson + nparam + number of parameters of the equation + names of parameters of the equation + nparamid + nparamid lines + equation
	Prop_Mat[1] << "Young";
	Prop_Mat[1] << LMT::to_string(Young*1e9); 
	Prop_Mat[2] << "Poisson";
	Prop_Mat[2] << LMT::to_string(Poisson); 
	Prop_Mat[2+1] << "nparam";
	Prop_Mat[2+1] << LMT::to_string(nparam);
	for (int npa = 0; npa < nparam; npa++){
	    std::string stri;
	    stri = "param" + LMT::to_string(npa+1);
	    Prop_Mat[2+2+npa] << stri;
	    std::string par = load_param_str(param_file,stri);
	    Prop_Mat[2+2+npa] << par;
	}
	for (int npa = 0; npa < nparam; npa++){
	    std::string stri;
	    stri = "param" + LMT::to_string(npa+1) + "_symbol";
	    Prop_Mat[2+2+nparam+npa] << stri;
	    std::string par = load_param_str(param_file,stri);
	    Prop_Mat[2+2+nparam+npa] << par;
	}
	  
	Prop_Mat[2+2+2*nparam] << "nparamid";
	Prop_Mat[2+2+2*nparam] << LMT::to_string(nparamid);
	
	for (int npa = 0; npa < nparamid; npa++){
	    std::string stri;
	    stri = "paramid" + LMT::to_string(npa+1);
	    Prop_Mat[2+2+2*nparam+1+npa] << stri;///
	    std::string par = load_param_str(param_file,stri);
	    Prop_Mat[2+2+2*nparam+1+npa] << par;
	}
	std::string eq = load_param_str(param_file,"equation");
	Prop_Mat[2+2+2*nparam+1+nparamid] << "equation";
	Prop_Mat[2+2+2*nparam+1+nparamid] << eq;
    }
    else { // no UMAT or equation
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
    
    Prop_Mat.resize(Prop_Mat.size()+1);
    Prop_Mat[Prop_Mat.size()-1] << computation_type;
    
    if (computation_type == "3Dex"){
	double n_el = mp [ "n_el" ];
	Prop_Mat[Prop_Mat.size()-1] << LMT::to_string(n_el);
    }
    else{
	Prop_Mat[Prop_Mat.size()-1] << "0";
    }
    
    if (ex_field == 0)
	PRINT("NO INPUT FIELD OR MESH");
    
    MP bs = ch[ nbs ];
    MP chil = bs[ "_children" ]; 
    force_files.resize(chil.size());
    for( int ii = 0; ii < chil.size(); ++ii ) {
	MP c = chil[ ii ];
	QString Qforce_file = c["force"]; 
	force_files[ii] << Qforce_file.toStdString();
	sign = c["force_sign.num"];
	PRINT(sign);
	if (sign == 0)
	   force_files[ii] << "+";
	else if (sign == 1)
	   force_files[ii] << "-";
	else if (sign == 2)
	   force_files[ii] << "0";
	
    }
    
    
}

void put_string_in_MP(Vec <std::string> strvec, MP &mp){

    std::istringstream iss(strvec[1]);
    double prop;
    iss >> prop;
    QString qstr = QString::fromStdString(strvec[1]);
    if (strvec[0] == "Young"){
	prop /= 1e9;
	qstr = QString::number(prop);
    }
    if (strvec[0] == "sigma_0"){
	prop /= 1e6;
	qstr = QString::number(prop);
    }
    qDebug() << qstr;
    mp[(strvec[0] + "[0]").c_str()] = prop;
    mp.flush();
}

void push_back_material_parameters( MP &mp, Vec < Vec < std::string > > Prop_Mat){

    MP ch = mp[ "_children" ]; 
    for( int ii = 0; ii < ch.size(); ++ii ) {
	MP c = ch[ ii ];
	if ( c.type() == "MaterialABQItem" ) {
	    for (int nprop =0; nprop < Prop_Mat.size(); nprop++)
		put_string_in_MP(Prop_Mat[nprop], c);
	}
    }
    mp.flush();
    
}


void put_result_in_MP (Vec<TM> maillages, MP &mp, FieldSet &fs_output){// SORTIE DANS UN FieldSet "calcul"

    PRINT("coucou");
    double pix2m = mp[ "pix2m" ];
    for (int num_mesh = 0; num_mesh < maillages.size(); num_mesh++){
    PRINT("coucou");
	for (int no = 0; no < maillages[num_mesh].node_list.size(); no++ ) {
	    fs_output.fields[0].values[num_mesh].data[no] = maillages[num_mesh].node_list[no].dep[0]/pix2m;
	    fs_output.fields[1].values[num_mesh].data[no] = maillages[num_mesh].node_list[no].dep[1]/pix2m;
	} 
    }
    PRINT("coucou");
    fs_output.save(mp["_output[0]"]);
    PRINT("coucou");
}

    
void write_identification_report (std::string report_address, Vec<TM> &Mesh_Vector_output, Vec <Vec<std::string> > Prop_Mat, int it, int iterations, Mat<double,Sym<>,SparseLine<> >M_d, Vec<Mat<double,Sym<>,SparseLine<> > >M_f, Vec<double>F_d, Vec <Vec<double> > F_f, Vec <Vec<double> > calc_force, Vec <Vec<double> > meas_force, Vec<int>prop2id, double ponderation, Vec<double> dif){

    system(("touch " + report_address).c_str());
    std::ofstream report (report_address.c_str());
    
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    
    report << "Report generated on " << asctime(timeinfo) << "\n";
    report << " \n";
    report << "There were " << Mesh_Vector_output.size() << " steps in the computation.\n";
    report << "The used mesh contains " << Mesh_Vector_output[0].node_list.size() << " nodes and " << Mesh_Vector_output[0].elem_list.size() << " elements.\n";
    report << " \n";
    if (it==0)
	report << "1 iteration of the identification procedure was ran (" << iterations << " max).\n";
    else
	report << it+1 << " iterations of the identification procedure were ran (" << iterations << " max).\n";
    report << "\n";
    
    if (it+1 == iterations){
	report << "!!!! /!\\ !!!!\n";
	report << "The procedure did not converge before the final number of iterations. You might want to re-run it with a larger number of iterations.\n";
	report << "!!!! /!\\ !!!!\n";
	report << "\n";
	report << "The last parameters were :\n";
    }
    else 
	report << "The identified parameters were :\n";
    report << "\n";
    for (int p2id =0; p2id<prop2id.size(); p2id++){
	report << Prop_Mat[prop2id[p2id]][0] << " : " << Prop_Mat[prop2id[p2id]][1] << "\n";
    }
    report << "(last relative increments - 1 is 100% - : ";
    for (int ii =0; ii< dif.size(); ii++)
	report << dif[ii] << ", ";
    report << ")\n";
    report << "\n";
    report << "\n";
    report << "The correlation matrix associated to displacements was :\n";
    for (int ii = 0; ii < M_d.col(0).size(); ii++){
        for (int jj = 0; jj < M_d.row(0).size(); jj++){
            report << double(M_d(ii,jj)) << " " ;
        }
        report << "\n";
    }
    report << "\n";
    report << "The second member associated to displacements was :\n";
    for (int ii = 0; ii < F_d.size(); ii++){
        report << F_d[ii] << " " ;
        report << "\n";
    }
    report << "\n";
    report << "The correlation matrixes associated to forces were :\n";
    for (int nbc = 0; nbc < F_f.size(); nbc++){
	Mat<double,Sym<>,SparseLine<> > M_ff = M_f[nbc];
	report << "Boundary n° " << nbc+1 << "\n" ; 
	for (int ii = 0; ii < M_d.col(0).size(); ii++){
	    for (int jj = 0; jj < M_d.row(0).size(); jj++){
		report << double(M_ff(ii,jj))*ponderation << " " ;
	    }
	    report << "\n";
	}
	report << "\n";
    }
    report << "The second members associated to forces were :\n";
    for (int nbc = 0; nbc < F_f.size(); nbc++){
	Vec<double> F_ff = F_f[nbc];
	report << "Boundary n° " << nbc+1 << "\n" ; 
	for (int ii = 0; ii < M_d.col(0).size(); ii++){
	    report << F_ff[ii]*ponderation << " " ;
	    report << "\n";
	}
	report << "\n";
    }
    
    report << "\n";
    report << "\n";
    report << "\n";
    report << "\n";
    report << "\n";
  
  
}