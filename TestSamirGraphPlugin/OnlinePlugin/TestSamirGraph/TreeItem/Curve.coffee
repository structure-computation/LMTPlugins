class Curve extends Model
    constructor: (ord_name, ord_vec, abs_name, abs_vec, num) ->
        super()
        # attributes
        @add_attr
            _name: ord_name
#             color: new Color
            colorName: new Str
#             marker: "diamond"#"square" #"dot" 
            number: num
            curveTypeName: ord_name?.split(" ")[0]
            
            ordinate_name: ord_name
            _ordinate_vec: ord_vec
            ordinate_unity:"pixel"
            
            abscissa_name: abs_name
            _abscissa_vec: abs_vec
            abscissa_unity:"pixel"
# #             cf d3_rgb_names