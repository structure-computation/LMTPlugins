# Copyright 2012 Structure Computation  www.structure-computation.com
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
class GoogleChartView extends View
    constructor: ( @el, @mydata, @mydata_name, params = {} ) ->  
        super @mydata
        
        @nb_try = 0
        @is_loaded = false
        @icon_container = new_dom_element
            nodeName  : "div"
            parentNode: @el
            

    drawChart: ()->
        if google.visualization?
#             console.log "drawChart"

            if @mydata.has_been_directly_modified  
                data = new google.visualization.DataTable()
                for i in [ 0 ... @mydata.length ]
                    data.addColumn 'number', @mydata_name[i].get()
                

                data_rows = []
                for j in  [ 0 ... @mydata[0].length ]
                    data_rows_line = []
                    for i in [ 0 ... @mydata.length ]
                        data_rows_line.push @mydata[i][j].get()
                    data_rows.push  data_rows_line
                    
                data.addRows(data_rows )  

                options =
                    chart:
                        title: 'Box Office Earnings in First Two Weeks of Opening'
                        subtitle: 'in millions of dollars (USD)'
                    width: @el.offsetWidth
                    height: @el.offsetHeight
                
                
                chart = new google.charts.Line @el #document.getElementById('id_graph_container')
                chart.draw data, options


    onchange: ->
          options = 
              packages: ['corechart','line']
              callback : @myCallback
          google.load('visualization', '1', options);
#         google.load('visualization', '1.1', {packages: ['line']})
#         console.log google.visualization
#           google.setOnLoadCallback @drawChart()
        
          google.setOnLoadCallback @drawChart()
#           if google.visualization?
#               @is_loaded = true
#               console.log "onchange"
#               
#               setTimeout(@drawChart(), 3000);
              
              
    sleep:(milliSeconds) ->
        startTime = new Date().getTime()
        while (new Date().getTime() < startTime + milliSeconds)
            #do_nothing
            ""

    myCallback: ->
#         console.log "myCallback"
    
    drawChart_0: ()->
        @is_loaded = true
        console.log "is_loaded"
        setTimeout(@drawChart(), 3000);
        

    
            
            
            
            