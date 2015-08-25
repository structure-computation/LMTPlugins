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


class TreeItem_Curves extends TreeItem
    constructor: (sDName = "Curves") ->
        super()
        @add_attr
            _curves_List: new Lst     
        @add_attr
            curves: new Choice  
        @_viewable.set false
        @visible = false

        if sDName?
            @_name.set sDName 
      
    fill_curves: (graphTreeItem)->
        graphti = graphTreeItem
#         if not graphti._NotfirstDrawing?
#         console.log "graphti._allowToDraw"
#         console.log graphti._allowToDraw
        
        
        console.log "At TreeItem_Curves graphti._allowToDraw.get():", graphti._allowToDraw.get()
        if graphti._allowToDraw.get()
            @_curves_List.clear()    
            @_curves_List.set graphti.detect_vector()
            
            #setting the curves
            
            @curves.lst.clear()
            for objCurve in @_curves_List.get()#TEST
                curveChoice = new CurveChoice(objCurve)
                @curves.lst.push curveChoice
            
#             for child in graphti._children
#                 if child instanceof TreeItem_Curves
#                     ch_curves = child.curves.lst.get()
#                     if ch_curves.length == 0
#                         ch_curves =  _curves_List
#                     else
#                         _curves_List = ch_curves.get()                           
        return @_curves_List
#         elastic_isotrop_mat = new ElasticIsotropMaterial
#         ramberg_osgood_mat = new RambergOsgoodMaterial
#         dp_hardening_mat = new DPHardeningMaterial
#         creep_time_hardening_mat = new CreepTimeHardeningMaterial
#        
#         @type.lst.push elastic_isotrop_mat
#         @type.lst.push ramberg_osgood_mat
#         @type.lst.push dp_hardening_mat
#         @type.lst.push creep_time_hardening_mat
  
    
    has_nothing_to_draw: ->
            true
    
    accept_child: ( ch ) ->
    
    draw: ( info ) ->
    
    get_model_editor_parameters: ( res ) ->
       res.model_editor[ "curves" ] = ModelEditorItem_ChoiceWithEditableItems #TEST TODO "curves"

    z_index: ->
    
#     update_min_max: ( x_min, x_max ) =>
#     
#     anim_min_max: ->