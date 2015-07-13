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
    
    drawSVG_MultiVec:( app_data, info, @curvesList, firstDrawing, allowToDraw, graphSetting ) ->
        console.log "fonctiondrawSVG_MultiVec, firstDrawing:", firstDrawing
        
#         if not firstDrawing? #TEST
        @initDrawing_MultiVec(app_data, info, @curvesList, allowToDraw, graphSetting )

    initDrawing_MultiVec:(app_data, info, @curvesList, allowToDraw, graphSetting )  ->    

        @GS = graphSetting        
        console.log "@GS:", @GS           
                    
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
# TEST        
#         .x( (d)=>
#                   console.log "d in line:"
#                   console.log d
#                   keys = subk for own subk of k (k for own k of d)
#                   return @_x(d[k[keys[0]]]))
#         .y( (d)=>
#                 keys = subk for own subk of k (k for own k of d)
#                 return @_y(d[k[keys[1]]]))
#       TEST          
        
#         console.assert(
        @_svgD3 = d3.select( info.ctx_svg()).style("background","white")
#         .style('position','relative')
#         .datum(@_data)
        .append("svg")
        .attr("width", @_widthSVG + @_margin.left + @_margin.right)
        .attr("height", @_heightSVG + @_margin.top + @_margin.bottom)
        .append("g")
        .attr("transform", "translate(#{@_margin.left},#{@_margin.top})") 
        
#         @_svgD3.append("g")
#         .attr("transform", "translate(#{@_margin.left},#{@_margin.top})")  
        
# TEST à remettre         
#         .append("rect")
#         .attr("width", "100%")
#         .attr("height", "100%")
#         .attr("fill", "white")
# TEST à remettre final

        curvesListIndexMax = @curvesList.length-1
        color.domain( [0..curvesListIndexMax])             
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
#             .attr("transform", "translate(#{@_margin.left},#{@_margin.top})") 
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
# .append("text")
#         .attr("transform",(d)=> 
#                         dvalue = d.value
#                         keys = (k for own k of dvalue)
#                         return "translate(" + @_x(dvalue[keys[0]]) + "," + @_y(dvalue[keys[1]])+ ")")
# #         .attr("transform",(d)-> "translate(" + @_x(d.value.date) + "," + @_y(d.value.temperature) + ")"
#         .attr("x",  @GS.xTextPosition)
#         .attr("dy", @GS.dyTextOffset)
#         .text((d) -> 
#                 keys = (k for own k of d.value)
#                 keys[1]+":"+d.name)
        
                
        @_svgD3.append("g")
#             .attr("transform", "translate(#{@_margin.left},#{@_margin.top})") 
            .attr("class", "y axis")
            .call(@_yAxis)
#         .append("text")
#             .attr("transform", "rotate(-90)")
#             .attr("y", 6)
#             .attr("dy", ".71em")
#             .style("text-anchor", "end")
#             .text("Temperature (oF)")
        
# # TODO a remettre

#         console.log "@_data:" 
#         console.log @_data
#         
#         @_svgD3.selectAll(".dot")
#         .data(@_data)
#         .enter().append("circle")
#         
#         .attr("class", "dot")
#         .attr("cx", line.x())
#         .attr("cy", line.y())
#         .attr("r", 3.5)
        
        sessionItem = app_data.tree_items.detect ( x ) -> x instanceof SessionItem
        graphti = sessionItem._children.detect ( x ) -> x instanceof TestSamirGraphItem 
        @tic = graphti._children.detect ( x ) -> x instanceof TreeItem_Curves
        console.log "tic",@tic
        
        
        curve = @_svgD3.selectAll(".curve")
        .data(curves)
        .enter().append("g")
#         .attr("transform", "translate(#{@_margin.left},#{@_margin.top})") 
        .attr("class", "curve")
              
        curve.append("path")
        .attr("class", "line")
        .attr("d", (d) -> 
                line(d.values))
        .style("stroke", (d)=>                      
                        col = @hexToRgb(color(d.name).toString())
                        for curv in @tic._curves_List when curv._name.equals d.name
                            curv.color.color.set new Color col.r, col.g, col.b
#                             curv.color.r.set col.r
#                             curv.color.g.set col.g
#                             curv.color.b.set col.b
                        for cs in @tic.curves.lst when cs.aggregate.name.equals d.name
#                             cs.aggregate.color.lst.clear()
#                             col = @hexToRgb(color(d.name).toString())

#                             cs.aggregate.color.set new Color col.r, col.g, col.b
#                             TEST a remettre
#                             cs.aggregate.color.r.set col.r
#                             cs.aggregate.color.g.set col.g
#                             cs.aggregate.color.b.set col.b
#                             remove: ( item ) ->
#                             unshift: ( element ) ->
#                             has: ( f ) ->
#                             indexOf: ( v ) ->
#                             cs.aggregate.color.unshift()
                            
                            colorD3JS = new ColorD3JS
                            searchedColor = key+"" for own key, value of colorD3JS.colors when value == color(d.name)
                            for own key, value of colorD3JS.colors when value == color(d.name)
                                console.log "value"
                                console.log value
                                console.log "color(d.name)"
                                console.log color(d.name)
                                
#                                 TEST
#                             try 
#                                 cs.aggregate.color.remove(searchedColor) 
#                                 cs.aggregate.color.unshift(searchedColor)#TODO amelioration
#                             catch error
#                                 console.error()
#                                 TEST final

#                             cs.aggregate.color.lst.push new Color col.r, col.g, col.b
#                             TODO cs.aggregate.gradient.add_color col.r, col.g, col.b
                            console.log "color.range():", color.range()
#                       TEST à remettre debut + le clear au dessus 
#                             for co in color.range() when co != color(d.name)
#                                 console.log "co.toString():"
#                                 console.log co.toString()
#                                 c = @hexToRgb(co.toString())
#     #                             c = @hexToRgb(d3_rgbNumber(co))
#                                 console.log "c:"Lst.co
#                                 console.log c
#                                 cs.aggregate.color.lst.push new Color c.r, c.g, c.b
#                       TEST fin
#                                 TODO cs.aggregate.gradient.add_color c.r, c.g, c.b
                        return color(d.name))#only this line is mandatory TODO faire une fonction pour le reste
