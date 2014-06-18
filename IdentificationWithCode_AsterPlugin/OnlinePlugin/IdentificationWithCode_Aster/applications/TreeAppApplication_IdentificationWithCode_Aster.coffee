class TreeAppApplication_IdentificationWithCode_Aster extends TreeAppApplication
    constructor: ->
        super()
         
        @name = 'IdentificationWithCode_Aster'
        @powered_with    = 'LMT'
        @publication_link = "http://www.eikosim.com/PluginsManuals/IdentificationWithCode_AsterPlugin.pdf"
        
        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
        
        @actions.push
            ico: "img/IdentificationWithCode_Aster_bouton.png"
            siz: 1
            txt: "test collection item"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                IdentificationWithCode_Aster = @add_item_depending_selected_tree app.data, IdentificationWithCode_AsterItem
        
