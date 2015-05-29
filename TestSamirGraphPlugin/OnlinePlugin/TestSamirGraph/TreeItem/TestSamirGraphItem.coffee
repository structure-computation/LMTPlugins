

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
        
#         @add_attr
#             vecteur_abscisse           : new Vec
#             vecteur_ordonnee           : new Vec
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
        

#         treeItem_x = new TreeItem_Vector(@_vec_x, "abscisse")
#         treeItem_y = new TreeItem_Vector(@_vec_y, "ordonnee ")
        treeItem_x = new TreeItem_SingleData("abscisse")
        treeItem_y = new TreeItem_SingleData("ordonnee")
        
# #         @mod_attr @_children, [treeItem_x, treeItem_y]
        
        @add_attr
            _issimGraph   : new IssimGraph
            constrVal: new ConstrainedVal( 7, { min: 0, max: 15 } )
# 
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
            fun: ( evt, app ) =>
#                 instance._allowToDraw.set true
                CurCanvasManager = app.selected_canvas_inst()?[ 0 ]?.cm
                if CurCanvasManager?
                    thislikeItem = CurCanvasManager.items.detect ( x ) -> x instanceof TestSamirGraphItem #TODO +modif le selected+faire == this 
#                     thislikeItem._firstDrawing.set undefined TODO
                    thislikeItem._allowToDraw.set true
                    CurCanvasManager.draw()
#                 vec_arr = detect_vector()
#                 new NewCanvasPanelAdder         
                
#             ico: "../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"               
#             ico: "img/TestSamirImg.png"
#             ico: "../../../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
#             ico: "~/LMTLabsScetup/software_library/LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
#             siz: 1
#             vis:true
#             TS_instance : this

                
    draw: ( info ) ->        
        if @_allowToDraw.get() == true
            SingSVG = SingletonSVG.getInstance()
            Vec_List = []            
            if not @firstDrawing?
                Vec_List = @_detect_vector()                
#             else
#                 Vec_List.push @_vec_x#TEST A retirer 
#                 Vec_List.push @_vec_y#TEST A retirer
    #         alert "detected vec: "+Vec_List.join "\n"     
            @vecteur_abscisse =  Vec_List[0]   
            @vecteur_ordonnee =  Vec_List[1]
              
            SingSVG.drawing_SVG.drawSVG(info, @vecteur_abscisse, @vecteur_ordonnee, @firstDrawing )
            
            if not @firstDrawing? 
                @firstDrawing = false
            @_allowToDraw = false
        return true

#     detect_vector: ->
#         res= []
#         for child in @_children
#             if child instanceof TreeItem_Vector
#                 res.push child.vec
#         return res         

    _detect_vector: ->
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

    addNewCanvasPanel : (app)->
#         TODO mettre les variables locales en parametres
#     getCurrentDisplaySettingsItem:->
        display_settings = app.data.selected_display_settings()
#         session = @data.selected_session()
#     getCurrentLayoutManager:->
        currentLayoutManagerData = display_settings._layout        
        newPanelStrengthCoeff = 1 #TEST        
        visiblePanels = currentLayoutManagerData.panel_id_of_term_panels()
        lastVisiblePanels = visiblePanels.length-1
        idLastVisibleCanvas = lastVisiblePanels[visiblePanels.length-1]
        isHorizontal = 0 # 0: vertical
        isNewPanelOnRightSide = 1
        currentLayoutManagerData.mk_split( isHorizontal, isNewPanelOnRightSide, idLastVisibleCanvas, newPanelStrengthCoeff)
        idGraphCanvas = lastVisiblePanels[visiblePanels.length]
        return idGraphCanvas
        
#         curLmRoot = CurrentLayoutManagerData.root
#     
# #     VisiblePanels = getVisiblePanels: ->
#         visiblePanels = curLmRoot.panel_id_of_term_panels()
#                         
# #     add_panel_id to curLmRoot                    #at the end !!
#   
# 
# #         create a newLayoutManagerData
# #         curLmRoot
# #         
# #     { panel_id: "id_0" }
# #         setCurrentDisplaySettingsItem(newLayoutManagerData)


