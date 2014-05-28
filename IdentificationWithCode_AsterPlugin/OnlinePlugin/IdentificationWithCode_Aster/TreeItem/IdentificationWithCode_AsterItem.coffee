#
class IdentificationWithCode_AsterItem extends TreeItem_Computable
    constructor: ( name = "IdentificationWithCode_Aster" ) ->
        super()
        
        @add_attr
           iterations : 10
           relaxation : 1
           offset : 0.01
           id_done : 0.0
        
        @_name.set name
        @_ico.set "img/IdentificationWithCode_Aster_bouton.png"
        @_viewable.set false
        
        @_computation_mode.set false
#        @_add_output new MeshItem
        @add_child new Code_Aster_DataItem
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

    