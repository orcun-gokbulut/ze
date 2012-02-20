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

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

set(ZEBUILD_VERSION_MAJOR 0)
set(ZEBUILD_VERSION_MINOR 5)
set(ZEBUILD_VERSION_INTERNAL 0)
set(ZEBUILD_VERSION_BUILD 5)

## Configuration
#########################################################################################
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

include(build_dir_check)
include(parse_arguments)
include(append_property)
include(ze_functions)

macro_ensure_out_of_source_build ("${PROJECT_NAME} requires an out of source build.")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /Zi")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /DEBUG /VERBOSE:LIB")


## Platform Detection
#########################################################################################
message(STATUS "[ZEBuild] Detecting Platform.")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(ZEBUILD_PLATFORM_CLASS "Windows")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZEBUILD_PLATFORM "Win32")
	else()
		set(ZEBUILD_PLATFORM "Win64")
	endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(ZEBUILD_PLATFORM_CLASS "MacOSX")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZEBUILD_PLATFORM "MacOSX32")
	else()
		set(ZEBUILD_PLATFORM "MacOSX64")
	endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(ZEBUILD_PLATFORM_CLASS "Linux")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZEBUILD_PLATFORM "Linux32")
	else()
		set(ZEBUILD_PLATFORM "Linux64")
	endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Unix")
	set(ZEBUILD_PLATFORM_CLASS "Unix")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZEBUILD_PLATFORM "Unix32")
	else()
		set(ZEBUILD_PLATFORM "Unix64")
	endif()
else()
	message(SEND_ERROR "[ZEBuild] Platform is not supported. Operating System : ${CMAKE_SYSTEM_NAME}, Achitecture : ${CMAKE_SYSTEM_PROCESSOR}, Compiler : ${CMAKE_CXX_COMPILER}")
endif()

if (CMAKE_CXX_COMPILER MATCHES "cl")
	set(ZEBUILD_PLATFORM_COMPILER "MSVC")
elseif (CMAKE_CXX_COMPILER MATCHES "gcc")
	set(ZEBUILD_PLATFORM_COMPILER "GCC")
else()
	set(ZEBUILD_PLATFORM_COMPILER "UNKNOWN")
endif()

set(ZEBUILD_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

message(STATUS "[ZEBuild] Major Version    : " ${ZEBUILD_VERSION_MAJOR})
message(STATUS "[ZEBuild] Minor Version    : " ${ZEBUILD_VERSION_MINOR})
message(STATUS "[ZEBuild] Internal Version : " ${ZEBUILD_VERSION_INTERNAL})
message(STATUS "[ZEBuild] Build            : " ${ZEBUILD_VERSION_BUILD})
message(STATUS "[ZEBuild] Platform Class   : " ${ZEBUILD_PLATFORM_CLASS})
message(STATUS "[ZEBuild] Platform         : " ${ZEBUILD_PLATFORM})
message(STATUS "[ZEBuild] Architecture     : " ${ZEBUILD_ARCHITECTURE})
message(STATUS "[ZEBuild] C++ Compiler     : " ${ZEBUILD_COMPILER})
message(STATUS "[ZEBuild] Platform detected.")

if (ZEBUILD_PLATFORM_COMPILER MATCHES "UNKNOWN")
message(ERROR "Unknown compiler detected. You can only compile Zinek Engine with Microsoft Visual C++ Compiler, Intel C++ Compiler, GCC and CLANG. Other compiler are not supported.")
endif()

## Definitions
#########################################################################################
string(TOUPPER ${ZEBUILD_PLATFORM} ZEBUILD_PLATFORM_UPPER_CASE)
string(TOUPPER ${ZEBUILD_PLATFORM_CLASS} ZEBUILD_PLATFORM_CLASS_UPPER_CASE)
string(TOUPPER ${ZEBUILD_PLATFORM_COMPILER} ZEBUILD_PLATFORM_COMPILER_UPPER_CASE)

append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS 
	ZE_ZINEK
	"ZE_PLATFORM_${ZEBUILD_PLATFORM_UPPER_CASE}"
	"ZE_PLATFORM_CLASS_${ZEBUILD_PLATFORM_CLASS_UPPER_CASE}"
	"ZE_PLATFORM_COMPILER_${ZEBUILD_PLATFORM_COMPILER_UPPER_CASE}"

	ZE_PLATFORM="${ZEBUILD_PLATFORM}"
	ZE_PLATFORM_CLASS="${ZEBUILD_PLATFORM_CLASS}"
	ZE_PLATFORM_COMPILER="${ZEBUILD_PLATFORM_COMPILER}"

	ZE_VERSION_MAJOR=${ZEBUILD_VERSION_MAJOR}
	ZE_VERSION_MINOR=${ZEBUILD_VERSION_MINOR}
	ZE_VERSION_INTERNAL=${ZEBUILD_VERSION_INTERNAL}
	ZE_VERSION_BUILD=${ZEBUILD_VERSION_BUILD})

	

