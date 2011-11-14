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

set(ZE_ZINEK_VERSION_MAJOR 0)
set(ZE_ZINEK_VERSION_MINOR 5)
set(ZE_ZINEK_VERSION_INTERNAL 0)
set(ZE_ZINEK_VERSION_BUILD 5)

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
	set(ZE_BUILD_PLATFORM_CLASS "Windows")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZE_BUILD_PLATFORM "Win32")
	else()
		set(ZE_BUILD_PLATFORM "Win64")
	endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(ZE_BUILD_PLATFORM_CLASS "MacOSX")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZE_BUILD_PLATFORM "MacOSX32")
	else()
		set(ZE_BUILD_PLATFORM "MacOSX64")
	endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(ZE_BUILD_PLATFORM_CLASS "Linux")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZE_BUILD_PLATFORM "Linux32")
	else()
		set(ZE_BUILD_PLATFORM "Linux64")
	endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Unix")
	set(ZE_BUILD_PLATFORM_CLASS "Unix")
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(ZE_BUILD_PLATFORM "Unix32")
	else()
		set(ZE_BUILD_PLATFORM "Unix64")
	endif()
else()
	message(SEND_ERROR "[ZEBuild] Platform is not supported. Operating System : ${CMAKE_SYSTEM_NAME}, Achitecture : ${CMAKE_SYSTEM_PROCESSOR}, Compiler : ${CMAKE_CXX_COMPILER}")
endif()

set(ZE_BUILD_COMPILER ${CMAKE_CXX_COMPILER})
set(ZE_BUILD_LINKER ${CMAKE_LINKER})
set(ZE_BUILD_LIBRARIAN ${CMAKE_LINKER})
set(ZE_BUILD_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

message(STATUS "[ZEBuild] Major Version    : " ${ZE_ZINEK_VERSION_MAJOR})
message(STATUS "[ZEBuild] Minor Version    : " ${ZE_ZINEK_VERSION_MINOR})
message(STATUS "[ZEBuild] Internal Version : " ${ZE_ZINEK_VERSION_INTERNAL})
message(STATUS "[ZEBuild] Build            : " ${ZE_ZINEK_VERSION_BUILD})
message(STATUS "[ZEBuild] Platform Class   : " ${ZE_BUILD_PLATFORM_CLASS})
message(STATUS "[ZEBuild] Platform         : " ${ZE_BUILD_PLATFORM})
message(STATUS "[ZEBuild] Architecture     : " ${ZE_BUILD_ARCHITECTURE})
message(STATUS "[ZEBuild] Compiler         : " ${ZE_BUILD_COMPILER})
message(STATUS "[ZEBuild] Linker           : " ${ZE_BUILD_LINKER})
message(STATUS "[ZEBuild] Librarian        : " ${ZE_BUILD_LIBRARIAN})
message(STATUS "[ZEBuild] Platform detected.")


## Definitions
#########################################################################################
string(TOUPPER ${ZE_BUILD_PLATFORM_CLASS} ZE_BUILD_PLATFORM_CLASS_UPPER_CASE)
string(TOUPPER ${ZE_BUILD_PLATFORM} ZE_BUILD_PLATFORM_UPPER_CASE)

append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS 
	ZE_ZINEK
	ZE_ZINEK_VERSION_PLATFORM=${ZE_BUILD_PLATFORM}
	"ZE_ZINEK_VERSION_PLATFORM_${ZE_BUILD_PLATFORM_CLASS_UPPER_CASE}"
	ZE_ZINEK_VERSION_PLATFORM_CLASS=${ZE_BUILD_PLATFORM_CLASS}
	"ZE_ZINEK_VERSION_PLATFORM_${ZE_BUILD_PLATFORM_UPPER_CASE}"
	ZE_ZINEK_VERSION_MAJOR=${ZE_ZINEK_VERSION_MAJOR}
	ZE_ZINEK_VERSION_MINOR=${ZE_ZINEK_VERSION_MINOR}
	ZE_ZINEK_VERSION_INTERNAL=${ZE_ZINEK_VERSION_INTERNAL}
	ZE_ZINEK_VERSION_BUILD=${ZE_ZINEK_VERSION_BUILD}
	ZE_ZINEK_VERSION_PLATFORM)

	

## Static Checking & Testing Options
#########################################################################################
set(ZEBUILD_STATIC_CHECKS_ENABLE			FALSE	CACHE BOOL "Enable static checks.")
set(ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK	FALSE	CACHE BOOL "Enable cppcheck static checker.")
set(ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT		FALSE	CACHE BOOL "Enable pc-lint static checker.")
set(ZEBUILD_UNIT_TESTS_ENABLE				TRUE	CACHE BOOL "Enable unit tests.")
set(ZEBUILD_UNIT_TESTS_COVERAGE_ENABLE		TRUE	CACHE BOOL "Enables code coverage instrumentation.")
set(ZEBUILD_DEBUG_MODE						FALSE	CACHE BOOL "Enable ZEBuild debug mode.")
set(ZEBUILD_DEBUG_HEAP_CHECK_ENABLE			FALSE	CACHE BOOL "Enable heap checking.")
set(ZEBUILD_DEBUG_BREAK_ON_ERRORS			TRUE	CACHE BOOL "Break if an error occures.")
set(ZEBUILD_DEBUG_BREAK_ON_WARNINGS			FALSE	CACHE BOOL "Break if an warning occures.")

