class TestSamirGraphItem extends TreeItem_Computable
    constructor: (name = "Graph") ->
        super()
        
        # default values
        @_name.set name
        @_viewable.set true
        
        @_nb_values    = 361         #To see until 360    
        @_vec_x        = new Vec [1, 72, 3, 24]
        @_vec_y        = new Vec [45, 2, 3, 0]
        # attributes
        
#         @fill_x_y()                 # TEST   TODO A remettre si onload non necessaire
        
        treeItem_x = new TreeItem_Vector(@_vec_x, "EssaiAbscissa")
        treeItem_y = new TreeItem_Vector(@_vec_y, "EssaiOrdinate")

#         @mod_attr @_children, [treeItem_x, treeItem_y]
        @add_child treeItem_x
        @add_child treeItem_y
        @add_attr
            _issimGraph   : new IssimGraph
            constrVal: new ConstrainedVal( 7, { min: 0, max: 15 } )
        
#             _vec_x        : new Vec
#             _vec_y        : new Vec
#             _v1           : new Vec
#             _v2           : new Vec
#             _v3           : new Vec
            
        

                
    #TEST             
    cosDeg: (number, precision)->
        console.log number*(Math.PI/180)
        parseFloat (Math.cos(number*(Math.PI/180))).toPrecision(5)


    fill_x_y: ()->
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
    sub_canvas_items: ->
        [ @_issimGraph ]     
    
    display_suppl_context_actions: ( context_action )  ->
        context_action.push
            txt: "link"
            ico: "img/TestSamirImg.png"
#             ico: "../../../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
#             ico: "~/LMTLabsScetup/software_library/LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
            siz: 1
            TS_instance : this
            fun: ( evt, app) =>
                vec_arr = detect_vector()
                         

                app.active_key.set false
    
    #TODO
    draw: ( info ) =>        
        Canvas_div = info.cm.canvas
        width = Canvas_div.offsetWidth
        height = Canvas_div.offsetHeight        
        
#         Vec_List = @attr_Veclist this #TODO
        
        Vec_List = @_detect_vector()
        @_vec_x = Vec_List[0] #TEST
        @vec_Y_tab = Vec_List[1..]
#         @_data = MatrixVecListInversion Vec_List       
        @_data= MatrixInversionMulti(@_vec_x, @vec_Y_tab)
#         @_data= MatrixInversion(@_vec_x, @_vec_y)
        
        @_margin = {top: 20, right: 30, bottom: 30, left: 40}
    #       @_margin = {top: @_top, right: 30, bottom: 30, left: @_left},     
        @_width = width - @_margin.left - @_margin.right
        @_height = height - @_margin.top - @_margin.bottom
                   

        [minVecX, maxVecX] = min_max_Vec [@_vec_x]
        [minVecY, maxVecY] = min_max_Vec @vec_Y_tab
        
        @_x = d3.scale.linear()
            .domain([minVecX, maxVecX])
            .range([0, @_width])
        a = d3     

        @_y = d3.scale.linear()
            .domain([minVecY, maxVecY])
            .range([@_height, 0])

        @_xAxis = d3.svg.axis()
            .scale(@_x)
            .orient("bottom")

        @_yAxis = d3.svg.axis()
            .scale(@_y)
            .orient("left")

        line = d3.svg.line()
            .interpolate("monotone")
#             .interpolate("basis")
            .x( (d)=> @_x(d[0]))
            .y( (d)=> @_y(d[1]))
                
          
    #         @_svg = d3.select("."+@el.className).append("svg")
          
#           @_svg = d3.select("."+divCanvas.className).append("canvas")
#           @_svg = d3.selectNode("canvas")
        @_svg = d3.select("CANVAS").datum(@_data)
                    .attr("width", @_width + @_margin.left + @_margin.right)
#                     .attr("height", @_height + @_margin.top + @_margin.bottom)
#                     .append("g")
#                     .attr("transform", "translate(" + @_margin.left + "," + @_margin.top + ")") 
                
    _detect_vector: ->
        res= []
        for child in @_children
            if child instanceof TreeItem_Vector
                res.push child.vec
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
    
    VecToArray = (VectX)->
        VectXlength = VectX.length-1         
        x=[]
        for i in [0..VectXlength]
  #             elem_i = VectX.slice(i, i+1)
            elem_i = VectX[i]
            data_i_n = elem_i.get()
            data_i = data_i_n.valueOf()
            x.push data_i
        return x 
    
    
    
        # vec_tab is an array of Vec
    # outputs: min and max values from vec_tab  
    min_max_Vec = ( vec_tab  = [])->
        resMin =[]
        resMax =[]
        vec_tabLength = vec_tab.length - 1
        for i in [0..vec_tabLength]
            vec=[]
            vec[i] = VecToArray(vec_tab[i])
            veciLength = vec[i].length - 1
            vec[i].sort( (a,b) -> a - b)
            resMin[i] = vec[i][0]
            resMax[i] = vec[i][veciLength]
        resMin.sort((a,b) -> a - b)     
        resMax.sort((a,b) -> a - b)
        resMaxLength = resMax.length - 1
        min = resMin[0]
        max = resMax[resMaxLength]
        [min, max]    
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
    
    
    
    
    MatrixInversion = (VecX, VecY)->
        # VecY or VecX empty # TODO
        # VecY or VecX Not same size # TODO
        x = VecToArray(VecX)
        y = VecToArray(VecY)    
        xmax = x.length-1
        res = []
        res[i] = [x[i], y[i]] for i in [0..xmax]
        return res           
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
      
        
#     onchange: ->#TODO 
#         if @graphItem.has_been_directly_modified
        #TODO A finir            
#             block = new_dom_element
#                 parentNode : @el
#                 nodeName   : "span"
#                 
#         @clear_page(@container_global)
#         if @app_data.selected_tree_items.has_been_directly_modified    
      
      
      
      
      
      
      
      
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
