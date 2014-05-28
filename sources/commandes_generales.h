
#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include "LMT/include/containers/vec.h"
#include "LMT/include/containers/eig_lapack.h"
#include "LMT/include/containers/mat_true_inv.h"
#include "dic/correlation/ImgInterp.h"
#include "dic/correlation/DicCPU.h"
#include "LMT/include/mesh/mesh.h"
#include "dic/correlation/mesh_carac_correlation.h"
#include <iostream>
#include <boost/graph/graph_concepts.hpp>
#include "FieldSet.h"
#include <time.h>
#include "make_fields.h"

typedef LMT::Mesh<Mesh_carac_pb_correlation_basic<double,2> > TM;
typedef TM::Pvec Pvec;
typedef ImgInterp<double,2/*,ImgInterpOrder3_Kernel*/> I2;
LMT::Vec<I2> images;

struct GetEpsInVecs {
    template<class TE>
    void operator()( const TE &el ) {
        Vec<double> val = el.epsilon[0];
	e << val;
    }
    Vec< Vec<double> > e;
};

// Tests if a file exists
bool exists( const std::string & file )
{
     std::ifstream fichier( file.c_str() );
    return !fichier.fail();
}

// Creates void file or replace an existing one by a void file
void put_void_file_in(std::string resfile){
    if (exists(resfile)) 
      remove(resfile.c_str()); 
    int vide = system(("touch " + resfile).c_str());
}

// Creates a directory or a tree of dirs and subdirs
bool create_dir( const std::string & dir_path )
{
    int b = system(("mkdir -p " + dir_path).c_str());
    return exists(dir_path);
}

// Finds the positions of a smaller string (e.g. a letter) in a larger string. 
// Returns the positions of the first letter of the smallest string in the larger one.
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

// Counts the number of lines in a file
double count_lines(std::string file_name){
    std::string numlines;
    int value = system("touch res_system.txt");
    value = system(("wc -l " + file_name + "> res_system.txt").c_str());
    std::ifstream fichier("res_system.txt");
    std::string res;
    std::getline(fichier, res);
    for (int i = 0; i<(res.size()-file_name.size()-1);i++) numlines+=res[i];
    int numlines_int=atoi(numlines.c_str());
    return numlines_int;
}

// Loads a double in a text parameter file. If the File is as follows :
//
//  Blabla
//  4
//
//  E
//  18
//
// Then this functions looks for the first line that is equal to "param_name" and takes the next line to be the output. Ex : load_param(filename, "E") will be the double 18.
double load_param(std::string file_name, std::string param_name){
    std::ifstream strim(file_name.c_str());
    std::string ligne;
    double param;
    int numlines=count_lines(file_name);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
        if (ligne==param_name){
            std::getline(strim, ligne);
            param = atof(ligne.c_str());
            break;
        }
    }
    return param;
}

// Loads a str in a text parameter file. If the File is as follows :
//
//  Blabla
//  super
//
//  E
//  18
//
// Then this functions looks for the first line that is equal to "param_name" and takes the next line to be the output. Ex : load_param(filename, "E") will be the std::string "super".
std::string load_param_str(std::string file_name, std::string param_name){
    std::ifstream strim(file_name.c_str());
    std::string ligne;
    std::string param;
    int numlines=count_lines(file_name);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
        if (ligne==param_name){
            std::getline(strim, ligne);
            param = ligne;
            break;
        }
    }
    return param;
}

// Loads a text file into a double vector. One line for each element of the vector.
Vec<double> load_res(std::string file_name){
    std::ifstream strim(file_name.c_str());
    std::string ligne;
    Vec<double> resultat;
    
    int numlines=count_lines(file_name);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
	double a;
	std::istringstream convert(ligne);
	convert >> a;
        resultat.push_back(a);
    }
    return resultat;
}


// Loads a text file into a double vector of vectors. One line for each element of the vector of vectors.
Vec<Vec<double> >  load_vecvec(std::string filename){
    std::ifstream strim(filename.c_str());
    std::string ligne;
    Vec<Vec<double> > res;
    int numlines=count_lines(filename);
    for (int i = 0; i<numlines; i++){
	Vec <double> tmp;
        std::getline(strim, ligne);
	if (ligne.size() == 0)
	   bool dummy;
	else {
	    Vec<std::size_t> nplaces = find_str_in_str(ligne, " ");
	    Vec<int> endspaces, begspaces;
	    for (int ii = 0; ii<nplaces.size()-1; ii++){
		if (nplaces[ii+1] != nplaces[ii]+1){
		    endspaces << nplaces[ii];
		    begspaces << nplaces[ii+1];
		}
	    }
	    for (int ii = 0; ii<begspaces.size(); ii++){
		std::string tmpstr = ligne.substr(endspaces[ii]+1,begspaces[ii]-1-endspaces[ii]);
		std::istringstream iss(tmpstr);
		double number;
		iss >> number;
		tmp << number;
	    }
	    if (nplaces[nplaces.size()-1]<ligne.size()){
		std::string tmpstr = ligne.substr(nplaces[nplaces.size()-1]+1,ligne.size()-nplaces[nplaces.size()-1]);
		std::istringstream iss(tmpstr);
		double number;
		iss >> number;
		tmp << number;
	    }
	    res.push_back(tmp);
	}
    }
    return res;
}