## Static Checking & Testing Options
#########################################################################################
set(ZEBUILD_STATIC_CHECKS_ENABLE			FALSE	CACHE BOOL "Enable static checks.")
set(ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK	FALSE	CACHE BOOL "Enable cppcheck static checker.")
set(ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT		FALSE	CACHE BOOL "Enable pc-lint static checker.")
set(ZEBUILD_UNIT_TESTS_ENABLE				TRUE	CACHE BOOL "Enable unit tests.")
set(ZEBUILD_UNIT_TESTS_COVERAGE_ENABLE		TRUE	CACHE BOOL "Enables code coverage instrumentation.")
set(ZEBUILD_DEBUG_ENABLE					TRUE	CACHE BOOL "Enable debug extensions.")
set(ZEBUILD_DEBUG_HEAP_CHECK_ENABLE			FALSE	CACHE BOOL "Enable heap checking.")
set(ZEBUILD_DEBUG_BREAK_ON_ERROR			TRUE	CACHE BOOL "Break if an error occures.")
set(ZEBUILD_DEBUG_BREAK_ON_WARNING			FALSE	CACHE BOOL "Break if an warning occures.")
set(ZEBUILD_VERBOSE							FALSE	CACHE BOOL "Enable ZEBuild verbose mode.")

if (ZEBUILD_DEBUG_ENABLE)
	append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG ZE_DEBUG_ENABLE)
	
	if (ZEBUILD_DEBUG_BREAK_ON_ERROR)
		append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG ZE_DEBUG_BREAK_ON_ERROR)
	endif()

	if (ZEBUILD_DEBUG_BREAK_ON_WARNING)
		append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG _ZE_DEBUG_BREAK_ON_WARNING)
	endif()

	if (ZEBUILD_DEBUG_HEAP_CHECK_ENABLE)
		append_property(GLOBAL PROPERTY COMPILE_DEFINITIONS_DEBUG ZE_DEBUG_MEMORY_CHECKS_ENABLE)
	endif()
endif()

if (ZEBUILD_UNIT_TESTS_ENABLE)
	ENABLE_TESTING()
	include(test_cocoon)
endif()

if (ZEBUILD_STATIC_CHECKS_ENABLE)
	if (ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK)
		include(cppcheck.cmake)
	endif()

	if (ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT)
		include(pc-lint.cmake)
	endif()
endif()

## Symbol & Source Server Options
#########################################################################################
set(ZEBUILD_SYMBOL_SERVER_ENABLE BOOL		FALSE	CACHE BOOL "Upload symbols to symbol server.")
set(ZEBUILD_SYMBOL_SERVER_ADDRESS "\\\\Server\\Symbols" CACHE STRING "Symbol server address.")
set(ZEBUILD_SYMBOL_SERVER_SOURCE_INDEX BOOL	FALSE	CACHE BOOL "Index code in sybols with svn address.")


## DLL Options
#########################################################################################
set(ZEBUILD_MONOLITHIC FALSE CACHE BOOL "Compile Zinek as monolithic. No dll will be generated and external plugins will be disabled.")


## Directories
#########################################################################################
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/Source/ZEEngine
	${CMAKE_SOURCE_DIR}/Source/ZEFoundation
	${CMAKE_SOURCE_DIR}/Source/ZEToolSDK
	${CMAKE_SOURCE_DIR}/Source/ZETest
	${CMAKE_SOURCE_DIR}/Include/${ZEBUILD_PLATFORM}
	${CMAKE_SOURCE_DIR}/Include/${ZEBUILD_PLATFORM}/DirectX
	${CMAKE_SOURCE_DIR}/Include/${ZEBUILD_PLATFORM}/PhysX2/Cooking 
	${CMAKE_SOURCE_DIR}/Include/${ZEBUILD_PLATFORM}/PhysX2/Foundation 
	${CMAKE_SOURCE_DIR}/Include/${ZEBUILD_PLATFORM}/PhysX2/PhysXLoader
	${CMAKE_SOURCE_DIR}/Include/${ZEBUILD_PLATFORM}/PhysX2/Physics
	${CMAKE_SOURCE_DIR}/Include/${ZEBUILD_PLATFORM}/3dsMax/2010)
	
link_directories(
	${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}
	#${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}/Release
	#${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}/Release/vrpn

	${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}/$(ConfigurationName)
	${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}/$(ConfigurationName)/vrpn
	${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}/$(ConfigurationName)/Bullet
	${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}/DirectX
	${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}/PhysX2
	${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}/PhysX3
	${CMAKE_SOURCE_DIR}/Lib/${ZEBUILD_PLATFORM}/3dsMax/2010)

