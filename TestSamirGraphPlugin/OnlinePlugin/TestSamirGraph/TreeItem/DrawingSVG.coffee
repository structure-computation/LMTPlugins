# Copyright 2012 Structure Computation  www.structure-computation.com
# Copyright 2012 Hugo Leclerc
#
# This file is part of Soda.
#
# Soda is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Soda is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with Soda. If not, see <http://www.gnu.org/licenses/>.

# from mbostock’s block #3884955

class DrawingSVG
    constructor: ->    
    #nothing for now
    
    drawSVG_MultiVec:( app_data, info, @curvesList, notfirstDrawing, allowToDraw, graphSetting ) ->
        console.log "In Drawing, au début allowToDraw.get():", allowToDraw.get()
#         if not firstDrawing? #TEST
        @initDrawing_MultiVec(app_data, info, @curvesList, notfirstDrawing, allowToDraw, graphSetting )

    initDrawing_MultiVec:(app_data, info, @curvesList, notfirstDrawing, allowToDraw, graphSetting )  ->    
        console.log "@curvesList"
        console.log @curvesList
        
        @GS = graphSetting        
#         console.log "@GS:", @GS           
                    
        vec_Lst = new Lst
        vec_Lst.push @curvesList[0]?.abscissa_vec.get()
        for c in @curvesList
            vec_Lst.push c.ordinate_vec.get()
        
        VecX = @curvesList[0]?.abscissa_vec.get()
        VecY_arr = [] 
        VecY_arr.push c?.ordinate_vec.get() for c in curvesList             
        @_data = matrixTransposeMulti VecX, VecY_arr
        console.log "@_data:", @_data
        console.log "info :", info

        width = info.w 
        height = info.h
        console.log "width = info.w :", width
        console.log "height = info.h :", height
        
        m= @GS.margin
        @_margin = {top: m.top, right: m.right, bottom: m.bottom, left: m.left} #TODO +40 correspond à la partie haute de la fenetre avec des possiblité de spliter       
        
        @_widthSVG = width - @_margin.left - @_margin.right
        @_heightSVG = height - @_margin.top - @_margin.bottom
        
        console.log "@_widthSVG:", @_widthSVG
        
        [minVecY, maxVecY] = min_max_Vec vec_Lst        # essayer avec D3.extent qui donne min et max tableau
        
        console.log "minVecY:", minVecY
        console.log "maxVecY:", maxVecY
       
        @_x = d3.scale.linear()
        .range([0, @_widthSVG])
        
        @_y = d3.scale.linear()
        .range([@_heightSVG, 0])

        color = d3.scale.category20()
        @_xAxis = d3.svg.axis()
        .scale(@_x)
        .orient(@GS.X_orient)# TEST should be "bottom"
        
        @_yAxis = d3.svg.axis()
        .scale(@_y)
        .orient(@GS.Y_orient)# TEST should be "right"
        
        console.log "d3.svg.line()!!!"
        console.log d3.svg.line() 
        
        line = d3.svg.line()
        .interpolate(@GS.interpolation)    # TEST should be "monotone"
        .x( (d)=>
                  console.log "d in line:"
                  console.log d
                  keys = (k for own k of d)
                  return @_x(d[keys[0]]))
        .y( (d)=>
                keys = (k for own k of d)
                return @_y(d[keys[1]]))  

        console.log "notfirstDrawing in drawing before appending svg"
        console.log notfirstDrawing
    
        if notfirstDrawing
            d3.selectAll("svg#svg_id").remove()

        @_svgD3 = d3.select( info.ctx_svg()).style("background","white")           
        .append("svg")
        .attr("id", "svg_id")
        .attr("width", @_widthSVG + @_margin.left + @_margin.right)
        .attr("height", @_heightSVG + @_margin.top + @_margin.bottom)
        .append("g")
        .attr("transform", "translate(#{@_margin.left},#{@_margin.top})")            

        console.log "@_svgD3 after appending svg"
        console.log @_svgD3
        
