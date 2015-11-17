#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_external.cmake
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

function(ze_external_scan_directory PARAM_VARIABLE PARAM_DIRECTORY PARAM_EXTENSION)
	file(GLOB VAR_FILE_LIST "${PARAM_DIRECTORY}/*.${PARAM_EXTENSION}")
	foreach(VAR_CURRENT_FILE ${VAR_FILE_LIST})
		set(VAR_FILE_LIST_FULL_PATH ${VAR_FILE_LIST_FULL_PATH} "${VAR_CURRENT_FILE}")
	endforeach()
	
	set(${PARAM_VARIABLE} ${VAR_FILE_LIST_FULL_PATH} PARENT_SCOPE)
endfunction()

function(ze_external_scan_directories PARAM_VARIABLE PARAM_DIRECTORY PARAM_EXTENSION)
	ze_external_scan_directory(VAR_TEMP "${PARAM_DIRECTORY}" ${PARAM_EXTENSION})
	set(${PARAM_VARIABLE} ${VAR_TEMP} PARENT_SCOPE)
	
	ze_external_scan_directory(VAR_TEMP "${PARAM_DIRECTORY}/Debug" ${PARAM_EXTENSION})
	set(${PARAM_VARIABLE}_DEBUG ${VAR_TEMP} PARENT_SCOPE)
	
	ze_external_scan_directory(VAR_TEMP "${PARAM_DIRECTORY}/Release" ${PARAM_EXTENSION})
	set(${PARAM_VARIABLE}_RELEASE ${VAR_TEMP} PARENT_SCOPE)
endfunction()

function(ze_external_scan_files PARAM_VARIABLE PARAM_DIRECTORY PARAM_EXTENSION)
	set(PARAM_FILES ${ARGV})
	
	list(REMOVE_AT PARAM_FILES 0 1 2)

	foreach(VAR_CURRENT_FILE ${PARAM_FILES})
		set(VAR_CURRENT_FILE_GENERAL "${PARAM_DIRECTORY}/${VAR_CURRENT_FILE}.${PARAM_EXTENSION}")
		if (EXISTS ${VAR_CURRENT_FILE_GENERAL})
			set(VAR_GENERAL ${VAR_GENERAL} ${VAR_CURRENT_FILE_GENERAL})
		else()
			set(VAR_CURRENT_FILE_DEBUG "${PARAM_DIRECTORY}/Debug/${VAR_CURRENT_FILE}.${PARAM_EXTENSION}")
			if (EXISTS ${VAR_CURRENT_FILE_DEBUG})
				set(VAR_DEBUG ${VAR_DEBUG} ${VAR_CURRENT_FILE_DEBUG})
			endif()
			
			set(VAR_CURRENT_FILE_RELEASE "${PARAM_DIRECTORY}/Release/${VAR_CURRENT_FILE}.${PARAM_EXTENSION}")
			if (EXISTS ${VAR_CURRENT_FILE_RELEASE})
				set(VAR_RELEASE ${VAR_RELEASE} ${VAR_CURRENT_FILE_RELEASE})
			endif()
		endif()
	endforeach()

	set(${PARAM_VARIABLE} ${VAR_GENERAL} PARENT_SCOPE)
	set(${PARAM_VARIABLE}_DEBUG ${VAR_DEBUG} PARENT_SCOPE)
	set(${PARAM_VARIABLE}_RELEASE ${VAR_RELEASE} PARENT_SCOPE)

endfunction()

function (ze_external_register_library)
	parse_arguments(PARAM "LIBS;DLLS;SYSTEM_LIBS;${ze_check_parameters}" "NON_COMBINABLE" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	# Get Target Name
	list(GET PARAM_DEFAULT_ARGS 0 PARAM_NAME)
	
	set(VAR_LIB_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Lib")
	set(VAR_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Include")
	set(VAR_DLL_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/DLL")

	if (NOT PARAM_LIBS)
		ze_external_scan_directories(VAR_LIBS "${VAR_LIB_DIRECTORY}" ${ZEBUILD_PLATFORM_LIB_EXTENSION})
	else()
		ze_external_scan_files(VAR_LIBS "${VAR_LIB_DIRECTORY}" ${ZEBUILD_PLATFORM_LIB_EXTENSION} ${PARAM_LIBS})
	endif()
	
	if (NOT PARAM_DLLS)
		ze_external_scan_directories(VAR_DLLS "${VAR_DLL_DIRECTORY}" ${ZEBUILD_PLATFORM_DLL_EXTENSION})
	else()
		ze_external_scan_files(VAR_DLLS "${VAR_DLL_DIRECTORY}" ${ZEBUILD_PLATFORM_DLL_EXTENSION} ${PARAM_DLLS})
	endif()
	
	add_custom_target(${PARAM_NAME})
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_TYPE "EXTERNAL")
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_ROOT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_LIB_DIRECTORY ${VAR_LIB_DIRECTORY})
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_INCLUDE_DIRECTORY ${VAR_INCLUDE_DIRECTORY})
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_DLL_DIRECTORY ${VAR_DLL_DIRECTORY})
	
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_LIBS ${VAR_LIBS} ${PARAM_SYSTEM_LIBS})
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_LIBS_DEBUG ${VAR_LIBS_DEBUG})
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_LIBS_RELEASE ${VAR_LIBS_RELEASE})
	
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_DLLS ${VAR_DLLS})
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_DLLS_DEBUG ${VAR_DLLS_DEBUG})
	set_property(TARGET ${PARAM_NAME} PROPERTY ZEBUILD_DLLS_RELEASE ${VAR_DLLS_RELEASE})
	
	set_property(TARGET ${PARAM_NAME} PROPERTY FOLDER "External")
	
	if (NON_COMBINABLE)
		set_property(TARGET ${PARAM_TARGET} PROPERTY ZEBUILD_COMBINABLE FALSE)
	else()
		set_property(TARGET ${PARAM_TARGET} PROPERTY ZEBUILD_COMBINABLE TRUE)
	endif()
	
	message(STATUS "[ZEBuild] External library \"${PARAM_NAME}\" has been registered.")
