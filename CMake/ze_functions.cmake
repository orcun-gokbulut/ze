#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_functions.cmake
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

macro(ze_append_property SCOPE PROPERTY_IDEN PROPERTY_NAME)
	get_property(OLD_VALUES ${SCOPE} PROPERTY ${PROPERTY_NAME})
	set_property(${SCOPE} PROPERTY ${PROPERTY_NAME} ${OLD_VALUES} ${ARGN})
endmacro()

function(ze_set_project_folder)
	parse_arguments(PARAMETER "" "" ${ARGV})
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_PROJECT_FOLDER)

	set(ZEBUILD_PROJECT_FOLDER ${PARAMETER_PROJECT_FOLDER})
	set(ZEBUILD_PROJECT_FOLDER ${ZEBUILD_PROJECT_FOLDER} PARENT_SCOPE)
endfunction()

function (ze_copy_files)
	parse_arguments(PARAMETER "TARGET;BASE;DESTINATION" " " ${ARGV})

	foreach(PARAMETER_FILE ${PARAMETER_DEFAULT_ARGS})
		string(REPLACE ${PARAMETER_BASE} "\0" MODIFIED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE})
		add_custom_command(
			OUTPUT "${PARAMETER_DESTINATION}${MODIFIED_PATH}"
			COMMAND ${CMAKE_COMMAND} 
			ARGS "-E copy \"${PARAMETER_FILE}\" \"${PARAMETER_DESTINATION}${MODIFIED_PATH}\""
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			MAIN_DEPENDENCY ${PARAMETER_TARGET}
			DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE})
	endforeach()
endfunction()

function (ze_set_property_all_config)
	parse_arguments(PARAMETER "TARGET;PROPERTY" "" ${ARGV})

	list(GET PARAMETER_PROPERTY 0 PARAMETER_PROPERTY_NAME)
	list(REMOVE_AT PARAMETER_PROPERTY 0)
	
	if (CMAKE_CONFIGURATION_TYPES)
		foreach(CONFIGURATION ${CMAKE_CONFIGURATION_TYPES})
			string(TOUPPER ${CONFIGURATION} CONFIGURATION_UPPER) 
			set_property(TARGET ${PARAMETER_TARGET} PROPERTY ${PARAMETER_PROPERTY_NAME}_${CONFIGURATION_UPPER} ${PARAMETER_PROPERTY})
		endforeach()
	else()
		set_property(TARGET ${PARAMETER_TARGET} PROPERTY ${PARAMETER_PROPERTY_NAME} ${PARAMETER_PROPERTY})
	endif()
endfunction()

set(ze_check_parameters "PLATFORMS;COMPILERS;ARCHS")
macro(ze_check)
	set(CHECK_SUCCEEDED FALSE)
	if (PARAMETER_PLATFORMS)
		set(CHECK_SUCCEEDED FALSE)
		foreach(ARG ${PARAMETER_PLATFORMS})
			string(TOUPPER ${ARG} TEMP_UPPER)
			if (ZEBUILD_PLATFORM_${TEMP_UPPER})
				set(CHECK_SUCCEEDED TRUE)
				break()
			endif()
		endforeach()
	else()
		set(CHECK_SUCCEEDED TRUE)
	endif()

	if (CHECK_SUCCEEDED)
		if (PARAMETER_ARCHS)
			string(TOUPPER ${ZEBUILD_PLATFORM_ARCHITECTURE} TEMP_UPPER)
			set(CHECK_SUCCEEDED FALSE)
			foreach(ARG ${PARAMETER_ARCHS})
				string(TOUPPER ${ARG} ARG_UPPER)
				if (${ARG_UPPER} MATCHES ${TEMP_UPPER})
					set(CHECK_SUCCEEDED TRUE)
					break()
				endif()
			endforeach()
		else()
			set(CHECK_SUCCEEDED TRUE)
		endif()
	endif()

	if (CHECK_SUCCEEDED)
		if (PARAMETER_COMPILERS)
			string(TOUPPER ${ZEBUILD_PLATFORM_COMPILERS} TEMP_UPPER)
			set(CHECK_SUCCEEDED FALSE)
			foreach(ARG ${PARAMETER_COMPILERS})
				string(TOUPPER ${ARG} ARG_UPPER)
				if (${ARG_UPPER} MATCHES ${TEMP_UPPER})
					set(CHECK_SUCCEEDED TRUE)
					break()
				endif()
			endforeach()
		else()
			set(CHECK_SUCCEEDED TRUE)
		endif()
	endif()

	if (CHECK_SUCCEEDED)
		if (PLATFORM_FLAGS)
			string(TOUPPER ${ZEBUILD_PLATFORM_ARCHITECTURE} TEMP_UPPER)
			set(CHECK_SUCCEEDED FALSE)
			foreach(ARG ${PARAMETER_ARCHS})
				string(TOUPPER ${ARG} ARG_UPPER)
				if (${ARG_UPPER} MATCHES ${TEMP_UPPER})
					set(CHECK_SUCCEEDED TRUE)
					break()
				endif()
			endforeach()
		else()
			set(CHECK_SUCCEEDED TRUE)
		endif()
	endif()
