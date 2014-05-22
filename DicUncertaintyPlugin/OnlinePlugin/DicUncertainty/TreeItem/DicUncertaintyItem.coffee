#
class DicUncertaintyItem extends TreeItem_Computable
    constructor: ( name = "DicUncertainty" ) ->
        super()
        
        # default values
        @_name.set name
        @_ico.set "img/mesher.png"
        @_viewable.set false        
        @add_child new ImgSetItem
        @add_child new SketchItem
        @add_child new DiscretizationItem
        
        @add_output new FieldSetCorreliItem
        
        @add_attr
           add_noise : false
           noise_level : 1
           uncertainty_type : new Choice( 0, [ "Displacement", "Strain"] )
           specific_mesh : false
           min_value : 0
           max_value : 1
           step : 0.5
           _graph : new Graph 
                marker: 'dot',
                marker_color: "#f00"
                line_width  : 3,
                line_color: "#f00"
                marker_size: 10,
                font_size : 24,
                legend_x_division: 10,
                legend_y_division: 10,
                x_axis: 'D (pix)',
                y_axis: 'Uncertainty'
#    display_suppl_context_actions: ( context_action )  ->
        
    accept_child: ( ch ) ->
        ch instanceof MaskItem or 
        ch instanceof DiscretizationItem or
        ch instanceof SketchItem or 
        ch instanceof ImgSetItem
#         ch instanceof TransformItem
        
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

    