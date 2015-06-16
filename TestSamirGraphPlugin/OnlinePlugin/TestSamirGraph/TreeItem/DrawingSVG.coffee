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
    
    drawSVG_MultiVec:( info, @curvesList, firstDrawing, graphSetting ) ->
        if not firstDrawing? 
            @initDrawing_MultiVec( info, @curvesList, graphSetting )
    
#     drawSVG_MultiVec_Long:( info, vecX, vecY_lst, firstDrawing ) ->
#         if not firstDrawing? 
#             @initDrawing_MultiVec_Long( info, vecX, vecY_lst )

    initDrawing_MultiVec:( info, @curvesList, graphSetting )  ->           
        @GS = graphSetting        
        console.log "@GS:"
        console.log @GS
        
#         curvesListMaxIndex = @curvesList.length-1
#         
#         fill_vec = (vec, res, i)->
#                 vecMaxIndex = vec.length-1
#                 res_2 = new Array(vec.length)
#                 for j in [0..vecMaxIndex]
#                     res_2[j] = vec[j].get()
#                 return res_2
#                 
#         res = new Array(@curvesList.length)
#         for i in [0..curvesListMaxIndex]
#             if i == 0
#                 vec = @curvesList[0]?.abscissa_vec
#             else
#                 vec = @curvesList[i]?.ordinate_vec                 
                    
        vec_Lst = new Lst
        vec_Lst.push @curvesList[0]?.abscissa_vec.get()
#         vec_Lst.push @curvesList[0]?.abscissa_vec.deep_copy()
        for c in @curvesList
            vec_Lst.push c.ordinate_vec.get()
#             curvesListMaxIndex = @curvesList.length-1
#             vec_temp = []
#         for i in [0..curvesListMaxIndex]
#             vec_Lst[i+1] = @curvesList[i]?.ordinate_vec.deep_copy()
#             vec_Lst.push vec_temp

            
#         console.log "Matrix to transpose (first vector is abscissa):"
#         console.log vec_Lst.join ','
        
        VecX = @curvesList[0]?.abscissa_vec.get()
        VecY_arr = [] 
        VecY_arr.push c?.ordinate_vec.get() for c in curvesList
        @_data = matrixTransposeMulti(VecX, VecY_arr)
        console.log "@_data:"
        console.log @_data
#         @_data = d3.transpose(vec_Lst)

        console.log "info :"
        console.log info

        width = info.w 
        height = info.h
        console.log "width = info.w :"
        console.log width
        
        console.log "height = info.h :"
        console.log height
        
        m= @GS.margin
        @_margin = {top: m.top, right: m.right, bottom: m.bottom, left: m.left}        
#         console.log "@_margin:"
#         console.log @_margin
        
        @_widthSVG = width - @_margin.left - @_margin.right
        @_heightSVG = height - @_margin.top - @_margin.bottom
        
        console.log "@_widthSVG:"
        console.log @_widthSVG
        
        [minVecY, maxVecY] = min_max_Vec vec_Lst        # essayer avec D3.extent qui donne min et max tableau
        
        console.log "minVecY:"
        console.log minVecY
        console.log "maxVecY:"
        console.log maxVecY
       
        @_x = d3.scale.linear()
#         .domain([minVecX, maxVecX])
        .range([0, @_widthSVG])
        
        console.log "@_x:"
        console.log @_x
        
        @_y = d3.scale.linear()
#         .domain([minVecY, maxVecY])
        .range([@_heightSVG, 0])
        
        color = d3.scale.category10()   #TEST
        
        @_xAxis = d3.svg.axis()
        .scale(@_x)
        .orient(@GS.X_orient)# TEST should be "bottom"
        
        @_yAxis = d3.svg.axis()
        .scale(@_y)
        .orient(@GS.Y_orient)# TEST should be "right"
        
