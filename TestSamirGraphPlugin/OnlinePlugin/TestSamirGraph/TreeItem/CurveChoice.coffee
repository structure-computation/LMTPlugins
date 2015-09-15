#
class CurveChoice extends Model
    constructor: (curve = new Curve) ->
        super()        
        console.log  "CurveChoice is built"
        @add_attr
            curve: curve
            
        @add_attr
            _name: @curve._name
            aggregate:    
                #color: @curve.color #new Color #LineTheme#new Choice# new Color#(0, [curve.color])#new Choice
                colorName: new Choice
            
            #TEST marker1
                
                #TEST marker1
                marker: new Choice(0, [ "dot", "cross", "diamond", "square", "triangle-down", "triangle-up"])
                
                #TEST marker2
#                 marker: new Choice(0, [ "dot", "square", "cross", "diamond", "bar" ]) 
                
                markerWidth : new ConstrainedVal( 4, { min: 0, max: 40 } )  #TODO mettre dans graphSetting 
                markerHeight : new ConstrainedVal( 4, { min: 0, max: 40 } )
                markerRadius : new ConstrainedVal( 4, { min: 0, max: 40 } )
                markerColor : new Choice
                markerSize : new ConstrainedVal( 75, { min: 0, max: 150 } )
                
            #TEST marker2
#                 marker: new Choice(0, [ "circle", "square", "arrow", "stub" ]) 
#                 modify_curve: new Choice(0, ["no", "yes"]) 
#                 modify_curve: new Button
                name: @curve._name     
   
        colorD3JS = new ColorD3JS
        
        console.log "colorD3JS.colors"
        console.log colorD3JS.colors
        
        #TEST colChoice2 
        @aggregate.colorName.lst.push key+"" for key, value of colorD3JS.colors
        
        #position the colorNam
        #TEST colNam1
#         colNameIndex = @aggregate.colorName.lst.indexOf curve.colorName#.get()
        
        #TEST colNam2
        colNameIndex = @aggregate.colorName.lst.indexOf @curve.colorName.get()
        @aggregate.colorName.num.set colNameIndex
        
        #markerColor
        @aggregate.markerColor.lst.push key+"" for key, value of colorD3JS.colors
        
        #position the colorNam
        colNameIndex = @aggregate.markerColor.lst.indexOf "red" #.get()
        @aggregate.markerColor.num.set colNameIndex
                
        #matching curve choice and curve object parameter
#         bind @aggregate.modify_curve, =>
#             if @aggregate.modify_curve.get() == "yes" and @aggregate.modify_curve.has_been_modified()
#                 console.log "choice modified"
#                 if @aggregate.color.has_been_modified()       
#                     @curve.color.set @aggregate.color.get()
#                 if @aggregate.marker.has_been_modified() 
#                     @curve.marker.set @aggregate.marker.get()

          #TEST 1 no bind
          
          #TEST 2 bind
#         bind @curve.color, =>
#             if @curve.color.has_been_modified()
#                 alert "color has changed"
#                 console.log "color has changed"
#                 console.log "@curve.color.get()"
#                 console.log @curve.color.get()           
                
#                 console.log "@curve.color._date_last_modification"
#                 console.log @curve.color._date_last_modification
#                 console.log "Model._counter" 
#                 console.log Model._counter
#                 console.log "Model._force_m" 
#                 console.log Model._force_m
#                 console.log "@curve.color.views"
#                 console.log @curve.color.views
                
#                 @aggregate.color.set @curve.color.get()
              
        #TEST 1 no bind
        
#         bind @curve.colorName, =>
#             if @curve.colorName.has_been_modified()
#                 alert "colorName has changed"
#                 console.log "colorName has changed"
#                 @aggregate.colorName.set @curve.colorName.get()
                
#         bind @curve._name, =>
#             if @curve._name.has_been_modified()
#                 @aggregate.name.set @curve._name.get()
        
    toString: ->
        @_name.get()
        
    hexToRgb:(hex)->
        result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex)
        if result? 
            return {
                r: parseInt( result[1], 16 ), 
                g: parseInt( result[2], 16 ),
                b: parseInt( result[3], 16 )
                } 
        
    get_model_editor_parameters: ( res ) ->
        res.model_editor[ "colorName" ] = ModelEditorItem_ChoiceWithEditableItems 
        res.model_editor[ "marker" ] = ModelEditorItem_ChoiceWithEditableItems #TEST TODO "curves"
#         res.model_editor[ "modify_curve" ] = ModelEditorItem_ChoiceWithEditableItems #TEST TODO "curves"
#         res.model_editor[ "color" ] = ModelEditorItem_ChoiceWithEditableItems #TEST TODO "curves"
    
    #cosmetic_attribute: ( name ) ->
    #    name in [ "elasticity" ]
    
#     accept_child: ( ch ) ->
#         #
# 
#     z_index: ->
#         #
#          
#     sub_canvas_items: ->
#         [ ]
#     