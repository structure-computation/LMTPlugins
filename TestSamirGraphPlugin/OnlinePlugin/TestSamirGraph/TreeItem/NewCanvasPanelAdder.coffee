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

class NewCanvasPanelAdder
    constructor: ()->
    
    addNewCanvasPanel : (app)->
   
        display_settings = app.data.selected_display_settings()
        layout = display_settings._layout        
        console.log "layout avant "
        console.log layout
        
        newPanelStrengthCoeff = 1 #TEST            
        isHorizontal = 0 # 0: vertical
        isNewPanelOnRightSide = 1
        new_panel_id = "Graph_id"
        
        for panel_id in app.data.selected_canvas_pan
#             app._next_view_item_cam = cam
#             app._next_view_item_child = child
            idGraphCanvas = layout.mk_split( isHorizontal, isNewPanelOnRightSide, panel_id, newPanelStrengthCoeff, new_panel_id)
        console.log "layout après "
        console.log layout
        
        idOfVisiblePanels = layout.panel_id_of_term_panels()
        console.log "idOfVisiblePanels:"
        console.log idOfVisiblePanels
#         TEST du 03.06.2015
#         lm = new LayoutManager app.el, display_settings._layout #TEST TODO si probleme essayer layout
#         lm.new_panel_instance = ( app.data ) => @_new_panel_instance display_settings, app.data
        
#         d = app.data.selected_display_settings()
        #app.data.update_associated_layout_data d  TEST ne sert à rien à verifier 

        return idGraphCanvas
        
        
        
    _new_LayoutManager: ( session ) ->
        display_settings = session._children.detect ( x ) -> x instanceof DisplaySettingsItem
        res = new LayoutManager @el, display_settings._layout
        #res.disp_top = @icobar.disp_top + @icobar.height
        res.new_panel_instance = ( data ) => @_new_panel_instance display_settings, data
        return res
# #     getCurrentDisplaySettingsItem:->
#         display_settings = app.data.selected_display_settings()
# #         session = @data.selected_session()
# #     getCurrentLayoutManager:->
#         currentLayoutManagerData = display_settings._layout
#         
#         newPanelStrengthCoeff = 1 #TEST
#         
#         visiblePanels = currentLayoutManagerData.panel_id_of_term_panels()
#         lastVisiblePanels = visiblePanels.length-1
#         idLastVisibleCanvas = lastVisiblePanels[visiblePanels.length-1]
#         isHorizontal = 0 # 0: vertical
#         isNewPanelOnRightSide = 1
#         currentLayoutManagerData.mk_split( isHorizontal, isNewPanelOnRightSide, idLastVisibleCanvas, newPanelStrengthCoeff)
        
        
#         curLmRoot = CurrentLayoutManagerData.root
#     
# #     VisiblePanels = getVisiblePanels: ->
#         visiblePanels = curLmRoot.panel_id_of_term_panels()
#                         
# #     add_panel_id to curLmRoot                    #at the end !!
#   
# 
# #         create a newLayoutManagerData
# #         curLmRoot
# #         
# #     { panel_id: "id_0" }
# #         setCurrentDisplaySettingsItem(newLayoutManagerData)


# TEST presque reussi du 02.05.2015
#     addNewCanvasPanel : (app)->
# #         TODO mettre les variables locales en parametres
# #     getCurrentDisplaySettingsItem:->
#         display_settings = app.data.selected_display_settings()
# #         session = @data.selected_session()
# #     getCurrentLayoutManager:->
#         currentLayoutManagerData = display_settings._layout        
#         newPanelStrengthCoeff = 0.5 #TEST        
#         idOfVisiblePanels = currentLayoutManagerData.panel_id_of_term_panels()
#         lastVisiblePanelindex = idOfVisiblePanels.length-1
#         #TODO if lastVisiblePanelindex is zero
#         # app.data.last_canvas_pan
#         idLastVisibleCanvas = idOfVisiblePanels[lastVisiblePanelindex]
#         isHorizontal = 0 # 0: vertical
#         isNewPanelOnRightSide = 1
#         console.log "currentLayoutManagerData avant "
#         console.log currentLayoutManagerData
# #         idGraphCanvas = currentLayoutManagerData.mk_split( isHorizontal, isNewPanelOnRightSide, idLastVisibleCanvas, newPanelStrengthCoeff)
#         
#         d = app.data.selected_display_settings()
#         for panel_id in app.data.selected_canvas_pan
#             idGraphCanvas = d._layout.mk_split( isHorizontal, isNewPanelOnRightSide, panel_id, newPanelStrengthCoeff)
# 
#         d = app.data.selected_display_settings()
#         app.data.update_associated_layout_data d  
# #             idGraphCanvas = currentLayoutManagerData.mk_split( isHorizontal, isNewPanelOnRightSide, idLastVisibleCanvas, newPanelStrengthCoeff)
#         
# #         TODO app.data.update_associated_layout_data(d)
#         
# #         console.log "currentLayoutManagerData avant "
# #         console.log currentLayoutManagerData
#         
# #         if (lastVisiblePanelindex < idOfVisiblePanels.length-1)
# #             alert "windows is well displayed"
# #         else
# #             alert "windows is not well displayed"
# #         idGraphCanvas = idOfVisiblePanels[lastVisiblePanelindex]
#         return idGraphCanvas