# TEST à remettre         
#         .append("rect")
#         .attr("width", "100%")
#         .attr("height", "100%")
#         .attr("fill", "white")
# TEST à remettre final

        curvesListIndexMax = @curvesList.length-1
        color.domain( [0..curvesListIndexMax])    
        console.log "color:"                                                          
        console.log color
        
        curves = color.domain().map (curvesIndex)=>
                                    c = @curvesList[curvesIndex]
                                    vecIndexMax = c.ordinate_vec.length-1
                                    return res_c = 
                                            name : c._name.get(), #TEST sinon curvesIndex only
                                            values: @_data.map (d) =>  
                                                                    key_abs = c.abscissa_name.get()
                                                                    key_ord = c.ordinate_name.get()
                                                                    res = {}
                                                                    res[key_abs] = d[0]# equivalent to d.date
                                                                    res[key_ord] = d[curvesIndex+1]                                                                       
                                                                    return res                                

        console.log "curves"                                                          
        console.log curves
        
        @_x.domain(d3.extent(@_data, (d) -> d[0]))

        @_y.domain([
          d3.min(curves, (c)-> d3.min(c.values, (v) ->
                                                  keys = (k for own k of v)
                                                  return v[keys[1]])),
          d3.max(curves, (c)-> d3.max(c.values, (v) -> 
                                                  keys = (k for own k of v)
                                                  return v[keys[1]]))
          ])
          
        console.log  "@_heightSVG: ", @_heightSVG

        @_svgD3.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + @_heightSVG+ ")")
            .call(@_xAxis)
            #TEST
        @_svgD3.append("text")
#             .attr("transform", "rotate(-90)")
            .attr("x", @_widthSVG)
            .attr("y", @_heightSVG)
            .attr("dx", ".71em")
            .style("text-anchor", "end")
            .text(@curvesList[0].abscissa_name.get())
                
        @_svgD3.append("g")
            .attr("class", "y axis")
            .call(@_yAxis)

        sessionItem = app_data.tree_items.detect ( x ) -> x instanceof SessionItem
        graphti = sessionItem._children.detect ( x ) -> x instanceof TestSamirGraphItem 
        @tic = graphti._children.detect ( x ) -> x instanceof TreeItem_Curves
        console.log "tic",@tic
        
        curve = @_svgD3.selectAll(".curve")
        .data(curves)
        .enter().append("g")
        .attr("class", "curve")
              
        curve.append("path")
        .attr("class", "line")
        .attr("d", (d) -> 
                line(d.values))
        .style("stroke", (d)=>   
                        colorD3JS = new ColorD3JS
                        searchedColor = key+"" for own key, value of colorD3JS.colors when value == color(d.name)
                        for own key, value of colorD3JS.colors when value == color(d.name)
                                console.log "value"
                                console.log value
                                console.log "color(d.name)"
                                console.log color(d.name)
                        
                        col = @hexToRgb(color(d.name).toString())
                        for curv in @tic._curves_List when curv._name.equals d.name
                            curve_color = new Color col.r, col.g, col.b
                            curv.color.set curve_color
                            curv.colorName.set searchedColor

                       
                        for cs in @tic.curves.lst when cs.aggregate.name.equals d.name
                            console.log "color.range():", color.range()

                        return color(d.name))#only this line is mandatory TODO faire une fonction pour le reste

     
#         curve.selectAll(".curve").data((d)->d.values)
#         .enter().append("circle")
#         .style("stroke", "black")
#         .attr("fill", (d)-> "black")
#         .attr("cx", (d)=>
#                         keys = (k for own k of d)
#                         return @_x(d[keys[0]]))
#         .attr("cy",  (d)=>
#                         keys = (k for own k of d)
#                         return @_y(d[keys[1]]))
#         .attr("r",3.5)

