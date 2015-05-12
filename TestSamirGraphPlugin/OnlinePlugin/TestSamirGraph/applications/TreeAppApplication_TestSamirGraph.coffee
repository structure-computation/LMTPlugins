class TreeAppApplication_TestSamirGraph extends TreeAppApplication
    constructor: ->
        super()
        
        @name = 'TestSamirGraph application'
        @powered_with    = 'LMT Cachan'

        _ina = ( app ) =>
            app.data.focus.get() != app.selected_canvas_inst()?[ 0 ]?.cm.view_id
            
        @actions.push
            ico: "img/testGraphs_bouton.png"
#             ico: "../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/testGraphs_bouton.png"
#             ico: "../../../../LMTPlugins/TestSamirGraphPlugin/OnlinePlugin/TestSamirGraph/img/TestSamirImg.png"
            siz: 1
#             vis: false
            txt: "Tests"
            ina: _ina
            fun: ( evt, app ) =>
                app.undo_manager.snapshot()
                mesher = @add_item_depending_selected_tree app.data, TestSamirGraphItem
#                 mesher = @add_item_depending_selected_tree app.data, TestAllItem
        
            key: [ "Shift+M" ]

        
            
