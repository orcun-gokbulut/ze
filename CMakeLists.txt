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
 
set(ZINEK_VERSION_MAJOR 0)
set(ZINEK_VERSION_MINOR 5)
set(ZINEK_VERSION_INTERNAL 0)
set(ZINEK_VERSION_BUILD 5)
set(ZINEK_VERSION_PLATFORM "WIN32")

## Configuration
#########################################################################################
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})
 
include(build_dir_check)
include(parse_arguments)
include(add_source)
include(append_property)

macro_ensure_out_of_source_build ("${PROJECT_NAME} requires an out of source build.")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")


## Platform Detection
#########################################################################################
message(STATUS "[ZEBuild] Detecting Platform.")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(ZINEK_BUILD_PLATFORM_CLASS "Windows")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZINEK_BUILD_PLATFORM "Win32")
	else (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZINEK_BUILD_PLATFORM "Win64")
	endif (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(ZINEK_BUILD_PLATFORM_CLASS "MacOSX")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZINEK_BUILD_PLATFORM "MacOSX32")
	else (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZINEK_BUILD_PLATFORM "MacOSX64")
	endif (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(ZINEK_BUILD_PLATFORM_CLASS "Linux")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZINEK_BUILD_PLATFORM "Linux32")
	else (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZINEK_BUILD_PLATFORM "Linux64")
	endif (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Unix")
	set(ZINEK_BUILD_PLATFORM_CLASS "Unix")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZINEK_BUILD_PLATFORM "Unix32")
	else (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZINEK_BUILD_PLATFORM "Unix64")
	endif (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
else (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message(SEND_ERROR "[ZEBuild] Platform is not supported. Operating System : ${CMAKE_SYSTEM_NAME}, Achitecture : ${CMAKE_SYSTEM_PROCESSOR}, Compiler : ${CMAKE_CXX_COMPILER}")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

set (ZINEK_BUILD_COMPILER ${CMAKE_CXX_COMPILER})
set (ZINEK_BUILD_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

	set(ZINEK_BUILD_PLATFORM "Win x86")

message(STATUS "[ZEBuild] Platform Class  : " ${ZINEK_BUILD_PLATFORM_CLASS})
message(STATUS "[ZEBuild] Platform        : " ${ZINEK_BUILD_PLATFORM})
message(STATUS "[ZEBuild] Architecture    : " ${ZINEK_BUILD_ARCHITECTURE})
message(STATUS "[ZEBuild] Compiler        : " ${ZINEK_BUILD_COMPILER})
message(STATUS "[ZEBuild] Platform detected.")

## Directories
#########################################################################################
include_directories(
	${PROJECT_SOURCE_DIR} 
	${PROJECT_SOURCE_DIR}/Include 
	${PROJECT_SOURCE_DIR}/Include/DirectX
	${PROJECT_SOURCE_DIR}/Include/OpenAL
	${PROJECT_SOURCE_DIR}/Include/DirectX
	${PROJECT_SOURCE_DIR}/Include/PhysX/Cooking
	${PROJECT_SOURCE_DIR}/Include/PhysX/Foundation
	${PROJECT_SOURCE_DIR}/Include/PhysX/NxCharacter
	${PROJECT_SOURCE_DIR}/Include/PhysX/Physics
	${PROJECT_SOURCE_DIR}/Include/Curl
	${PROJECT_SOURCE_DIR}/Include/3dsMax/2010)
	
link_directories(
	"${PROJECT_SOURCE_DIR}/Lib/${ZINEK_BUILD_PLATFORM}"
	"${PROJECT_SOURCE_DIR}/Lib/${ZINEK_BUILD_PLATFORM}/Release" 
	"${PROJECT_SOURCE_DIR}/Lib/${ZINEK_BUILD_PLATFORM}/DirectX" 
	"${PROJECT_SOURCE_DIR}/Lib/${ZINEK_BUILD_PLATFORM}/OpenAL" 
	"${PROJECT_SOURCE_DIR}/Lib/${ZINEK_BUILD_PLATFORM}/PhysX"
	"${PROJECT_SOURCE_DIR}/Lib/${ZINEK_BUILD_PLATFORM}/3dsMax/2010")


## Definitions
#########################################################################################
append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS 
	ZE_ZINEK
	ZE_ZINEK_VERSION_PLATFORM=ZE_VP_WIN32
	ZE_ZINEK_VERSION_MAJOR=${ZINEK_VERSION_MAJOR}
	ZE_ZINEK_VERSION_MINOR=${ZINEK_VERSION_MINOR}
	ZE_ZINEK_VERSION_INTERNAL=${ZINEK_VERSION_INTERNAL}
	ZE_ZINEK_VERSION_BUILD=${ZINEK_VERSION_BUILD})

append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG 
	ZE_DEBUG_ENABLED 
	ZE_DEBUG_BREAK_ON_ERROR 
	_ZE_DEBUG_BREAK_ON_WARNING
	_ZE_DEBUG_ENABLE_MEMORY_CHECKS)
	
	
## Files
#########################################################################################
add_subdirectory (Source)
add_subdirectory (ZEToolSDK)
add_subdirectory (ZEFoundation)
add_subdirectory (ZE3dsMax)
add_subdirectory (ZEditor)
add_subdirectory (ZETests)


## Packing
#########################################################################################
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "Zinek Engine")
set(CPACK_PACKAGE_VENDOR "Zinek Code House and Game Studio")
set(CPACK_PACKAGE_VERSION_MAJOR ${ZINEK_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${ZINEK_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Zinek")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/Installation/Description.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\Installation\\\\InstallIcon.bmp")

set(CPACK_NSIS_DISPLAY_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_NSIS_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_NSIS_INSTALLED_ICON_NAME "ZE.exe")
set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.zinekengine.com\\\\en\\\\docs\\\\install")
set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.zinekengine.com")
set(CPACK_NSIS_CONTACT "contact@zinekengine.com")
set(CPACK_NSIS_MODIFY_PATH 0)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/Installation/License.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/Installation/Welcome.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/Installation/Readme.txt")

set(CPACK_COMPONENTS_ALL_GROUPS_IN_ONE_PACKAGE 1)

set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "")
set(CPACK_COMPONENT_RUNTIME_REQUIRED 1)

set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Static libraries that are required to build Zinek Engine based projects.")

set(CPACK_COMPONENT_INCLUDES_DISPLAY_NAME "Headers")
set(CPACK_COMPONENT_INCLUDES_DESCRIPTION  "C/C++ header files for use with Zinek Engine")

set(CPACK_COMPONENT_RUNTIME_GROUP "Zinek Engine")
set(CPACK_COMPONENT_LIBRARIES_GROUP "C++ Development")
set(CPACK_COMPONENT_INCLUDES_GROUP "C++ Development")

include(CPack)
