#
class DICCITVirtualGaugeItem extends TreeItem_Computable
    constructor: ( name = "DICCITVirtualGauge" ) ->
        super()

        @_name.set name
#         @_ico.set "img/plot3D.png"
        @_viewable.set true
        
        @add_attr
            _point       : new PointMesher [ 0, 0, 0 ], 2, 6 
        
        @add_attr
            center      : @_point.point
            width       : 2.0
            height      : 1.0
            nb_values   : new ConstrainedVal( 10, { min: 2, max: 20, div: 18 } )
        
        @add_attr    
            _point_rz     : new PointMesher [ @width.get(), 0, 0 ], 2, 6 
            _mesh         : new Mesh( not_editable: true )
             
          
        @add_attr  
            poinrz        : @_point_rz.point
        
            visualization : @_mesh.visualization
            _field        : new NodalField @_mesh
          
        @add_attr
            _np           : new NamedParametrizedDrawable "tst", @_field 
            field_set     : new FieldSet
          
        @field_set.color_by.lst.push @_np
        @field_set.warp_by.lst.push new NamedParametrizedDrawable "dep", new VectorialField "dep", [ @_np, @_np, @_np ] 
          
          
        @visualization.display_style.num.set 1
        
        @bind =>
            if @center.has_been_modified() or @width.has_been_modified() or @height.has_been_modified() or @nb_values.has_been_modified()
              @make_mesh()
                
    
    fill_v1_v2_v3: () ->
        _v1 = new Vec
        _v2 = new Vec
        _v3 = new Vec
        _v1_scale = new Vec
        _v2_scale = new Vec
        _v3_scale = new Vec
        #alert @_f_t + " " + @_tmin + " " + @_tmax + " " + @nb_values
        
        width0 = @center.pos[0].get() - (@width.get() / 2)
        height0 = @center.pos[1].get() - (@height.get() / 2)
                
#         console.log width0
#         console.log height0
        
        for i in [ 0 ... @nb_values.get() ]
            _v1.push 0
            _v2.push 0
            _v1_scale.push 0
            _v2_scale.push 0
            _v1[ i ].set (width0 + i * (@width.get())/(@nb_values.get()-1))
            _v2[ i ].set (height0 + i * (@height.get())/(@nb_values.get()-1))
            for j in [ 0 ... @nb_values.get() ]
                _v3.push 0
                _v3_scale.push 0
                
         
        #alert _v3
        for i in [ 0 ... @nb_values.get() ]
            for j in [ 0 ... @nb_values.get() ]
                x = _v1[ i ]
                y = _v2[ j ]
                str = "x=" + x + "; y=" + y + "; val_z = " + @center.pos[2] + ";"
                val_f = eval(str)
                _v3[ (i * @nb_values.get() + j) ].set val_z
        

        _v1_scale = _v1
        _v2_scale = _v2
        _v3_scale = _v3
#         for i in [ 0 ... @nb_values.get() ]
#             @_v1_scale = @_v1
#             @_v2_scale = @_v1
#             for j in [ 0 ... @nb_values.get() ]
#                 _v3_scale = _v3
            
        #alert _v3_scale
        
        return [_v1_scale, _v2_scale, _v3_scale]
       
    make_mesh: ()->
#         p1_p2 = new Vec_3 [ 2, 0, 0 ]
# #         @poinrz.pos.set Vec_3.add @center.pos, p1_p2
#         @poinrz.pos.set p1_p2
    
        [_v1_scale, _v2_scale, _v3_scale] = @fill_v1_v2_v3()
        @_mesh.points.clear()
        @_mesh._elements.clear()
        
        for i in [ 0 ... @nb_values.get() ]
            for j in [ 0 ... @nb_values.get() ]
                @_mesh.add_point [ _v1_scale[ i ], _v2_scale[ j ], _v3_scale[ (i * @nb_values.get() + j) ] ]
        
        el = new Element_TriangleList
        el.indices.resize [ 3, (2*(@nb_values.get()-1)*(@nb_values.get()-1)) ]
        num_element = 0
        for i in [ 0 ... (@nb_values.get()-1) ]
            for j in [ 0 ... @nb_values.get()-1 ]
                pt_1 = i * @nb_values.get() + j
                pt_2 = i * @nb_values.get() + (j+1)
                pt_3 = (i+1) * @nb_values.get() + j
                pt_4 = (i+1) * @nb_values.get() + (j+1)
                
                el.indices.set_val [ 0, num_element ], pt_1
                el.indices.set_val [ 1, num_element ], pt_3
                el.indices.set_val [ 2, num_element ], pt_2
                
                num_element += 1
                
                el.indices.set_val [ 0, num_element ], pt_4
                el.indices.set_val [ 1, num_element ], pt_2
                el.indices.set_val [ 2, num_element ], pt_3
                
                num_element += 1

        #@_mesh._elements.set el.indices
        @_mesh.add_element el
#         @_field._mesh = @_mesh
#         
#         for i in [ 0 ... @_mesh.nb_points() ]
#             console.log i
#             console.log @_mesh.points[i].pos[2].get()
#             @_field._data.set_val i, @_mesh.points[i].pos[2].get()
#             
#         #alert @_mesh.nb_points()
#         #alert @_field._data
#         console.log @_field._data
    
    
    cosmetic_attribute: ( name ) ->
        super( name ) or ( name in [ "_mesh", "_field", "_np", "visualization" ] )
    
    accept_child: ( ch ) ->
        
        
    sub_canvas_items: ->
        [ @_point, @_point_rz, @_mesh]
    #    if @nothing_to_do()
    #        [ @_mesh ]
    #    else
    #        []
    
    
    z_index: ->
        @_mesh.z_index()
        
    disp_only_in_model_editor: ->
#         @mesh
    