#*********        
        # #         info.cm._init_ctx()
# #         Canvas_div = info.cm.canvas
#         
#         v1 = new Vec [1, 72, 3, 24]
#         v2 = new Vec [45, 2, 3, 0]
#         Vec_List.push v1
#         Vec_List = [] #TEST 
#         Vec_List.push v2
#         
#         width = info.w  #Canvas_div.offsetWidth
#         height = info.h #Canvas_div.offsetHeight        
# 
#         @_vec_x = Vec_List[0] 
#         @_vec_y = Vec_List[1]        
#         
#                 
#         @_data= MatrixInversion(@_vec_x, @_vec_y)
# #         @vec_Y_tab = Vec_List[1..]
# #         @_data= MatrixInversionMulti(@_vec_x, @vec_Y_tab)
# #         Vec_List = @attr_Veclist this
# #         @_data = MatrixVecListInversion Vec_List       
#         
#     #       @_margin = {top: @_top, right: 30, bottom: 30, left: @_left},     
#         @_margin = {top: 20, right: 30, bottom: 30, left: 40}
#         @_width = width - @_margin.left - @_margin.right
#         @_height = height - @_margin.top - @_margin.bottom
#     
# #         [minVecY, maxVecY] = min_max_Vec @vec_Y_tab
#         [minVecY, maxVecY] = min_max_Vec [@_vec_y] 
#         [minVecX, maxVecX] = min_max_Vec [@_vec_x]
#         
#         @_x = d3.scale.linear()
#             .domain([minVecX, maxVecX])
#             .range([0, @_width])
#             @_y = d3.scale.linear()@_svg = d3.select info.ctx_svg()
#             @_svg.style "background-color", "white"
#             .domain([minVecY, maxVecY])
#             .range([@_height, 0])
#             
#         @_xAxis = d3.svg.axis()
#             .scale(@_x)
#             .orient("bottom")
# 
#         @_yAxis = d3.svg.axis()
#             .scale(@_y)
#             .orient("left")
#         
#         line = d3.svg.line()
#             .interpolate("monotone")
# #             .interpolate("basis")
#             .x( (d)=> @_x(d[0]))
#             .y( (d)=> @_y(d[1]))
#         
# #         @_svg = d3.select(ctx).append("svgGraph")
#         
#         @_svg = d3.select info.ctx_svg()
#         @_svg.style "background-color", "white"
#         @_svg.append("svg")
#             .datum(@_data)
#             .attr("width", @_width + @_margin.left + @_margin.right)
#             .attr("height", @_height + @_margin.top + @_margin.bottom)
#             .append("g")
#             .attr("transform", "translate(" + @_margin.left + "," + @_margin.top + ")")    
#         
# #         @_svg.append("rect")
# #             .attr("width", "100%")
# #             .attr("height", "100%")
# #             .attr("fill", "white");
#             
# #         @_svg.append("g")
# #             .attr("class", "x axis")
# #             .attr("transform", "translate(0," + @_height + ")")
# #             .call(@_xAxis)
# # 
# #         @_svg.append("g")
# #             .attr("class", "y axis")
# #             .call(@_yAxis)
# # 
# #         @_svg.append("path")
# #             .attr("class", "line")
# #             .attr("d", line)
# # 
# # #         newFormat = d3.format(",.4") #TEST
# #             
# #         @_svg.selectAll(".dot")
# #             .data(@_data)
# #             .enter().append("circle")
# #             .attr("class", "dot")
# # #             .attr("cx", newFormat line.x())#TEST
# # #             .attr("cy", newFormat line.y())
# #             .attr("cx", line.x())
# #             .attr("cy", line.y())
# #             .attr("r", 3.5)
#         return true
 
          
#         @_svg = d3.select("."+@el.className).append("svg")
          
