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
    constructor: ->    
    #nothing for now
    
    
    #TEST param1
    drawSVG_MultiVec:( app_data, info, graphti, graphSetting ) ->
        console.log "In Drawing, au début allowToDraw.get():", graphti._allowToDraw.get()
#         if not firstDrawing? #TEST
        @initDrawing_MultiVec(app_data, info, graphti, graphSetting )
    
    #TEST param2
#     drawSVG_MultiVec:( app_data, info, @curvesList, notfirstDrawing, allowToDraw, graphSetting ) ->
#         console.log "In Drawing, au début allowToDraw.get():", allowToDraw.get()
# #         if not firstDrawing? #TEST
#         @initDrawing_MultiVec(app_data, info, @curvesList, notfirstDrawing, allowToDraw, graphSetting )
    
    #TEST param1
    initDrawing_MultiVec:(app_data, info, graphti, graphSetting )  ->   
    
    #TEST param2
#     initDrawing_MultiVec:(app_data, info, @curvesList, notfirstDrawing, allowToDraw, graphSetting )  ->  
        
        @tic = graphti._children.detect ( x ) -> x instanceof TreeItem_Curves
        console.log "tic",@tic
        
        @curvesList = graphti.curves_List
        notfirstDrawing = graphti._NotfirstDrawing
        allowToDraw = graphti._allowToDraw
        
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
        VecY_arr.push c?.ordinate_vec.get() for c in @curvesList             
        @_data = matrixTransposeMulti VecX, VecY_arr
        console.log "@_data:", @_data
        console.log "info :", info

        width = info.w 
        height = info.h
        console.log "width = info.w :", width
        console.log "height = info.h :", height
        
        m= @GS.margin
        @_margin = {top: m.top, right: m.right, bottom: m.bottom, left: m.left} 
        
        @_widthSVG = width - @_margin.left - @_margin.right
        @_heightSVG = height - @_margin.top - @_margin.bottom
        
        console.log "@_widthSVG:", @_widthSVG
        
        [minVecY, maxVecY] = min_max_Vec vec_Lst        # essayer avec D3.extent qui donne min et max tableau
        
        console.log "minVecY:", minVecY
        console.log "maxVecY:", maxVecY
       
        @_x = d3.scale.linear()
        .range([0, @_widthSVG])
        
        #TEST 1_y   cf TEST m_y
        @_y = d3.scale.linear()
        .range([@_heightSVG, 0])
                
        #TEST m_y 
        #Listing the number of curve Type
        curveTypeNameList = new Lst
        for curve in @curvesList
            if not curveTypeNameList.contains curve.curveTypeName 
                curveTypeNameList.push curve.curveTypeName
        
        console.log "curveTypeNameList"
        console.log curveTypeNameList
        
#         TEST 1 # imitation de d3.scale.category20() avec des noms de couleurs reconnues par D3
        # une boucle infinie est constatée pour la mise à jour de l'attribut couleur de l'objetCurve 
        d3_Issim_category20 = [ "darkblue",  "blue", 
                                "darkorange", "orange",
                                "green", "springgreen",
                                "red", "antiquewhite",
                                "purple", "violet",
                                "maroon", "sandybrown",
                                "pink", "salmon",
                                "darkgrey", "darkslategray",    
                                "yellowgreen", "yellow",
                                "cyan", "indigo"]
                     
        #TEST colRange1 
#         console.log "@tic._colorRange.get()"
#         console.log @tic._colorRange.get()
        
        colorRangeTab= []
        
        colorRangeTab.push co for co in @tic._colorRange
        color = d3.scale.ordinal().range(colorRangeTab)

        
        #TEST colRange2                        
#         color = d3.scale.ordinal().range(d3_Issim_category20)
#         
#         #TEST 2
#         #color = d3.scale.category20()
#         #trouver les noms des couleurs sur internet 
      
        #TEST 3
