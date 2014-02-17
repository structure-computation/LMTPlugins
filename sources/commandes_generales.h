
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


Vec<TM> load_FieldSetItem_into_LMTpp_Mesh(MP c){
    FieldSet fs_input(c);
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