// Writes a Mat in a text file
void write_mat (Mat<double> M, std::string filename){
    std::ofstream mat (filename.c_str());
    for (int ii = 0; ii < M.col(0).size(); ii++){
        for (int jj = 0; jj < M.row(0).size(); jj++){
            mat << double(M(ii,jj)) << " " ;
        }
        mat << "\n";
    }
}

// Writes a Vec in a text file
void write_vec (Vec<double> V, std::string filename){
    std::ofstream vec (filename.c_str());
    for (int ii = 0; ii < V.size(); ii++){
        vec << double(V[ii]) << " " ;
        vec << "\n";
    }
}
// Compute the distance between a point and a segment (defined by its two extreme points).
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

// Selection of the nodes where boundary conditions are applied in a computation. 
// If segments have been selected, returns a vector containing the number of the segment associated with the node (else the Vector will be empty)
Vec <double> select_cn (Vec<TM> &meshes, MP ch, std::string &CL, int nbs, Vec <int> &constrained_nodes, double pix2m){

	    int numbc=0;
	    Vec<int> indices_bc_cn, bc_num, units;
	    Vec< Vec< double > > vdx, vdy;
	    if (CL=="picked"){
	      
		MP bs = ch[ nbs ];
		MP bs2 = bs[ 2 ]; // va chercher la liste dans le bsitem
		Vec < Vec < Vec < double > > >  coor_nds_bc;
		if (bs2.size() == 0)
		    CL = "nope";
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
			
			MP BCType = morceau["_children[0]"];
			int choice = BCType["Choice.num"];
			int unit = BCType["Input_unit.num"];
			units << unit;
			if (choice == 0) // from field
			    bool dummy; // do nothing
			else if (choice == 1){ // disps drom files
			    bc_num << numbc;
			    QString Qdx = BCType["Displacement_X"]; 
			    std::string dxfile ;
			    dxfile = Qdx.toStdString();
			    Vec <double> dx = load_res(dxfile);
			    vdx.push_back(dx);
			    QString Qdy = BCType["Displacement_Y"]; 
			    std::string dyfile ;
			    dyfile = Qdy.toStdString();
			    Vec <double> dy = load_res(dyfile);
			    vdy.push_back(dy);
			}
		    }
		    numbc++;
		}
		meshes[0].update_skin();
		for( int nds = 0; nds < meshes[0].skin.node_list.size(); ++nds ){ //  Sélection des noeuds contraints
		      bool close_to_the_border = 0 ;
		      int indice;
		      for (int nbc = 0; nbc < coor_nds_bc.size(); nbc++){
			      Vec < double > pos_nd = meshes[0].skin.node_list[ nds ].pos;
			      double dist = calc_dist_segment( pos_nd, coor_nds_bc[nbc]);
			      if (dist < 0.001){
				  close_to_the_border = 1 ;
				  indice = nbc;
			      }
		      }
		      if (close_to_the_border){
			    constrained_nodes << meshes[0].skin.node_list[ nds ].number;
			    indices_bc_cn << indice;
			    for (int nbc = 0; nbc < bc_num.size(); nbc++){ // modification of the input (measured) mesh if the bc is taken from a file
			    // nbc is the number of bc from text files
				  if (indice == bc_num[nbc]){
				      double coef;
				      if (units[nbc] == 0) coef=pix2m;
				      else if (units[nbc] == 1) coef=1;
				      if ((vdx[nbc].size() >= meshes.size()) and (vdy[nbc].size() >= meshes.size())){
					  for (int num_mesh =0; num_mesh < meshes.size(); num_mesh++){
					      meshes[num_mesh].node_list[ meshes[0].skin.node_list[ nds ].number ].dep[0] = vdx[nbc][num_mesh]/coef;
					      meshes[num_mesh].node_list[ meshes[0].skin.node_list[ nds ].number ].dep[1] = vdy[nbc][num_mesh]/coef;
					      meshes[num_mesh].node_list[ meshes[0].skin.node_list[ nds ].number ].is_on_skin = 1;
					  }
				      }
				      else PRINT("NOT ENOUGH INFORMATION IN THE TEXTE FILE (BC" + to_string(nbc) +")");
				  }
			    }
		      }
		}
	     }
	     if (CL=="nope"){
		for( int i = 0; i < meshes[0].skin.node_list.size(); ++i )
		    constrained_nodes << meshes[0].skin.node_list[ i ].number;
	     }
	     return indices_bc_cn;
}

// Loads a MeshItem in a 2D LMTpp Mesh object
TM load_MeshMP_into_2DLMTpp_Mesh(MP mesh, std::string typeitem){ 
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

                        o[ j ] = qMin( indices->operator[]( ct++ ), (int)dic_mesh.node_list.size() -1 );
                    }
                    dic_mesh.add_element( Triangle(), DefaultBehavior(), o );
                }
            }
        }
    }
    //dic_mesh.remove_unused_nodes();
    return dic_mesh;
}


