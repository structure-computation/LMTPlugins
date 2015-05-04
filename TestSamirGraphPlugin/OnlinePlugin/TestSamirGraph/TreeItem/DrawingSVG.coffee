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

class DrawingSVG
    constructor: ()->
       
    drawSVG: ( info ) ->    
        #         info.cm._init_ctx()
        #         Canvas_div = info.cm.canvas
        
        v1 = new Vec [1, 72, 3, 24]
        v2 = new Vec [45, 2, 3, 0]
        Vec_List = [] #TEST 
        Vec_List.push v1
        Vec_List.push v2
        
        #         Vec_List = @_detect_vector()        
        @_vec_x = Vec_List[0] 
        @_vec_y = Vec_List[1]        
        @_data= MatrixTranspose(@_vec_x, @_vec_y)
        
        width = info.w  #Canvas_div.offsetWidth
        height = info.h #Canvas_div.offsetHeight        
        #       @_margin = {top: @_top, right: 30, bottom: 30, left: @_left},     
        @_margin = {top: 20, right: 30, bottom: 30, left: 40}
                     
        #         @vec_Y_tab = Vec_List[1..]
        #         @_data= MatrixTransposeMulti(@_vec_x, @vec_Y_tab)
        #         Vec_List = @attr_Veclist this
        #         @_data = MatrixVecListTranspose Vec_List       
        
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
            
         #             .interpolate("basis")       
        @_xAxis = d3.svg.axis()
            .scale(@_x)
            .orient("bottom")
        
        @_yAxis = d3.svg.axis()
            .scale(@_y)
            .orient("left")

        line = d3.svg.line()
                  .interpolate("monotone")
                  .x( (d)=> @_x(d[0]))
                  .y( (d)=> @_y(d[1]))

#         @_svg = d3.select info.ctx_svg()
#         @_svg.style "background-color", "white"#TODO
        
#         @_svg = d3.select info.ctx_svg()
#         @_svg.style "background-color", "white"

#         @_svg.append("svg")
        @_svgD3 = d3.select( info.ctx_svg()).append("svg")
                .datum(@_data)
                .attr("width", @_widthSVG + @_margin.left + @_margin.right)
                .attr("height", @_heightSVG + @_margin.top + @_margin.bottom)
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
        
#         @_svg = d3.select(ctx).append("svgGraph")
        @_svgD3.selectAll(".dot")
              .data(@_data)
            .enter().append("circle")
              .attr("class", "dot")
              .attr("cx", line.x())
              .attr("cy", line.y())
              .attr("r", 3.5)        
#             .attr("cx", newFormat line.x())#TEST
#             .attr("cy", newFormat line.y())

        
    MatrixTranspose = (VecX, VecY)->
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
        
        #         @_svgD3.append("rect")
        #             .attr("width", "100%")
        #             .attr("height", "100%")
        #             .attr("fill", "white");
        
        #         @_svgD3.append("g")
        #             .attr("class", "x axis")
        #             .attr("transform", "translate(0," + @_heightSVG + ")")
        #             .call(@_xAxis)
        # 
        #         @_svgD3.append("g")
        #             .attr("class", "y axis")
        #             .call(@_yAxis)
        # 
        #         @_svgD3.append("path")
        #             .attr("class", "line")
        #             .attr("d", line)
        # 
        # #         newFormat = d3.format(",.4") #TEST
        #             
        #         @_svgD3.selectAll(".dot")
        #             .data(@_data)
        #             .enter().append("circle")
        #             .attr("class", "dot")
        # #             .attr("cx", newFormat line.x())#TEST
        # #             .attr("cy", newFormat line.y())
        #             .attr("cx", line.x())
        #             .attr("cy", line.y())
        #             .attr("r", 3.5)
#         return true
    