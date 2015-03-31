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
                        
                inst = undefined
                for inst_i in app.selected_canvas_inst()
#                     console.log inst_i
                    inst = inst_i
                    
                                
                if (inst.divCanvas)?
#                   Ptop   = TS_instance.getTop( inst.div )
                  Ptop   = @getTop( inst.div )
                  Pleft  = @getLeft( inst.div )  
                  Pwidth = inst.divCanvas.offsetWidth
                  Pheight = inst.divCanvas.offsetHeight
                  Pheight = Pheight + 22
                
                else
                  Ptop   = 100
                  Pleft  = 100
                  Pwidth = 800 
                  Pheight = 500 
                
#                 p = new_popup "My graph", event: evt, child: @_container_global, top_x: Pleft, top_y: Ptop, width: Pwidth, height: Pheight, onclose: =>
#                     @onPopupClose( app ) 
#                   new GenericManagerPanelInstance( this, el, @app_data, undo_manager )

                el = undefined   
                data = undefined # @data = new TreeAppData
                @genericPanel_inst = new GenericManagerPanelInstance( el, data, title = "", elem_kind = "div", this )   
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
