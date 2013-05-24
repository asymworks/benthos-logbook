#------------------------------------------------------------------------------
# CMake File for the Scuba Logbook C++ Library (liblogbook)
#
# <proj_root>/src Subdirectory
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

INCLUDE (CheckIncludeFileCXX)
INCLUDE (CheckCXXSourceCompiles)

# C++ Flags
if (WIN32)
	add_definitions( -D_SCL_SECURE_NO_WARNINGS )
else (WIN32)
	add_definitions( -std=c++0x -fPIC -g )
endif (WIN32)

# Check for include files
check_include_file_cxx( cxxabi.h HAVE_CXXABI_H )

if ( HAVE_CXXABI_H )

	check_cxx_source_compiles( "
#include <stdlib.h>
#include <typeinfo>
#include <cxxabi.h>
int main(int argc, char ** argv)
{
	int status;
	char * res = abi::__cxa_demangle(\"int\", NULL, NULL, & status);
	free(res);
	return 0;
}
" HAVE_CXA_DEMANGLE )

else ( HAVE_CXXABI_H )
	set ( HAVE_CXA_DEMANGLE FALSE )
endif ( HAVE_CXXABI_H )

# Boost Headers Required
find_package( Boost 1.39 REQUIRED )

message("Found Boost in ${Boost_INCLUDE_DIRS}")

# Sqlite3 Required
find_package( Sqlite3 REQUIRED )

# yajl Required
find_package( Yajl REQUIRED )

# Setup Include Directories
include_directories(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_BINARY_DIR}/include
	${Boost_INCLUDE_DIRS}
	${SQLITE3_INCLUDE_DIRS}
	${YAJL_INCLUDE_DIRS}
)

# Configure Files
configure_file(
	${CMAKE_SOURCE_DIR}/include/benthos/logbook/config.hpp.in 
	${CMAKE_BINARY_DIR}/include/benthos/logbook/config.hpp 
	@ONLY
)

configure_file(
	${CMAKE_SOURCE_DIR}/benthos-logbook.pc.in
	${CMAKE_BINARY_DIR}/benthos-logbook.pc
	@ONLY
)

# Build the logbook, logging and dbapi modules
add_subdirectory(logbook)
add_subdirectory(logging)
add_subdirectory(dbapi)

# Build the logbook library
add_library(benthos-logbook SHARED
	${CMAKE_BINARY_DIR}/include/benthos/logbook/config.hpp
	$<TARGET_OBJECTS:logbook_module>
	$<TARGET_OBJECTS:logging_module>
	$<TARGET_OBJECTS:dbapi_module>
)

# Link the logbook library
target_link_libraries(benthos-logbook
	${SQLITE3_LIBRARIES}
	${YAJL_LIBRARIES}
)

# Install Shared Library
install(TARGETS benthos-logbook
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static
)

# Install Headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include
	PATTERN "*.in" EXCLUDE)
install(FILES ${CMAKE_BINARY_DIR}/include/benthos/logbook/config.hpp DESTINATION include/benthos/logbook)
install(FILES ${CMAKE_BINARY_DIR}/benthos-logbook.pc DESTINATION lib/pkgconfig)