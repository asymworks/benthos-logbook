#------------------------------------------------------------------------------
# CMake Module Finder for sqlite3
#
# Sets the following variables:
#   SQLITE3_INCLUDE_DIR - Where to find Sqlite 3 header files (directory)
#   SQLITE3_LIBRARIES - Sqlite 3 libraries
#   SQLITE3_LIBRARY_RELEASE - Where the release library is
#   SQLITE3_LIBRARY_DEBUG - Where the debug library is
#   SQLITE3_FOUND - set to TRUE if we found everything (library, includes and executable)
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

if( SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY_RELEASE AND SQLITE3_LIBRARY_DEBUG )
    set(SQLITE3_FIND_QUIETLY TRUE)
endif( SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY_RELEASE AND SQLITE3_LIBRARY_DEBUG )

find_path( SQLITE3_INCLUDE_DIR sqlite3.h  )
find_library(SQLITE3_LIBRARY_RELEASE NAMES sqlite3 )

if( SQLITE3_LIBRARY_RELEASE OR SQLITE3_LIBRARY_DEBUG AND SQLITE3_INCLUDE_DIR )
	set( SQLITE3_FOUND TRUE )
endif( SQLITE3_LIBRARY_RELEASE OR SQLITE3_LIBRARY_DEBUG AND SQLITE3_INCLUDE_DIR )

if( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )
	# if the generator supports configuration types then set
	# optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
	if( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
		set( SQLITE3_LIBRARIES optimized ${SQLITE3_LIBRARY_RELEASE} debug ${SQLITE3_LIBRARY_DEBUG} )
	else( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
		set( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_RELEASE} )
	endif( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
elseif( SQLITE3_LIBRARY_RELEASE )
	set( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_RELEASE} )
else( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )
	set( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_DEBUG} )
endif( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )

if( SQLITE3_FOUND )
	if( NOT SQLITE3_FIND_QUIETLY )
		message( STATUS "Found Sqlite3 header file in ${SQLITE3_INCLUDE_DIR}")
		message( STATUS "Found Sqlite3 libraries: ${SQLITE3_LIBRARIES}")
	endif( NOT SQLITE3_FIND_QUIETLY )
else(SQLITE3_FOUND)
	if( SQLITE3_FIND_REQUIRED)
		message( FATAL_ERROR "Could not find Sqlite3" )
	else( SQLITE3_FIND_REQUIRED)
		message( STATUS "Optional package Sqlite3 was not found" )
	endif( SQLITE3_FIND_REQUIRED)
endif(SQLITE3_FOUND)