// Loads a FieldSetItem in a 2D LMTpp Mesh Vector (with displacements)
Vec<TM> load_FieldSetItem_into_LMTpp_Mesh(FieldSet fs_input){
    Mesh_vecs maillage = fs_input.mesh;
    MP maillage_transfert = maillage.save();
    TM dic_mesh = load_MeshMP_into_2DLMTpp_Mesh(maillage_transfert, "FieldSetItem");
    Vec<TM> Mesh_vector_input;
    PRINT(fs_input.fields[0].values.size());
    Mesh_vector_input.resize(fs_input.fields[0].values.size()); 
    for (int num_mesh = 0; num_mesh < Mesh_vector_input.size(); num_mesh++){
        Mesh_vector_input[num_mesh]=dic_mesh;
        for (int no = 0; no < Mesh_vector_input[num_mesh].node_list.size(); no++ ) {
            Mesh_vector_input[num_mesh].node_list[no].dep[0] = fs_input.fields[0].values[num_mesh].data[no];
            Mesh_vector_input[num_mesh].node_list[no].dep[1] = fs_input.fields[1].values[num_mesh].data[no];
        }
        Mesh_vector_input[num_mesh].update_skin();        
    }
    return Mesh_vector_input;
}

// Adds a vec(vec(double)) containing the displacements from a vec(mesh) to a vector of vec(vec(double))
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


// Adds a vec(vec(double)) containing the nodal forces (at some constrained_nodes) from a vec(mesh) to a vector of vec(vec(double))
void extract_fnod_from_LMTppMesh ( Vec<TM> res_ref, std::string senstrac , Vec < Vec <Vec<double> > > &calc_force , Vec<int> constrained_nodes){

    Vec <Vec<double> > fnodaltot;
    for( int num_mesh = 0; num_mesh < res_ref.size(); ++num_mesh ) {
	Vec<double> fnodal;
        for( int n = 0; n < constrained_nodes.size(); ++n ){
	    if (senstrac == "hor")
		fnodal << res_ref[num_mesh].node_list[constrained_nodes[n]].f_nodal[0];
	    else if (senstrac == "ver")
		fnodal << res_ref[num_mesh].node_list[constrained_nodes[n]].f_nodal[1];
	    else if (senstrac == "nope")
		fnodal << pow(pow(res_ref[num_mesh].node_list[constrained_nodes[n]].f_nodal[0],2) + pow(res_ref[num_mesh].node_list[constrained_nodes[n]].f_nodal[1],2), 0.5);
        }
        fnodaltot.push_back(fnodal);
    } 
    
    calc_force.push_back(fnodaltot);
}

void extract_f_from_LMTppMesh (Vec<TM> Mesh_Vector_output, Vec<int> constrained_nodes, Vec<int> indices_bc_cn, Vec <Vec <double> > &calc_force){
  
    Vec <Vec <Vec <double> > > calc_force_nodes;
    extract_fnod_from_LMTppMesh( Mesh_Vector_output, "ver", calc_force_nodes, constrained_nodes );
    
    calc_force.resize(max(indices_bc_cn) + 1 - min(indices_bc_cn));
    
    for (int ncl = min(indices_bc_cn); ncl < max(indices_bc_cn)+1; ncl++){
	calc_force[ncl].resize(Mesh_Vector_output.size());
	for (int nim =0; nim<Mesh_Vector_output.size(); nim ++){
	    calc_force[ncl][nim] =0;
	    for (int nn =0; nn<calc_force_nodes[0][0].size(); nn++){
		if (indices_bc_cn[nn] == ncl){
		    calc_force[ncl][nim] += calc_force_nodes[0][nim][nn];
		}
	    }
	}
    }
}
// Loads images from a MP children
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
                        //return false;
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

