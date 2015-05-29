#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtGui/QImage>
#include <QtCore/QtDebug>
#include "DicUncertaintyUpdater.h"
#include <mesh/make_rect.h>
#include <mesh/make_mesh_from_mask.h>
#include <correlation/DicCPU.h>
#include <sstream>
// #include <string.h>
#include "dec_image.h"
#include "make_fields.h"
#include "soda_mesh_from_lmtpp_mesh.h"

#include  "LMT/include/containers/mat.h"
//  puis utiliser l'espace LMT
using namespace std;
using namespace LMT;


bool DicUncertaintyUpdater::run( MP mp ) {
    
    MP ch = mp[ "_children" ];
    const unsigned int dim = 2;

    typedef DicCPU<double,dim>::TM_exemple TM;
    typedef TM::Pvec Pvec;
    typedef ImgInterp<double,dim/*,ImgInterpOrder3_Kernel*/> I2;
    typedef LMT::DicCPU<double,dim/*,false*/> DC;
    
    MP mesh;// = mp[ "_output[1]" ];
//     MP mesh = output[ "_mesh" ];
    TM dic_mesh;
    LMT::Vec<I2> images;
    Vec<int,dim> xmin (1e6,1e6);
    Vec<int,dim> xmax (-1e6,-1e6);
    // output
    //MP output_field = mp[ "_output[ 0 ]" ];
    
    
    
    for( int i = 0; i < ch.size(); ++i ) {
        MP c = ch[ i ];
        qDebug() <<  c.type() ;
        // Load reference image
        if ( c.type() == "ImgSetItem" ) {
            for( int j = 0; j < c[ "_children" ].size(); ++j ) {
                MP im = c[ "_children" ][ j ];
                if ( im.type() == "ImgItem" ) {
                    I2 *img = images.new_elem();
                    QString name = im[ "img.src" ];
//                     if ( im[ "img.src" ].type() != "Path" )
//                         name = "../CorreliOnline/html/" + name;

                    try {
                        img->load( name.toAscii().data() );
                        img->reverse_y();
                    } catch ( std::string msg ) {
                        add_message( mp, Updater::ET_Error, "Img " + name + " does not exist" );
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
                        updater->add_message( mp, Updater::ET_Error, "Img " + name + " does not exist" );
                        return false;
                    }
                    PRINT( name.toAscii().data() );
                }*/
            }
        }
        // Mesh generation
        // load existing mesh
        if ( int(mp[ "specific_mesh" ]) ){
            if ( c.type() == "DiscretizationItem" ) {
                mesh.reassign( c[ "_children._mesh" ] );
                for( int i = 0, n = mesh[ "points" ].size(); i < n; ++i ) {
                    MP pos = mesh[ "points" ][ i ][ "pos" ];
                    dic_mesh.add_node( Pvec( pos[ 0 ], pos[ 1 ], pos[ 2 ] ) );
                }

                for( int nel = 0, mel = mesh[ "_elements" ].size(); nel < mel; ++nel ) {
                    MP el = mesh[ "_elements" ][ nel ];
                    if ( el.type() == "Element_TriangleList" and dim == 2 ) {
                        if ( TypedArray<int> *indices = dynamic_cast<TypedArray<int> *>( el[ "indices" ].model() ) ) {
                            for( int nt = 0, ct = 0; nt < indices->size( 1 ); ++nt ) {
                                unsigned o[ 3 ];
                                for( int j = 0; j < 3; ++j )
                                    o[ j ] = qMin( indices->operator[]( ct++ ), (int)dic_mesh.node_list.size() - 1 );
                                dic_mesh.add_element( Triangle(), DefaultBehavior(), o );
                            }
                        }
                    } else if ( el.type() == "Element_TetrahedraList" and dim == 3 ) {
                        if ( TypedArray<int> *indices = dynamic_cast<TypedArray<int> *>( el[ "indices" ].model() ) ) {
                            for( int nt = 0, ct = 0; nt < indices->size( 1 ); ++nt ) {
                                unsigned o[ 4 ];
                                for( int j = 0; j < 4; ++j )
                                    o[ j ] = qMin( indices->operator[]( ct++ ), (int)dic_mesh.node_list.size() - 1 );
                                dic_mesh.add_element( Tetra(), DefaultBehavior(), o );
                            }
                        }
                    }
                }
            }
        }
        // Mesh generation from sketch
        if ( c.type() == "SketchItem" ) {
            mesh.reassign( c[ "mesh" ] );
            for( int i = 0, n = mesh[ "points" ].size(); i < n; ++i ) {
                MP pos = mesh[ "points" ][ i ][ "pos" ];
                xmin = min( xmin , Pvec (pos[0],pos[1]) );
                xmax = max( xmax , Pvec (pos[0],pos[1]) );
            }
            PRINT(xmin);
            PRINT(xmax);
            Pvec XD(20,20);
            make_rect( dic_mesh, Quad(), xmin-xmin, xmax-xmin, XD );
            //mesh.reassign( soda_mesh_from_lmtpp_mesh( dic_mesh ) );
//             mesh.flush();    qDebug() << list_param;
        }
    }

    // Load parameters
    bool add_noise = false;
    if ( int (mp[ "add_noise" ] ) )
        add_noise = true;
    double noise_level = mp[ "noise_level" ];
    int uncertainty_type = mp[ "uncertainty_type.num" ];
    double min_value = mp[ "min_value" ];
    double max_value = mp[ "max_value" ];
    double step = mp[ "step" ];
   
    // Deform reference image
    Vec<double> Dimp;
    if ( uncertainty_type == 0 ){ // displacement uncertainty
        // Generate shifted images
        PRINT("displacement uncertainty");
        for (double i=min_value;i<=max_value;i+=step){
            I2 *img = images.new_elem();
            *img = dec_image(images[0], xmin, xmax, i, i, add_noise, noise_level );
            Dimp.push_back(i);
        }
        for (int i=0;i<images.size();++i) images[i].save( "toto"+to_string(i)+".png" );
    }
    else if ( uncertainty_type == 1 ){// strain uncertainty
        // Generate strain vector
        PRINT("strain uncertainty");
        // Generate strained images
    }

    Vec<double> UncX; UncX.resize(images.size() - 1);
    Vec<double> ErrX; ErrX.resize(images.size() - 1);
    UncX[0] = 0;
    ErrX[0] = 0;
    Vec<double> UncY; UncY.resize(images.size() - 1);
    Vec<double> ErrY; ErrY.resize(images.size() - 1);
    UncY[0] = 0;
    ErrY[0] = 0;
    for( int j = 2; j < images.size(); ++j ) {
        // DIC on series of images
        DC dic;
        dic.min_norm_inf_dU = 1e-3;
        dic.div_pixel = 1;
        dic.max_cpt_iter = 10;
        dic.multi_resolution = 0;
 //     calcul du champ de deplacement obtenu par correlation
        dic.exec( images[1],images[j] , dic_mesh , dep_DM(), lum_DM() );
        // disp field
        //QVector<MP> displacements = make_field( output_field, dic_mesh.dim, "Displacement" );
        for( int d = 0; d < dic_mesh.dim; ++d ) {
            // data
            QVector<int> s; s << dic_mesh.node_list.size();
            TypedArray<double> *data = new TypedArray<double>( s );
            Vec<double> Ux; Ux.resize(dic_mesh.node_list.size());
            Vec<double> Uy; Uy.resize(dic_mesh.node_list.size());
            for( int i = 0; i < dic_mesh.node_list.size(); ++i ){
                data->operator[]( i ) = dic_mesh.node_list[ i ].dep[ d ];
                if (d==0) 
                 Ux[i] = dic_mesh.node_list[ i ].dep[ d ] ;
                if (d==1) 
                 Uy[i] = dic_mesh.node_list[ i ].dep[ d ] ;
                dic_mesh.node_list[ i ].dep[ d ] = 0.0;
            }
            
            // NodalField
            //add_field_in_Interpolated( displacements[ d ], mesh, data, j-2 );
            if (d==0){
                UncX[j-1] = standard_deviation(Ux);
                ErrX[j-1] = mean(Ux) - Dimp[j-1];
//                 PRINT(Dimp[j-1]);
//                 PRINT(mean(Ux));
            }
            if (d==1){
                UncY[j-1] = standard_deviation(Uy);
                ErrY[j-1] = mean(Uy) - Dimp[j-1];
//                 PRINT(Dimp[j-1]);
//                 PRINT(mean(Uy));
            }
        }
    }
    // Calculate mean and standard deviation for the series of field (strain or displacement)
    PRINT(UncX);
    PRINT(ErrX);
    PRINT(UncY);
    PRINT(ErrY);  
    
    MP DimpMPVec = MP::new_lst( "Vec" );
    MP UncXMPVec = MP::new_lst( "Vec" );
    MP UncYMPVec = MP::new_lst( "Vec" );
    MP ErrXMPVec = MP::new_lst( "Vec" );
    MP ErrYMPVec = MP::new_lst( "Vec" );    
    
    MP msg = MP::new_obj( "Model" );
    QString res = "OK";
    
    MP mpDimpTreeItem_Vec = MP::new_obj( "TreeItem_Vector" );//TEST
    MP mpUncXTreeItem_Vec = MP::new_obj( "TreeItem_Vector" );//TEST
    MP mpUncYTreeItem_Vec = MP::new_obj( "TreeItem_Vector" );//TEST
    MP mpErrXTreeItem_Vec = MP::new_obj( "TreeItem_Vector" );//TEST
    MP mpErrYTreeItem_Vec = MP::new_obj( "TreeItem_Vector" );//TEST

   // msg[ "Writing TreeItem_Vector: " ] = res ;//TEST TODO A corriger car produit erreur et bloque chargement lab.html 
    //mp[ "_messages" ] << msg;
    
    mp[ "_output" ][0].clear();
    mp[ "_output" ][1].clear();
    mp[ "_output" ][2].clear();
    mp[ "_output" ][3].clear();
    mp[ "_output" ][4].clear();
    
    set_vecToTreeItemVec(Dimp, DimpMPVec, mp[ "_output" ][0], "Imposed Displacement");
    set_vecToTreeItemVec(UncX, UncXMPVec, mp[ "_output" ][1], "Uncertainty of X");
    set_vecToTreeItemVec(UncY, UncYMPVec, mp[ "_output" ][2], "Uncertainty of Y");
    set_vecToTreeItemVec(ErrX, ErrXMPVec, mp[ "_output" ][3], "Error of X");
    set_vecToTreeItemVec(ErrY, ErrYMPVec, mp[ "_output" ][4], "Error of Y");    
    
    qDebug()  << mp[ "_output" ][0];
    qDebug(" ");
    qDebug()  << mp[ "_output" ][1];
    qDebug(" ");
    qDebug()  << mp[ "_output" ][2];
    qDebug(" ");
    qDebug()  << mp[ "_output" ][3];
    qDebug(" ");
    qDebug()  << mp[ "_output" ][4];
    qDebug(" ");
    
//     mp[ "_output[0]"].clear();
//     mp[ "_output[1]"].clear();
//     mp[ "_output[2]"].clear();
//     mp[ "_output[3]"].clear();
//     mp[ "_output[4]"].clear();
//     
//     set_vecToTreeItemVec(Dimp, DimpMPVec, mp[ "_output[0]"], "Imposed Displacement");
//     set_vecToTreeItemVec(UncX, UncXMPVec, mp[ "_output[1]"], "Uncertainty of X");
//     set_vecToTreeItemVec(UncY, UncYMPVec, mp[ "_output[2]"], "Uncertainty of Y");
//     set_vecToTreeItemVec(ErrX, ErrXMPVec, mp[ "_output[3]"], "Error of X");
//     set_vecToTreeItemVec(ErrY, ErrYMPVec, mp[ "_output[4]"], "Error of Y");    
//     
//     qDebug()  << mp[ "_output[0]"];
//     qDebug(" ");
//     qDebug()  << mp[ "_output[1]"];
//     qDebug(" ");
//     qDebug()  << mp[ "_output[2]"];
//     qDebug(" ");
//     qDebug()  << mp[ "_output[3]"];
//     qDebug(" ");
//     qDebug()  << mp[ "_output[4]"];
//     qDebug(" ");
    
    // Calculate difference between results and imposed displacement or strain

    // Plot graphs
    mp.flush();
    return true;
}

//TEST

void DicUncertaintyUpdater::set_vecToTreeItemVec(const Vec<double> &vec, MP MPvec, MP mpTreeItem_Vec, QString attrName){ 
    mpTreeItem_Vec[ "_name" ]= attrName;
    
    int i;
    for(i=0; i<vec.size(); i++){
        MPvec << vec[i];
        mpTreeItem_Vec["vec"]<<MPvec[i];     
    }
//     mpTreeItem_Vec["vec"]<<MPvec;

}
//set a vector into an MP attribute  
// void DicUncertaintyUpdater::set_vector(Vec<double> vec, const MP &MPvec, const MP &pMP, QString attrName){ 
//     int i;
//     for(i = 0; i < vec.size(); ++i ){
//        MPvec[""+i] << vec[ i ];
//     }
//     ++i;
//     MPvec[""+i] <<0;
//     pMP["vec"]= MPvec;
// }

// void DicUncertaintyUpdater::set_TreeItemVector(const &MP mpTreeItem_Vec, const MP &pMP, QString attrName){ 
//   
//   
// }  
