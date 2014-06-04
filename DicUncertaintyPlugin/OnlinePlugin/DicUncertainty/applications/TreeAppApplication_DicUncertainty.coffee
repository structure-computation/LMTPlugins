class TreeAppApplication_DicUncertainty extends TreeAppApplication
    constructor: ->
        super()
         
        @name = 'DicUncertainty'
        @powered_with    = 'LMT Cachan'
        
        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
        
        @actions.push
            ico: "img/DicUncertainty_bouton.png"
            siz: 1
            txt: "test collection item"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                DicUncertainty = @add_item_depending_selected_tree app.data, DicUncertaintyItem
        
