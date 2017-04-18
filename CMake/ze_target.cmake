#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_target.cmake
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

function(ze_set_project_folder)
	parse_arguments(PARAMETER "" "" ${ARGV})
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_PROJECT_FOLDER)

	set(ZEBUILD_PROJECT_FOLDER ${PARAMETER_PROJECT_FOLDER})
	set(ZEBUILD_PROJECT_FOLDER ${ZEBUILD_PROJECT_FOLDER} PARENT_SCOPE)
endfunction()

function(ze_link)
	parse_arguments(PARAMETER "TARGET;LIBS;${ze_check_parameters}" "" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	foreach(CURRENT_LIB ${PARAMETER_LIBS})
		if (TARGET ${CURRENT_LIB})
			get_property(CURRENT_LIB_TYPE TARGET ${CURRENT_LIB} PROPERTY ZEBUILD_TYPE)
			
			if (CURRENT_LIB_TYPE MATCHES "EXTERNAL")		
				get_property(CURRENT_LIB_DIRECTORY TARGET ${CURRENT_LIB} PROPERTY ZEBUILD_LIB_DIRECTORY)		
				get_property(CURRENT_LIB_INCLUDE_DIRECTORY TARGET ${CURRENT_LIB} PROPERTY ZEBUILD_INCLUDE_DIRECTORY)		
				get_property(CURRENT_LIBS TARGET ${CURRENT_LIB} PROPERTY ZEBUILD_LIBS)	
				get_property(CURRENT_LIBS_DEBUG TARGET ${CURRENT_LIB} PROPERTY ZEBUILD_LIBS_DEBUG)	
				get_property(CURRENT_LIBS_RELEASE TARGET ${CURRENT_LIB} PROPERTY ZEBUILD_LIBS_RELEASE)	

				set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES ${CURRENT_LIB_INCLUDE_DIRECTORY})

				if (CURRENT_LIBS)
					target_link_libraries(${PARAMETER_TARGET} ${CURRENT_LIBS})
				endif()
				
				foreach(LIBRARY ${CURRENT_LIBS_DEBUG})
					target_link_libraries(${PARAMETER_TARGET} debug ${LIBRARY})
				endforeach()
				
				foreach(LIBRARY ${CURRENT_LIBS_RELEASE})
					target_link_libraries(${PARAMETER_TARGET} optimized ${LIBRARY})
				endforeach()
			else()
				target_link_libraries(${PARAMETER_TARGET} ${CURRENT_LIB})
			endif()
		else()
			target_link_libraries(${PARAMETER_TARGET} ${CURRENT_LIB})
		endif()
		ze_add_dependency(TARGET ${PARAMETER_TARGET} DEPENDENCIES ${CURRENT_LIB})	
	endforeach()
endfunction()

function(ze_add_source)
	parse_arguments(PARAMETER "${ze_source_processors};${ze_check_parameters}" "" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_FILE)
	list(GET PARAMETER_DEFAULT_ARGS 1 PARAMETER_SOURCE_LIST)
	set(PARAMETER_EXTRA_SOURCE_LISTS ${PARAMETER_DEFAULT_ARGS})
	list(REMOVE_AT PARAMETER_EXTRA_SOURCE_LISTS 0 1)
	
	get_filename_component(PARAMETER_FILE ${PARAMETER_FILE} ABSOLUTE)
	
	# Add To Source List
	list(APPEND ${PARAMETER_SOURCE_LIST} "${PARAMETER_FILE}")
	source_group("" FILES ${PARAMETER_FILE})
	
	# Add File to Lists
	foreach (CURRENT_EXTRA_SOURCE_LIST ${PARAMETER_EXTRA_SOURCE_LISTS})
		list(APPEND ${CURRENT_EXTRA_SOURCE_LIST} ${PARAMETER_FILE})
		set(${CURRENT_EXTRA_SOURCE_LIST} ${${CURRENT_EXTRA_SOURCE_LIST}} PARENT_SCOPE)
	endforeach()

	set(${PARAMETER_SOURCE_LIST} ${${PARAMETER_SOURCE_LIST}} PARENT_SCOPE)
endfunction()

