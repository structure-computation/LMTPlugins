class TreeAppApplication_IdentificationWithAbaqus extends TreeAppApplication
    constructor: ->
        super()
         
        @name = 'Identification With Abaqus'
        @powered_with    = 'LMT'
        
        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
        
        @actions.push
            ico: "img/IdentificationWithAbaqus_bouton.png"
            siz: 1
            txt: "Identification with Abaqus - FEMU"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                tester = @add_item_depending_selected_tree app.data, IdentificationWithAbaqusItem
        
