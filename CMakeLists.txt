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

cmake_minimum_required (VERSION 2.8)

project (ZinekEngine) 

## Configuration
#########################################################################################
set(CMAKE_MODULE_PATH ${ZinekEngine_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})
 
include(MacroOutOfSourceBuild)
include(ParseArguments)
include(AddSource)

macro_ensure_out_of_source_build ("${PROJECT_NAME} requires an out of source build.")

SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")

add_definitions(-DZE_ZINEK_ENGINE)

#	add_definition (-DZE_DEBUG_ENABLED)
#	add_definition (-DZE_DEBUG_BREAK_ON_ERROR)
#	add_definition (-DZE_DEBUG_BREAK_ON_WARNING)
#	add_definition (-DZE_DEBUG_ENABLE_MEMORY_CHECKS)


set (ZESDK_INCLUDES)
set (ZEToolSDK_INCLUDES)
set (ZEGameSDK_INCLUDES)
set (ZEModuleSDK_INCLUDES)

set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

include_directories (Source Include)
link_directories("${PROJECT_SOURCE_DIR}/Lib/Win x86/Debug" "${PROJECT_SOURCE_DIR}/Lib/Win x86/DirectX" "${PROJECT_SOURCE_DIR}/Lib/Win x86/OpenAL" "${PROJECT_SOURCE_DIR}/Lib/Win x86/PhysX")

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS OFF)

source_group (ZPP REGULAR_EXPRESSION ".*\\.zpp")

## Files
#########################################################################################
add_subdirectory (Source)


## PACKING
#########################################################################################
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Zinek Engine")
SET(CPACK_PACKAGE_VENDOR "Zinek Code House and Game Studio")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/Installation/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/Installation/License.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "5")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Zinek Engine")
SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/Installation/Release\\\\InstallIcon.bmp")
SET(CPACK_NSIS_INSTALLED_ICON_NAME "ZinekEngine.exe")
SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Zinek Engine")
SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.zinekengine.com\\\\en\\\\docs\\\\install")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.zinekengine.com")
SET(CPACK_NSIS_CONTACT "contact@zinekengine.com")
SET(CPACK_NSIS_MODIFY_PATH ON)

SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")
INCLUDE(CPack)
