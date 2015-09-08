#
class DicUncertaintyItem extends TreeItem_Computable
    constructor: ( name = "DicUncertainty" ) ->
        super()
        
        # default values
        @_name.set name
        #@_ico.set "img/mesher.png"
        @_viewable.set false        
        @add_child new ImgSetItem
        @add_child new DiscretizationItem
        
        #@add_output new FieldSetCorreliItem
        #@add_output new MeshItem
        
        @add_attr
           add_noise : false
           noise_level : 1
           uncertainty_type : new Choice( 0, [ "Displacement", "Strain"] )
           specific_mesh : false
           min_value : 0
           max_value : 1
           step : 0.5
        
        t0= new TreeItem_Vector
        t1= new TreeItem_Vector
        t2= new TreeItem_Vector
        t3= new TreeItem_Vector
        t4= new TreeItem_Vector
        
#         TEST
#         @add_attr
#             _ConstT0: new ConstOrNotModel true, t0
#             _ConstT1: new ConstOrNotModel true, t1
#             _ConstT2: new ConstOrNotModel true, t2
#             _ConstT3: new ConstOrNotModel true, t3
#             _ConstT4: new ConstOrNotModel true, t4
#                 
#         @add_output @_ConstT0
#         @add_output @_ConstT1
#         @add_output @_ConstT2
#         @add_output @_ConstT3
#         @add_output @_ConstT4
           
        @add_output t0
        @add_output t1
        @add_output t2
        @add_output t3
        @add_output t4
           
#            _graph : new Graph 
#                 marker: 'dot',
#                 marker_color: "#f00"
#                 line_width  : 3,
#                 line_color: "#f00"
#                 marker_size: 10,
#                 font_size : 24,
#                 legend_x_division: 10,
#                 legend_y_division: 10,
#                 x_axis: 'D (pix)',
#                 y_axis: 'Uncertainty'
    
    display_suppl_context_actions: ( context_action )  ->
        context_action.push new TreeAppModule_Mesher
        context_action.push new TreeAppModule_Sketch
        
    accept_child: ( ch ) ->
        ch instanceof MaskItem or 
        ch instanceof DiscretizationItem or
        ch instanceof SketchItem or 
        ch instanceof ImgSetItem or
        ch instanceof Graph or
        ch instanceof TreeItem_Vector
#         ch instanceof TransformItem
        
#     sub_canvas_items: ->
#         [ @_graph ]
   
    draw: ( info ) ->
        return false
#         draw_point = info.sel_item[ @model_id ]
#         if @_graph.points.length && draw_point
#             for pm in @_graph.points
#                 pm.draw info
#         if @graph.points.size > 0
#             @graph.draw()
#     z_index: ->
#         @_mesh.z_index()
        
#    disp_only_in_model_editor: ->
#         @mesh

        