if (ZEBUILD_DEBUG_MODE)
	append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG ZE_DEBUG_ENABLED)
	
	if (ZEBUILD_DEBUG_HEAP_CHECK_ENABLE)
		append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG ZE_DEBUG_BREAK_ON_ERROR)
	endif()

	if (ZEBUILD_DEBUG_BREAK_ON_ERRORS)
		append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG _ZE_DEBUG_BREAK_ON_WARNING)
	endif()

	if (ZEBUILD_DEBUG_BREAK_ON_WARNINGS)
		append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG ZE_DEBUG_MEMORY_CHECKS_ENABLE)
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
	${CMAKE_SOURCE_DIR}/Source
	${CMAKE_SOURCE_DIR}/ZEFoundation
	${CMAKE_SOURCE_DIR}/ZEToolSDK
	${CMAKE_SOURCE_DIR}/ZETest
	${CMAKE_SOURCE_DIR}/Include/${ZE_BUILD_PLATFORM}
	${CMAKE_SOURCE_DIR}/Include/${ZE_BUILD_PLATFORM}/DirectX
	${CMAKE_SOURCE_DIR}/Include/${ZE_BUILD_PLATFORM}/PhysX2/Cooking 
	${CMAKE_SOURCE_DIR}/Include/${ZE_BUILD_PLATFORM}/PhysX2/Foundation 
	${CMAKE_SOURCE_DIR}/Include/${ZE_BUILD_PLATFORM}/PhysX2/PhysXLoader
	${CMAKE_SOURCE_DIR}/Include/${ZE_BUILD_PLATFORM}/PhysX2/Physics
	${CMAKE_SOURCE_DIR}/Include/${ZE_BUILD_PLATFORM}/3dsMax/2010)
	
link_directories(
	${CMAKE_SOURCE_DIR}/Lib/${ZE_BUILD_PLATFORM}
	#${CMAKE_SOURCE_DIR}/Lib/${ZE_BUILD_PLATFORM}/Release
	${CMAKE_SOURCE_DIR}/Lib/${ZE_BUILD_PLATFORM}/$(ConfigurationName)
	${CMAKE_SOURCE_DIR}/Lib/${ZE_BUILD_PLATFORM}/$(ConfigurationName)/vrpn
	${CMAKE_SOURCE_DIR}/Lib/${ZE_BUILD_PLATFORM}/$(ConfigurationName)/Bullet
	${CMAKE_SOURCE_DIR}/Lib/${ZE_BUILD_PLATFORM}/DirectX
	${CMAKE_SOURCE_DIR}/Lib/${ZE_BUILD_PLATFORM}/PhysX2
	${CMAKE_SOURCE_DIR}/Lib/${ZE_BUILD_PLATFORM}/PhysX3
	${CMAKE_SOURCE_DIR}/Lib/${ZE_BUILD_PLATFORM}/3dsMax/2010)


## Projects
#########################################################################################
add_subdirectory(CMake)

ze_add_conditional_build(Source				TRUE TITLE "Build ZEEngineAPI.")
ze_add_conditional_build(ZEditor			TRUE TITLE "Build ZEditor.")
ze_add_conditional_build(ZE3dsMax			TRUE TITLE "Build ZE3dsMax Exporters.")
ze_add_conditional_build(ZEFoundation		TRUE TITLE "Build ZEFoundationAPI.")
ze_add_conditional_build(ZEToolSDK			TRUE TITLE "Build ZEToolAPI.")
ze_add_conditional_build(Applications		TRUE TITLE "Build Applications.")
ze_add_conditional_build(Demos				FALSE TITLE "Build Demos.")
ze_add_conditional_build(Samples			FALSE TITLE "Build Samples.")
add_subdirectory(ZEAIPrototype)
if (ZEBUILD_UNIT_TESTS_ENABLE)
	add_subdirectory (ZETest)
endif()

## Install & Packing
#########################################################################################
set(ZEBUILD_BUILD_INSTALLER		TRUE CACHE BOOL "Build Installer.")

if (ZEBUILD_BUILD_INSTALLER)
	install(DIRECTORY Dll/${ZE_BUILD_PLATFORM}/ 
		DESTINATION Bin 
		CONFIGURATIONS Release
		FILES_MATCHING 
			PATTERN "*.dll" 
			PATTERN "Debug" EXCLUDE
			PATTERN "Release" EXCLUDE)

	install(DIRECTORY Dll/${ZE_BUILD_PLATFORM}/Release/
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
