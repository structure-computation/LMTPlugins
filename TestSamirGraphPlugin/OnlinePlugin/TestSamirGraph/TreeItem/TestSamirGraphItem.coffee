class TestSamirGraphItem extends TreeItem_Computable
    constructor: (name = "Grapĥ") ->
        super()
        
        # default values
        @_name.set name
        @_viewable.set false
        
        # attributes
        @add_attr
            _nb_values    : 361         #To see until 360    
            _vec_x        : new Vec
            _vec_y        : new Vec
            constrVal: new ConstrainedVal( 7, { min: 0, max: 15 } )
        
        @fill_x_y()                 # TEST   TODO A remettre si onload non necessaire
        #@add_child new ImgDirectorySetItem
#         onload = =>
#             @_signal_change()
#             if @_vec_y.length == 0
#                 @fill_x_y()
                
#         @bind =>
#             if  @_vec_x.has_been_modified() or @_vec_y.has_been_modified() or @_nb_values.has_been_modified()
#                 @make_mesh() #TODO
                
    #TEST             
    cosDeg: (number, precision)->
        console.log number*(Math.PI/180)
        parseFloat (Math.cos(number*(Math.PI/180))).toPrecision(5)


    fill_x_y: ()->
        zero = new Number(0).valueOf()
        
        @_vec_x.clear()
        @_vec_y.clear()
        
        for i in [ 0 ... @_nb_values.get() ]
            try
                @_vec_x.push 0
                @_vec_y.push 0
                @_vec_x[ i ].set new Number(i).valueOf()                             

#                 if typeof(cosDeg(i)) != "number"
#                     alert cosDeg(i)+"is not a number"
                if csd_i <0 and csd_i > -2e-16        
                    @_vec_y[ i ].set zero
#                 if (i == 270) 
#                     @_vec_y[ i ].set zero ### #TODO A retirer!!! TODO
#                 else 
#                     if (i == 90)
#                         @_vec_y[ i ].set zero
# #                         alert "i == 90 a changé"+@_vec_y[ i ].get()
                else
                    csd_i = cosDeg(i)
                    @_vec_y[ i ].set new Number(csd_i).valueOf()
                
         #TODO A retirer!!! TODO
#                     alert "csd_i: "+csd_i
#                     alert "@_vec_y[ i ]: "+@_vec_y[ i ]
#                 else 
#                     if Math.abs(csd_i) == Number.NEGATIVE_INFINITY
#                         @_vec_y[ i ].set zero                      
            catch error
                return false      
        return true
            
    display_suppl_context_actions: ( context_action )  ->
        context_action.push
            txt: "link"
            ico: "img/TestSamirImg.png"
#             ico: "../../../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
#             ico: "~/LMTLabsScetup/software_library/LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
            siz: 1
            TS_instance : this
            fun: ( evt, app) =>
                @_container_global = new_dom_element
                    id        : "id_apps_container"
                    className : "apps_container"
#                     style     : #TODO A  remettre TODO
                    height     : 30
                    width      : "100%"
                    border     : "1px solid blue"
                    background : "#ffffff"
#                     position: "absolute"
#                     left    : 10
#                     right   : 10
#                     top     : 40
#                     bottom  : 10
                        
#                 inst = undefined
#                 console.log app
#                 console.log app.all_canvas_inst()
              
                    
                #TODO A  remettre Debut
#                 if (inst.divCanvas)?
# #                   Ptop   = TS_instance.getTop( inst.div )
#                   Ptop   = @getTop( inst.div )
#                   Pleft  = @getLeft( inst.div )  
#                   Pwidth = inst.divCanvas.offsetWidth
#                   Pheight = inst.divCanvas.offsetHeight
#                   Pheight = Pheight + 22
#                 
#                 else
#                   Ptop   = 100
#                   Pleft  = 100
#                   Pwidth = 800 
#                   Pheight = 500 
                  #TODO A  remettre  Fin
                  
