#ifndef QUAD_TO_TRI_H
#define QUAD_TO_TRI_H

template<class TM1, class TM2>
void Quad_to_Tri(const TM1 &m1, TM2 &m2) {
    // construction des noeuds du nouveau maillage commun
    std::map<unsigned,unsigned> corr_nodes;
    for(unsigned i=0;i<m1.elem_list.size();++i) {
        for(unsigned n=0;n<m1.elem_list[i]->nb_nodes_virtual();++n) {
            unsigned num_node = m1.elem_list[i]->node_virtual(n)->number_in_original_mesh();
            if ( corr_nodes.find( num_node ) == corr_nodes.end() ) {
                corr_nodes[ num_node ] = m2.node_list.size();
                m2.add_node( m1.elem_list[i]->node_virtual(n)->pos );
            }
        }
    }
    
    for(unsigned i=0;i<m1.elem_list.size();++i) {
        if(m1.elem_list[i]->nb_nodes_virtual()==4) {
            //quad
            Vec<typename TM2::TNode *,3> vn1(
                &m2.node_list[ corr_nodes[m1.elem_list[i]->node_virtual(0)->number_in_original_mesh()] ],
                &m2.node_list[ corr_nodes[m1.elem_list[i]->node_virtual(1)->number_in_original_mesh()] ],
                &m2.node_list[ corr_nodes[m1.elem_list[i]->node_virtual(2)->number_in_original_mesh()] ]
            );
            m2.add_element( Triangle(), DefaultBehavior(), vn1.ptr() );
            Vec<typename TM2::TNode *,3> vn2(
                &m2.node_list[ corr_nodes[m1.elem_list[i]->node_virtual(0)->number_in_original_mesh()] ],
                &m2.node_list[ corr_nodes[m1.elem_list[i]->node_virtual(2)->number_in_original_mesh()] ],
                &m2.node_list[ corr_nodes[m1.elem_list[i]->node_virtual(3)->number_in_original_mesh()] ]
            );
            m2.add_element( Triangle(), DefaultBehavior(), vn2.ptr() );
        }
        else {
            cout << "reperage automatique des interfaces non implemente pour ce type d'element" << endl;
            assert(0);

        }
    }
}

#endif // QUAD_TO_TRI_H