#           @_svg = d3.select("."+divCanvas.className).append("canvas")
#           @_svg = d3.selectNode("canvas")
#         @_svg = d3.selectNode("CANVAS")
#         range = document.createRange()
#         add_class info.cm.canvas, "Graph_div"#TODO
#         @_svg = d3.select("Graph_div")
#         @_svg = d3.select("."+Canvas_div.className).append("Graph_div")

#         d3.select("div.SelectedCanvas")#TODO TODO
#         @_svg = d3.select("canvas.Graph_div").append("svg")
#         @_svg = d3.select("div.SelectedCanvas").append("svg")#TODO 

#         Canvas_div = new_dom_element
#             parentNode: info.cm.canvas
#             nodeName  : 'div'
#         
#         add_class Canvas_div, "Graph_div"
        
#         @_svg = d3.select("div.Graph_div").append("svg")
        
#         d3.select("div").insert("svg", "canvas")
        
            
        #background for svg
            
            
         
            
            
            
#         SVG_selection = d3.select(".Graph_div")
#         @_svg = SVG_selection   
          
#         @_svg = d3.select("."+Canvas_div.className).append("Graph_div")
#                     .attr("width", @_width + @_margin.left + @_margin.right)
#                     .attr("height", @_height + @_margin.top + @_margin.bottom)
#                     .append("g")
#                     .attr("transform", "translate(" + @_margin.left + "," + @_margin.top + ")") 
# **********************
#         width = ctx.w  #Canvas_div.offsetWidth
#         height = ctx.h #Canvas_div.offsetHeight        
#         
#         
# #         Vec_List = @_detect_vector()
#         v1 = new Vec [1, 72, 3, 24]
#         v2 = new Vec [45, 2, 3, 0]
#         Vec_List = [] #TEST
#         Vec_List.push v1
#         Vec_List.push v2
#         
#         @_vec_x = Vec_List[0] 
#         @_vec_y = Vec_List[1]        
#         
#         @_data= MatrixInversion(@_vec_x, @_vec_y)
# #         @vec_Y_tab = Vec_List[1..]
# #         @_data= MatrixInversionMulti(@_vec_x, @vec_Y_tab)
# #         Vec_List = @attr_Veclist this
# #         @_data = MatrixVecListInversion Vec_List       
#         
#         @_margin = {top: 20, right: 30, bottom: 30, left: 40}
#     #       @_margin = {top: @_top, right: 30, bottom: 30, left: @_left},     
#         @_width = width - @_margin.left - @_margin.right
#         @_height = height - @_margin.top - @_margin.bottom
#                    
# 
# #         [minVecY, maxVecY] = min_max_Vec @vec_Y_tab
#         [minVecY, maxVecY] = min_max_Vec [@_vec_y] 
#         [minVecX, maxVecX] = min_max_Vec [@_vec_x]
#         
#         @_x = d3.scale.linear()
#             .domain([minVecX, maxVecX])
#             .range([0, @_width])
# #         a = d3     
# 
#         @_y = d3.scale.linear()
#             .domain([minVecY, maxVecY])
#             .range([@_height, 0])
# 
#         @_xAxis = d3.svg.axis()
#             .scale(@_x)
#             .orient("bottom")
# 
#         @_yAxis = d3.svg.axis()
#             .scale(@_y)
#             .orient("left")
# 
#         line = d3.svg.line()
#             .interpolate("monotone")
# #             .interpolate("basis")
#             .x( (d)=> @_x(d[0]))
#             .y( (d)=> @_y(d[1]))
#                 
#           
# #         @_svg = d3.select("."+@el.className).append("svg")
#           
# #           @_svg = d3.select("."+divCanvas.className).append("canvas")
# #           @_svg = d3.selectNode("canvas")
# #         @_svg = d3.selectNode("CANVAS")
# #         range = document.createRange()
# #         add_class info.cm.canvas, "Graph_div"#TODO
# #         @_svg = d3.select("Graph_div")
# #         @_svg = d3.select("."+Canvas_div.className).append("Graph_div")
# 
# #         d3.select("div.SelectedCanvas")#TODO TODO
# #         @_svg = d3.select("canvas.Graph_div").append("svg")
# #         @_svg = d3.select("div.SelectedCanvas").append("svg")#TODO 
# 
# #         Canvas_div = new_dom_element
# #             parentNode: info.cm.canvas
# #             nodeName  : 'div'
# #         
# #         add_class Canvas_div, "Graph_div"
#         
# #         @_svg = d3.select("div.Graph_div").append("svg")
#         
# #         d3.select("div").insert("svg", "canvas")
#         
#         @_svg = d3.select(ctx).append("svgGraph")
#             .datum(@_data)
#             .attr("width", @_width + @_margin.left + @_margin.right)
#             .attr("height", @_height + @_margin.top + @_margin.bottom)
#             .append("g")
#             .attr("transform", "translate(" + @_margin.left + "," + @_margin.top + ")")    
#             
#         #background for svg
#         @_svg.append("rect")
#             .attr("width", "100%")
#             .attr("height", "100%")
#             .attr("fill", "white");
#             
#         @_svg.append("g")
#             .attr("class", "x axis")
#             .attr("transform", "translate(0," + @_height + ")")
#             .call(@_xAxis)
# 
#         @_svg.append("g")
#             .attr("class", "y axis")
#             .call(@_yAxis)
# 
#         @_svg.append("path")
#             .attr("class", "line")
#             .attr("d", line)
# 
# #         newFormat = d3.format(",.4") #TEST
#             
#         @_svg.selectAll(".dot")
#             .data(@_data)
#             .enter().append("circle")
#             .attr("class", "dot")
# #             .attr("cx", newFormat line.x())#TEST
# #             .attr("cy", newFormat line.y())
#             .attr("cx", line.x())
#             .attr("cy", line.y())
#             .attr("r", 3.5)    
#             
#          
#             
#             
#             
# #         SVG_selection = d3.select(".Graph_div")
# #         @_svg = SVG_selection   
#           
# #         @_svg = d3.select("."+Canvas_div.className).append("Graph_div")
# #                     .attr("width", @_width + @_margin.left + @_margin.right)
# #                     .attr("height", @_height + @_margin.top + @_margin.bottom)
# #                     .append("g")
# #                     .attr("transform", "translate(" + @_margin.left + "," + @_margin.top + ")") 
# **********************
#*********        
#     VecToArray = (VectX)->
#         VectXlength = VectX.length-1         
#         x=[]
#         for i in [0..VectXlength]
#   #             elem_i = VectX.slice(i, i+1)
#             elem_i = VectX[i]
#             data_i_n = elem_i
#             data_i = data_i_n.valueOf()
#             x.push data_i
#         return x 
    
    
    
        # vec_tab is an array of Vec
    # outputs: min and max values from vec_tab  
