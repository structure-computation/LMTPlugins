

class TestSamirGraphItem extends TreeItem_Computable
    constructor: (name = "Graph") ->
        super()
        @_name.set name
#         @_ico.set "../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/testGraphs_bouton.png"
        @_ico.set "img/testGraphs_bouton.png"
        @_viewable.set true
        
        @firstDrawing = undefined  #ensures that only one single svg will be drawn at any mouse event occurring during the drawing, ie when _allowToDraw = true  
        
        @add_attr
            _allowToDraw : false
        
        vecteur_abscisse           = new Vec
        vecteur_ordonnee           = new Vec 
#         @fill_v1_v2()    
#         @_nb_values    = 361         #To see until 360
    
        # default values
#         @add_attr
#             _vec_x        : new Vec [1, 2, 3, 4, 5]
#             _vec_y        : new Vec [45, 23, 3, 0, 7]
            
        @_vec_x        = new Vec [10, 1, 2, 3, 4, 5]
        @_vec_y        = new Vec [15, 0, 23, 3, 0, 7]
                       
        # attributes 
#         @fill_x_y()                 # TEST  A remettre si onload non necessaire
        
        treeItem_x = new TreeItem_SingleData("abscisse")
        treeItem_y = new TreeItem_SingleData("ordonnee")

        @add_attr
            _issimGraph   : new IssimGraph
            constrVal: new ConstrainedVal( 7, { min: 0, max: 15 } )

        @add_child treeItem_x#TEST
        @add_child treeItem_y           
          
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
        instance = this
        context_action.push
            txt: "Display your graphs"
            ico: "img/TestSamirImg.png"
            fun: ( evt, app) =>
#                 curCanvasManager = app.selected_canvas_inst()?[ 0 ]?.cm TEST
#                 graphCanvasManager = curCanvasManager TEST
                all_canvas_inst = app.all_canvas_inst()                    
                if not @firstDrawing?
                    newCanPanAdder = new NewCanvasPanelAdder()
                    idGraphCanvas = newCanPanAdder.addNewCanvasPanel(app)#TEST1 TEST 2
                    graphPanManagerInstIndex = all_canvas_inst.length-1
                    graphCanvasManagerPanelInstance = all_canvas_inst[graphPanManagerInstIndex]
                    graphCanvasManagerPanelInstance.title = "graphCanvasManagerPanelIns" 
                    
                else
                    graphCanvasManagerPanelInstance = all_canvas_inst.detect ( x ) -> x.title.equals "graphCanvasManagerPanelIns"
                
                console.log "app.all_canvas_inst() verifier si plusieurs et titre = graphCanvasManagerPanelIns:"
                console.log app.all_canvas_inst()
                console.log "graphCanvasManagerPanelInstance:"
                console.log graphCanvasManagerPanelInstance
                graphCanvasManager = graphCanvasManagerPanelInstance.cm# TODO auto_fit 
#                 TODO graphCanvasManager.allow_gl.set false
                @newInfo = graphCanvasManager.info
                      
#                 graphPanManagerInstIndex = all_canvas_inst.length-1
#                 graphCanvasManager = all_canvas_inst[graphPanManagerInstIndex].cm
                #TODO LayoutManager._pan_vs_id(idGraphCanvas).cm
                #TODO dans treeappdata panel_id_list(), rm_selected_panels()
                if graphCanvasManager?
                    thislikeItem = graphCanvasManager.items.detect ( x ) -> x instanceof TestSamirGraphItem #TODO +modif le selected+faire == this 
                    graphCanvasManager.items.clear()#TEST
                    graphCanvasManager.items.push thislikeItem  #TEST TODO avec instance ou avec les "z" pour l'ordre        
                    
#                     if not thislikeItem?
#                         graphCanvasManager.items.push @this
#                         thislikeItem = graphCanvasManager.items.detect ( x ) -> x instanceof TestSamirGraphItem #TODO +modif le selected+faire == this 
                    console.log "thislikeItem  (verifier si = TestSamirGraphItem):"
                    console.log thislikeItem
                    console.log "@newInfo:"
                    console.log @newInfo
                    thislikeItem._allowToDraw.set true                   
                    graphCanvasManager.draw(@newInfo)
                         
#             ico: "../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"               
#             ico: "img/TestSamirImg.png"
#             ico: "../../../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
#             ico: "~/LMTLabsScetup/software_library/LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
#             siz: 1
#             vis:true
#             TS_instance : this
                
    draw: ( info ) -> 
        console.log " 'this' in draw() (should be ..graphItem:"
        console.log this
        
        console.log "@_allowToDraw.get():"
        console.log @_allowToDraw.get()
        if @_allowToDraw.get() == true
            SingSVG = SingletonSVG.getInstance()
            Vec_List = []            
            if not @firstDrawing?
                Vec_List = @detect_vector()                
    
            @vecteur_abscisse =  Vec_List[0]   
            @vecteur_ordonnee =  Vec_List[1]
              
            SingSVG.drawing_SVG.drawSVG(info, @vecteur_abscisse, @vecteur_ordonnee, @firstDrawing )
            
            if not @firstDrawing? 
                @firstDrawing = false
            @_allowToDraw.set false
        return true

