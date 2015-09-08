

class TestSamirGraphItem extends TreeItem_Computable
    constructor: (name = "Graph") ->
        super()
        @_name.set name
        
#         @_ico.set "img/SamirGraph.png"
#         @_ico.set "../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/testGraphs_bouton.png"
#         @_ico.set "img/testGraphs_bouton.png"
        @_viewable.set true
        
        @_NotfirstDrawing = undefined  #ensures that only one single svg will be drawn at any mouse event occurring during the drawing, ie when _allowToDraw = true  

        @add_attr
            _allowToDraw : false
        console.log "At TestSamirGraphItem constructor @_allowToDraw.get():", @_allowToDraw.get()
        treeItem_x = new TreeItem_SingleData("abscissa")
        treeItem_y = new TreeItem_SingleData("ordinate")
   
        @add_child new TreeItem_GraphSettings
        @add_child new TreeItem_Curves
        @add_child treeItem_x#TEST
        @add_child treeItem_y      
				#@add_child new ImgItem
        
						#img_n = new Img 
			#	imgItem = new ImgItem
				#imgItem.img.set img_n				
				#@add_child imgItem

#     display_suppl_context_actions: ( context_action )  ->
#   context_action.push new TreeAppModule_Mesher
#     context_action.push new TreeAppModule_Sketch    
    #TEST             
    cosDeg: (number, precision)->
        console.log number*(Math.PI/180)
        parseFloat (Math.cos(number*(Math.PI/180))).toPrecision(5)

    fill_v1_v2: () ->
            @_v1.clear()
            @_v2.clear()
            #alert @_f_t + " " + @_tmin + " " + @_tmax + " " + @_nb_values
            
            for i in [ 0 ... @_nb_values.get() ]
                @_v1.push 0
                @_v2.push 0
                @_v1[ i ].set (@_tmin.get() + i * (@_tmax.get() - @_tmin.get())/(@_nb_values.get()-1))
            
            #alert @_v1
            for i in [ 0 ... @_nb_values.get() ]
                t = @_v1[ i ]
                str = "t=" + t + "; val_t = " + @_f_t + ";"
                #alert str
                val_f = eval(str)
                #alert val_f
                #alert val_t
                @_v2[ i ].set val_t
                
                
    fill_x_y_cos: ()->
        zero = new Number(0).valueOf()
        
        @_vec_x.clear()
        @_vec_y.clear()
        
        for i in [ 0 ... @_nb_values]#.get() ]
            try
                @_vec_x.push 0
                @_vec_y.push 0
                @_vec_x[ i ]= new Number(i).valueOf()                             

#                 if typeof(cosDeg(i)) != "number"
#                     alert cosDeg(i)+"is not a number"
                if csd_i <0 and csd_i > -2e-16        
                    @_vec_y[ i ]= zero
#                 if (i == 270) 
#                     @_vec_y[ i ].set zero ### #TODO A retirer!!! TODO
#                 else 
#                     if (i == 90)
#                         @_vec_y[ i ].set zero
# #                         alert "i == 90 a changé"+@_vec_y[ i ].get()
                else
                    csd_i = cosDeg(i)
                    @_vec_y[ i ] =  new Number(csd_i).valueOf()
                
         #TODO A retirer!!! TODO
#                     alert "csd_i: "+csd_i
#                     alert "@_vec_y[ i ]: "+@_vec_y[ i ]
#                 else 
#                     if Math.abs(csd_i) == Number.NEGATIVE_INFINITY
#                         @_vec_y[ i ].set zero                      
            catch error
                return false      
        return true
        
#     sub_canvas_items: ->
#         [ @_issimGraph ]     
    
    display_suppl_context_actions: ( context_action )  ->
#        context_action.push
 #       new TreeAppModule_SaveGraph
        instance = this
        context_action.push
            txt: "Display your graphs"
            ico: "img/TestSamirImg.png" #
#            /home/eikosimcomputer1/LMTLabsScetup/Javascript/plugins/TestSamirGraph/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png
#             vis: false #TEST false => not display on menu
            fun: ( evt, app) =>
                all_canvas_inst = app.all_canvas_inst() 
                console.log "graph action push: all_canvas_inst="
                console.log all_canvas_inst
                console.log "graph action push: @_NotfirstDrawing"
                console.log @_NotfirstDrawing
                
      