#         console.log "@GS.interpolation:"
#         console.log @GS.interpolation
        
        line = d3.svg.line()
        .interpolate(@GS.interpolation)    # TEST should be "monotone"
        .x( (d)=>
                  keys = (k for own k of d)
                  return @_x(d[keys[0]]))
        .y( (d)=>
                keys = (k for own k of d)
                return @_y(d[keys[1]]))

        console.log "line:"
        console.log line
        
        @_svgD3 = d3.select( info.ctx_svg()).append("svg")
        .datum(@_data)
        .attr("width", @_widthSVG + @_margin.left + @_margin.right)
        .attr("height", @_heightSVG + @_margin.top + @_margin.bottom)
#         .style('position','absolute')
        .append("g")
        .attr("transform", "translate(" + @_margin.left + "," + @_margin.top + ")")    
         
        #background for svg
        @_svgD3.append("rect")
        .attr("width", "100%")
        .attr("height", "100%")
        .attr("fill", "white");
        
        curvesListIndexMax = @curvesList.length-1
        color.domain( [0..curvesListIndexMax])        
#         curves = color.domain().map((name)-> 
#                                             name :name, 
#                                             values: data.map( d->
#                                             abscissa: d[0]), ordinate: +d[name]) 

        curves = color.domain().map (curvesIndex)=>
                                    c = @curvesList[curvesIndex]
                                    vecIndexMax = c.ordinate_vec.length-1
                                    console.log "vecIndexMax:"
                                    console.log vecIndexMax
                                    return res_c = 
                                            name : c._name.get(), #TEST sinon curvesIndex only
                                            values: @_data.map (d) =>  
                                                                    key_abs = c.abscissa_name.get()
                                                                    key_ord = c.ordinate_name.get()
                                                                    res = {}
                                                                    res[key_abs] = d[0]# equivalent to d.date
                                                                    res[key_ord] = d[curvesIndex+1]                                                                       
#                                                                     console.log "res:"
#                                                                     console.log res
                                                                    return res
                                

        @_x.domain(d3.extent(@_data, (d) ->
                                            console.log "d[0]:"
                                            console.log d[0]
                                            d[0]))
            
        console.log "y domain min"
        console.log d3.min(curves, (c)-> d3.min(c.values, (v) ->
                                                  keys = (k for own k of v)
                                                  return v[keys[1]]))
        console.log "y domain max"
        console.log d3.max(curves, (c)-> d3.max(c.values, (v) -> 
                                                  keys = (k for own k of v)
                                                  return v[keys[1]]))
           
        @_y.domain([
          d3.min(curves, (c)-> d3.min(c.values, (v) ->
                                                  keys = (k for own k of v)
                                                  return v[keys[1]])),
          d3.max(curves, (c)-> d3.max(c.values, (v) -> 
                                                  keys = (k for own k of v)
                                                  return v[keys[1]]))
          ])
          
        console.log  "@_heightSVG"
        console.log  @_heightSVG
        
        @_svgD3.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + @_heightSVG + ")")
            .call(@_xAxis)

        @_svgD3.append("g")
        .attr("class", "y axis")
        .call(@_yAxis)
# TODO a remettre
        @_svgD3.selectAll(".dot")
        .data(@_data)
        .enter().append("circle")
        .attr("class", "dot")
        .attr("cx", line.x())
        .attr("cy", line.y())
        .attr("r", 3.5)

        curve = @_svgD3.selectAll(".curve")
        .data(curves)
        .enter().append("g")
        .attr("class", "curve")
        
#         @_svgD3.append("path")
        curve.append("path")
        .attr("class", "line")
        .attr("d", (d) -> 
                console.log "d.values"
                console.log d.values
                line(d.values))
#         .attr("d", line)
        .style("stroke", (d)-> color(d.name))

        
        curve.append("text")
        .datum((d)-> 
                    console.log "d final:"
                    console.log d
                    {name: d.name, value: d.values[d.values.length - 1]})
        .attr("transform",(d)-> 
                        dvalue = d.value
                        keys = (k for own k of dvalue)
                        console.log "keys:"
                        console.log keys
                        console.log "d.value:"
                        console.log d.value
                        return "translate(" + @_x(dvalue[keys[0]]) + "," + @_y(dvalue[keys[1]]) + ")")
#         .attr("transform",(d)-> "translate(" + @_x(d.value.date) + "," + @_y(d.value.temperature) + ")"
        .attr("x",  @GS.xTextPosition)
        .attr("dy", @GS.dyTextOffset)
        .text((d) -> d.name)  
 