include(ze_qt)

##set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Rundir)

## Projects
#########################################################################################
add_subdirectory(CMake)
add_subdirectory(Source)

## Install & Packing
#########################################################################################
set(ZEBUILD_BUILD_INSTALLER		TRUE CACHE BOOL "Build Installer.")

if (ZEBUILD_BUILD_INSTALLER)
	install(DIRECTORY Dll/${ZEBUILD_PLATFORM}/ 
		DESTINATION Bin 
		CONFIGURATIONS Release
		FILES_MATCHING 
			PATTERN "*.dll" 
			PATTERN "Debug" EXCLUDE
			PATTERN "Release" EXCLUDE)

	install(DIRECTORY Dll/${ZEBUILD_PLATFORM}/Release/
		DESTINATION Bin
		CONFIGURATIONS Release)
			
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
	set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "Runtime Required to Run Zinek Engine base Projects")
	set(CPACK_COMPONENT_RUNTIME_GROUP "Zinek Engine")
	set(CPACK_COMPONENT_RUNTIME_REQUIRED 1)

	set(CPACK_COMPONENT_DEMOS_DISPLAY_NAME "Demos")
	set(CPACK_COMPONENT_DEMOS_DESCRIPTION "Demos that are developed by Zinek Engine")
	set(CPACK_COMPONENT_DEMOS_GROUP "Zinek Engine")

	set(CPACK_COMPONENT_ZEEDITOR_DISPLAY_NAME "ZEEditor")
	set(CPACK_COMPONENT_ZEEDITOR_DESCRIPTION "Zinek Engine Editor")
	set(CPACK_COMPONENT_ZEEDITOR_GROUP "Content Creation")
		
	set(CPACK_COMPONENT_ZE3DSMAX_EXPORTERS_DISPLAY_NAME "Autodesk 3ds Max Exporters")
	set(CPACK_COMPONENT_ZE3DSMAX_DESCRIPTION "3ds Max plugins for exporting ZEModel and ZEMap files.")
	set(CPACK_COMPONENT_ZE3DSMAX_GROUP "Content Creation")

	set(CPACK_COMPONENT_ZEMAYA_EXPORTERS_DISPLAY_NAME "Autodesk Maya Exporters")
	set(CPACK_COMPONENT_ZEMAYA_DESCRIPTION "Autodesk Maya Exporters plugins for exporting ZEModel and ZEMap files.")
	set(CPACK_COMPONENT_ZEMAYA_GROUP "Content Creation")

	set(CPACK_COMPONENT_ZESOFTIMAGE_EXPORTERS_DISPLAY_NAME "Autodesk Softimage XSI Exporters")
	set(CPACK_COMPONENT_ZESOFTIMAGE_DESCRIPTION "Autodesk Softimage XSI plugins for exporting ZEModel and ZEMap files.")
	set(CPACK_COMPONENT_ZESOFTIMAGE_GROUP "Content Creation")

	set(CPACK_COMPONENT_ZEBLENDER_EXPORTERS_DISPLAY_NAME "Blender Exporters")
	set(CPACK_COMPONENT_ZEBLENDER_DESCRIPTION "Blender plugins for exporting ZEModel and ZEMap files.")
	set(CPACK_COMPONENT_ZEBLENDER_GROUP "Content Creation")
	
	set(CPACK_COMPONENT_ZECONENTSAMPLES_EXPORTERS_DISPLAY_NAME "Sample and Base Contents")
	set(CPACK_COMPONENT_ZECONENTSAMPLES_DESCRIPTION "Sample and base conent files for creating and deriving new content.")
	set(CPACK_COMPONENT_ZECONENTSAMPLES_GROUP "Content Creation")
	
	set(CPACK_COMPONENT_ZESDK_DISPLAY_NAME "ZESDK")
	set(CPACK_COMPONENT_ZESDK_DESCRIPTION "Zinek Engine C++ Software Development Kit.")
	set(CPACK_COMPONENT_ZESDK_GROUP "C++ Development")

	set(CPACK_COMPONENT_ZESDKSAMPLES_DISPLAY_NAME "Samples")
	set(CPACK_COMPONENT_ZESDKSAMPLES_DESCRIPTION "Sample applications that are developed by ZESDK.")
	set(CPACK_COMPONENT_ZESDKSAMPLES_GROUP "C++ Development")

	set(CPACK_COMPONENT_ZESDKDOCUMENTS_DISPLAY_NAME "Documentation")
	set(CPACK_COMPONENT_ZESDKDOCUMENTS_DESCRIPTION "Developers Guide and referance documentations.")
	set(CPACK_COMPONENT_ZESDKDOCUMENTS_GROUP "C++ Development")

	include(CPack)
endif()
