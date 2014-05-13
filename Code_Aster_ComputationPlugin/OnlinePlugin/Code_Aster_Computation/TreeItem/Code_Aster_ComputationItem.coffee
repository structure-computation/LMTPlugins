#
class Code_Aster_ComputationItem extends TreeItem_Computable
    constructor: ( name = "Code_Aster_Computation" ) ->
        super()
        
        @_name.set name
        @_ico.set "img/Code_Aster_Computation_bouton.png"
        @_viewable.set false
        
        @_computation_mode.set false

        @add_child new Code_Aster_DataItem
        @add_output new FieldSetItem
        
    
    display_suppl_context_actions: ( context_action )  ->
       
        
    accept_child: ( ch ) ->
	
        
    sub_canvas_items: ->
        [  ]
        
    disp_only_in_model_editor: ->
#         @mesh

    