#                 if not @_firstDrawing?
#                     newCanPanAdder = new NewCanvasPanelAdder()
#                     idGraphCanvas = newCanPanAdder.addNewCanvasPanel(app)#TEST1 TEST 2
#                     graphPanManagerInstIndex = all_canvas_inst.length-1
#                     graphCanvasManagerPanelInstance = all_canvas_inst[graphPanManagerInstIndex]
#                     graphCanvasManagerPanelInstance.title = "graphCanvasManagerPanelIns"                     
#                 else
#                     graphCanvasManagerPanelInstance = all_canvas_inst.detect ( x ) -> x.title.equals "graphCanvasManagerPanelIns"
                
                addGraphPanel=(app, all_canvas_inst)->
                    newCanPanAdder = new NewCanvasPanelAdder()
                    idGraphCanvas = newCanPanAdder.addNewCanvasPanel(app)#TEST1 TEST 2
                    graphPanManagerInstIndex = all_canvas_inst.length-1
                    graphCanvasManagerPanelInstance = all_canvas_inst[graphPanManagerInstIndex]
#                     graphCanvasManagerPanelInstance.title = "graphCanvasManagerPanelIns" 
                
                
                if @_NotfirstDrawing
                    console.log "@_NotfirstDrawing == true"
#                     graphCanvasManagerPanelInstance = all_canvas_inst.detect ( x ) -> x.title.equals "graphCanvasManagerPanelIns"
                    display_settings = app.data.selected_display_settings()
                    layout = display_settings._layout
                    
                    console.log "layout:"
                    console.log layout
                    console.log "layout._views"
                    console.log layout._views
                    
                    
                    lmArray = layout._views.filter ( x ) ->  x instanceof LayoutManager
                    lm = lmArray[0]#TODO A améliorer
#                      lm = layout._views.detect ( x ) ->  x instanceof LayoutManager 
                    console.log "lm:"
                    console.log lm
                    console.log "lm._pan_vs_id"
                    console.log lm._pan_vs_id
                    
                    
                    if lm._pan_vs_id.Graph_id?
                        graphCanvasManagerPanelInstance = lm._pan_vs_id.Graph_id
                        console.log "graphCanvasManagerPanelInstance:"
                        console.log graphCanvasManagerPanelInstance
                    else 
                        graphCanvasManagerPanelInstance = addGraphPanel(app, all_canvas_inst)#console.log "The chart has been removed"
                else
                    if not @_NotfirstDrawing?
                        graphCanvasManagerPanelInstance = addGraphPanel(app, all_canvas_inst)
#                         newCanPanAdder = new NewCanvasPanelAdder()
#                         idGraphCanvas = newCanPanAdder.addNewCanvasPanel(app)#TEST1 TEST 2
#                         graphPanManagerInstIndex = all_canvas_inst.length-1
#                         graphCanvasManagerPanelInstance = all_canvas_inst[graphPanManagerInstIndex]
#                         graphCanvasManagerPanelInstance.title = "graphCanvasManagerPanelIns"                     

                        
#                     all_canvas_inst = app.all_canvas_inst() 
                console.log "graphCanvasManagerPanelInstance"
                console.log graphCanvasManagerPanelInstance

                
                graphCanvasManager = graphCanvasManagerPanelInstance.cm# TODO auto_fit 
#                 TODO graphCanvasManager.allow_gl.set false
                @newInfo = graphCanvasManager.cam_info
                
                console.log "for canvasManger, @newInfo:"
                console.log @newInfo
                #TODO LayoutManager._pan_vs_id(idGraphCanvas).cm
                #TODO dans treeappdata panel_id_list(), rm_selected_panels()
                if graphCanvasManager?
                    thislikeItem = graphCanvasManager.items.detect ( x ) -> x instanceof TestSamirGraphItem #TODO +modif le selected+faire == this 
                    console.log "graphCanvasManager.items before clearing(verifier qu'il y a au moins GraphViewItem):"
                    console.log graphCanvasManager.items
#                     graphCanvasManager.items.clear()#TEST
#                     graphCanvasManager.items.push thislikeItem  #TEST TODO avec instance ou avec les "z" pour l'ordre                           
                    thislikeItem._allowToDraw.set true
                    console.log "At TestSamirGraphItem contextAction @_allowToDraw.get() (true):", @_allowToDraw.get()
                    console.log "At TestSamirGraphItem contextAction thislikeItem._allowToDraw.get() (true):", thislikeItem._allowToDraw.get()     
                   #TEST
                    graphCanvasManager.cam.threeD.set false
#                     graphCanvasManager.resize 700, 400
                    #TEST
                    graphCanvasManager.fit 0
                    @pauseMouse(graphCanvasManager)
                    graphCanvasManager.draw(@newInfo)
                         