// Extracts the properties to be identified (related to the position of the property in the Prop_Mat)
void extract_id_properties( MP mp, Vec < Vec < std::string > > Prop_Mat, Vec<int> &prop2id){
  
   int cochea, cochen, coches;
   MP ch = mp[ "_children" ]; 
   for( int ii = 0; ii < ch.size(); ++ii ) {
	MP c = ch[ ii ];
	if ( c.type() == "Material_Code_Aster_Item" ) {
		coches = c["sigma_y[1]"];
		cochea = c["a[1]"];
		cochen = c["n[1]"];
	}
   }
	  
    std::string thelaw = Prop_Mat[0][1];
    
    if (thelaw == "Elas_iso"){
	prop2id << 2; // nu
    }
    else if (thelaw == "Elas_ortho"){
	prop2id << 2;// nu
	prop2id << 3;// E1/E2
    }
    else if (thelaw == "Powerlaw"){
	if (cochea == 1)
	  prop2id << 3;// a
	if (cochen == 1)
	  prop2id << 4;// n
	if (coches == 1)
	  prop2id << 5;// sigma_y
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

// Extracts the computation properties in a computation Item (e.g. AbaqusDataItm or Code_Aster_DataItem) : mesh, boundary conditions, material...
void extract_computation_parameters( MP mp, Vec<TM> &Mesh_vector_input, Vec<int> &constrained_nodes,  Vec<int> &indices_bc_cn, Vec < Vec < std::string > > &Prop_Mat, FieldSet &fs_input_bckp, Vec<Vec<std::string> > &force_files){
    
    MP ch = mp[ "_children" ]; 
    double pix2m = mp[ "pix2m" ];
    double Young, Poisson, loi, rapport, sigma_0, n, ct, sign, sigma_y, a ;
    std::string param_file, umatname, computation_type;
    int umat_ndepvar, umat_nparam, umat_nparamid, nparam, nparamid;
    MP mat;
    ct = mp["computation_type.num"];
    int ex_field =0; 
    
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
	    ex_field++;
	    FieldSet fs_input(c); 
	    fs_input_bckp.load(c);
	    Mesh_vector_input = load_FieldSetItem_into_LMTpp_Mesh(fs_input);
	    indices_bc_cn = select_cn (Mesh_vector_input,  ch,  CL, nbs, constrained_nodes, pix2m);
	}
	else if (( c.type() == "MeshItem" ) ){
	    ex_field++;
	    Mesh_vecs maillage; maillage = c["_mesh"];
	    MP maillage_transfert = maillage.save();
	    TM mesh = load_MeshMP_into_2DLMTpp_Mesh(maillage_transfert, "MeshItem");
	    double n_timesteps = mp["n_timesteps"];
	    fs_input_bckp.mesh = maillage;
	    for (int ts = 0; ts < n_timesteps; ts++)
		Mesh_vector_input << mesh;
	    indices_bc_cn = select_cn (Mesh_vector_input,  ch,  CL, nbs, constrained_nodes, pix2m);
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
	
	}
	else if ( c.type() == "Material_Code_Aster_Item" ) {
	    mat=c;
	    loi = c["Comportement.num"];
	    Young = c["Young[0]"];
	    Poisson = c["Poisson[0]"];
	    if (ct == 0)
		computation_type = "2DPS";
	    else if (ct == 1)
		computation_type = "2DPE";
	    else if (ct == 2)
		computation_type = "3Dex";
	    if (loi == 1){ // Powerlaw
		sigma_y = c["sigma_y[0]"];
		int coches = c["sigma_y[1]"];
		a = c["a[0]"];
		int cochea = c["a[1]"];
		n = c["n[0]"];
		int cochen = c["n[1]"];
	    }
	    
	    if (loi == 0) // Elas_iso
		Prop_Mat.resize(3);
	    if (loi == 1) // Powerlaw
		Prop_Mat.resize(6);
	    
	    Prop_Mat[0] << "thelaw";
	    if (loi == 0){
		Prop_Mat[0] << "Elas_iso";
		Prop_Mat[1] << "Young";
		Prop_Mat[1] << LMT::to_string(Young*1e9); 
		Prop_Mat[2] << "Poisson";
		Prop_Mat[2] << LMT::to_string(Poisson); 
	    }
	    else if (loi == 1){
		Prop_Mat[0] << "Powerlaw";
		Prop_Mat[1] << "Young";
		Prop_Mat[1] << LMT::to_string(Young*1e9); 
		Prop_Mat[2] << "Poisson";
		Prop_Mat[2] << LMT::to_string(Poisson); 
		Prop_Mat[3] << "a";
		Prop_Mat[3] << LMT::to_string(a); 
		Prop_Mat[4] << "n";
		Prop_Mat[4] << LMT::to_string(n); 
		Prop_Mat[5] << "sigma_y";
		Prop_Mat[5] << LMT::to_string(sigma_y*1e6); 
	    }
	}
    }
    
    
    // REECRITURE DES PROPRIETES MATERIAU DANS UN FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])

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
    if (ex_field > 1)
	PRINT("MORE THAN 1 INPUT FIELD, POSSIBLE CONFUSION");
    
    MP bs = ch[ nbs ];
    MP chil = bs[ "_children" ]; 
    force_files.resize(chil.size());
    for( int ii = 0; ii < chil.size(); ++ii ) {
	MP c = chil[ ii ];
	QString Qforce_file = c["force"]; 
	force_files[ii] << Qforce_file.toStdString();
	sign = c["force_sign.num"];
	if (sign == 0)
	   force_files[ii] << "+";
	else if (sign == 1)
	   force_files[ii] << "-";
	else if (sign == 2)
	   force_files[ii] << "0";
    }
    
}

// Creates a vec(vec(double) with the displacements from a Mesh Vector
Vec<Vec<double> >  fill_disp_vectors_with(Vec<TM> Mesh_Vector){
  
	Vec<Vec<double> > disp;
	for (int num_mesh = 0; num_mesh < Mesh_Vector.size(); ++num_mesh ) {
	      Vec<double> disp_tmp;
	      for( int n = 0; n < Mesh_Vector[ num_mesh ].node_list.size(); ++n ) {
		  for( int d = 0; d < TM::dim; ++d ) {
		      disp_tmp << Mesh_Vector[ num_mesh ].node_list[ n ].dep[ d ];
		  } 
	      }
	      disp.push_back(disp_tmp);
	}
	return disp;
	
}

