#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - CMakeLists.txt
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*****************************************************************************]]
#ZE_SOURCE_PROCESSOR_END()

cmake_minimum_required(VERSION 3.6.0)
 
project(ZinekEngine)



## CMake Policies
#########################################################################################
if(POLICY CMP0043)
	cmake_policy(SET CMP0043 OLD)
endif()

	
## Edition Configuration
#########################################################################################
set(ZEBUILD_EDITION "Development" CACHE STRING "Determines Zinek Engine Edition.")
set_property(CACHE ZEBUILD_EDITION PROPERTY STRINGS "Standard" "Development" "Demo")
string(TOUPPER ${ZEBUILD_EDITION} ZEBUILD_EDITION_UPPER)
add_definitions(-DZE_EDITION_${ZEBUILD_EDITION_UPPER})
add_definitions(-DZE_EDITION=${ZEBUILD_EDITION_UPPER})


## Build system initalization
#########################################################################################
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake/External" "${PROJECT_SOURCE_DIR}/CMake" "${CMAKE_MODULE_PATH}")
set(CMAKE_LIBRARY_PATH "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x64")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_DEFINITIONS ZE_ZINEK)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


## Out of source build
#########################################################################################
include(External/build_dir_check)
macro_ensure_out_of_source_build("[ZEBuild] Error : Project requires an out of source build.")


## ZEBuild Modules
#########################################################################################
include(ze_modules)
ze_modules_init()


## DLL Options
#########################################################################################
set(ZEBUILD_MONOLITHIC FALSE CACHE BOOL "Compile Zinek as monolithic. No dll will be generated and external plugins will be disabled.")


## Directories
#########################################################################################
include_directories(
	${CMAKE_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/Source
	${CMAKE_CURRENT_SOURCE_DIR}/Source/ZEEngine
	${CMAKE_CURRENT_SOURCE_DIR}/Source/ZEFoundation)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


## Source Files
#########################################################################################
add_subdirectory(CMake)
add_subdirectory(Source)


## Install & Packing
#########################################################################################
add_subdirectory(Installation)
