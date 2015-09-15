#
class FieldSetCorreliItem extends TreeItem_Automatic
    constructor: ( name = "Field_V2", id_c = 0 ) ->
        super()
        
        # default values
        @_name.set name
        #@_ico.set "img/part_collection.png"
        @_viewable.set true
        
        field_set_data = new FieldSetDataCorreliModel
        
        # attributes
        @add_attr
            id : id_c
            visualization: new FieldSet
            
            # data results
            _file_field_set_data   : new  File (@_name.get() + "_field_data"), field_set_data
            
            #visualisation attributes
            _has_result : false
            _old_visualisation_field : ""
            _old_time_step : -1
            time : -1
            
            # results
            _norm_i_history       : []
            _norm_2_history       : []
            _residual_history     : []
            
            _residual_adv         : new NamedParametrizedDrawable( "Residual adv"    , new InterpolatedField )
            _residual             : new NamedParametrizedDrawable( "Residual"        , new InterpolatedField )
            _residual_int_adv     : new NamedParametrizedDrawable( "Residual int adv", new InterpolatedField )
            _residual_int         : new NamedParametrizedDrawable( "Residual int"    , new InterpolatedField )
        
    display_suppl_context_actions: ( context_action )  ->
        context_action.push
            txt: "info"
            ico: "img/info.png"
            fun: ( evt, app ) =>
                for data in @visualization.color_by.lst[0].data._data
                    console.log data.pos.get()
   
    accept_child: ( ch ) ->
        false
    
    sub_canvas_items: ->
        res = [@visualization]
        return res
        
    cosmetic_attribute: ( name ) ->
        super( name ) or ( name in [ "visualization", "_residual", "_residual_adv", "_residual_int", "_residual_int_adv", "_norm_i_history", "_norm_2_history", "_residual_history" ] )
   
    information: ( div ) ->
        if not @txt?
            @txt = new_dom_element
                parentNode: div
                
        @txt.innerHTML = @_norm_2_history.get()
   
   