#         color = d3.scale.category20() 
#         #noms provisoires des couleurs
#         Category20ColorNames=
#         {"#1f77b4":"blue+",
#         "#aec7e8":"blue-",
#         "#ff7f0e":"orange+",
#         "#ffbb78":"orange-",
#         "#2ca02c":"green+",
#         "#98df8a":"green-",
#         "#d62728":"red+",
#         "#ff9896":"red-",
#         "#9467bd":"purple+",
#         "#c5b0d5":"purple-",
#         "#8c564b":"brown+",
#         "#c49c94":"brown-",
#         "#e377c2":"pink+",        @_title = d3.selectAll("svg#svg_id").insert("text", "Issim_chart")
#         "#f7b6d2":"pink-",
#         "#7f7f7f":"grey+",
#         "#c7c7c7":"grey-",
#        translate "en fonction de" "#bcbd22":"yellow+",
#         "#dbdb8d":"yellow-",
#         "#17becf":"skyblue+",
#         "#9edae5":"skyblue-"}  
        
        @_xAxis = d3.svg.axis()
        .scale(@_x)
        .orient(@GS.X_orient)# TEST should be "bottom"      
        
        @_yAxis = d3.svg.axis()
        .scale(@_y)
        .orient(@GS.Y_orient)# TEST should be "right"
        
        console.log "d3.svg.line() before !!!"
        console.log d3.svg.line()
        
        console.log "graphSettings.interpolation"
        console.log @GS.interpolation
        
        
        line = d3.svg.line()
        .interpolate(@GS.interpolation)                # TEST should be "monotone"
        .x( (d)=>
#                   console.log "d in line:"
#                   console.log d
                  keys = (k for own k of d)
                  return @_x(d[keys[0]]))
        .y( (d)=>
                keys = (k for own k of d)
                return @_y(d[keys[1]]))            
                
        console.log "d3.svg.line() after !!!"
        console.log d3.svg.line()         
                
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
#          #TEST _title
#         .append("text")
#         .attr("class", "ChartTitle")
#         .attr("x", @_widthSVG/2)
#         .attr("y", @_heightSVG)
#         .style("font-size", "11px")
#         .style("font-weight","bold")
#         .style("text-anchor", "end")        
#         #TEST 2
#         #.style("text-anchor", "middle")
#         .text(@curvesList[0].curveTypeName+" as a function of "+@curvesList[0].abscissa_name)
        #TEST END _title
        
#         d3.select("svg") 
#         .append("g")
#         .attr("id", "Issim_chart")
        #.attr("transform", "translate(#{@_margin.left},#{@_margin.top})")           

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
#                                     vecIndexMax = c.ordinate_vec.length-1
                                    return res_c = 
                                            name : c._name.get() #TEST TODO remettre une virgule
                                            values: @_data.map (d) =>  
                                                                    key_abs = c.abscissa_name.get()
                                                                    key_ord = c.ordinate_name.get()
                                                                    res = {}
                                                                    res[key_abs] = d[0]# equivalent to d.date
                                                                    res[key_ord] = d[curvesIndex+1]
                                                                    res['curve_Index']=curvesIndex
                                                                    return res   
                                            number:c.number.get()
                                            
                                            #TEST leg1
#                                             legendValuesFunc: (d)=> 
#                                                                 console.log "d in legendValues"
#                                                                 console.log d
#                                                                 legY = (d)=> @_heightSVG-30*d.number
#                                                                 console.log "legY"
#                                                                 console.log legY
#                                                                 
#                                                                 res = {}
#     #                                                             res['curve_Index']=curvesIndex
#                                                                 for i in [1..3]    
#                                                                     res['x'] = 30 *i
#                                                                     res['y'] = legY(d)
#                                                                 return res 
#                                             legendValues: @legendValuesFunc(this)
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
        
        #Legend
        @_ChartLegend = @_svgD3.append("g")
            .attr("class", "ChartLegend")
        
        #Abscissa label
        @_svgD3.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + @_heightSVG+ ")")
            .call(@_xAxis)
            #TEST
            
        @_svgD3.append("text")
            .attr("x", @_widthSVG/2)
            .attr("y", @_heightSVG+26)
            .attr("dx", ".71em")
            .style("text-anchor", "end")
            .text(@curvesList[0].abscissa_name.get()+" ("+@curvesList[0].abscissa_unity.get()+")")

        #Ordinate Label
        @_svgD3.append("g")
            .attr("class", "y axis")
            .call(@_yAxis)
            
            #TEST rot1
#           #.attr("transform", "rotate(-90)")
#             .attr("transform", "translate(0," + @_heightSVG+ ")")
        
        @_svgD3.append("text")          
          #TEST x1
          
          #TEST forRotate1
          .attr("x", @_heightSVG/2)
          
          #TEST forRotate2