endfunction()

macro (ze_external_init)
	message(STATUS "[ZEBuild] Checking Externals...")

	set(ZEBUILD_EXTERNALS_PATH "" CACHE PATH "Path to a folder which contains external libraries.")
	message(STATUS "[ZEBuild] External Folder             :  \"${ZEBUILD_EXTERNALS_PATH}\"")
	
	if (NOT IS_DIRECTORY "${ZEBUILD_EXTERNALS_PATH}")
		message(WARNING "[ZEBuild] External folder \"${ZEBUILD_EXTERNALS_PATH}\" is missing.")
	else()
		ze_version_get(${CMAKE_SOURCE_DIR})
		ze_version_get_revision_number(${CMAKE_SOURCE_DIR})
	
		message(STATUS "[ZEBuild] External Version             : " "v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_INTERNAL} Build ${VERSION_BUILD}")
		message(STATUS "[ZEBuild] External Major Version       : " ${VERSION_MAJOR})
		message(STATUS "[ZEBuild] External Minor Version       : " ${VERSION_MINOR})
		message(STATUS "[ZEBuild] External Internal Version    : " ${VERSION_INTERNAL})
		message(STATUS "[ZEBuild] External Build               : " ${VERSION_BUILD})
	
		set(ZEBUILD_EXTERNALS_DISABLE_VERSION_CHECK FALSE CACHE BOOL "Disables external version check.")
		if (NOT ZEBUILD_EXTERNALS_DISABLE_VERSION_CHECK)
			if (NOT (VERSION_MAJOR EQUAL ZEBUILD_VERSION_MAJOR))
				message(FATAL_ERROR "[ZEBuild] Error : External major version mismatch.")
			endif()
			
			if (NOT (VERSION_MINOR EQUAL ZEBUILD_VERSION_MINOR))
				message(WARNING "[ZEBuild] Error : External minor version mismatch.")
			endif()
		endif()

		message(STATUS "[ZEBuild] Externals has been checked.")
		message(STATUS "")
		message(STATUS "")

		message(STATUS "[ZEBuild] Initializing Externals... Path: ${ZEBUILD_EXTERNALS_PATH}")
		file(GLOB VAR_DIRECTORIES "${ZEBUILD_EXTERNALS_PATH}/*")
		foreach(VAR_DIRECTORY ${VAR_DIRECTORIES})
			if ((IS_DIRECTORY "${VAR_DIRECTORY}") AND (EXISTS "${VAR_DIRECTORY}/CMakeLists.txt"))
				get_filename_component(VAR_DIRECTORY_NAME "${VAR_DIRECTORY}" NAME)
				add_subdirectory(${VAR_DIRECTORY} "External/${VAR_DIRECTORY_NAME}")
			endif()		
		endforeach()
		
		message(STATUS "[ZEBuild] Externals initialized.")
		message(STATUS "")
		message(STATUS "")
	endif()

	set (ZEBUILD_EXTERNALS_SOURCE_DIR_PATH "${CMAKE_SOURCE_DIR}/External/${ZEBUILD_PLATFORM_NAME}")
	if (IS_DIRECTORY "${ZEBUILD_EXTERNALS_SOURCE_DIR_PATH}")
		message(STATUS "[ZEBuild] Initializing Source Directory Externals... Path: ${ZEBUILD_EXTERNALS_SOURCE_DIR_PATH}")
		file(GLOB VAR_DIRECTORIES "${ZEBUILD_EXTERNALS_SOURCE_DIR_PATH}/*")
		foreach(VAR_DIRECTORY ${VAR_DIRECTORIES})
			if ((IS_DIRECTORY "${VAR_DIRECTORY}") AND (EXISTS "${VAR_DIRECTORY}/CMakeLists.txt"))
				get_filename_component(VAR_DIRECTORY_NAME "${VAR_DIRECTORY}" NAME)
				add_subdirectory(${VAR_DIRECTORY} "External/${VAR_DIRECTORY_NAME}")
			endif()		
		endforeach()

		message(STATUS "[ZEBuild] Source Directory Externals initialized.")
		message(STATUS "")
		message(STATUS "")
	endif()
endmacro()
