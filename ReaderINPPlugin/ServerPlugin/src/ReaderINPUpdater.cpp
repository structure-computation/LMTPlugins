#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include "ReaderINPUpdater.h"
#include "../../../../ScwalPlugins/FieldsComparatorPlugin/ServerPlugin/src/Mesh.h"

#include "../dic/correlation/mesh_carac_3D.h"
#include "../LMT/include/mesh/ReaderINP.h"

typedef LMT::Mesh<Mesh_carac_pb_3D<double,3> > TM3;
typedef TM3::Pvec Pvec3;

bool ReaderINPUpdater::run( MP mp ) { 
    
    QString Qlocation = mp[ "location" ];std::string location = Qlocation.toStdString();
    TM3 Mesh_output;
    
    char* name = strcpy((char*)malloc(location.length()+1), location.c_str());
    // Reading mesh from inp file
    LMT::ReaderINP<TM3> ReaderINP( Mesh_output, name );
    
     write_mesh_vtk( "/home/mathieu/aa.vtu",Mesh_output);
    
    MP output = mp[ "_output[0]" ];
    MP om = output[ "_mesh" ];
    om[ "points" ].clear();
    om[ "_elements" ].clear();
    
    // Putting nodes in mesh
    for (int ii=0; ii<Mesh_output.node_list.size(); ii++){
         MP pos = MP::new_lst( "Vec_3" );
         pos << double(Mesh_output.node_list[ii].pos[0]) << Mesh_output.node_list[ii].pos[1] << Mesh_output.node_list[ii].pos[2];
         MP pnt = MP::new_obj( "Point" );
         pnt[ "pos" ] = pos;
         om[ "points" ] << pnt;
    }

    MP triangles = MP::new_obj( "Element_TriangleList" );
    TypedArray<int> *triangle_con = new TypedArray<int>;
    MP tetrahedra = MP::new_obj( "Element_TetrahedraList" );
    TypedArray<int> *tetra_con = new TypedArray<int>;
    // Putting elements in mesh
    for (int ii=0; ii<Mesh_output.elem_list.size(); ii++){
        if (Mesh_output.elem_list[ii]->name_virtual() == "Tetra"){
            for( int jj = 0; jj < 4; ++jj ) {
                tetra_con->_data << Mesh_output.elem_list[ii]->node_virtual(jj)->number;
            }
        }
        else if (Mesh_output.elem_list[ii]->name_virtual() == "Triangle"){
            for( int jj = 0; jj < 3; ++jj ) {
                triangle_con->_data << Mesh_output.elem_list[ii]->node_virtual(jj)->number;
            }
        }
    }
    
    if (triangle_con->_data.size() > 0){
        // triangle
        triangle_con->_size.resize( 2 );
        triangle_con->_size[ 0 ] = 3;
        triangle_con->_size[ 1 ] = triangle_con->_data.size() / 3;
        triangles[ "indices" ] = triangle_con;
        om[ "_elements" ] << triangles;   
    }
    if (tetra_con->_data.size() > 0){
        // tetrahedra
        tetra_con->_size.resize( 2 );
        tetra_con->_size[ 0 ] = 4;
        tetra_con->_size[ 1 ] = tetra_con->_data.size() / 4;
        tetrahedra[ "indices" ] = tetra_con;
        om[ "_elements" ] << tetrahedra;
    }
    
    mp.flush();
    
}



