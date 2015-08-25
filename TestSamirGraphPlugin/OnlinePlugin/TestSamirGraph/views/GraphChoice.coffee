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

class GraphChoice extends View
    constructor: (el, @app_data, undo_manager, model, @view_item)->
        super @view_item
        new_dom_element
            nodeName: "button"
#            nodeName  : "input"
#             type      : "button" ##
            txt: "Graph"
            siz: 1
            parentNode: el
            onclick: ( evt ) ->
                @app_data = undefined 
                undo_manager = undefined
                new GraphManagerPanelInstance( el, @app_data, undo_manager,  model )
                  
        new_dom_element
            nodeName: "button"
#             nodeName  : "input"
#             type      : "button"
            txt: "Canvas"
            siz: 1
            parentNode: el
            onclick: ( evt ) ->
                new CanvasManagerPanelInstance( el, @app_data, @view_item, undo_manager )
                
        new_dom_element
#             nodeName: "button"
            nodeName  : "input"
            type      : "button"            
            txt: "Map"
            siz: 1
            parentNode: el
            onclick: ( evt ) ->