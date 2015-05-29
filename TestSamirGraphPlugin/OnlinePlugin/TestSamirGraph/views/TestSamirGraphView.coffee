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

#

class TestSamirGraphView extends View
    
#     constructor: ( @el, vec_x, vec_y, width, height ) ->#TODO  A remettre ?  
    constructor: ( @graphItemModel, @el, vec_x, @vec_Y_tab, width, height ) -> #TODO  A simplifier avec uniquement graphItemModel, @el
        super @graphItemModel #TODO  A remettre ?               
        
#         @icon_container = new_dom_element
#         @panel_container = new_dom_element#TODO  A remettre ? (debut)
#             nodeName  : "div"
#             parentNode: @el
#             id  : "id_Graph"
#             className : "div_Graph" 
#             svg : @_svg
#             style      :
#             padding    : "0 0 0 0" #TODO  A remettre ? (fin)

        # constructor without @el:#TEST
#         constructor: ( @graphItemModel, vec_x, @vec_Y_tab, width, height )
#         if @el is "Vec" or @el is "Lst" 
#             vec_x = @el
#             @vec_Y_tab = vec_x
#             width = @vec_Y_tab
#             height = width 
            
        @_vec_x = vec_x       

        @_width = width
        @_height = height
        
        #TEST:
        @vec_Y_tab = [@graphItemModel._vec_y] #TODO A améliorer pour détecter et mettre tous les attributs "de type vecteur"vecteurs 
        
        Vec_List = @attr_Veclist @graphItemModel
        @_data = MatrixVecListInversion Vec_List       
#         @_data= MatrixInversionMulti(@_vec_x, @vec_Y_tab)
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

#         @_svg = d3.select(".apps_container").append("svg")#TODO A  remettre TODO
        @_svg = d3.select("."+@el.className).append("svg")
            .datum(@_data)
            .attr("width", @_width + @_margin.left + @_margin.right)
            .attr("height", @_height + @_margin.top + @_margin.bottom)
            .append("g")
            .attr("transform", "translate(" + @_margin.left + "," + @_margin.top + ")")        
        
        #background for svg
        @_svg.append("rect")
            .attr("width", "100%")
            .attr("height", "100%")
            .attr("fill", "white");
            
        @_svg.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + @_height + ")")
            .call(@_xAxis)

        @_svg.append("g")
            .attr("class", "y axis")
            .call(@_yAxis)

        @_svg.append("path")
            .attr("class", "line")
            .attr("d", line)

#         newFormat = d3.format(",.4") #TEST
            
        @_svg.selectAll(".dot")
            .data(@_data)
            .enter().append("circle")
            .attr("class", "dot")
#             .attr("cx", newFormat line.x())#TEST
#             .attr("cy", newFormat line.y())
            .attr("cx", line.x())
            .attr("cy", line.y())
            .attr("r", 3.5) 
            
    #copied from ModelEditor:
    attr_Veclist: ( model)->
#         console.log "model.get_state()"+model.get_state()
        res = {}
        for name in model._attribute_names when model[name] instanceof Lst#TODO should be Vec but return false
             console.log "\nmodel[name]: "+model[name]
             console.log "\nmodel[name]._underlying_fs_type: "+model[name]._underlying_fs_type
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
        
    clear_page: ( div_to_clear ) ->
        while div_to_clear.firstChild?
            div_to_clear.removeChild div_to_clear.firstChild
    


# A rajouter dans DomHelper ?? #TODO
# real position of an object
# get_bottom = ( l ) ->
#     if l.offsetParent?
#         return l.offsetLeft + get_left( l.offsetParent )
#     else
#         return l.offsetLeft
# 
# # real position of an object
# get_top = ( l ) ->
#     if l.offsetParent?
#         return l.offsetTop + get_top( l.offsetParent )
#     else
#         return l.offsetTop