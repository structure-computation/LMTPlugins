
#pragma cpp_path ../../../LMTpp
#pragma inc_path ../../../LMTpp 
#pragma cpp_path ../../../dic
#pragma inc_path ../../../dic



#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include <LMTpp/include/containers/vec.h>
#include "TestFloUpdater.h"
#include "dic/correlation/ImgInterp.h"
#include "LMTpp/include/mesh/mesh.h"
#include "dic/correlation/mesh_carac_correlation.h"
//#include "formulation.Ramberg_Osgood.Triangle.name_der_vars=param.h"
//#include "lib_flo.h"



//template<unsigned dim>
bool TestFloUpdater::run( MP mp ) {
    const int dim = 2;
    typedef LMT::ImgInterp<double,dim/*,ImgInterpOrder3_Kernel*/> I2;
    typedef LMT::Mesh<Mesh_carac_pb_correlation_basic<double,dim> > TM2;
    typedef typename TM2::Pvec Pvec;
    
    TM2 dic_mesh;
    
    int valeur1 = mp[ "valeur1" ];
    std::cout << valeur1 << std::endl;
    mp[ "valeur1" ]=100;
    add_message( mp, ET_Info, "Valeur 1 modifiée" );
    mp.flush();
    
    MP ch = mp[ "_children" ]; 
    for( int ii = 0; ii < ch.size(); ++ii ) {
        MP c = ch[ ii ];
        qDebug() << c.type();
        if ( c.type() == "MeshItem" ) {
            MP mesh = c["_mesh" ]; 
            add_message( mp, ET_Info, "Maillage récupéré" );
            mp.flush();
            qDebug() << "Nb de noeuds : " << mesh[ "points" ].size() << ", Nb d'éléments : " << mesh[ "_elements" ].size();
            //WriteINP (Vec<TM> &m, std::string racine_fic, std::string nom_fic_param, Vec<double> constrained_nodes, double pix2m);
            
            for( int i = 0, n = mesh[ "points" ].size(); i < n; ++i ) {
                MP pos = mesh[ "points" ][ i ][ "pos" ];
                dic_mesh.add_node( Pvec( pos[ 0 ], pos[ 1 ], pos[ 2 ] ) );
                qDebug() <<  pos[ 0 ];
            }
            for( int nel = 0, mel = mesh[ "_elements" ].size(); nel < mel; ++nel ) {
                MP el = mesh[ "_elements" ][ nel ];
                if ( el.type() == "Element_TriangleList" and dim == 2 ) {
                    if ( TypedArray<int> *indices = dynamic_cast<TypedArray<int> *>( el[ "indices" ].model() ) ) {
                        for( int nt = 0, ct = 0; nt < indices->size( 1 ); ++nt ) {
                            unsigned o[ 3 ];
                            for( int j = 0; j < 3; ++j )
                                o[ j ] = qMin( indices->operator[]( ct++ ), (int)dic_mesh.node_list.size() - 1 );
                            //dic_mesh.add_element( Triangle(), DefaultBehavior(), o );
                        }
                    }
                } else if ( el.type() == "Element_TetrahedraList" and dim == 3 ) {
                    if ( TypedArray<int> *indices = dynamic_cast<TypedArray<int> *>( el[ "indices" ].model() ) ) {
                        for( int nt = 0, ct = 0; nt < indices->size( 1 ); ++nt ) {
                            unsigned o[ 4 ];
                            for( int j = 0; j < 4; ++j )
                                o[ j ] = qMin( indices->operator[]( ct++ ), (int)dic_mesh.node_list.size() - 1 );
                            // dic_mesh.add_element( Tetra(), DefaultBehavior(), o );
                        }
                    }
                }
            }
        }
    }
    
    add_message( mp, ET_Info, "Travail terminé" );
    
    
}