#         switch day
#           when "Mon" then go work
#           when "Tue" then go relax
#           when "Thu" then go iceFishing
#           when "Fri", "Sat"
#             if day is bingoDay
#               go bingo
#               go dancing
#           when "Sun" then go church
#           else go work
# score = 76
# grade = switch
#   when score < 60 then 'F'
#   when score < 70 then 'D'
#   when score < 80 then 'C'
#   when score < 90 then 'B'
#   else 'A'

#TEST .call pour d3js func

# ****************MARKER *************************************
#         markerWidth = 3.5
#         markerHeight = 3.5
#         curve.selectAll(".curve").data((d)->d.values)
#         .enter()
#         .append("g")
# #         .attr("id", "rectangle")
#         .append("rect")
#         .style("stroke", "black")
#         .attr("fill", (d)-> "red")
#         .attr("x", (d)=>
#                         keys = (k for own k of d)
#                         leftTopcornerXrec= @_x(d[keys[0]])
#                         return leftTopcornerXrec - (markerWidth/2)
#                         )
#         .attr("y",  (d)=>
#                         keys = (k for own k of d)
#                         leftTopcornerYrec=  @_y(d[keys[1]])
#                         return leftTopcornerYrec - (markerHeight/2)
#                         )
# #         .attr("transform", "rotate(-90)")
#         .attr("width",3.5)
#         .attr("height",3.5)
        @markerWidth = 3.5  #TODO mettre dans graphSetting 
        @markerHeight = 3.5
                
        curve.selectAll(".curve").call(@makeCircle)

#         curve.selectAll(".curve").call(@makeRectangle)
        
        
#********************************************************************
        
#         curve.selectAll(".curve").data((d)->d.values)
#         .enter()
#         .append("g")
#         .attr("transform", "translate(-"+ (markerWidth)/2+",-"+ (markerHeight)/2+")")
# #         .attr("id", "rectangle")
# #         .attr("transform", "rotate(-90)")
#         .append("rect")
#         .style("stroke", "black")
#         .attr("fill", (d)-> "red")
#         .attr("x", (d)=>
#                         keys = (k for own k of d)
#                         leftTopcornerXrec= @_x(d[keys[0]])
#                         return leftTopcornerXrec 
#                         )
#         .attr("y",  (d)=>
#                         keys = (k for own k of d)
#                         leftTopcornerYrec=  @_y(d[keys[1]])
#                         return leftTopcornerYrec
#                         )
#         .attr("transform", (d)=>
#                         keys = (k for own k of d)
#                         leftTopcornerXrec= @_x(d[keys[0]])
#                         leftTopcornerYrec=  @_y(d[keys[1]])
#                         return "rotate(-90, "+leftTopcornerXrec+","+ leftTopcornerYrec+")"
#                         )
#         .attr("width",3.5)
#         .attr("height",3.5)
        
#********************************************************************         
        
#         @.SelectAll("#rectangle")
#         .attr("transform", "rotate(" + Math.PI * 90 + ")")
        
