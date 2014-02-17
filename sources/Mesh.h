#ifndef MESH_H
#define MESH_H

#include "containers/vec.h"
#include <Soca/MP.h>


struct Mesh_vecs
{
    LMT::Vec< LMT::Vec<double> >   nodes;
    LMT::Vec< LMT::Vec<unsigned> > elements;
    LMT::Vec< LMT::Vec<unsigned> > skin_elements;
    
    void operator=(MP mesh_data);
    
    void load(MP mesh);
    MP save() const;
};

#endif //MESH_H