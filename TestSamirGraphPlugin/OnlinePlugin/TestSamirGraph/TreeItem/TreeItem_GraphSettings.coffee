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


class TreeItem_GraphSettings extends TreeItem
    constructor: (sDName = "GraphSettings") ->
        super()
        @add_attr
            graphSettings: new GraphSettings
        
        @_viewable.set false
        @visible = false

        if sDName?
            @_name.set sDName 
    
    has_nothing_to_draw: ->
            true
    
    accept_child: ( ch ) ->
    
    draw: ( info ) ->
    
    z_index: ->
    
#     update_min_max: ( x_min, x_max ) =>
#     
#     anim_min_max: ->