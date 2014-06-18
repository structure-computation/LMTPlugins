#
class AbaqusComputationItem extends TreeItem_Computable
    constructor: ( name = "AbaqusComputation" ) ->
        super()
        
        @_name.set name
        @publication_link = "http://www.eikosim.com/PluginsManuals/AbaqusComputationPlugin.pdf"
        @_ico.set "img/AbaqusComputation_bouton.png"
        @_viewable.set false
        
        @_computation_mode.set false

        @add_child new AbaqusDataItem
        @add_output new FieldSetItem
        
    
    display_suppl_context_actions: ( context_action )  ->
       
        
    accept_child: ( ch ) ->
	
        
    sub_canvas_items: ->
        [  ]
        
    disp_only_in_model_editor: ->
#         @mesh
