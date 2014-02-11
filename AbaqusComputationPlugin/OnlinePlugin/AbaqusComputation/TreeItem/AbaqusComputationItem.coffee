#
class AbaqusComputationItem extends TreeItem_Computable
    constructor: ( name = "AbaqusComputation" ) ->
        super()
        
        @_name.set name
        @_ico.set "img/mesher.png"
        @_viewable.set false
        
        @_computation_mode.set false
        
        @add_attr
            pix2m : 0.00002

        @add_child new MaterialABQItem
        @add_child new BoundariesSelectionItem
        @add_output new FieldSetItem
        
#        @_mesh.add_point([0, 0, 0])
#        @_mesh.add_point([0, 1, 0])
#        @_mesh.add_point([1, 1, 0])
#        @_mesh.add_point([1, 0, 0])
        
        #@add_attr 
        #    visualization : @_mesh.visualization
        #    points : @_mesh.points
    
    display_suppl_context_actions: ( context_action )  ->
       
        
    accept_child: ( ch ) ->
        ch instanceof FieldSetCorreliItem or
        ch instanceof BoundariesSelectionItem or
        ch instanceof FieldSetItem or
        ch instanceof FieldItem or
        ch instanceof MeshItem or
        ch instanceof MaterialABQItem
        
    sub_canvas_items: ->
        [  ]
#        [ @_mesh ]
#         if @nothing_to_do()
#             [ @_mesh ]
#         else
#             []
    
    
#     z_index: ->
#         @_mesh.z_index()
        
    disp_only_in_model_editor: ->
#         @mesh