#       ********************** TEXT
        curve
        .datum((d)-> 
                    console.log "d final:", d
                    {name: d.name, value: d.values[d.values.length - 1]})
        .append("text")
        .attr("transform",(d)=> 
                        dvalue = d.value
                        keys = (k for own k of dvalue)
                        return "translate(" + @_x(dvalue[keys[0]]) + "," + @_y(dvalue[keys[1]])+ ")")
        .attr("x",  @GS.xTextPosition)
        .attr("y",  20)
        .attr("dy", @GS.dyTextOffset)
        .text((d) -> 
                keys = (k for own k of d.value)
                keys[1]+":"+d.name)  

        allowToDraw.set false #TEST
        console.log "At the end of Drawing, (should be false) allowToDraw.get():", allowToDraw.get()
        notfirstDrawing= true
    
    makeCircle:(selection)=>
        selection
            .data((d)->d.values)
            .enter().append("circle")
            .style("stroke", "black")
            .attr("fill", (d)-> "black")
            .attr("cx", (d)=>
                            keys = (k for own k of d)
                            return @_x(d[keys[0]]))
            .attr("cy",  (d)=>
                            keys = (k for own k of d)
                            return @_y(d[keys[1]]))
            .attr("r",3.5)
                
                
    makeRectangle:(selection)=>
        selection
            .data((d)->d.values)
            .enter()
            .append("g")
            .attr("transform", "translate(-"+ (markerWidth)/2+",-"+ (markerHeight)/2+")")
    #         .attr("id", "rectangle")
    #         .attr("transform", "rotate(-90)")
            .append("rect")
            .style("stroke", "black")
            .attr("fill", (d)-> "red")
            .attr("x", (d)=>
                            keys = (k for own k of d)
                            leftTopcornerXrec= @_x(d[keys[0]])
                            return leftTopcornerXrec 
                            )
            .attr("y",  (d)=>
                            keys = (k for own k of d)
                            leftTopcornerYrec=  @_y(d[keys[1]])
                            return leftTopcornerYrec
                            )
            .attr("transform", (d)=>
                            keys = (k for own k of d)
                            leftTopcornerXrec= @_x(d[keys[0]])
                            leftTopcornerYrec=  @_y(d[keys[1]])
                            return "rotate(-90, "+leftTopcornerXrec+","+ leftTopcornerYrec+")"
                            )
            .attr("width",3.5)
            .attr("height",3.5)
    
    
    
    wr: (d, message=d)-> 
        console.log if message? message+"" else d+""
        console.log d
        
    drawSVG_2_Vec: ( info, vector1, vector2, firstDrawing ) ->
        if not firstDrawing? 
            @initDrawing_2_Vec( info, vector1, vector2 )

    initDrawing_2_Vec: ( info, vector1, vector2 ) ->    
        Vec_List = []
        Vec_List.push vector1
        Vec_List.push vector2
      
        @_vec_x = Vec_List[0] 
        @_vec_y = Vec_List[1] 
        XArray = VecToArray(@_vec_x)
        XYArray = matrixTranspose(@_vec_x, @_vec_y)
        @_data= OrderAccordingAbscisse(XArray, XYArray)
        
        @_data = matrixTranspose(@_vec_x, @_vec_y)
        width = info.w
        height = info.h        
        @_margin = {top: 20, right: 30, bottom: 30, left: 40}

        @_widthSVG = width - @_margin.left - @_margin.right
        @_heightSVG = height - @_margin.top - @_margin.bottom

        [minVecY, maxVecY] = min_max_Vec [@_vec_y] 
        [minVecX, maxVecX] = min_max_Vec [@_vec_x]
        
        @_x = d3.scale.linear()
            .domain([minVecX, maxVecX])
            .range([0, @_widthSVG])
        @_y = d3.scale.linear()
            .domain([minVecY, maxVecY])
            .range([@_heightSVG, 0])
                 
        @_xAxis = d3.svg.axis()
            .scale(@_x)
            .orient("bottom")
        
        @_yAxis = d3.svg.axis()
            .scale(@_y)
            .orient("right")

        line = d3.svg.line()
                  .interpolate("monotone")
                  .x( (d)=> @_x(d[0]))
                  .y( (d)=> @_y(d[1]))

        @_svgD3 = d3.select( info.ctx_svg()).append("svg")
                .datum(@_data)
                .attr("width", @_widthSVG + @_margin.left + @_margin.right)
                .attr("height", @_heightSVG + @_margin.top + @_margin.bottom)
                .style('position','absolute')
              .append("g")
                .attr("transform", "translate(" + @_margin.left + "," + @_margin.top + ")")    
   
        #background for svg
        @_svgD3.append("rect")
            .attr("width", "100%")
            .attr("height", "100%")
               .style('position','relative')
            .attr("fill", "white");
            
        @_svgD3.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + @_heightSVG +")")
            .call(@_xAxis)

        @_svgD3.append("g")
            .attr("class", "y axis")
            .call(@_yAxis)

        @_svgD3.append("path")
                .attr("class", "line")
                .attr("d", line)
        

    hexToRgb:(hex)->
        result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex)
        if result? 
            return {
                r: parseInt( result[1], 16 ), 
                g: parseInt( result[2], 16 ),
                b: parseInt( result[3], 16 )
                }
                
    matrixTranspose = (VecX, VecY)->
        # VecY or VecX empty # TODO
        # VecY or VecX Not same size # TODO
        x = VecToArray(VecX)
        y = VecToArray(VecY)    
        xmax = x.length-1
        res = []
        res[i] = [x[i], y[i]] for i in [0..xmax]
        return res           
            
    VecToArray = (VectX)->
        VectXlength = VectX.length-1         
        x=[]
        for i in [0..VectXlength]
      #             elem_i = VectX.slice(i, i+1)
            elem_i = VectX[i]
            data_i_n = elem_i
            if data_i_n instanceof Val             
                data_i = data_i_n._data
            else
                data_i = data_i_n.valueOf()
            x.push data_i
        return x 
     
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
        
        
    # X =   [10, 1, 3, 2, 4, 5]
    # Y =  [15, 0, 23, 3, 0, 7]
    # 
    # XY = [ [10, 15], [1, 0], [3, 23], [ 2, 3], [ 4, 0], [ 5, 7]]
    # output:

    # [ [ 1, 0 ], [ 2, 3 ], [ 3, 23 ], [ 4, 0 ], [ 5, 7 ], [ 10, 15 ] ]

    OrderAccordingAbscisse = (XAbscisseArray, XYArray)->
        XMax = XAbscisseArray.length-1
        Xbuff=[]
        Xbuff = XAbscisseArray#.slice(0)
