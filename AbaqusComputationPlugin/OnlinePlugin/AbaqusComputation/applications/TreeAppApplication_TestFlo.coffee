class TreeAppApplication_AbaqusComputation extends TreeAppApplication
    constructor: ->
        super()
        
        @name = 'Abaqus Computation'
        @powered_with    = 'LMT'

        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
            
        @actions.push
            ico: "img/AbaqusComputation_bouton.png"
            siz: 1
            txt: "Abaqus Computation"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                tester = @add_item_depending_selected_tree app.data, AbaqusComputationItem
        

        
            