#           .attr("x", 25)
          #TEST x2
          #.attr("x", 0)
          
          #TEST forRotate1
          .attr("y", 25)
          
          #TEST forRotate2
#           .attr("y", @_heightSVG/2)
          
          .attr("dx", ".71em")
          
          #TEST 1
          .style("text-anchor", "end")
          
          #TEST 2
          #.style("text-anchor", "middle")
          .text(@curvesList[0].curveTypeName.get()+" ("+@curvesList[0].ordinate_unity.get()+")")
          .attr("transform", "rotate(90)")
        
        
      # TITLE
        #TEST insert1
#         @_title = d3.selectAll("svg#svg_id").insert("text", "Issim_chart")
#           .attr("x", @_widthSVG/2)
#           .attr("y", @_heightSVG)
# #           .attr("dx", "100em")
#         @_svgD3.selectAll(".ChartTitle")  
#           .style("font-size", "11px")
#           .style("font-weight","bold")
#           .style("text-anchor", "end")        
#           #TEST 2
#           #.style("text-anchor", "middle")
#           .text(@curvesList[0].curveTypeName+" as a function of "+@curvesList[0].abscissa_name)
        
        @_svgD3.append("text")
        .attr("x", (@_widthSVG / 2))
        #TEST title1
#         .attr("y", (@_margin.top / 2))
        
        #TEST title2
        .attr("y", 0 - (@_margin.top / 2)+4)
        .attr("text-anchor", "middle")  
        .style("font-size", "16px") 
        .style("text-decoration", "underline")
        .style("font-weight", "underline")  
        .text(@curvesList[0].curveTypeName+" as a function of "+@curvesList[0].abscissa_name)
        
        
        console.log "app_data.tree_items"
        console.log app_data.tree_items
        
        console.log "app_data.selected_tree_items"
        console.log app_data.selected_tree_items
        
        console.log "app_data.get_root_path_in_selected TestSamirGraphItem"
        console.log app_data.get_root_path_in_selected TestSamirGraphItem
        
        console.log "app_data.get_selected_tree_items()"
        console.log app_data.get_selected_tree_items()
        
        #TEST param2
#         graphti = app_data.get_selected_tree_items().detect ( x ) -> x instanceof TestSamirGraphItem
        
        #TEST 2
#         sessionItem = app_data.tree_items.detect ( x ) -> x instanceof SessionItem
#         graphti = sessionItem._children.detect ( x ) -> x instanceof TestSamirGraphItem 
        legendLine = d3.svg.line()
        .interpolate("basis")
        .x((d)=>
              console.log "d in legendLine"
              console.log d
              return d.x)
        .y((d)=>d.y)
        
        legendCurve = @_ChartLegend.selectAll(".legendCurve")
        .data(curves)
        .enter().append("g")
        .attr("class", "legendCurve")
        
        #TEST shape1
