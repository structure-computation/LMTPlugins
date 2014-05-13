#
class DicUncertaintyItem extends TreeItem_Computable
    constructor: ( name = "DicUncertainty" ) ->
        super()
        
        # default values
        @_name.set name
        @_ico.set "img/mesher.png"
        @_viewable.set false        
        @add_child new ImgSetItem
        @add_child new MaskItem
        @add_child new DiscretizationItem
        
        @add_output new FieldSetCorreliItem
        
        @add_attr
           noise_level : 1
           add_noise : false
        
#    display_suppl_context_actions: ( context_action )  ->
        
    accept_child: ( ch ) ->
        ch instanceof MaskItem or 
        ch instanceof DiscretizationItem or
        ch instanceof SketchItem or 
        ch instanceof ImgSetItem or
        ch instanceof TransformItem
        
#    sub_canvas_items: ->
#        [ @_mesh ]
#         if @nothing_to_do()
#             [ @_mesh ]
#         else
#             []
    
#    draw: ( info ) ->
        
#     z_index: ->
#         @_mesh.z_index()
        
#    disp_only_in_model_editor: ->
#         @mesh

    