// build_matrix(to be inverted for ID)_for_the_kinematic_part : integrated dic or femu.
void build_matrix_for_the_kinematic_part(Mat<double,Sym<>,SparseLine<> > &M_red, Vec<double> &F_red, Vec<TM> &Mesh_Vector_input, Vec<TM> &Mesh_Vector_output, Vec<I2> images, Vec <Vec <Vec <double> > > comp_disp, double pix2m, double offset, std::string method){	
	
	Vec<Vec<double> > meas_disp = fill_disp_vectors_with(Mesh_Vector_input) * pix2m;
	for( int num_mesh = 0; num_mesh < Mesh_Vector_input.size(); ++num_mesh ) { // num_mesh <=> steps
	  
	    Vec<Vec<double> > der;
	    //// transfert des CS dans "der"
	    for (int nsf=1; nsf<comp_disp.size(); nsf++){
		Vec < Vec < double > > CS;
		CS = (comp_disp[0] - comp_disp[nsf])/offset/pix2m;
		Vec <double> temp;
		for (int ii=0+num_mesh*Mesh_Vector_input[ num_mesh ].node_list.size(); ii< Mesh_Vector_input[ num_mesh ].node_list.size()+num_mesh*Mesh_Vector_input[ num_mesh ].node_list.size(); ii++){
		    //PRINT(ii);
		    //PRINT(CS[0][ii]);  
		    //PRINT(CS[1][ii]);
		    temp.push_back(CS[0][ii]);
		    temp.push_back(CS[1][ii]);
		}
		der.push_back(temp);
	    }
	    
            if (method == "int"){
		DicCPU<double,2> dic;
		dic.ass_red( images[ 0 ], images[ num_mesh + 1 ], Mesh_Vector_output[ num_mesh ], dep_DM(), lum_DM(), der ); // assemblage
                M_red += dic.M_red;
                F_red += dic.F_red;
            }
            else if (method == "femu"){
		if (num_mesh == 0)
		    PRINT("     NO WEIGHTING WITH THE CORRELATION MATRIX ");
		Vec<double> deplcal;
		for( int n = 0; n < Mesh_Vector_output[ num_mesh ].node_list.size(); ++n ) {
		    for( int d = 0; d < TM::dim; ++d ) {
			deplcal << Mesh_Vector_output[ num_mesh ].node_list[ n ].dep[ d ];
		    }
		}
		
		Vec<double> depmes;
		depmes=meas_disp[num_mesh];
		
		for( int r = 0; r < F_red.size(); ++r ) {
		    for( int c = 0; c <= r; ++c ){
			M_red( r, c ) += dot( der[ r ], der[ c ] );
		    }
		    F_red[ r ] += dot( der[ r ], (depmes - deplcal)/pix2m );
		}
	    }
	    
	}
	
}
// build_matrix(to be inverted for ID)_for_the_force_part, for all the boundary conditions segments selected
void build_matrix_for_the_force_part(Vec<Mat<double, Sym<> ,SparseLine<> > > &VMF, Vec <Vec<double> > &VFF, Vec<Vec< std::string> > force_files, Vec <Vec<double> > calc_force, Vec <Vec <Vec <double> > > calc_force_nodes, Vec<int> indices_bc_cn, double n_im, double n_prop, Vec <Vec <Vec <double> > > comp_disp, double pix2m, double offset, std::string method){
		
		Vec<double> meas_force, res_f;
		
		// transfert from nodal forces to global forces associated to the bc groups
		for (int ncl = min(indices_bc_cn); ncl < max(indices_bc_cn)+1; ncl++){
		    meas_force = load_res(force_files[ncl][0]);
		    meas_force.resize(calc_force_nodes[0].size());
		    calc_force.resize(calc_force_nodes.size());
		    if (force_files[ncl][1] == "-")
			meas_force = - meas_force;
		    else if (force_files[ncl][1] == "0")
			meas_force *= 0;
		    Mat<double,Sym<>,SparseLine<> > MF_red( n_prop );
		    Vec<double> FF_red; FF_red.resize( n_prop, 0 );
		    calc_force_nodes.resize(comp_disp.size());
		    for (int nsf =0; nsf<comp_disp.size(); nsf ++){
			calc_force[nsf].resize(n_im);
			for (int nim =0; nim<n_im; nim ++){
			    calc_force[nsf][nim] =0;
			    for (int nn =0; nn<calc_force_nodes[0][0].size(); nn++){
				
				if (indices_bc_cn[nn] == ncl){
				    calc_force[nsf][nim] += calc_force_nodes[nsf][nim][nn];
				}
			    }
			}
		    }
		    
		    res_f=meas_force-calc_force[0];
		    PRINT(meas_force);
		    PRINT(calc_force[0]);
		    PRINT(res_f);
		    //write_vec(calc_force[0],"dernier_effort_calc.txt");

		    for( int r = 0; r < n_prop; ++r ) {
			for( int c = 0; c <= r; ++c ){
			    MF_red( r, c ) += dot( (calc_force[ 0 ] - calc_force [ r + 1 ]) / offset, (calc_force[ 0 ] - calc_force [ c + 1 ]) / offset );
			}
			FF_red[ r ] += dot( (calc_force[ 0 ] - calc_force [ r + 1 ]) / offset, res_f );
		    }
		    VMF << MF_red;
		    VFF << FF_red;
		    
		}
}

