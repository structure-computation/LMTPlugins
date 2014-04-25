class TreeAppApplication_Code_Aster_Computation extends TreeAppApplication
    constructor: ->
        super()
         
        @name = 'Code_Aster_Computation'
        @powered_with    = '???'
        
        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
        
        @actions.push
            ico: "img/Code_Aster_Computation_bouton.png"
            siz: 1
            txt: "test collection item"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                Code_Aster_Computation = @add_item_depending_selected_tree app.data, Code_Aster_ComputationItem
        