set(ze_process_source_parameters "ZEMC;ZEFC")
macro(ze_process_sources)
	if (PARAMETER_ZEMC)
		ze_meta_compiler(TARGET ${PARAMETER_TARGET} FILES ${PARAMETER_ZEMC})
	endif()
	
	if (PARAMETER_ZEFC)
		ze_file_compiler(TARGET ${PARAMETER_TARGET} FILES ${PARAMETER_ZEFC})
	endif()
endmacro()

function(ze_generate_export_h)
	parse_arguments(PARAMETER "TARGET;HEADERS" "" ${ARGV})

	string(TOUPPER "${PARAMETER_TARGET}" PARAMETER_TARGET_UPPER)

	configure_file(
		"${CMAKE_SOURCE_DIR}/CMake/ze_export.h.in"
		"${CMAKE_CURRENT_BINARY_DIR}/ZEExport.${PARAMETER_TARGET}.h"
		@ONLY)
	target_sources(${PARAMETER_TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/ZEExport.${PARAMETER_TARGET}.h")
	source_group("Generated" FILES "${CMAKE_CURRENT_BINARY_DIR}/ZEExport.${PARAMETER_TARGET}.h")
endfunction()

function(ze_add_executable)
	parse_arguments(PARAMETER "TARGET;SOURCES;LIBS;DESCRIPTION;DESTINATION;${ze_process_source_parameters};${ze_check_parameters}" "INSTALL;CONSOLE;DLL;BUNDLE" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()
	
	# Compile
	set(VARIABLE_EXECUTABLE_TYPE "")
	if (PARAMETER_CONSOLE)
		set(VARIABLE_EXECUTABLE_TYPE "")
	elseif (PARAMETER_BUNDLE)
		set(VARIABLE_EXECUTABLE_TYPE "MACOSX_BUNDLE")
	else()
		if (ZEBUILD_PLATFORM_WINDOWS)
			set(VARIABLE_EXECUTABLE_TYPE "WIN32")
		endif()
	endif()

	add_executable(${PARAMETER_TARGET} ${VARIABLE_EXECUTABLE_TYPE} ${PARAMETER_SOURCES} ${PARAMETER_HEADERS})

	ze_version_generate_version_info(${PARAMETER_TARGET} "exe" "${PARAMETER_DESCRIPTION}")
	#ze_generate_export_h(TARGET ${PARAMETER_TARGET})
	
	string(TOUPPER "${PARAMETER_TARGET}" PARAMETER_TARGET_UPPER)
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_BINARY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_TYPE EXECUTABLE)
	set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS "ZE_ENGINE" "ZE_TARGET_${PARAMETER_TARGET_UPPER}")
	set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY VERSION "${ZEBUILD_VERSION_MAJOR}.${ZEBUILD_VERSION_MINOR}")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Rundir")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Rundir")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/Rundir")
	
	ze_link(TARGET ${PARAMETER_TARGET} LIBS ${PARAMETER_LIBS})
	ze_copy_runtime_dlls(TARGET ${PARAMETER_TARGET})
	ze_static_analysis(${PARAMETER_TARGET})
	
	ze_process_sources()

	message(STATUS "[ZEBuild] Executable \"${PARAMETER_TARGET}\" project has been added.")
endfunction()

function(ze_add_plugin)
	parse_arguments(PARAMETER "TARGET;SOURCES;LIBS;HEADERS;${ze_process_source_parameters};${ze_check_parameters}" "INSTALL" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()
	
	add_library(${PARAMETER_TARGET} SHARED ${PARAMETER_SOURCES} ${PARAMETER_HEADERS})

	ze_version_generate_version_info(${PARAMETER_TARGET} "dll" "${PARAMETER_DESCRIPTION}")
	#ze_generate_export_h(TARGET ${PARAMETER_TARGET})
	
	string(TOUPPER "${PARAMETER_TARGET}" PARAMETER_TARGET_UPPER)
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_BINARY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY CLEAN_DIRECT_OUTPUT 1)
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER "${ZEBUILD_PROJECT_FOLDER}")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_TYPE PLUGIN)
	set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS "ZE_ENGINE" "ZE_${PARAMETER_TARGET_UPPER}" "ZE_${PARAMETER_TARGET_UPPER}_SHARED")
	set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Rundir")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Rundir")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/Rundir")
	
	ze_link(TARGET ${PARAMETER_TARGET} LIBS ${PARAMETER_LIBS})
	ze_copy_runtime_dlls(TARGET ${PARAMETER_TARGET})
	ze_static_analysis(${PARAMETER_TARGET})

	ze_process_sources()
	message(STATUS "[ZEBuild] Plugin \"${PARAMETER_TARGET}\" project has been added.")
