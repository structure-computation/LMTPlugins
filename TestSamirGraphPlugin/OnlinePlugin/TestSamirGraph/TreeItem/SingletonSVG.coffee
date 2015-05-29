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


# class designed as a Singleton
class SingletonSVG #extends DrawingSVG###
 
    instance = null
    # Create a private class that we can initialize however
    # defined inside this scope to force the use of the
    # singleton class.
    class PrivateClass
        constructor: () ->
            @drawing_SVG = new DrawingSVG
#         echo: -> @message
      # This is a static method used to either retrieve the
      # instance or create a new one.
    
    @getInstance: () ->
        instance ?= new PrivateClass()


# class SingletonSVG extends DrawingSVG
#     _constructor: ( ) ->    
#         super()
#     
#     class _SingletonHolder
#         SingletonSVG INSTANCE = new SingletonSVG
#     
#     _SingletonSVG getInstance:->
#         return _SingletonHolder.INSTANCE
    