#                 p = new_popup "My graph", event: evt, child: @_container_global, top_x: Pleft, top_y: Ptop, width: Pwidth, height: Pheight, onclose: =>
#                     @onPopupClose( app ) 
#                   new GenericManagerPanelInstance( this, el, @app_data, undo_manager )
                Ptop   = 100
                Pleft  = 100
                Pwidth = 800 
                Pheight = 500
                
                el = undefined   
#                 data = undefined # @data = new TreeAppData               
                
#                 sc_inst = app.selected_canvas_inst()
#                 if sc_inst.length == 0
#                     el = new_dom_element
#                         parentNode: @bel
#                         id        : "main_window"
#                         style:
#                             position: "absolute"
#                             left    : 0
#                             right   : 0
#                             top     : "61px"
#                             bottom  : 0
#                 else
#                     for inst_i in sc_inst
#                         inst = inst_i
#                     el= inst.divCanvas
                
                 #TODO               
#                 NewLayoutManagerData = new LayoutManagerData.panel_id_of_term_panels
                
                panel_id_list = app.data.panel_id_list()# de type Array!
                console.log panel_id_list
                #output:  ["MainView"] 
                # puis
                #output: ["id_1", "id_6"] 
                
                panel_id_list0 = panel_id_list[0]
                console.log panel_id_list0
                #output: MainView
                # puis
                #output: id_1
                
                for panel_id_i in panel_id_list
                    if panel_id_i == "MainView"
                        console.log panel_id_i+"= "+panel_id_i.indexOf()
                #output: MainView= -1 
                
                session = app.data.selected_session()
                CurrentLayoutManager = app.layouts[ session.model_id ] 
#                 console.log CurrentLayoutManager._pan_vs_id( "MainView")+"_pan_vs_id( 'MainView')" # panel instance for a given panel_id
                # output:  not available
                
                console.log "CurrentLayoutManager.flat"
                console.log CurrentLayoutManager.flat
                # output LayoutManager.flat donne l'equivalent de LayoutManagerData._make_graph_rec
                
                display_settings = app.data.selected_display_settings()
                console.log display_settings + "display_settings"
                
                CurrentLayoutManagerData = display_settings._layout
                console.log CurrentLayoutManagerData
                console.log "= CurrentLayoutManagerData"
                
                # output:  ContextBar,TreeView,EditView,id_1,id_6 panel
                
                border_size = CurrentLayoutManager.border_size
                
                CLM_make_info =CurrentLayoutManagerData.make_info( 200, 300, border_size )
                #output similaire à flat
                
#                 console.log CurrentLayoutManagerData.find_item_with_panel_id+" find_item_with_panel_id"
#                 console.log CurrentLayoutManagerData.find_parent_of_panel_id( id, data = @root )+" find_parent_of_panel_id( id, data = @root )" 
#                 console.log CurrentLayoutManagerData.mk_split( d, s, id, c, new_panel_id = @_find_new_panel_id [] )+" mk_split( d, s, id, c, new_panel_id = @_find_new_panel_id [] )"
#                 console.log CurrentLayoutManagerData.rm_panel: ( id )+" rm_panel: ( id )"
#                 console.log CurLMData_t_ids+" panel_id_of_term_panels"
                console.log CurrentLayoutManagerData.panel_ids()+"panel_ids()" 
                # output:  id_2,ContextBar,id_3,id_4,TreeView,EditView,id_7,id_1,id_6
#                 
                console.log CurrentLayoutManagerData["root"]+" CurrentLayoutManagerData[root]"
                
                
#                 sub_LayoutManager = new SUBLayoutManager(app.el, CurrentLayoutManagerData,?)
#                 
#                 sub_LayoutManager._pan_vs_id[panel_id]#panel_id pas directement de type string mais Str ?
#                 sub_LayoutManager.set_message:
#                 sub_LayoutManager.resize_div( obj, p_min, p_max )
#                 sub_LayoutManager.getLeft
#                 sub_LayoutManager.getTop
                
                #!!!!!!!!!!!!!!!
