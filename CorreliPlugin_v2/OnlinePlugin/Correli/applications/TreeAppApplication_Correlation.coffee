class TreeAppApplication_Correlation extends TreeAppApplication
    constructor: ->
        super()
                
        @name = 'Image correlation'
        @powered_with    = 'LMT Cachan'
        @publication_link = "http://www.eikosim.com/PluginsManuals/CorreliPlugin.pdf"
            
        @actions.push
            ico: "img/correli_bouton.png"  #"img/correlation.png"
            siz: 2
            txt: "Correlation"
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                @add_corr app.data


    add_corr: ( app_data ) =>
        #
        m = @add_item_depending_selected_tree app_data, CorrelationItem, (object) =>
            if object._children[3]?
                object._children[3].time = app_data.time
                object._children[3]._app_data = app_data
            if object._output[0]?
                object._output[0].time = app_data.time
        