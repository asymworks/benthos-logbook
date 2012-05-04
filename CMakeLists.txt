#------------------------------------------------------------------------------
# Master CMake File for the Scuba Logbook C++ Library (liblogbook)
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

cmake_minimum_required(VERSION 2.8.8)
project(liblogbook)

set(LIBLOGBOOK_MAJOR_VERSION 0)
set(LIBLOGBOOK_MINOR_VERSION 1)
set(LIBLOGBOOK_PATCH_VERSION 0)
set(LIBLOGBOOK_VERSION
	${LIBLOGBOOK_MAJOR_VERSION}.${LIBLOGBOOK_MINOR_VERSION}.${LIBLOGBOOK_PATCH_VERSION})

set(CMAKE_MODULE_PATH 
	${CMAKE_MODULE_PATH} 
	${CMAKE_SOURCE_DIR}/cmake/modules/
)

# All source files are in the src directory
add_subdirectory(src)