#     detect_vector: ->
#         res= []
#         for child in @_children
#             if child instanceof TreeItem_Vector
#                 res.push child.vec
#         return res         

    detect_vector: ->
        res= []
        for child in @_children
            if child instanceof TreeItem_SingleData
                if child._children[0] instanceof TreeItem_Vector 
                    detectedVec = child._children[0].vec
                    if not detectedVec?
                        detectedVec = new Vec [0]# return Vector zero, TODO put an alert
                    res.push detectedVec  #TODO verifier le type de children et try ca
    #                 alert "Vector detect"+
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

    
    
    MatrixVecListInversion = (Vec_List)->     
        VecListSize = 0 
        for key, vector of Vec_List
            VecListSize++
            Vecmax = vector.length-1# TODO A Ameliorer car repetition
              
        res = new Array(Vecmax+1)
        for i in [0..Vecmax]
            res[i]= new Array(VecListSize)
            j=0
            for key, vector of Vec_List
                res[i][j] = vector[i]
                j++
        res  
    
# Test: myVecY_List = { 
#                 y1: [5, 56, 17, 9],
#                 y2: [25, 46, 7, 4],
#                 y3: [500, 60.2, 7111, 22] 
#               }
#              
# m = MatrixVecListInversion(myVecY_List)
# console.log m
# console.log m[1]
# console.log m[0]
# console.log m[2]
# Output 
# 
# [ [ 5, 25, 500 ],
#   [ 56, 46, 60.2 ],
#   [ 17, 7, 7111 ],
#   [ 9, 4, 22 ] ]
    
    MatrixInversionMulti = (VecX, VecY_arr)->
#         MatrixInversionMulti = (x, VecY_arr)->
        # VecY or VecX empty # TODO
        # VecY or VecX Not same size # TODO      
        x = VecToArray(VecX) 
        xmax = x.length-1
        iVecYmax = VecY_arr.length-1
        res = new Array(x.length)          
        for i in [0..xmax]
            res[i]= new Array(VecY_arr.length)
            res[i][0] = x[i]
            for j in [0..iVecYmax]
                y = VecToArray(VecY_arr[j]) 
                res[i][j+1] = y[i]  # We assume that x and yi have the same size
    #                 res[i][j+1] = VecY_arr[j][i] # TODO A retirer
        return res
            
        
 #TEST
#       myVecY_arr = [ [5, 56, 17, 9], [25, 46, 7, 4], [500, 60.2, 7111, 22] ]
#       m = MatrixInversionMulti([ 1, 2, 3], myVecY_arr)
#       console.log m
#       console.log m[1]
#       console.log m[0]
#       console.log m[2]
#         Output:
# [ [ 1, 5, 25, 500 ],
#   [ 2, 56, 46, 60.2 ],
#   [ 3, 17, 7, 7111 ],
#   [ 5, 9, 4, 22 ] ]
# [ 2, 56, 46, 60.2 ]
# [ 1, 5, 25, 500 ]
# [ 3, 17, 7, 7111 ]
    
    
    
    
#     MatrixInversion = (VecX, VecY)->
#         # VecY or VecX empty # TODO
#         # VecY or VecX Not same size # TODO
#         x = VecToArray(VecX)
#         y = VecToArray(VecY)    
#         xmax = x.length-1
#         res = []
#         res[i] = [x[i], y[i]] for i in [0..xmax]
#         return res           
    #TEST
    # m = MatrixInversion([ 1, 2, 3], [5, 6, 7])
    # console.log m
    # console.log m[1]
    # console.log m[0]
    # console.log m[2]
    # Test Output 
    # [ [ 1, 5 ], [ 2, 6 ], [ 3, 7 ] ]
    # [ 2, 6 ]
    # [ 1, 5 ]
    # [ 3, 7 ] 
      
      
    # obtenir la position réelle dans le canvas
    getLeft: ( l ) ->
        if l.offsetParent?
            return l.offsetLeft + @getLeft( l.offsetParent )
        else
            return l.offsetLeft

    # obtenir la position réelle dans le canvas
    getTop: ( l ) ->
        if l.offsetParent?
            return l.offsetTop + @getTop( l.offsetParent )
        else
            return l.offsetTop
            
    onPopupClose: ( app ) =>
        document.onkeydown = undefined
        app.active_key.set true
        
        #TODO A completer
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

