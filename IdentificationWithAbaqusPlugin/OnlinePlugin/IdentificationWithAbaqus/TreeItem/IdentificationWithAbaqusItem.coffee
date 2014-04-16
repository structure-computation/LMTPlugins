#
class IdentificationWithAbaqusItem extends TreeItem_Computable
    constructor: ( name = "IdentificationWithAbaqus" ) ->
        super()
        
        @add_attr
           iterations : 10
           relaxation : 1
           offset : 0.01
           force : "/media/mathieu/Data/bourgueil/plast_florent/effort_2219_L5.txt"
           id_done : 0
        
        @_name.set name
        @_ico.set "img/IdentificationWithAbaqus_bouton.png"
        @_viewable.set false
        
        @_computation_mode.set false
#        @_add_output new MeshItem
        @add_child new AbaqusDataItem
        @add_output new FieldSetItem
        
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

    