// assemble kinematic and force parts of an ID problem
void assemble_global_matrix (Mat<double,Sym<>,SparseLine<> > &M_tot, Vec<double> &F_tot, Mat<double,Sym<>,SparseLine<> > M_red, Vec<double> F_red, Vec<Mat<double, Sym<> ,SparseLine<> > > VMF, Vec <Vec<double> > VFF, bool UF, Vec<int> indices_bc_cn, double ponderation_efforts, double w){
	
	M_tot = M_red;
	F_tot = F_red ;
	if (UF){
		for (int ncl = min(indices_bc_cn); ncl < max(indices_bc_cn)+1; ncl++){
    //	        int ncl = 1;
		    M_tot +=  ponderation_efforts * VMF[ncl] * w / (max(indices_bc_cn)+1);
		    F_tot +=  ponderation_efforts * VFF[ncl] * w / (max(indices_bc_cn)+1);
		    //M_tot =  ponderation_efforts * VMF[ncl] * w / (max(indices_bc_cn)+1);
		    //F_tot =  ponderation_efforts * VFF[ncl] * w / (max(indices_bc_cn)+1);
		}
	}
	
 	PRINT(M_red);
 	PRINT(F_red);
	
	if (UF){
	    std::cout << " " << std::endl;
	    PRINT(VMF);
	    PRINT(VFF);
	    std::cout << " " << std::endl;	
	    PRINT(M_tot);
	    PRINT(F_tot);
	}
}

// Solves an inverse problem using_eig_sym ; result (dif) limited to a min/max_level for each component
Vec<double> solve_with_max(Mat<double,Sym<>,SparseLine<> > M_tot, Vec<double> F_tot, double max_level, double resolution, double relaxation){

	Vec<double> dif = - relaxation * solve_using_eig_sym( M_tot , F_tot , resolution ); // RESOLUTION dp=N-1 B
	PRINT(dif);

	for (int jj=0; jj< dif.size(); jj++){
	    if (dif[jj] >  max_level){
		dif[jj]= max_level;
		PRINT( dif );
	    }
	    if (dif[jj] < -max_level){
		dif[jj]=-max_level;
		PRINT( dif );
	    }
	}
	return dif;
}

// Updates material propoerties of an ID problem after solving
void update_properties(Vec < Vec < std::string > > &Prop_Mat, Vec < Vec < std::string > > Prop_Mat_Backup, Vec<int> prop2id, Vec<double> dif, std::string thelaw){

        Prop_Mat = Prop_Mat_Backup;
	// Properties update
	for (int nsf = 0; nsf < dif.size(); nsf++){
	    double prop;
	    std::string texte = Prop_Mat[prop2id[nsf]][1];
	    std::istringstream iss(texte);
	    iss >> prop;
	    prop *= (1 + dif[nsf]);
	    if (thelaw == "RO"){
		if (Prop_Mat[prop2id[nsf]][1] == "n"){
		    if (prop < 1.01)
		       prop =1.1;
		}
	    }
	    Prop_Mat[prop2id[nsf]][1] = LMT::to_string(prop);
	}
  
}

// Puts a string in a double
void put_string_in_double(std::string str, double &prop){

    std::istringstream iss(str);
    iss >> prop;
    
}

// Send a set of material properties to a MP
void push_back_material_parameters( MP &mp, Vec < Vec < std::string > > Prop_Mat){

    MP ch = mp[ "_children" ]; 
    double prop;
    std::string thelaw = Prop_Mat[0][1];
    for( int ii = 0; ii < ch.size(); ++ii ) {
	MP c = ch[ ii ];
	if ( c.type() == "MaterialABQItem" ) {
	    if ((thelaw == "Elas_iso") or (thelaw == "Elas_ortho") or (thelaw == "RO")){
		put_string_in_double(Prop_Mat[1][1], prop);
		PRINT(prop);
		c["Young[0]"]=prop;
		put_string_in_double(Prop_Mat[2][1], prop);
		PRINT(prop);
		c["Poisson[0]"]=prop;
	    }
	    if ((thelaw == "Elas_ortho")){
		put_string_in_double(Prop_Mat[3][1], prop);
		c["rapport[0]"]=prop;
	    }
	    if ((thelaw == "RO")){
		put_string_in_double(Prop_Mat[3][1], prop);
		c["sigma_0[0]"]=prop/1e6;
		put_string_in_double(Prop_Mat[4][1], prop);
		c["n[0]"]=prop;
	    }
	}
	if ( c.type() == "Material_Code_Aster_Item" ) {
	    if ((thelaw == "Elas_iso") or (thelaw == "Powerlaw")){
		put_string_in_double(Prop_Mat[1][1], prop);
		c["Young[0]"]=prop/1e9;
		put_string_in_double(Prop_Mat[2][1], prop);
		c["Poisson[0]"]=prop;
	    }
	    if ((thelaw == "Powerlaw")){
		put_string_in_double(Prop_Mat[3][1], prop);
		c["a[0]"]=prop;
		put_string_in_double(Prop_Mat[4][1], prop);
		c["n[0]"]=prop;
		put_string_in_double(Prop_Mat[5][1], prop);
		c["sigma_y[0]"]=prop/1e6;
	    }
	}
    }
    mp.flush();
    
}

