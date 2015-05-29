#
class FieldSetDataCorreliModel extends Model
    constructor: ( id_c = 0 ) ->
        super()
        
      
        # attributes
        @add_attr
            id : id_c
            visualization: new FieldSet
            
            # results
            _norm_i_history       : []
            _norm_2_history       : []
            _residual_history     : []
            
            _residual_adv         : new NamedParametrizedDrawable( "Residual adv"    , new InterpolatedField )
            _residual             : new NamedParametrizedDrawable( "Residual"        , new InterpolatedField )
            _residual_int_adv     : new NamedParametrizedDrawable( "Residual int adv", new InterpolatedField )
            _residual_int         : new NamedParametrizedDrawable( "Residual int"    , new InterpolatedField )
        
   