#     drawSVG_MultiVec_WithoutCurves:( info, vec_Lst, firstDrawing, GraphSetting ) ->
#         if not firstDrawing? 
#         @initDrawing_MultiVec( info, vec_Lst )
    
#     drawSVG_MultiVec_Long:( info, vecX, vecY_lst, firstDrawing ) ->
#         if not firstDrawing? 
#             @initDrawing_MultiVec_Long( info, vecX, vecY_lst )

#     initDrawing_MultiVec_WithoutCurves:( info, vec_Lst, GraphSetting )  ->#TODO a finir           
#         @GS = GraphSetting        
#         alert "Matrix to transpose (first vector is abscissa)"+ vec_Lst.join ','
#         @_data = d3.transpose(vec_Lst);
#         alert "transposed Matrix (first coordinate is abscissa)"+ vec_Lst.join ','
#         
#         width = info.w  #Canvas_div.offsetWidth
#         height = info.h #Canvas_div.offsetHeight            
# #         @_margin = {top: 20, right: 30, bottom: 30, left: 40}
#         @_margin = {top: @GS.top, right: @GS.right, bottom: @GS.bottom, left: @GS.left}        
#         
#         #         @vec_Y_tab = Vec_List[1..]
#         #         @_data= matrixTransposeMulti(@_vec_x, @vec_Y_tab)
#         #         Vec_List = @attr_Veclist this
#         #         @_data = matrixVecListTranspose Vec_List       
#         
#         @_widthSVG = width - @_margin.left - @_margin.right
#         @_heightSVG = height - @_margin.top - @_margin.bottom
#         
#         [minVecY, maxVecY] = min_max_Vec vec_Lst        # essayer avec D3.extent qui donne min et max tableau
# #         [minVecY, maxVecY] = min_max_Vec [@_vec_y] 
# #         [minVecX, maxVecX] = min_max_Vec [@_vec_x]
#         
#         @_x = d3.scale.linear()
#         .domain([minVecX, maxVecX])
#         .range([0, @_widthSVG])
#         
#         @_y = d3.scale.linear()
#         .domain([minVecY, maxVecY])
#         .range([@_heightSVG, 0])
#         
#         color = d3.scale.category10()   #TEST
#         
#         @_xAxis = d3.svg.axis()
#         .scale(@_x)
#         .orient("bottom")
#         
#         @_yAxis = d3.svg.axis()
#         .scale(@_y)
#         .orient("right")
#         
#         line = d3.svg.line()
#         .interpolate("monotone")    #TODO pouvoir choisir "basis"
#         .x( (d)=> @_x(d[0]))
#         .y( (d)=> @_y(d[1]))
#         
#         @_svgD3 = d3.select( info.ctx_svg()).append("svg")
#         .datum(@_data)
#         .attr("width", @_widthSVG + @_margin.left + @_margin.right)
#         .attr("height", @_heightSVG + @_margin.top + @_margin.bottom)
#         .style('position','absolute')
#         .append("g")
#         .attr("transform", "translate(" + @_margin.left + "," + @_margin.top + ")")    
#         
#         #background for svg
#         @_svgD3.append("rect")
#         .attr("width", "100%")
#         .attr("height", "100%")
#         .attr("fill", "white");
#         
#         color.domain( 
#         
#         localCurves = color.domain().map((name)-> 
#                                             name :name, 
#                                             values: data.map( d->
#                                             abscissa: d[0]), ordinate: +d[name])
#             
#         @_svgD3.append("g")
#         .attr("class", "x axis")
#         .attr("transform", "translate(0," + @_heightSVG + ")")
#         .call(@_xAxis)
#         
#         @_svgD3.append("g")
#         .attr("class", "y axis")
#         .call(@_yAxis)
# #         TODO suggestion en plus:
# #         .append("text")
# #           .attr("transform", "rotate(-90)")
# #           .attr("y", 6)
# #           .attr("dy", ".71em")
# #           .style("text-anchor", "end")
# #           .text("Temperature (oF)");
# 
#         @_svgD3.append("path")
#         .attr("class", "line")
#         .attr("d", (d) -> line(d.values))
# #         .attr("d", line)
#         .style("stroke", (d)-> color(d.name))
# 
#         @_svgD3.selectAll(".dot")
#         .data(@_data)
#         .enter().append("circle")
#         .attr("class", "dot")
#         .attr("cx", line.x())
#         .attr("cy", line.y())
#         .attr("r", 3.5)
#         
# #         city.append("text")
# #         .datum((d)-> {name: d.name, value: d.values[d.values.length - 1]})
# #         .attr("transform",(d)-> "translate(" + x(d.value.date) + "," + y(d.value.temperature) + ")"
# #         .attr("x", 3)
# #         .attr("dy", ".35em")
# #         .text((d) -> d.name)
        
    initDrawing_MultiVec_Long:( info, vecX, vecY_lst, firstDrawing )  ->           
        @_vec_x = vecX
        @_vec_y = vecY_lst 
        XArray = VecToArray(@_vec_x)
        XYArray = matrixTranspose(@_vec_x, @_vec_y)
        @_data= OrderAccordingAbscisse(XArray, XYArray)
        
        @_data = matrixTranspose(@_vec_x, @_vec_y)
        width = info.w  #Canvas_div.offsetWidth
        height = info.h #Canvas_div.offsetHeight            
        @_margin = {top: 20, right: 30, bottom: 30, left: 40}
                     
        #         @vec_Y_tab = Vec_List[1..]
        #         @_data= matrixTransposeMulti(@_vec_x, @vec_Y_tab)
        #         Vec_List = @attr_Veclist this
        #         @_data = matrixVecListTranspose Vec_List       
        
        @_widthSVG = width - @_margin.left - @_margin.right
        @_heightSVG = height - @_margin.top - @_margin.bottom
        
        #         [minVecY, maxVecY] = min_max_Vec @vec_Y_tab
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
            .attr("fill", "white");
            
        @_svgD3.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + @_heightSVG + ")")
            .call(@_xAxis)

        @_svgD3.append("g")
            .attr("class", "y axis")
            .call(@_yAxis)

        @_svgD3.append("path")
                .attr("class", "line")
                .attr("d", line)
        
        @_svgD3.selectAll(".dot")
              .data(@_data)
            .enter().append("circle")
              .attr("class", "dot")
              .attr("cx", line.x())
              .attr("cy", line.y())
              .attr("r", 3.5)        
        
    drawSVG_2_Vec: ( info, vector1, vector2, firstDrawing ) ->
        if not firstDrawing? 
            @initDrawing_2_Vec( info, vector1, vector2 )
 
        #         info.cm._init_ctx()
        #         Canvas_div = info.cm.canvas
       
        