MP soda_mesh_from_lmtpp_mesh( const TM &m ) {
    MP om = MP::new_obj( "Mesh" );
    om[ "points" ] = MP::new_lst( "Lst_Point" );
    om[ "_elements" ] = MP::new_lst();
    om[ "_selected_points" ] = MP::new_lst();
    om[ "_pelected_points" ] = MP::new_lst();
    om[ "_selected_elements" ] = MP::new_lst();
    om[ "_pelected_elements" ] = MP::new_lst();

    // nodes
    for( int i = 0; i < m.node_list.size(); ++i ) {
        MP pos = MP::new_lst( "Vec_3" );
        for( int d = 0; d < 3; ++d )
            pos << m.node_list[ i ].pos[ d ];

        MP pnt = MP::new_obj( "Point" );
        pnt[ "pos" ] = pos;

        om[ "points" ] << pnt;
    }

    // elements
    TypedArray<int> *tr_con = new TypedArray<int>;
    TypedArray<int> *te_con = new TypedArray<int>;
    for( int n = 0; n < m.elem_list.size(); ++n ) {
        if ( m.elem_list[ n ]->nb_nodes_virtual() == 3 )
            for( int i = 0; i < m.elem_list[ n ]->nb_nodes_virtual(); ++i )
                tr_con->_data << m.elem_list[ n ]->node_virtual( i )->number;
        else if ( m.elem_list[ n ]->nb_nodes_virtual() == 4 )
            for( int i = 0; i < m.elem_list[ n ]->nb_nodes_virtual(); ++i )
                te_con->_data << m.elem_list[ n ]->node_virtual( i )->number;
    }

    for( int n = 0; n < m.skin.elem_list.size(); ++n )
        if ( m.skin.elem_list[ n ]->nb_nodes_virtual() == 3 )
            for( int i = 0; i < m.skin.elem_list[ n ]->nb_nodes_virtual(); ++i )
                tr_con->_data << m.skin.elem_list[ n ]->node_virtual( i )->number;

    // triangle
    tr_con->_size.resize( 2 );
    tr_con->_size[ 0 ] = 3;
    tr_con->_size[ 1 ] = tr_con->_data.size() / 3;

    MP triangles = MP::new_obj( "Element_TriangleList" );
    triangles[ "indices" ] = tr_con;
    om[ "_elements" ] << triangles;

    // tetra
    te_con->_size.resize( 2 );
    te_con->_size[ 0 ] = 4;
    te_con->_size[ 1 ] = te_con->_data.size() / 4;

    MP tetrahedra = MP::new_obj( "Element_TetrahedraList" );
    tetrahedra[ "indices" ] = te_con;
    om[ "_elements" ] << tetrahedra;

    return om;
}

struct GetEps {
    template<class TE,class TD>
    void operator()( const TE &e, int i, TD *data, int d ) const {
        data->operator[]( i ) = e.epsilon[ 0 ][ d ];
    }
};

// Send a vec(Mesh) with its displacements to a MP
void put_result_in_MP (Vec<TM> meshes, MP &mp, FieldSet &fs_output){// SORTIE DANS UN FieldSet "calcul"

    DicCPU<double, TM::dim> dic;
    
    MP param = mp["_children[0]"];
    MP ch = param["_children"];
    double pix2m = param[ "pix2m" ];
    MP maillage_transfert;
    if (fs_output.fields.size() == 0){
	for( int ii = 0; ii < ch.size(); ++ii ) {
	    MP c = ch[ ii ];
	    if (( c.type() == "MeshItem" )){
		maillage_transfert.reassign( c["_mesh"] );
	    }
	}
	MP output_field = mp[ "_output[ 0 ]" ];
	QVector<MP> displacements = make_field( output_field, TM::dim, "Displacement" );
	    
 	for (int num_mesh = 0; num_mesh < meshes.size(); num_mesh++){
	    dic.get_epsilon( meshes[num_mesh] );
	    for( int d = 0; d < TM::dim; ++d ) {
		QVector<int> s; s << meshes[0].node_list.size();
		TypedArray<double> *data = new TypedArray<double>( s );
		for( int i = 0; i < meshes[0].node_list.size(); ++i ){
		    data->operator[]( i ) = meshes[num_mesh].node_list[ i ].dep[ d ];
		}
		
		add_field_in_Interpolated( displacements[ d ], maillage_transfert, data, num_mesh + 1 );
	    }
	}
    }
    else {
	for (int num_mesh = 0; num_mesh < meshes.size(); num_mesh++){
	    for (int no = 0; no < meshes[num_mesh].node_list.size(); no++ ) {
		for (int dim =0; dim < TM::dim; dim++){
		    fs_output.fields[dim].values[num_mesh].data[no] = meshes[num_mesh].node_list[no].dep[dim]/pix2m;
		}
	    } 
	}
	fs_output.save(mp["_output[0]"]);
    }
}

// During the computation of sensitivity fields, gives bak a Prop_Mat vector which is the reference one modified for the "sf"th parameter
Vec < Vec < std::string > > update_material_parameters(Vec < Vec < std::string > > Prop_Mat_Backup, Vec<int> prop2id, int sf, double offset){

	    Vec < Vec < std::string > > Prop_Mat = Prop_Mat_Backup;
	    std::cout << "Sensitivity field " << sf << " : " << Prop_Mat[prop2id[sf]][0] << std::endl;
	    double prop;
	    std::string texte = Prop_Mat[prop2id[sf]][1];
	    std::istringstream iss(texte);
	    iss >> prop;
	    prop *= (1 + offset);
	    Prop_Mat[prop2id[sf]][1] = LMT::to_string(prop); 
	    PRINT(Prop_Mat[prop2id[sf]][0]);
	    PRINT(Prop_Mat[prop2id[sf]][1]);
	    return Prop_Mat;
  
}