#         .data((d)->d.values)

#         .attr('marker-mid', (d,i)=> 'url(#marker_' + @tic._curves_List[i].marker.get()  + ')' )
              
#         curve.append("g")
#               .attr("d", (d)->circle(d.values))
#               .append("circle")
#               .attr("class", "dot")
#               .attr("cx", line.x())
#               .attr("cy", line.y())
#               .attr("r", 3.5)  
                
        
#         @_svgD3.selectAll("path")
# #               .data(curves)
# #             .enter()
#               .append("circle")
#               .attr("class", "dot")
#               .attr("cx", line.x())
#               .attr("cy", line.y())
#               .attr("r", 3.5)  
                        
#         .style({'stroke': 'black', 'stroke-width': 2})
#         console.log "hello08"
#         @markerConv = {"dot": "circle"}
#         @attribute = ""
        console.log "hello09"
        
#         circle = @_svgD3
#         .append("g")
# #         .attr("transform", "translate(#{@_margin.left},#{@_margin.top})")
#         .selectAll(".dot").data(curves)
#         
#         circle.exit().remove()
# 
#         circle.enter().append("circle")#"circle"
#         .attr("class", "dot")#"dot")
#         .attr("cx", line.x((d)->d.values))
#         .attr("cy", line.y((d)->d.values))
#         .attr("r", 3.5)
#         .style("stroke", "black") 
#         .style("fill", "bleu")
#         console.log curve.selectAll("path").data("path.data.values")=> TEST peut etre lu
#         console.log "curve.selectAll('path').data('path.data')"  => !! path existe aussi pour x_axis et y_axis  
#         console.log curve.selectAll('path').data('path.data')
        
#         console.log "curve.selectAll('g.line')"#.data(d)"    => tableau vide
#         console.log curve.selectAll('g.line')#.data(d)
        
        console.log "curve.selectAll(curve.values).data((d)->d)"   
        console.log curve.selectAll("curve.values").data((d)->d)
        
        curve.selectAll("curve.values").data((d)->d)
  #------------------------black      
        console.log "curve.selectAll(.dot).data(curve.values"   
        console.log curve.selectAll(".dot").data("curve.values")
        
        curve.selectAll(".curve").data((d)->d.values)
        .enter().append("circle")
        .style("stroke", "black")
        .attr("fill", (d)-> "black")
        .attr("cx", (d)=>
                        keys = (k for own k of d)
                        return @_x(d[keys[0]]))
        .attr("cy",  (d)=>
                        keys = (k for own k of d)
                        return @_y(d[keys[1]]))
        .attr("r", 3.5)

        curve#.append("text")
        .datum((d)-> 
                    console.log "d final:", d
                    {name: d.name, value: d.values[d.values.length - 1]})
        .append("text")
        .attr("transform",(d)=> 
                        dvalue = d.value
                        keys = (k for own k of dvalue)
                        return "translate(" + @_x(dvalue[keys[0]]) + "," + @_y(dvalue[keys[1]])+ ")")
#         .attr("transform",(d)-> "translate(" + @_x(d.value.date) + "," + @_y(d.value.temperature) + ")"
        .attr("x",  @GS.xTextPosition)
        .attr("dy", @GS.dyTextOffset)
        .text((d) -> 
                keys = (k for own k of d.value)
                keys[1]+":"+d.name)  

        allowToDraw.set false #TEST
        firstDrawing= false
    
    #         defs =  @_svgD3.append("defs")
#         markers = [   { id: 0, name: 'circle', path: 'M 0, 0  m -5, 0  a 5,5 0 1,0 10,0  a 5,5 0 1,0 -10,0', viewbox: '-6 -6 12 12' }
#                     , { id: 1, name: 'square', path: 'M 0,0 m -5,-5 L 5,-5 L 5,5 L -5,5 Z', viewbox: '-5 -5 10 10' }
#                     , { id: 2, name: 'arrow', path: 'M 0,0 m -5,-5 L 5,0 L -5,5 Z', viewbox: '-5 -5 10 10' }
#                     , { id: 2, name: 'stub', path: 'M 0,0 m -1,-5 L 1,-5 L 1,5 L -1,5 Z', viewbox: '-1 -5 2 10' } ]             
#         
#         marker = defs.selectAll('marker')
#           .data(markers)
#           .enter()
#           .append('svg:marker')
#             .attr('id', (d)->return 'marker_' + d.name)
#             .attr('markerHeight', 5)
#             .attr('markerWidth', 5)
#             .attr('markerUnits', 'strokeWidth')
#             .attr('orient', 'auto')
#             .attr('refX', 0)
#             .attr('refY', 0)
#             .attr('viewBox', (d)-> return d.viewbox )
#             .append('svg:path')
#             .attr('d', (d)-> return d.path )
#               .attr('fill', (d,i) -> return color(i))
    
    
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
        
#         @_svgD3.selectAll(".dot")
#               .data(@_data)
#             .enter().append("circle")
#               .attr("class", "dot")
#               .attr("cx", line.x())
#               .attr("cy", line.y())
#               .attr("r", 3.5)        
    
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