endfunction()

function (ze_add_library)
	parse_arguments(PARAMETER "TARGET;SOURCES;LIBS;HEADERS;${ze_process_source_parameters};${ze_check_parameters}" "INSTALL;DLL;COMBINE" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	string(TOUPPER "${PARAMETER_TARGET}" PARAMETER_TARGET_UPPER)
	
	if (PARAMETER_DLL)
		add_library(${PARAMETER_TARGET} SHARED ${PARAMETER_SOURCES} ${PARAMETER_HEADERS})
		set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_TYPE "DLL")
		set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS "ZE_${PARAMETER_TARGET_UPPER}_SHARED")
		ze_version_generate_version_info(${PARAMETER_TARGET} "dll" "${PARAMETER_DESCRIPTION}")
		#ze_generate_export_h(TARGET ${PARAMETER_TARGET})
	else()
		add_library(${PARAMETER_TARGET} ${PARAMETER_SOURCES})
		set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_TYPE "LIB")
		set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS "ZE_${PARAMETER_TARGET_UPPER}_STATIC")
	endif()
	
	set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS "ZE_ENGINE" "ZE_${PARAMETER_TARGET_UPPER}")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_BINARY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_LIBS ${PARAMETER_LIBS})
	set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Rundir")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Rundir")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/Rundir")
	
	if (PARAMETER_ZEMC)
		set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_ZE_META_REGISTER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaRegister")
	endif()
	
	ze_link(TARGET ${PARAMETER_TARGET} LIBS ${PARAMETER_LIBS})

	ze_process_sources()
	#ze_generate_export_h(TARGET ${PARAMETER_TARGET})
	
	if (NOT PARAMETER_DLL)
		message(STATUS "[ZEBuild] Library \"${PARAMETER_TARGET}\" project has been added.")
	else()
		message(STATUS "[ZEBuild] DLL \"${PARAMETER_TARGET}\" project has been added.")
	endif()

endfunction()

function(ze_begin_components)
	parse_arguments(PARAMETER "TARGET" "" ${ARGV})

	set(${PARAMETER_TARGET}_SOURCES PARENT_SCOPE)
	set(${PARAMETER_TARGET}_HEADERS PARENT_SCOPE)
	set(${PARAMETER_TARGET}_LIBS PARENT_SCOPE)
	set(${PARAMETER_TARGET}_ZEMC PARENT_SCOPE)
	set(${PARAMETER_TARGET}_ZEFC PARENT_SCOPE)
endfunction()

function(ze_end_components)
	parse_arguments(PARAMETER "TARGET" "" ${ARGV})

	set(PARAMETER_ZEMC ${${PARAMETER_TARGET}_ZEMC})
	set(PARAMETER_ZEFC ${${PARAMETER_TARGET}_ZEFC})
	ze_process_sources()
	
	target_sources(${PARAMETER_TARGET} PRIVATE ${${PARAMETER_TARGET}_SOURCES} ${${PARAMETER_TARGET}_HEADERS})
	ze_link(TARGET ${PARAMETER_TARGET} LIBS ${${PARAMETER_TARGET}_LIBS})

	foreach(VARIABLE_FILE ${${PARAMETER_TARGET}_SOURCES} ${${PARAMETER_TARGET}_HEADERS})
		file(RELATIVE_PATH VARIABLE_RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${VARIABLE_FILE})
		get_filename_component(VARIABLE_PARENT_DIR ${VARIABLE_RELATIVE} DIRECTORY)
		source_group("${VARIABLE_PARENT_DIR}" FILES ${VARIABLE_FILE})
	endforeach()
endfunction()