// In the case of an elastic ID problem(Poisson ratio only identified), cross-multiplication to deduce the Young modulus
void calc_young_for_elastic_case(Vec<int> indices_bc_cn,  Vec<Vec< std::string> > force_files, Vec <Vec<double> > calc_force, Vec <Vec <Vec <double> > > calc_force_nodes, double n_prop, double n_im, Vec < Vec < std::string > > &Prop_Mat){

		std::cout << " " << std::endl;
		
		Vec<double> meas_force, res_f, coef_vec;
		for (int ncl = min(indices_bc_cn); ncl < max(indices_bc_cn)+1; ncl++){
		    if (exists(force_files[ncl][0])){
			meas_force = load_res(force_files[ncl][0]);
			calc_force.resize(calc_force_nodes.size());
			if (force_files[ncl][1] == "-")
			    meas_force = - meas_force;
			else if (force_files[ncl][1] == "0")
			    meas_force *= 0;
			calc_force_nodes.resize(n_prop);
			for (int nsf =0; nsf<n_prop; nsf ++){
			    calc_force[nsf].resize(n_im);
			    for (int nim =0; nim<n_im; nim ++){
				calc_force[nsf][nim] =0;
				for (int nn =0; nn<calc_force_nodes[0][0].size(); nn++){
				    if (indices_bc_cn[nn] == ncl){
					calc_force[nsf][nim] += calc_force_nodes[nsf][nim][nn];
				    }
				}
			    }
			}
			meas_force.resize(calc_force[0].size());
			
			res_f=meas_force-calc_force[0];
			PRINT(meas_force);
			PRINT(calc_force[0]);
			PRINT(res_f);
			double coef = 0;
			if (meas_force[0] != 0)
			    coef = pow(dot(calc_force[0],calc_force[0])/ dot(meas_force, meas_force),0.5);
			coef_vec << coef;
			PRINT(coef);
		    }
		}
		
		if (res_f.size() != 0){
		    std::string texte = Prop_Mat[1][1];
		    std::istringstream iss(texte);
		    double prop;
		    iss >> prop;
		    prop /=  coef_vec[0];
		    Prop_Mat[1][1] = LMT::to_string(prop/1e9); 
		    PRINT(Prop_Mat[1][1]);
		}
	
		std::cout << " " << std::endl;
  
}

// write_identification_report
void write_identification_report (std::string report_address, Vec<TM> &Mesh_Vector_output, Vec <Vec<std::string> > Prop_Mat, int it, int iterations, Mat<double,Sym<>,SparseLine<> >M_d, Vec<Mat<double,Sym<>,SparseLine<> > >M_f, Vec<double>F_d, Vec <Vec<double> > F_f, Vec <Vec<double> > calc_force, Vec <Vec<double> > meas_force, Vec<int>prop2id, double ponderation, Vec<double> dif){
    
    bool ok=0;
    int num_report = 1 ;
    while (ok==0)
	if (exists((report_address + "_" + to_string(num_report) + ".txt").c_str()))
	    num_report++;
	else ok = 1;
	
    int refsize;
    if (F_f.size()!=0) refsize = F_f[0].size();
    else refsize = F_d.size();
	
    report_address = report_address + "_" + to_string(num_report) + ".txt";
    int value = system(("touch " + report_address).c_str());
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
    
    Mat<double,Sym<>,SparseLine<> > M_tot = M_d;
    Vec<double> F_tot = F_d ;
    for (int ncl = 0; ncl < M_f.size(); ncl++){
	M_tot +=  ponderation * M_f[ncl] / M_f.size();
	F_tot +=  ponderation * F_f[ncl] / M_f.size();
    }
    report << "The global matrix assembled from the previous ones was :\n";
    for (int ii = 0; ii < M_d.col(0).size(); ii++){
        for (int jj = 0; jj < M_d.row(0).size(); jj++){
            report << double(M_tot(ii,jj)) << " " ;
        }
        report << "\n";
    }
    
    bool inversible=1;
    for (int ii = 0; ii < M_tot.col(0).size(); ii++){
        for (int jj = 0; jj < M_tot.row(0).size(); jj++){
	    if ((M_tot(ii,jj) == 0) and (inversible)){
		PRINT("One of the parameters seems to have no influence on the result. Please dismiss it from the analysis");
		report << "One of the parameters seems to have no influence on the result. Please dismiss it from the analysis";
		inversible = 0;
	    }
	}
    }
    
    if (inversible){
	Mat<double > M_cov = 2*true_inv(M_tot);
	report << "\n";
	report << "The global covariance matrix was :\n";
	for (int ii = 0; ii < refsize; ii++){
	    for (int jj = 0; jj < refsize; jj++){
		report << double(M_cov(ii,jj)) << " " ;
	    }
	    report << "\n";
	}
	Mat<double > M_cov_norm; M_cov_norm.resize(refsize);
	for( int r = 0; r < refsize; ++r ) {
	    for( int c = 0; c < refsize; ++c ){
		M_cov_norm(r,c) = M_cov(r,c)/(pow( abs(M_cov(r,r) * M_cov(c,c)) , 0.5));
	    }
	}
	report << "\n";
	report << "The global normalized covariance matrix was :\n";
	for (int ii = 0; ii < refsize; ii++){
	    for (int jj = 0; jj < refsize; jj++){
		report << double(M_cov_norm(ii,jj)) << " " ;
	    }
	    report << "\n";
	}
	
    }
    report << "\n";
    report << "\n";
    report << "\n";
    report << "\n";
    report << "\n";
  
  
}