#         legendCurve.append("path")
#         .attr("class", "line")
#         .attr("d", (d) -> 
#                       console.log "d.legendValues"
#                       console.log d.legendValues
#                       legendLine(d.legendValues))

        legendCurve.append("line")
        .attr("x1", 30)
        .attr("y1", (d)=> 30*d.number)
        .attr("x2", 60)
        .attr("y2", (d)=> 30*d.number)
        
        #"(d)" est le parametre courbe
        .style("stroke", (d, i)=> return color(i))
        
        curve = @_svgD3.selectAll(".curve")
        .data(curves)
        .enter().append("g")
        .attr("class", "curve")
              
        curve.append("path")
        .attr("class", "line")
        .attr("d", (d) -> 
                line(d.values))
        .style("stroke", (d, i)=> 
                        # "d" est la donnée d'index "i"
#                         colorD3JS = new ColorD3JS
# #                         searchedColor = key+"" for own key, value of colorD3JS.colors when (valueHex = "#"+value.toString(16) )== color(d.name).toString()                         
#                         for own key, value of colorD3JS.colors
#                             valueHex = "#"+value.toString(16) 
# #                             if d3.rgb(valueHex) == color(d.name)
#                             #TEST 1
#                             if d3.rgb(valueHex) == d3.rgb(color(i))# d3.rgb(valueHex)=> <dt>( r=,g=...) et color(i)=> #8888 ou blue, ou "blue"...
#                             
#                             #TEST 2
#                             # colorD3JS = set of fixed colors , color(i) could be a color comprise between 2 following colors of D3JS
#                             
#                             #TEST 3
#                             # valueHex est mal calculé avec .toString(16)
#                             
#                             #TEST 4
#                             #if d3.rgb(valueHex) == color(i)
#                                 searchedColor = key+""
#                                 console.log "searchedColor"
#                                 console.log searchedColor
#                             console.log "Difference color(d.name) et color(i)"
#                             console.log "color(d.name)"
#                             console.log color(d.name)
#                             console.log "color(i)"
#                             console.log color(i)
#                             console.log "valueHex"
#                             console.log valueHex

#                         col = @hexToRgb(color(i).toString())
#                         =>undefined

                        console.log "color(i)"
                        console.log color(i)

                        col2 = d3.rgb(color(i)) #il n'est pas necessaire de mettre toString dans la fonction rgb() 
                        
                        #TEST 1
#                         col3 = color(i).rgb()
#                         Uncaught TypeError: undefined is not a function 
#                         console.log "col == col2"
#                         console.log col == col2
                        
                        console.log "col2"
                        console.log col2
                        
#                         console.log "col"
#                         console.log col
                        
                        console.log "@tic._curves_List Avant"
                        console.log @tic._curves_List
                        
                        # TEST 1
#                         pour remplacer @curvesList par @curvesList
#                         curve_color  = new Color col.r, col.g, col.b
                        curve_color2 = new Color col2.r, col2.g, col2.b
#                         curve_color3 = new Color col3.r, col3.g, col3.b
                        
#                         console.log "curve_color"
#                         console.log curve_color
                        
                        console.log "curve_color2"
                        console.log curve_color2
                        
#                         @curvesList[i].color.set curve_color2
                        
                        #TEST 1 
                        colorD3JS = new ColorD3JS
                        #TEST 1.2
                        for k, v of colorD3JS.colors 
                            #TEST 1.2.1
                            if @d3_rgbNumber(v).r == d3.rgb(color(i)).r
                                if @d3_rgbNumber(v).g == d3.rgb(color(i)).g
                                    if @d3_rgbNumber(v).b == d3.rgb(color(i)).b
                                        colName = k+"" 
                            
                            #TEST 1.2.2
                            #if @d3_rgbNumber(v) == d3.rgb(color(i))
                            #colName = k+"" 
                        
                        #TEST 1.2
#                         colName = k+"" for own k, v of colorD3JS.colors when @d3_rgbNumber(v) == d3.rgb(color(i))
                            
#                             console.log "@d3_rgbNumber(v)"
#                             console.log @d3_rgbNumber(v)
#                             console.log "d3.rgb(color(i))"
#                             console.log d3.rgb(color(i))
                            
                        #                         colName = color(i)      for entry in  d3.entries(colorD3JS.colors)                           
                        #TEST 2
#                         colName = @Category20ColorNames[color(i).toString()]
   
                        console.log "colName"
                        console.log colName
                        
#                         @curvesList[i].colorName.set colName
#                         @curvesList[i].colorName.set searchedColor
                        
                        #TEST 2 
                        #@tic._curves_List[i].color.set curve_color
                        
#                         #TEST 2.1
#                         @curvesList[i].colorName.set colName

                        #TEST 2.2
                        curvli  = @curvesList[i]
                        
                        console.log "@curvesList[i]"
                        console.log curvli
                        
                        #TEST fillColor1 already done in TreeItem_Curves                      
#                         curvli.colorName.set colName
#                         @tic._curves_List[i].colorName.set colName
                        
                        #TEST 3 
                        #mod_attr 
                        #@curvesList[i].mod_attr colorName, colName
                        #@tic._curves_List[i].mod_attr colorName, colName
                        
                        #TODO 3 a remettre
#                         for curv in c when curv._name.equals d.name
#                             curve_color = new Color col.r, col.g, col.b
#                             console.log "curve_color"
#                             console.log curve_color
#                             console.log "curv"
#                             console.log curv
#                             console.log "curv.color AVANT"
#                             console.log curv.color
#                             
#                             curv.color.set curve_color
#                             console.log "curv.color APRES"
#                             console.log curv.color
#                             
#                             curv.colorName.set searchedColor

                        console.log "@tic._curves_List"
                        console.log @tic._curves_List
                        
                        #TEST colChoice2 cf CurveChoice
#                         @tic.curves.set colName
#                         colNameIndex = @tic.curves.lst.indexOf colName
#                         @tic.curves.num.set colNameIndex
                        
                        #TODO a effacer ??
#                         for cs in @tic.curves.lst when cs.aggregate.name.equals d.name
#                             console.log "color.range():", color.range()

                        #console.log "@tic.curves.lst Après"
                        #console.log @tic.curves.lst    
                        
                        #TEST 1 
                        return color(i))
                      
                        #TEST 2
                        #return color(d.name))
     
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