#                 sub_LayoutManager.new_panel_instanceGeneric(data, "Generic" ) 
                  #TODO #TEST a ressesser si pertient
#                 CurLMData_t_ids = CurrentLayoutManagerData.panel_id_of_term_panels()
#                 last_id = CurLMData_t_ids[CurLMData_t_ids.length-1]
                console.log "last_id = "+last_id
                
                #TEST
                  
                  #TEST 1
#                 @lm = new LayoutManager el, CurrentLayoutManagerData
#                 @lm.new_panel_instance = ( data ) =>         
#                     new GenericManagerPanelInstance( el, app.data, "Generic title", "div", this, true )   
#                 @lm.show()
                  #TEST 2 fin
                  
                sc_inst = app.selected_canvas_inst()
#                 if sc_inst.length == 0
#                   el = new_dom_element
#                       parentNode: div
#                       id        : "MainView"
#                       style:
#                           position: "absolute"
#                           left    : 0
#                           right   : 0
#                           top     : "61px"
#                           bottom  : 0
#                 else
#                     for inst_i in sc_inst
#                         inst = inst_i
#                     el= inst.divCanvas
                  
                last_id= inst.divCanvas  
                CurrentLayoutManagerData.mk_split( 0, 0, id, 1, "GenericManagerPanelInstance" ) 
#                 mk_split:
#                 d = direction 1, s = up or down, id = panel_id, c = coeff
                CurrentLayoutManager.model = CurrentLayoutManagerData
                display_settings._layout = CurrentLayoutManagerData
                
                #TODO 
#                 display_settings._layout.mk_split( 0, 0, last_id, 1, "id_GenericPanel" ) 
                CurrentLayoutManager.new_panel_instance = ( data ) =>   
                    el =undefined
                    new GenericManagerPanelInstance( el, data, "Generic title", "div", this, true )                                  
                CurrentLayoutManager.show()
                app.data.update_associated_layout_data( display_settings )
#                 @genericPanel_inst = new GenericManagerPanelInstance( el, app.data, "Generic title", "div", this, true )
#                 graph_view = new TestSamirGraphView(
#                                    this,
#                                   @_container_global,
#                                   @_vec_x,
#                                   @_vec_y,
#                                   Pwidth,
#                                   Pheight)

                app.active_key.set false
               
                ##TODO
                
    # obtenir la position réelle dans le canvas
    getLeft: ( l ) ->
      if l.offsetParent?
          return l.offsetLeft + @getLeft( l.offsetParent )
      else
          return l.offsetLeft

# obtenir la position réelle dans le canvas
    getTop: ( l ) ->
        if l.offsetParent?
            return l.offsetTop + @getTop( l.offsetParent )
        else
            return l.offsetTop
            
    onPopupClose: ( app ) =>
        document.onkeydown = undefined
        app.active_key.set true
        
        #TODO A completer
    accept_child: ( ch ) ->
        ch instanceof FieldItem
        ch instanceof MaskItem or 
        ch instanceof DiscretizationItem or
        ch instanceof SketchItem or 
        ch instanceof ImgSetItem or
#         ch instanceof Graph or
        ch instanceof Vec
    
#     sub_canvas_items: -> #TODO A remettre
#         [ @_graph= new TestSamirGraphView(
#                                   this,
#                                   @_vec_x,
#                                   [@_vec_y],
#                                   Pwidth,
#                                   Pheight) ]
    
#         ch instanceof FieldSetCorreliItem or
#         ch instanceof FieldSetItem or
#         ch instanceof FieldItem or
#         ch instanceof MeshItem or
#         ch instanceof ImgSetItem

#     disp_only_in_model_editor: -> TODO A mettre ?
#         @mesh    