#         Xbuff[i].sort((a,b) -> a - b ) for i in [0..XMax]  
        Xbuff.sort((a,b) -> a - b )
        Res =[]
        Res = XYArray[XAbscisseArray.indexOf(Xbuff[i])] for i in [0..XMax]       
        
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
    

    matrixVecListTranspose = (Vec_List)->     
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
# m = matrixVecListTranspose(myVecY_List)
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
    
    matrixTransposeMulti = (VecX, VecY_arr)->
#         matrixTransposeMulti = (x, VecY_arr)->
        # VecY or VecX empty # TODO
        # VecY or VecX Not same size # TODO      
        x = VecToArray(VecX) 
        xmax = x.length-1
        VecY_arr_Max = VecY_arr.length-1
        res = new Array(x.length)          
        for i in [0..xmax]
            res[i]= new Array(VecY_arr.length)
            res[i][0] = x[i]
            for j in [0..VecY_arr_Max]
                y = VecToArray(VecY_arr[j]) 
                res[i][j+1] = y[i]  # We assume that x and yi have the same size
    #                 res[i][j+1] = VecY_arr[j][i] # TODO A retirer
        return res
            
        
 #TEST
#       myVecY_arr = [ [5, 56, 17, 9], [25, 46, 7, 4], [500, 60.2, 7111, 22] ]
#       m = matrixTransposeMulti([ 1, 2, 3, 5], myVecY_arr)
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
    

#     matrixTranspose = (VecX, VecY)->
#         # VecY or VecX empty # TODO
#         # VecY or VecX Not same size # TODO
#         x = VecToArray(VecX)
#         y = VecToArray(VecY)    
#         xmax = x.length-1
#         res = []
#         res[i] = [x[i], y[i]] for i in [0..xmax]
#         return res           
    #TEST
    # m = matrixTranspose([ 1, 2, 3], [5, 6, 7])
    # console.log m
    # console.log m[1]
    # console.log m[0]
    # console.log m[2]
    # Test Output 
    # [ [ 1, 5 ], [ 2, 6 ], [ 3, 7 ] ]
    # [ 2, 6 ]
    # [ 1, 5 ]
    # [ 3, 7 ] 
      
# data = [ [ 2.92, 4.22, 3.69], [2.69, 4.24, 3.77] ]
# Now you can use d3.transpose to pivot the data so you get
# tdata = d3.transpose(data);
# gives you
# [ [2.92, 2.69], [4.22, 4.24], [3.69, 3.77]]      