#     min_max_Vec = ( vec_tab  = [])->
#         resMin =[]
#         resMax =[]
#         vec_tabLength = vec_tab.length - 1
#         for i in [0..vec_tabLength]
#             vec=[]
#             vec[i] = VecToArray(vec_tab[i])
#             veciLength = vec[i].length - 1
#             vec[i].sort( (a,b) -> a - b)
#             resMin[i] = vec[i][0]
#             resMax[i] = vec[i][veciLength]
#         resMin.sort((a,b) -> a - b)     
#         resMax.sort((a,b) -> a - b)
#         resMaxLength = resMax.length - 1
#         min = resMin[0]
#         max = resMax[resMaxLength]
#         [min, max]    
#         min_max_Vec Test
#           y1 = [ 53, 66, 0, 38]    
#           y2 = [ 87, 11, 4, 1, 12]
#           y3 = [ 5, 6, 666]

#           tt = min_max_Vec [y1, y2, y3]
#           console.log "min= "+tt[0] 
#           console.log "max= "+tt[1]     
#       Outputs:
#         min= 0
#         max= 666
#         [minVecX, maxVecX] = min_max_Vec [@_vec_x1, @_vec_x2, @_vec_x3]
#         [minVecY, maxVecY] = min_max_Vec [@_vec_y1, @_vec_y2, @_vec_y3]
    
    
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

