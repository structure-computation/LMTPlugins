#
class DICCITVectorItem extends TreeItem
    constructor: ( name = "Vector", _center = new Point [0,0,0], _vec = new Vec_3 [0,0,1] ) ->
        super()

        @_name.set name
#         @_ico.set "img/plot3D.png"
        @_viewable.set true
        
        @add_attr
            center    : _center
            vector    : _vec
            ratio     : new ConstrainedVal( 1, { min: 1, max: 100 } )
            _mesh     : new Mesh
            
        @add_attr  
            visualization : @_mesh.visualization
     
        @bind =>
            if @ratio.has_been_modified()
                @make_mesh()
     
    make_mesh: ()->
        @_mesh.points.clear()
        @_mesh._elements.clear()
        
        @_mesh.add_point  [ @center.pos[0].get(), @center.pos[1].get(), @center.pos[2].get() ]
        @_mesh.add_point  [ @center.pos[0].get() + @ratio.get() * @vector[0].get() , @center.pos[1].get() + @ratio.get() * @vector[1].get() , @center.pos[2].get() + @ratio.get() * @vector[2].get() ]
        
#         console.log @_mesh.points
        
        liste = [0, 1]
        bar = new Element_Line(liste)
        @_mesh.add_element bar
            

    sub_canvas_items: ->
        lst = []
        lst.push @_mesh
        
        return lst
        
    disp_only_in_model_editor: ->
#         @mesh
    
