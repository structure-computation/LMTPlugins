#
class TestFlo1Item extends TreeItem_Computable
    constructor: ( name = "TestFlo1" ) ->
        super()
        
        
        @_name.set name
        @_ico.set "img/mesher.png"
        @_viewable.set false
        
        @_computation_mode.set false
        
        
        @add_attr
            valeur1 : 10
            valeur2 : new Choice( 0, [ "Bonjour", "coucou", "Salut Reno" ] )

    
    display_suppl_context_actions: ( context_action )  ->
    
        
        
    accept_child: ( ch ) ->
        
    sub_canvas_items: ->
        [  ]
#         if @nothing_to_do()
#             [ @_mesh ]
#         else
#             []
    
    draw: ( info ) ->
    
#     z_index: ->
#         @_mesh.z_index()
        
    disp_only_in_model_editor: ->
#         @mesh
