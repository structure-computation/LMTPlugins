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
        console.log  "TreeItem_Curves is built"
        
        @add_attr
            _curves_List: new Lst     
        @add_attr
            curves: new Choice 
        @add_attr    
            _colorRange: new Lst
      
        @_viewable.set false
        @visible = false

        if sDName?
            @_name.set sDName 
            
    #TEST colorRange3
#         bind @curves, =>
#             for curveChoice in @curves.lst
#                 if curveChoice.aggregate.colorName.has_been_modified?()
# 
#                     i = curveChoice.curve.number.get()-1
# #                     console.log  "i"
# #                     console.log  i
#                     oneColor = new Str curveChoice.aggregate.colorName.get()

#                     @_colorRange.set_or_push(i, oneColor)

    
    fill_curves: (graphTreeItem)->
        graphti = graphTreeItem
        
        console.log "At TreeItem_Curves graphti._allowToDraw.get():", graphti._allowToDraw.get()
        
        #TEST 
#         if graphti._allowToDraw.get()
        
        #@_curves_List
        #
        
        @_curves_List.clear()    
        @_curves_List.set graphti.detect_vector()
        
        #TEST fillColor1
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
                            
#         d3_Issim_category20 = new Lst [ "darkblue",  "blue", 
#                             "darkorange", "orange",
#                             "green", "springgreen",
#                             "red", "antiquewhite",
#                             "purple", "violet",
#                             "maroon", "sandybrown",
#                             "pink", "salmon",
#                             "darkgrey", "darkslategray",    
#                             "yellowgreen", "yellow",
#                             "cyan", "indigo"]
        
        i=0
        for curve in @_curves_List
            
            console.log "curve.colorName"
            console.log curve.colorName
            curve.colorName.set d3_Issim_category20[i++] 
            console.log "i:", i
            
        console.log "d3_Issim_category20[0...i]"
        console.log d3_Issim_category20[0...i]
        
        console.log "d3_Issim_category20"
        console.log d3_Issim_category20
        
        #@_colorRange
        #TEST colorR1
#         @_colorRange.push co for co in d3_Issim_category20[0...i]
        
        #TEST colorR2
        @_colorRange.clear()
        @_colorRange.push curve.colorName.get() for curve in @_curves_List
        
        console.log "@_colorRange" 
        console.log @_colorRange 
        
        console.log "curve.colorName" 
        console.log curve.colorName
        
        #setting the curves
        
        @curves.lst.clear()
        for objCurve in @_curves_List.get()#TEST
            console.log "objCurve"
            console.log objCurve
            
            curveChoice = new CurveChoice(objCurve)
            @curves.lst.push curveChoice
            
        #TEST allowTo
        graphti._allowToDraw.set not graphti._allowToDraw.get()                
        
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