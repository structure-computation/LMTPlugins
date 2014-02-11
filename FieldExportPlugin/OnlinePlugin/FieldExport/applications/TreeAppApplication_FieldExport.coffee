class TreeAppApplication_FieldExport extends TreeAppApplication
    constructor: ->
        super()
         
        @name = 'FieldExport'
        @powered_with    = '???'
        
        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
        
        @actions.push
            ico: "img/FieldExport_bouton.png"
            siz: 1
            txt: "test collection item"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                FieldExport = @add_item_depending_selected_tree app.data, FieldExportItem
        
