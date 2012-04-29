#------------------------------------------------------------------------------
# CMake Module Finder for yajl
#
# Sets the following variables:
#   YAJL_INCLUDE_DIR - Where to find yajl header files (directory)
#   YAJL_LIBRARIES - yajl libraries
#   YAJL_FOUND - set to TRUE if we found everything (library, includes and executable)
#------------------------------------------------------------------------------
#
# Copyright (C) 2011 Asymworks, LLC.  All Rights Reserved.
#
# Developed by: Asymworks, LLC <info@asymworks.com>
# 				 http://www.asymworks.com
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal with the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#   1. Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimers.
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimers in the
#      documentation and/or other materials provided with the distribution.
#   3. Neither the names of Asymworks, LLC, nor the names of its contributors
#      may be used to endorse or promote products derived from this Software
#      without specific prior written permission.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# WITH THE SOFTWARE.
#

if( YAJL_INCLUDE_DIR )
    set(YAJL_FIND_QUIETLY TRUE)
endif( YAJL_INCLUDE_DIR )

find_path( YAJL_INCLUDE_DIR yajl/yajl_common.h  )
find_library( YAJL_LIBRARIES yajl )

if( YAJL_LIBRARIES AND YAJL_INCLUDE_DIR )
	set( YAJL_FOUND TRUE )
endif( YAJL_LIBRARIES AND YAJL_INCLUDE_DIR )

if( YAJL_FOUND )
	if( NOT YAJL_FIND_QUIETLY )
		message( STATUS "Found yajl header file in ${YAJL_INCLUDE_DIR}")
		message( STATUS "Found yajl libraries: ${YAJL_LIBRARIES}")
	endif( NOT YAJL_FIND_QUIETLY )
else(YAJL_FOUND)
	if( YAJL_FIND_REQUIRED)
		message( FATAL_ERROR "Could not find yajl" )
	else( YAJL_FIND_REQUIRED)
		message( STATUS "Optional package yajl was not found" )
	endif( YAJL_FIND_REQUIRED)
endif(YAJL_FOUND)