#         @markerWidth = 3.5  #TODO mettre dans graphSetting 
#         @markerHeight = 3.5  
        
        #TEST marker1
        #select is like filter preserving the index => probleme du "this" et de => 
        
#         dotCurve = curve.select (d,i)=>
#                                           do()->
#                                               #this is window
#                                               DSThis = this.DrawingSVG
#                                               console.log "DSThis:"
#                                               console.log DSThis
#                                                     
#                                               res = if DSThis.curvesList[i].marker.get() =="dot" then this else null
        console.log "curve"                
        console.log curve
        
        #TEST makeMarker2
#         dotCurve = curve.filter (d, i)=> @curvesList[i].marker.get() =="dot"
#         squareCurve = curve.filter (d, i)=> @curvesList[i].marker.get() =="square"
#         diamondCurve = curve.filter (d, i)=> @curvesList[i].marker.get() =="diamond"
        
#         console.log "dotCurve"
#         console.log dotCurve
#         console.log "squareCurve"
#         console.log squareCurve
#         console.log "diamondCurve"
#         console.log diamondCurve
        
        #TEST makeMarker1
        #TEST cMarker1
        curve.call( @makeMarker, @tic.curves, (d, i)=> @curvesList[i].marker.get())
      
        #TEST cMarker2
#         curve.call( @makeMarker, @tic.curves)
      
        #TEST makeMarker2
#         if not dotCurve.empty()
#             console.log "not dotCurve.empty"
#             dotCurve.call( @makeDot, @tic.curves)
# 
#         if not squareCurve.empty()
#             console.log "not squareCurve.empty"
#             squareCurve.call( @makeSquare, @tic.curves)
#         
#         if not diamondCurve.empty()
#             console.log "not diamondCurve.empty"
#             diamondCurve.call( @makeDiamond, @tic.curves)
        
        
        
        
        #TEST TODO
#             diamondCurve.call( @makeDiamond2, @tic.curves)  #old version without d3.symbol
            
#TEST marker3
#         curve.call( (d, i)=>
# #                      if @curvesList[i].marker.get() =="square"
#                     console.log "@curvesList[i]"
#                     console.log @curvesList[i]
#                     
#                     console.log 
#                     
#                     console.log "@curvesList"
#                     console.log @curvesList
#                     
#                     marker = @curvesList[i].marker.get()
#                     console.log "hello1"
#                     switch marker
#                           #TEST selcurve1
#                           when "dot" then @makeDot @tic.curves 
#                           
#                           #TEST selcurve2
#                           #when "dot" then curve.selectAll(".curve").call(@makeDot)
# 
#                           when "square" then @makeSquare @tic.curves
#                           when "cross" then go iceFishing
#                           when "diamond" then go iceFishing
#                           when "Fri", "Sat"
#                             if day is bingoDay
#                               go bingo
#                               go dancing
#                           when "Sun" then go church
#                           else console.log "this marker does not exist")       

        #TEST marker3


#TEST marker2
#         squareCurve = curve.select (d,i)->
#                                           
#                                           do()->
#                                               #this is window
#                                               DSThis = this.DrawingSVG
#                                               console.log "DSThis:"
#                                               console.log DSThis
#                                               
#                                               #TEST do()1
#                                               console.log 'DSThis.curvesList[i].marker.get() =="square"' 
#                                               console.log DSThis.curvesList[i].marker.get() =="square"
#                                               res = if DSThis.curvesList[i].marker.get() =="square" then this else null
#                                           
#                                           #TEST do()2
# #                                           console.log '@curvesList[i].marker.get() =="square"' 
# #                                           console.log @curvesList[i].marker.get() =="square"
# #                                           res = if @curvesList[i].marker.get() =="square" then this else null
#         
#         console.log "squareCurve"                
#         console.log squareCurve
#         
#         #TEST switch1
#         if not dotCurve.empty()
#             dotCurve.call(@makeDot, @tic.curves)
#         if not squareCurve.empty()
#             squareCurve.call(@makeSquare, @tic.curves)
           
        #TEST switch2