#         v1 = new Vec [1, 72, 3, 24]
#         v2 = new Vec [45, 2, 3, 0]

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
        width = info.w  #Canvas_div.offsetWidth
        height = info.h #Canvas_div.offsetHeight        
        @_margin = {top: 20, right: 30, bottom: 30, left: 40}
                     
        #         @vec_Y_tab = Vec_List[1..]
        #         @_data= matrixTransposeMulti(@_vec_x, @vec_Y_tab)
        #         Vec_List = @attr_Veclist this
        #         @_data = matrixVecListTranspose Vec_List       
        
        @_widthSVG = width - @_margin.left - @_margin.right
        @_heightSVG = height - @_margin.top - @_margin.bottom
        
        #         [minVecY, maxVecY] = min_max_Vec @vec_Y_tab
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
            .attr("fill", "white");
            
        @_svgD3.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + @_heightSVG + ")")
            .call(@_xAxis)

        @_svgD3.append("g")
            .attr("class", "y axis")
            .call(@_yAxis)

        @_svgD3.append("path")
                .attr("class", "line")
                .attr("d", line)
        
        @_svgD3.selectAll(".dot")
              .data(@_data)
            .enter().append("circle")
              .attr("class", "dot")
              .attr("cx", line.x())
              .attr("cy", line.y())
              .attr("r", 3.5)        

        
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