endmacro()

function(ze_file_compiler)
	parse_arguments(PARAMETER "CLASS;OUTPUT" "" ${ARGV})
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_INPUT)

	get_property(ZEFC_BINARY_PATH TARGET ZEFC PROPERTY RUNTIME_OUTPUT_DIRECTORY)
	add_custom_command(
		COMMAND $<TARGET_FILE:ZEFC>
		ARGS "\"${PARAMETER_INPUT}\" -C \"${PARAMETER_CLASS}\" -S \"${PARAMETER_OUTPUT}.cpp\" -H \"${PARAMETER_OUTPUT}.h\""
		DEPENDS ${PARAMETER_INPUT} ZEFC
		OUTPUT "${PARAMETER_OUTPUT}.cpp" "${PARAMETER_OUTPUT}.h")
		
	if (NOT EXISTS "${PARAMETER_OUTPUT}.h")
		 file(WRITE "${PARAMETER_OUTPUT}.h"
			"#pragma once\n\n"
			"#include \"ZEData.h\"\n"
			"#include \"ZETypes.h\"\n"
			"\n"
			"class ${PARAMETER_CLASS} : public ZEData\n"
			"{\n"
			"\tpublic:\n"
				"\t\tvoid*\t\tGetData();\n"
				"\t\tZESize\t\tGetSize();\n"
			"};\n")			
	endif()
endfunction()

function(ze_meta_compiler)
	parse_arguments(PARAMETER "OUTPUT" "" ${ARGV})
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_INPUT)

	add_custom_command(
		COMMAND ${ZEBUILD_TOOL_PATH}\\zepp.exe
		ARGS "\"${PARAMETER_INPUT}\" \"${PARAMETER_OUTPUT}\""
		DEPENDS "${PARAMETER_INPUT}"
		OUTPUT ${PARAMETER_OUTPUT})
endfunction()

function(ze_add_source)
	parse_arguments(PARAMETER "${ze_check_parameters}" "ZEFC;ZEPP;ZEDOC" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_FILE)
	list(GET PARAMETER_DEFAULT_ARGS 1 PARAMETER_SOURCE_LIST)
	set(PARAMETER_SOURCE_LISTS ${PARAMETER_DEFAULT_ARGS})
	list(REMOVE_AT PARAMETER_SOURCE_LISTS 0 1)
	
	# Add To Source List
	list(APPEND ${PARAMETER_SOURCE_LIST} ${PARAMETER_FILE})
	source_group("" FILES ${PARAMETER_FILE})

	# Process File with ZEPP
	if (PARAMETER_ZEPP)
		ze_meta_compiler("${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE}"
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_FILE}.ZEMeta.cpp)
			
		source_group("Generated" FILES ${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_FILE}.ZEMeta.cpp)
		set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_FILE}.ZEMeta.cpp PROPERTY LANGUAGE CXX)
		list(APPEND ${PARAMETER_SOURCE_LIST} ${PARAMETER_FILE}.ZEMeta.cpp)
	endif()
	
	# Process File With ZEFC
	if (PARAMETER_ZEFC)
		string(REPLACE  "." "_" PARAMETER_CLASS ${PARAMETER_FILE} )
		ze_file_compiler(${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE} 
			CLASS ${PARAMETER_CLASS} 
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_CLASS})
	
		list(APPEND ${PARAMETER_SOURCE_LIST} ${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_CLASS}.h)
		list(APPEND ${PARAMETER_SOURCE_LIST} ${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_CLASS}.cpp)
		source_group("Generated" FILES ${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_CLASS}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_CLASS}.h)
	endif()
	
	# Process File With ZEDOC
	if (PARAMETER_ZEDOC)
	endif()
	
	# Add File to Lists
	foreach (SOURCE_LIST ${PARAMETER_SOURCE_LISTS})
		list(APPEND ${SOURCE_LIST} ${PARAMETER_FILE})
		set(${SOURCE_LIST} ${${SOURCE_LIST}} PARENT_SCOPE)
	endforeach()
	
	set(${PARAMETER_SOURCE_LIST} ${${PARAMETER_SOURCE_LIST}} PARENT_SCOPE)