#         console.log "dotCurve.size()"
#         console.log dotCurve.size()
#         
#         console.log "dotCurve.empty()"
#         console.log dotCurve.empty()
#         
#         dotCurve.call(@makeDot, @tic.curves)
#         squareCurve = curve.select (d,i)=>
#                                                     console.log '@curvesList[i].marker.get() =="square"' 
#                                                     console.log @curvesList[i].marker.get() =="square"
#                                                     res = if @curvesList[i].marker.get() =="square" then this else null
#                                                     
#         console.log "squareCurve"                
#         console.log squareCurve                                            
#         squareCurve.call(@makeSquare, @tic.curves)
        
        #TEST curvcir2
#         dotCurve.selectAll(".curve").call(@makeDot)
        
        #TEST marker2
#         switch ??.marker.get()
#           #TEST selcurve1
#           when "dot" then curve.call(@makeDot)
          
          #TEST selcurve2
#           when "dot" then curve.selectAll(".curve").call(@makeDot)

#           when "square" then go relax
#           when "cross" then go iceFishing
#           when "diamond" then go iceFishing
#           when "Fri", "Sat"
#             if day is bingoDay
#               go bingo
#               go dancing
#           when "Sun" then go church
#           else console.log"this marker does not exist"        
        
        #TEST marker3
#         curve.selectAll(".curve").call(@makeDot)

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
        
        #TEST curveName1
#         curve
        
        #TEST curveName2
        legendCurve
        .datum((d)-> 
                    console.log "d final:", d
                    {name: d.name, value: d.values[d.values.length - 1], dnumber: d.number})
        .append("text")
        #TEST curveName1
#         .attr("transform",(d)=> 
#                         dvalue = d.value
#                         keys = (k for own k of dvalue)
#                         return "translate(" + @_x(dvalue[keys[0]]) + "," + @_y(dvalue[keys[1]])+ ")")
#         
        #TEST curveName1
#         .attr("x",  @GS.xTextPosition)
#         .attr("y",  20)
        
        #TEST curveName2
        .attr("x",  65)
        .attr("y",  (d)=> 30*d.dnumber)
        
        .attr("dy", @GS.dyTextOffset)
        .text((d) -> 
                keys = (k for own k of d.value)
                keys[1]+":"+d.name)  

        #AllowToDraw        
        allowToDraw.set false #TEST
        console.log "At the end of Drawing, (should be false) allowToDraw.get():", allowToDraw.get()
        notfirstDrawing= true
    
    d3_rgbNumber : (value)->
        d3.rgb(value >> 16, value >> 8 & 255, value & 255)
        
    makeDot:(selection, curveChoices, isLegend = false)=>
        selection
            .selectAll(".curve")
            .data((d)-> 
                       if not isLegend
                          return d.values)
            .enter()    
            .append("circle")
            .style("stroke", "black")
            .attr("fill", (d)=> 
                              i = d.curve_Index
                              curveChoices.lst[i].aggregate.markerColor.get())
            .attr("cx", (d)=>  
                            keys = (k for own k of d)
                            return @_x(d[keys[0]]))
            .attr("cy",  (d)=>       
                            keys = (k for own k of d)
                            return @_y(d[keys[1]]))
                            
            .attr("r", (d)=> 
                            i = d.curve_Index
                            curveChoices.lst[i].aggregate.markerRadius.get())               
