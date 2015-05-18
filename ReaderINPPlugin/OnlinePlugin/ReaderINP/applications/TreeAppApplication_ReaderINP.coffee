class TreeAppApplication_ReaderINP extends TreeAppApplication
    constructor: ->
        super()
         
        @name = 'ReaderINP'
        @powered_with    = 'LMT'
        
        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
        
        @actions.push
            ico: "img/ReaderINP_bouton.png"
            siz: 1
            txt: "test collection item"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                ReaderINP = @add_item_depending_selected_tree app.data, ReaderINPItem
        
