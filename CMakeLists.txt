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

cmake_minimum_required(VERSION 2.8.10)

if(POLICY CMP0043)
	cmake_policy(SET CMP0043 OLD)
endif()
 
project(ZinekEngine)


set(ZEBUILD_EDITION "Development" CACHE STRINGS "Standard" "Development" "Demo")
string(TOUPPER ${ZEBUILD_EDITION} ZEBUILD_EDITION_UPPER)
add_definitions(-DZE_EDITION_${ZEBUILD_EDITION_UPPER})
add_definitions(-DZE_EDITION=${ZEBUILD_EDITION_UPPER})

## Build system initalization
#########################################################################################
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

include(ze)
ze_init()
ze_version_init()
ze_platform_init()
ze_external_init()
ze_debugging_init()
ze_unit_test_init()
ze_static_check_init()
ze_symbol_server_init()


## DLL Options
#########################################################################################
set(ZEBUILD_MONOLITHIC FALSE CACHE BOOL "Compile Zinek as monolithic. No dll will be generated and external plugins will be disabled.")




## Compiler & Linker Options
#########################################################################################
if ("${ZEBUILD_PLATFORM_COMPILER}" EQUAL "MSVC")
	if (ZEBUILD_PLATFORM_ARCHITECTURE_X86)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")	
	endif()
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	string(REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR- /EHsc-")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /IGNORE:4099")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /Zi")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /DEBUG /DEBUGTYPE:cv /OPT:REF,ICF")

	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS)
endif()


## Directories
#########################################################################################
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/Source
	${CMAKE_CURRENT_SOURCE_DIR}/Source/ZEEngine
	${CMAKE_CURRENT_SOURCE_DIR}/Source/ZEFoundation)

include(ze_qt)

add_definitions(-DFREEIMAGE_LIB)

## Source Files
#########################################################################################
add_subdirectory(CMake)
add_subdirectory(Source)


## Install & Packing
#########################################################################################
add_subdirectory(Installation)
