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
#            _DimpTreeItemVec     : new TreeItem_Vector(@Dimp, "Deplacement impose" )
#            _UncXTreeItemVec     : new TreeItem_Vector(@UncX, "Incertitude sur X" )
#            _UncYTreeItemVec     : new TreeItem_Vector(@UncY, "Incertitude sur Y")
#            _ErrXTreeItemVec     : new TreeItem_Vector(@ErrX, "Erreur sur X" )
#            _ErrYTreeItemVec     : new TreeItem_Vector(@ErrY, "Erreur sur Y")

#            Dimp     : new Vec
#            UncX     : new Vec
#            UncY     : new Vec
#            ErrX     : new Vec
#            ErrY     : new Vec
           
#            vec_Dimp     : new Vec [1, 72, 3, 24]
#            vec_UncX     : new Vec [45, 2, 3, 0]
#            vec_UncY     : new Vec [1, 21, 30, 2]
#            vec_ErrX     : new Vec [12, 2, 3, 4]
#            vec_ErrY     : new Vec [01, 2, 367, 1]
                      

#         t1= new TreeItem_Vector(@Dimp, "Deplacement impose" )
#         t2= new TreeItem_Vector(@UncX, "Incertitude sur X" )
#         t3= new TreeItem_Vector(@UncY, "Incertitude sur Y")
#         t4= new TreeItem_Vector(@ErrX, "Erreur sur X" )
#         t5= new TreeItem_Vector(@ErrY, "Erreur sur Y")
        
        t1= new TreeItem_Vector#(new Vec, "Imposed Displacement" )
        t2= new TreeItem_Vector#(new Vec, "Uncertainty of X" )
        t3= new TreeItem_Vector#(new Vec, "Uncertainty of Y")
        t4= new TreeItem_Vector#(new Vec, "Error of X" )
        t5= new TreeItem_Vector#(#new Vec, "Error of Y")
        
        @add_output t1
        @add_output t2
        @add_output t3
        @add_output t4
        @add_output t5
           
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
    
#    display_suppl_context_actions: ( context_action )  ->
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

        