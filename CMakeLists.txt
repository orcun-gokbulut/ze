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

cmake_minimum_required(VERSION 2.8)

project(Zinek)

## Build system initalization
#########################################################################################
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

include(ze)
ze_configure_init()
ze_configure_version()
ze_configure_platform()
ze_configure_externals()
ze_configure_debugging()
ze_configure_unit_tests()
ze_configure_static_checks()
ze_configure_symbol_server()


## DLL Options
#########################################################################################
set(ZEBUILD_MONOLITHIC FALSE CACHE BOOL "Compile Zinek as monolithic. No dll will be generated and external plugins will be disabled.")



## Compiler & Linker Options
#########################################################################################
if ("${ZEBUILD_PLATFORM_COMPILER}" EQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")	
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /Zi")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /DEBUG")
	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS)
endif()


## Directories
#########################################################################################
include_directories(
	${CMAKE_SOURCE_DIR}/Source
	${CMAKE_SOURCE_DIR}/Source/ZEEngine
	${CMAKE_SOURCE_DIR}/Source/ZEFoundation
	${CMAKE_SOURCE_DIR}/Source/ZEToolSDK
	${CMAKE_SOURCE_DIR}/Source/ZETest)

include_directories(
	${ZEBUILD_INCLUDE_PATH}/DirectX
	${ZEBUILD_INCLUDE_PATH}/PhysX2/Cooking 
	${ZEBUILD_INCLUDE_PATH}/PhysX2/Foundation 
	${ZEBUILD_INCLUDE_PATH}/PhysX2/PhysXLoader
	${ZEBUILD_INCLUDE_PATH}/PhysX2/Physics
	${ZEBUILD_INCLUDE_PATH}/3dsMax/2010)
	
link_directories(
	${ZEBUILD_LIB_PATH}/$(ConfigurationName)
	${ZEBUILD_LIB_PATH}/$(ConfigurationName)/vrpn
	${ZEBUILD_LIB_PATH}/$(ConfigurationName)/Bullet
	${ZEBUILD_LIB_PATH}/DirectX
	${ZEBUILD_LIB_PATH}/PhysX2
	${ZEBUILD_LIB_PATH}/PhysX3
	${ZEBUILD_LIB_PATH}/3dsMax/2010)

include(ze_qt)


## Source Files
#########################################################################################
add_subdirectory(CMake)
add_subdirectory(Source)

get_property(fancy_lib_location TARGET ZEFC PROPERTY LOCATION)
message (STATUS "fancy_lib_location == ${fancy_lib_location}")

## Install & Packing
#########################################################################################
add_subdirectory(Installation)
