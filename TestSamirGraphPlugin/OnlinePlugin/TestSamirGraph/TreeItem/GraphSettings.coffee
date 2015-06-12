# Copyright 2012 Structure Computation  www.structure-computation.com
# Copyright 2012 Hugo Leclerc
#
# This file is part of Soda.
#
# Soda is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Soda is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with Soda. If not, see <http://www.gnu.org/licenses/>.

# from mbostock’s block #3884955

class GraphSettings extends Model
    constructor: (atop = 20, aleft= 40 )-> #TEST sur 20 et 40  
        super()
        @add_attr
#         TODO mettre new Val(number ?
            margin : #TODO
                top: atop # TODO remettre 20
                right: 30 #TODO à déduire ou identique en valeur absolue
                bottom: 30
                left: aleft# TODO remettre 40
            interpolation: new Choice( 0, [ "basis", "monotone"]) 
            Y_orient: new Choice( 0, [ "right", "left"]) 
            X_orient: new Choice( 0, [ "bottom", "top"])
            dyTextOffset: new Choice( 0, [ ".35em"]) #TODO change to dy Text offset relative to the font size
            xTextPosition: 3
            
#                 interpolation: new Choice( 0, [ "basis", "monotone"]) 
#             Y_orient: new Choice( 0, [ "right", "left"]) 
#             X_orient: new Choice( 0, [ "bottom", "top"])
#             dyTextOffset: new Choice( 0, [ ".35em"]) #TODO change to dy Text offset relative to the font size
#             xTextPosition: 3