#             ico: "../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"               
#             ico: "img/TestSamirImg.png"
#             ico: "../../../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
#             ico: "~/LMTLabsScetup/software_library/LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
#             siz: 1
#             TS_instance : this
    pauseMouse:(canvasManager)->
        # events
        canvasManager.onmousewheel = ( evt ) => #@_mouse_wheel evt
        canvasManager.onmousedown  = ( evt ) => #@_mouse_down evt
        canvasManager.onmousemove  = ( evt ) => #@_mouse_move evt
        canvasManager.onmouseout   = ( evt ) => #@_mouse_out evt
        canvasManager.ondblclick   = ( evt ) => #@_dbl_click evt
        canvasManager.onmouseup    = ( evt ) => #@_mouse_up evt
#         canvasManager.addEventListener? "DOMMouseScroll", @canvas.onmousewheel, false
    
    draw: ( info ) -> 
# cf GraphViewItem
        
    has_nothing_to_draw: ->
        true

#     update_min_max: ( x_min, x_max ) ->
#         console.log "TestSamirGraphItem update_min_max"
#         console.log "x_min:"
#         console.log x_min
#         console.log "x_max:"
#         console.log x_max
#         
#         for d in [ 0 ... 3 ]
#                 x_min[ d ] = Math.min x_min[ d ], 0
#         x_max[ 0 ] = Math.max x_max[ 0 ], @data.#rgba.width
#         x_max[ 1 ] = Math.max x_max[ 1 ], @data.#rgba.height
#         x_max[ 2 ] = Math.max x_max[ 2 ], 0
        
        
        
    detect_vector: ->
        res= new Lst
        i = 0
        for child in @_children
            if child instanceof TreeItem_SingleData
                if child._children.length == 0
                    alert child._name+" is empty"
                    return res #TODO 
                for ch_child in child._children   
                        if child._name.get() == "ordinate"               
                            ord_name = ch_child._name.get()
                            ord_vec = ch_child.vec.get()
                            num = ++i
                            curve = new Curve(ord_name, ord_vec, abs_name, abs_vec, num)
                            curve._name.set "curve "+curve.number.get()
                            res.push curve
                            
                        if child._name.get() == "abscissa"
                            abs_vec = ch_child.vec
                            abs_name = ch_child._name
                            break # The first abscissa is only read                           
        for c in res 
            c.abscissa_name.set abs_name
            c.abscissa_vec.set abs_vec
        return res  

# used by GraphViewItem        
    detect_vector_Simple: ->#TODO TEST 
#         res= []
        res= new Lst
        for child in @_children
            if child instanceof TreeItem_SingleData
                if child._children.length == 0
                    alert child._name+" is empty"
                    return res #TODO 
                for ch_child in child._children   
                    if ch_child instanceof TreeItem_Vector 
                        res.push ch_child.vec
                        if child._name == "abscissa"
                            break
        return res         
        
        ##TODO
    VecToList:(vec)->
        LstRes = new Lst
        LstRes[i] = vec[i] for i in [0..vec.length]
        LstRes
          
        #copied from ModelEditor:
    attr_Veclist: ( model)->
#         console.log "model.get_state()"+model.get_state()
        res = {}
        for name in model._attribute_names when model[name] instanceof Vec
             console.log "\nmodel[name]: "+model[name]#TEST
             console.log "\nmodel[name]._underlying_fs_type: "+model[name]._underlying_fs_type#TEST
             res[ name ] = model[ name ]
        res

      
#     obtenir la position réelle dans le canvas
    getLeft: ( l ) ->
        if l.offsetParent?
            return l.offsetLeft + @getLeft( l.offsetParent )
        else
            return l.offsetLeft

#     obtenir la position réelle dans le canvas
    getTop: ( l ) ->
        if l.offsetParent?
            return l.offsetTop + @getTop( l.offsetParent )
        else
            return l.offsetTop
            
    onPopupClose: ( app ) =>
        document.onkeydown = undefined
        app.active_key.set true
        
#         TODO A completer
    accept_child: ( ch ) ->
        ch instanceof TreeItem_Vector
    
#     sub_canvas_items: -> #TODO A remettre
#         [ @_graph= new TestSamirGraphView(
#                                   this,
#                                   @_vec_x,
#                                   [@_vec_y],
#                                   Pwidth,
#                                   Pheight) ]
    
#         ch instanceof FieldSetCorreliItem or
#         ch instanceof FieldSetItem or
#         ch instanceof FieldItem or
#         ch instanceof MeshItem or
#         ch instanceof ImgSetItem

#     disp_only_in_model_editor: -> TODO A mettre ?
#         @mesh    
#####