#             .attr("r",3.5)
                
    makeSquare:(selection, curveChoices)=>
        selection
            .selectAll(".curve")      
            .data((d)-> d.values)
            .enter() 
            .append("rect")
            .style("stroke", "black")
            .attr("fill", (d)=> 
                              i = d.curve_Index
                              curveChoices.lst[i].aggregate.markerColor.get())
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
            .attr("width", (d)=> 
                              i = d.curve_Index
                              curveChoices.lst[i].aggregate.markerWidth.get()) 
            .attr("height", (d)=> 
                              i = d.curve_Index
                              curveChoices.lst[i].aggregate.markerHeight.get()) 
    
    
    
    makeMarker:(selection, curveChoices, marker)=>
        console.log "selection"
        console.log selection
        selection
            .selectAll(".curve")   
            .data((d, i)->
                         console.log "d:"
                         console.log d
                         d.values)
            .enter()
            #TEST d3Symb1
#             .append("g")
            .append("path")
            .attr("transform", (d)=> 
                                  console.log "diamond transform"
                                  keys = (k for own k of d)
                                  return "translate(" + @_x(d[keys[0]]) + "," + @_y(d[keys[1]]) + ")")
            .attr("d",d3.svg.symbol().size((d)=>
                                              console.log "d in size:"
                                              console.log d
                                              i = d.curve_Index  
                                              console.log curveChoices.lst[i].aggregate.markerSize.get()
                                              curveChoices.lst[i].aggregate.markerSize.get())
                                     .type((d)=>
                                              console.log "d in type:"
                                              console.log d
                                              i = d.curve_Index
                                              console.log curveChoices.lst[i].aggregate.marker.get()
                                              
                                              #TEST cmarker1
                                              ))
                                              #TEST cmarker2 TODO 
#                                               curveChoices.lst[i].aggregate.marker.get()))   
            .style("fill",(d)=> 
                              i = d.curve_Index
                              res = curveChoices.lst[i].aggregate.markerColor.get()
                              console.log "curveChoices.lst[i].aggregate.markerColor.get():"
                              console.log res
                              return res)
    
    
    makeDiamond:(selection, curveChoices)=>
        console.log "selection"
        console.log selection
        selection
            .selectAll(".curve")   
            .data((d, i)->
                         console.log "d:"
                         console.log d
                         d.values)
            .enter()
            #TEST d3Symb1
#             .append("g")
            .append("path")
            .attr("transform", (d)=> 
                                  console.log "diamond transform"
                                  keys = (k for own k of d)
                                  return "translate(" + @_x(d[keys[0]]) + "," + @_y(d[keys[1]]) + ")")
            .attr("d",d3.svg.symbol().size((d)=>curveChoices.lst[i].aggregate.markerSize.get()).type((d)=>"diamond"))   
            .style("fill",(d)=> 
                              i = d.curve_Index
                              res = curveChoices.lst[i].aggregate.markerColor.get()
                              console.log "curveChoices.lst[i].aggregate.markerColor.get():"
                              console.log res
                              return res)
                              
                              
    makeDiamond2:(selection, curveChoices)=>
        console.log "selection"
        console.log selection
        selection
            .data((d, i)->
                         console.log "d:"
                         console.log d
                         d.values)
            .enter()
            .append("g")
            .attr("transform", (d, i)=>

                                      console.log "curveChoices"
                                      console.log curveChoices
                                      console.log "i"
                                      console.log i
#                                       console.log "d.number"
#                                       console.log d.number
                                      console.log "curveChoices.lst[i]"
                                      console.log curveChoices.lst[i]
#                                       console.log "curveChoices.lst[d.number]"
#                                       console.log curveChoices.lst[d.number]
                                      "translate(-"+ (curveChoices.lst[i].aggregate.markerWidth.get())/2+",-"+ (curveChoices.lst[i].aggregate.markerHeight.get())/2+")")
                                                                                                  
    #         .attr("id", "rectangle")
    #         .attr("transform", "rotate(-90)")
            .append("rect")
            .style("stroke", "black")
            .attr("fill", (d, i)=> 
                                  console.log  "hello1"
                                  console.log "curveChoices"
                                  console.log curveChoices
                                  console.log "curveChoices.lst[i]"
                                  console.log curveChoices.lst[i]
                                  
                                  curveChoices.lst[i].aggregate.markerColor.get())
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
            .attr("width",(d, i)=>
                              console.log  "hello2"
                              curveChoices.lst[i].aggregate.markerWidth.get())#3.5)
            .attr("height",(d, i)=>
                              console.log  "hello3"
                              curveChoices.lst[i].aggregate.markerHeight.get())#3.5)
    
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

