#ifndef SODA_MESH_FROM_LMTPP_MESH_H
#define SODA_MESH_FROM_LMTPP_MESH_H

#include "Quad_to_Tri.h"

template<class TM>
MP soda_mesh_from_lmtpp_mesh( const TM &m1 ) {
    MP om = MP::new_obj( "Mesh" );
    om[ "points" ] = MP::new_lst( "Lst_Point" );
    om[ "_elements" ] = MP::new_lst();
    om[ "_selected_points" ] = MP::new_lst();
    om[ "_pelected_points" ] = MP::new_lst();
    om[ "_selected_elements" ] = MP::new_lst();
    om[ "_pelected_elements" ] = MP::new_lst();
    
    TM m;
    Quad_to_Tri( m1 , m );
    PRINT(m.elem_list.size());
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

#endif // SODA_MESH_FROM_LMTPP_MESH_H