#
class DICCITNormalVectorItem extends TreeItem_Computable
    constructor: ( name = "Normal vector", mesh ) ->
        super()

        @_name.set name
#         @_ico.set "img/plot3D.png"
        @_viewable.set true
        
        @add_attr
            _mesh       : mesh
        
        @add_attr
            center      : new Point
            point       : new Point
         
        @theme_x = new LineTheme( new Color( 255, 0, 0 ) ) 
                
    draw: ( info ) -> 
       
    make_mesh: ()->
        
    
    
    cosmetic_attribute: ( name ) ->
        super( name ) or ( name in [ "_mesh", "_field", "_np", "visualization" ] )
    
    accept_child: ( ch ) ->
        
        
    sub_canvas_items: ->
        [ @_point, @_mesh]
    #    if @nothing_to_do()
    #        [ @_mesh ]
    #    else
    #        []
    
    
    z_index: ->
        @_mesh.z_index()
        
    disp_only_in_model_editor: ->
#         @mesh
    
