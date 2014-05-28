#
class DicUncertaintyItem extends TreeItem_Computable
    constructor: ( name = "DicUncertainty" ) ->
        super()
        
        # default values
        @_name.set name
        #@_ico.set "img/mesher.png"
        @_viewable.set true        
        @add_child new ImgSetItem
        @add_child new DiscretizationItem
        
        @add_output new FieldSetCorreliItem
        #@add_output new MeshItem
        
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
    display_suppl_context_actions: ( context_action )  ->
        context_action.push new TreeAppModule_Mesher
        context_action.push new TreeAppModule_Sketch
        
    accept_child: ( ch ) ->
        ch instanceof MaskItem or 
        ch instanceof DiscretizationItem or
        ch instanceof SketchItem or 
        ch instanceof ImgSetItem or
        ch instanceof Graph
#         ch instanceof TransformItem
        
    sub_canvas_items: ->
        [ @_graph ]
   
#    draw: ( info ) ->
        
#     z_index: ->
#         @_mesh.z_index()
        
#    disp_only_in_model_editor: ->
#         @mesh

    