endfunction()

function(ze_add_executable)
	parse_arguments(PARAMETER "SOURCES;LIBS;DESTINATION;${ze_check_parameters}" "INSTALL;PLUGIN;CONSOLE" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
	include_directories(
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR})

	# Compile
	if (PARAMETER_CONSOLE)
		add_executable(${PARAMETER_TARGET} ${PARAMETER_SOURCES})
	else()
		if (ZEBUILD_PLATFORM_WINDOWS)
			add_executable(${PARAMETER_TARGET} WIN32 ${PARAMETER_SOURCES})
		elseif (ZEBUILD_PLATFORM_MACOSX OR ZEBUILD_PLATFORM_IOS)
			add_executable(${PARAMETER_TARGET} MACOSX_BUNDLE ${PARAMETER_SOURCES})
		else()
			add_executable(${PARAMETER_TARGET} ${PARAMETER_SOURCES})
		endif()
	endif()
	
	target_link_libraries(${PARAMETER_TARGET} ${PARAMETER_LIBS})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_LIBS ${PARAMETER_LIBS})

	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_ENGINE)

	# Static Analysis
	if (ZEBUILD_STATIC_CHECKS_ENABLE)
		if (ZEBUILD_ENABLE_STATIC_CHECKS)
			if (ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK)
				add_cppcheck(${PARAMETER_TARGET})
			endif()
			
			if (ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT)
				add_pc_lint(${PARAMETER_TARGET})
			endif()
		endif()
	endif()
	
	# Copy dependecies
	if (NOT PARAMETER_PLUGIN)
        if (ZEBUILD_PLATFORM_WINDOWS)
            FILE(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/Platform/${ZEBUILD_PLATFORM}/${ZEBUILD_PLATFORM_ARCHITECTURE}/Dll/*.dll" A)
            FILE(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/Platform/${ZEBUILD_PLATFORM}/${ZEBUILD_PLATFORM_ARCHITECTURE}/Dll/$(ConfigurationName)/*.dll" B)
            FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/$(ConfigurationName)" C)
            add_custom_command(
                TARGET ${PARAMETER_TARGET}
                COMMAND "xcopy"
                ARGS "${A}" "${C}" /D /Y)
            add_custom_command(
                TARGET ${PARAMETER_TARGET}
                COMMAND "xcopy"
                ARGS "${B}" "${C}" /D /Y)
        endif()
	endif()
	
	if (PARAMETER_INSTALL)
		if (NOT PARAMETER_DESTINATION)
			set(PARAMETER_DESTINATION "Engine")
		endif()
		
		ze_set_property_all_config(TARGET ${PARAMETER_TARGET} PROPERTY PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Output/Symbol)
		ze_set_property_all_config(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Output/${PARAMETER_DESTINATION})
	endif()
endfunction()

function(ze_add_plugin)
	parse_arguments(PARAMETER "SOURCES;LIBS;${ze_check_parameters}" "INSTALL;DLL" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
	include_directories(
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR})
	
	# Compile
	add_library(${PARAMETER_TARGET} SHARED ${PARAMETER_SOURCES})
  
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY SUFFIX ".zePlugin")
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY CLEAN_DIRECT_OUTPUT 1)

	target_link_libraries(${PARAMETER_TARGET} ${PARAMETER_LIBS})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_LIBS ${PARAMETER_LIBS})
	
	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLUGIN)
	
	# Static Analysis
	if (ZEBUILD_STATIC_CHECKS_ENABLE)
		if (ZEBUILD_ENABLE_STATIC_CHECKS)
			if (ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK)
				add_cppcheck(${PARAMETER_TARGET})
			endif()
			
			if (ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT)
				add_pc_lint(${PARAMETER_TARGET})
			endif()
		endif()
	endif()
		
	# Install
	if (PARAMETER_INSTALL)
		ze_set_property_all_config(TARGET ${PARAMETER_TARGET} PROPERTY PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Output/Symbol)
		ze_set_property_all_config(TARGET ${PARAMETER_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Output/Engine/Plugin)
	endif()
endfunction()

function (ze_add_library)
	parse_arguments(PARAMETER "SOURCES;LIBS;HEADERS;${ze_check_parameters}" "INSTALL;DLL" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
	include_directories(
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_CURRENT_SOURCE_DIR})
		
	# Compile
	if (PARAMETER_DLL)
		add_library(${PARAMETER_TARGET} SHARED ${PARAMETER_SOURCES})
	else()
		add_library(${PARAMETER_TARGET} ${PARAMETER_SOURCES})
	endif()

	target_link_libraries(${PARAMETER_TARGET} ${PARAMETER_LIBS})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_LIBS ${PARAMETER_LIBS})
	
	# Static Analysis
	if (ZEBUILD_STATIC_CHECKS_ENABLE)
		if (ZEBUILD_ENABLE_STATIC_CHECKS)
			if (ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK)
				add_cppcheck(${PARAMETER_TARGET})
			endif()
			
			if (ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT)
				add_pc_lint(${PARAMETER_TARGET})
			endif()
		endif()
	endif()

	if (PARAMETER_INSTALL)
		ze_set_property_all_config(TARGET ${PARAMETER_TARGET} PROPERTY PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Output/Symbol)
		ze_set_property_all_config(TARGET ${PARAMETER_TARGET} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Output/Lib)
	endif()
	
	if (PARAMETER_HEADERS)
		ze_copy_files(${PARAMETER_HEADERS} TARGET ${PARAMETER_TARGET} DESTINATION ${CMAKE_SOURCE_DIR}/Output/Include BASE ${CMAKE_SOURCE_DIR}/Source)
	endif()
		
endfunction()

function (ze_add_test)
	if (NOT ZEBUILD_UNIT_TESTS_ENABLE)
		return()
	endif()

	parse_arguments(PARAMETER "SOURCES;TEST_TARGET;EXTRA_SOURCES;LIBS;${ze_check_parameters}" "" ${ARGV})
		
	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
			
	add_executable(${PARAMETER_TARGET} ${PARAMETER_SOURCES} ${PARAMETER_EXTRA_SOURCES})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
	#set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER "${ZEBUILD_PROJECT_FOLDER}/Tests")
		
	if (PARAMETER_TEST_TARGET)
		get_property(TEST_TARGET_LINKS TARGET ${PARAMETER_TEST_TARGET} PROPERTY ZEBUILD_LIBS)
		target_link_libraries(${PARAMETER_TARGET} ${TEST_TARGET_LINKS})

		get_property(TEST_TARGET_TYPE TARGET ${PARAMETER_TEST_TARGET} PROPERTY TYPE)
		if (TEST_TARGET_TYPE EQUAL STATIC_LIBRARY)
			target_link_libraries(${PARAMETER_TARGET} ${PARAMETER_TEST_TARGET})
		endif()

	endif()

	get_property(OLD_VALUES TARGET ${PARAMETER_TARGET} PROPERTY INCLUDE_DIRECTORIES)
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY INCLUDE_DIRECTORIES ${OLD_VALUES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

	target_link_libraries(${PARAMETER_TARGET} ZETest)

	# Copy dependent DLLs
    if (ZEBUILD_PLATFORM_WINDOWS)
        FILE(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/Platform/${ZEBUILD_PLATFORM}/${ZEBUILD_PLATFORM_ARCHITECTURE}/Dll/*.dll" A)
        FILE(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/Platform/${ZEBUILD_PLATFORM}/${ZEBUILD_PLATFORM_ARCHITECTURE}/Dll/$(ConfigurationName)/*.dll" B)
        FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/$(ConfigurationName)" C)
        add_custom_command(
            TARGET ${PARAMETER_TARGET}
            COMMAND "xcopy"
            ARGS "${A}" "${C}" /D /Y)
        add_custom_command(
            TARGET ${PARAMETER_TARGET}
            COMMAND "xcopy"
            ARGS "${B}" "${C}" /D /Y)
    endif()

	source_group("" FILES ${PARAMETER_SOURCES})		
	add_test(${PARAMETER_TARGET}Test ${PARAMETER_TARGET})
endfunction()

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
endfunction()

function(ze_combine_libraries)
	parse_arguments(PARAMETER "LIBS;${ze_check_parameters}" "" ${ARGV})
	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
	if (ZEBUILD_PLATFORM_WINDOWS)
		foreach (TEMP ${PARAMETER_LIBS})
			get_property(TARGET_LOCATION TARGET ${TEMP} PROPERTY LOCATION)
			file(TO_NATIVE_PATH ${TARGET_LOCATION} NATIVE_LOCATION)
			set (LIBRARY_PATHS "${LIBRARY_PATHS} \"${NATIVE_LOCATION}\" ")	
			ze_set_property_all_config(TARGET ${TEMP} PROPERTY PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Output/Symbol)
		endforeach()

		if (NOT EXISTS "${CMAKE_SOURCE_DIR}/Output/Lib")
			file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/Output/Lib")
		endif()

		add_custom_target(${PARAMETER_TARGET}Combined
			COMMAND lib.exe ${LIBRARY_PATHS} /out:\"${PARAMETER_TARGET}.lib\"
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Output/Lib
			DEPENDS ${PARAMETER_LIBS})
		add_dependencies(ZE ${PARAMETER_TARGET}Combined)
	elseif (ZEBUILD_PLATFORM_DARWIN)
	elseif (ZEBUILD_PLATFORM_UNIX)
	endif()

	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})

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
endfunction()

macro(ze_get_version)
	file(READ "${ARGV0}/Version.txt" VERSION_STRING)

	set(VERSION_MAJOR 0)
	if ("${VERSION_STRING}" MATCHES "[ \\t]*Version[ \\t]*:[ \\t]*([0-9]+).*")
		set(VERSION_MAJOR ${CMAKE_MATCH_1})
	endif()

	set(VERSION_MINOR 0)
	if ("${VERSION_STRING}" MATCHES "[ \\t]*Version[ \\t]*:[ \\t]*[0-9]+[ \\t]*\\.[ \\t]*([0-9]+).*")
		set(VERSION_MINOR ${CMAKE_MATCH_1})
	endif()

	set(VERSION_INTERNAL 0)
	if ("${VERSION_STRING}" MATCHES "[ \\t]*Version[ \\t]*:[ \\t]*[0-9]+[ \\t]*\\.[ \\t]*[0-9]+\\.[ \\t]*([0-9]+).*")
		set(VERSION_INTERNAL ${CMAKE_MATCH_1})
	endif()

endmacro()

macro(ze_get_version_branch_name)
	file(READ "${ARGV0}/Version.txt" VERSION_STRING)

	set (VERSION_BRANCH_NAME "")
	if ("${VERSION_STRING}" MATCHES "[ \\t]*Branch[ \\t]*:[ \\t]*(.*)[ \\t]*")
		set(VERSION_BRANCH_NAME ${CMAKE_MATCH_1})
	endif()
endmacro()

macro(ze_get_version_revision_number)
	set (VERSION_REVISION "0")
	include(ZEFindSubversion)
	Subversion_WC_INFO(${ARGV0} SVN_INFO)
	if (Subversion_WC_RESULT)
		set(VERSION_REVISION ${SVN_INFO_WC_REVISION})
	else()
		set(VERSION_REVISION "0")
	endif()
endmacro()

macro(ze_check_externals)
	if (NOT EXISTS "${ARGV0}/Version.txt")
		set(EXTERNALS_OK -1)
	else()
		ze_get_version("${CMAKE_SOURCE_DIR}/Platform")
		if (NOT ${ARGV1} EQUAL VERSION_MAJOR)
			set(EXTERNALS_OK -2)
		elseif (${ARGV2} LESS VERSION_MINOR)
			set(EXTERNALS_OK -3)
		else()
			set(EXTERNALS_OK 0)
		endif()
	endif()
endmacro()

function(ze_link)
	parse_arguments(PARAMETER "LIBS;${ze_check_parameters}" "" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	target_link_libraries(${PARAMETER_TARGET} ${PARAMETER_LIBS})
endfunction()
