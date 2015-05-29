class TreeAppApplication_Blank extends TreeAppApplication
    constructor: ->
        super()
         
        @name = 'Blank'
        @powered_with    = '???'
        
        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
        
        @actions.push
            ico: "img/Blank_bouton.png"
            siz: 1
            txt: "test collection item"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                Blank = @add_item_depending_selected_tree app.data, BlankItem
        
