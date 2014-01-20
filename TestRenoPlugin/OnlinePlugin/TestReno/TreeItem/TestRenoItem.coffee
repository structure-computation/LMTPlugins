#
class TestRenoItem extends TreeItem_Computable
    constructor: ( name = "TestReno" ) ->
        super()
        
        @add_attr
           valeur1 : 1
           valeur2 : new Choice( 0, [ "pouet" , "toto" , "prout" ] ) 
           _mesh : new Mesh
           
        @_mesh.add_point([0,0,0])
        @_mesh.add_point([0,1,0])
        @_mesh.add_point([0,1,1])
        @_mesh.add_point([0,0,1])
        
        @_name.set name
        @_ico.set "img/mesher.png"
        @_viewable.set false
        
        @_computation_mode.set false
#        @_add_output new MeshItem
        
    display_suppl_context_actions: ( context_action )  ->
        
    accept_child: ( ch ) ->
        
    sub_canvas_items: ->
        [ @_mesh ]
#         if @nothing_to_do()
#             [ @_mesh ]
#         else
#             []
    
    draw: ( info ) ->
        
#     z_index: ->
#         @_mesh.z_index()
        
    disp_only_in_model_editor: ->
#         @mesh

    