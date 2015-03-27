class TreeAppApplication_DICCITVirtualGauge extends TreeAppApplication
    constructor: ->
        super()
         
        @name = 'DICCITVirtualGauge'
        @powered_with    = 'LMT'
        @publication_link = "http://www.eikosim.com/PluginsManuals/DICCITVirtualGauge.pdf"
        
        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
        
        @actions.push
            ico: "img/VirtualGauge_bouton.png"
            siz: 1
            txt: "test collection item"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                VirtualGauge = @add_item_depending_selected_tree app.data, DICCITVirtualGaugeItem_v1
        
