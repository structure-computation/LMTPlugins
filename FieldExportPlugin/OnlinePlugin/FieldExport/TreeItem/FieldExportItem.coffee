#
class FieldExportItem extends TreeItem_Computable
    constructor: ( name = "FieldExport" ) ->
        super()
        
        @add_attr
           Format : new Choice( 0, [ "txt", "vtk" ] ) 
           Adresse : "/media/mathieu/Data/resultats_test/"
           Nom : "resultat"
           
        @_name.set name
        @_ico.set "img/mesher.png"
        @_viewable.set false
        
        @_computation_mode.set false
#        @_add_output new MeshItem
        
    display_suppl_context_actions: ( context_action )  ->
        
    accept_child: ( ch ) ->
        ch instanceof FieldSetCorreliItem or
        ch instanceof FieldSetItem 
        
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

    