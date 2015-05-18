#
class ReaderINPItem extends TreeItem_Computable
    constructor: ( name = "ReaderINP" ) ->
        super()
        
        @add_attr
           location : "/home/mathieu/test.inp"
        
        @_name.set name
        @_ico.set "img/ReaderINP.png"
        @_viewable.set false
        
        @_computation_mode.set false
        @add_output new MeshItem
        
    draw: ( info ) ->
        
#     z_index: ->
#         @_mesh.z_index()
        
    disp_only_in_model_editor: ->
#         @mesh

    