macro(ze_add_component)
	parse_arguments(PARAMETER "TARGET;COMPONENT;SOURCES;HEADERS;LIBS;${ze_process_source_parameters};${ze_check_parameters}" "" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	set(PARAMETER_SOURCES ${PARAMETER_SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
	set(${PARAMETER_TARGET}_SOURCES ${${PARAMETER_TARGET}_SOURCES} ${PARAMETER_SOURCES} PARENT_SCOPE)
	set(${PARAMETER_TARGET}_HEADERS ${${PARAMETER_TARGET}_HEADERS} ${PARAMETER_HEADERS} PARENT_SCOPE)
	set(${PARAMETER_TARGET}_LIBS ${${PARAMETER_TARGET}_LIBS} ${PARAMETER_LIBS} PARENT_SCOPE)
	set(${PARAMETER_TARGET}_ZEMC ${${PARAMETER_TARGET}_ZEMC} ${PARAMETER_ZEMC} PARENT_SCOPE)
	set(${PARAMETER_TARGET}_ZEFC ${${PARAMETER_TARGET}_ZEFC} ${PARAMETER_ZEFC} PARENT_SCOPE)

	message(STATUS "[ZEBuild] Component \"${PARAMETER_COMPONENT}\" has been added to target \"${PARAMETER_TARGET}\".")
endmacro()

function(ze_add_module)
	set (PARAMETER_TITLE "")
	parse_arguments(PARAMETER "TITLE;${ze_check_parameters}" "DEFAULT;OPTIONAL" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()
	
	set (PARAMETER_NAME "ZEBUILD_BUILD")
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_DIRECTORY)
	
	string(TOUPPER ${PARAMETER_DIRECTORY} PARAMETER_DIRECTORY_UPPER)
	set(PARAMETER_NAME "ZEBUILD_MODULE_${PARAMETER_DIRECTORY_UPPER}")

	if (PARAMETER_OPTIONAL)
		set(OPTIONAL TRUE)
	else()
		set(OPTIONAL FALSE)
	endif()

	if (PARAMETER_DEFAULT)
		set(DEFAULT TRUE)
	else()
		set(DEFAULT FALSE)
	endif()
	
	if (PARAMETER_OPTIONAL)
		set(${PARAMETER_NAME} ${DEFAULT} CACHE BOOL "${PARAMETER_TITLE}")
		if (${PARAMETER_NAME})
			add_subdirectory(${PARAMETER_DIRECTORY})
			list(LENGTH PARAMETER_DEFAULT_ARGS PARAMETER_DEFAULT_ARGS_LENGTH)
			if (PARAMETER_DEFAULT_ARGS_LENGTH EQUAL 2)
	 			list(GET PARAMETER_DEFAULT_ARGS 1 PARAMETER_SET)
				list(APPEND ${PARAMETER_SET} ${PARAMETER_DIRECTORY})
				set(${PARAMETER_SET} ${${PARAMETER_SET}} PARENT_SCOPE)
			endif()
		endif()
	else()
		add_subdirectory(${PARAMETER_DIRECTORY})
		list(LENGTH PARAMETER_DEFAULT_ARGS PARAMETER_DEFAULT_ARGS_LENGTH)
		if (PARAMETER_DEFAULT_ARGS_LENGTH EQUAL 2)
	 		list(GET PARAMETER_DEFAULT_ARGS 1 PARAMETER_SET)
			list(APPEND ${PARAMETER_SET} ${PARAMETER_DIRECTORY})
			set(${PARAMETER_SET} ${${PARAMETER_SET}} PARENT_SCOPE)
		endif()
	endif()
	
	message(STATUS "[ZEBuild] Module \"${PARAMETER_DIRECTORY}\" has been added.")
endfunction()

function(ze_add_cmake_project)
	parse_arguments(PARAMETER "SOURCES" "" ${ARGV})
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_NAME)

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	add_custom_target(${PARAMETER_NAME}	SOURCES ${PARAMETER_SOURCES})
	set_property(TARGET ${PARAMETER_NAME} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
	message(STATUS "[ZEBuild] CMake \"${PARAMETER_NAME}\" project has been added.")
endfunction()
