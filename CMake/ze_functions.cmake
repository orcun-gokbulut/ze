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
		
	if (ZEBUILD_VERBOSE)
		message("ze_set_project_folder") 
		message("\tZEBUILD_PROJECT_FOLDER = ${ZEBUILD_PROJECT_FOLDER}")
	endif()
endfunction()

set(ze_check_parameters "PLATFORMS;COMPILERS;ARCHS")
macro(ze_check)
	if (PARAMETER_PLATFORMS)
		string(TOUPPER ${ZEBUILD_PLATFORM} PLATFORM_UPPER)
		set(CHECK_SUCCEEDED FALSE)
		foreach(ARG ${PARAMETER_PLATFORMS})
			string(TOUPPER ${ARG} ARG_UPPER)
			if (${ARG_UPPER} MATCHES ${PLATFORM_UPPER})
				set(CHECK_SUCCEEDED TRUE)
				break()
			endif()
		endforeach()
	else()
		set(CHECK_SUCCEEDED TRUE)
	endif()

    if (NOT CHECK_SUCCEEDED)
        if (PARAMETER_PLATFORMS)
            string(TOUPPER ${ZEBUILD_PLATFORM_BASE} PLATFORM_UPPER)
            set(CHECK_SUCCEEDED FALSE)
            foreach(ARG ${PARAMETER_PLATFORMS})
                string(TOUPPER ${ARG} ARG_UPPER)
                if ("${ARG_UPPER}" EQUAL "${PLATFORM_UPPER}")
                    set(CHECK_SUCCEEDED TRUE)
                    break()
                endif()
            endforeach()
        else()
            set(CHECK_SUCCEEDED TRUE)
        endif()
    endif()

	if (CHECK_SUCCEEDED)
		if (PARAMETER_ARCHS)
			string(TOUPPER ${ZEBUILD_PLATFORM_ARCHITECTURE} PLATFORM_UPPER)
			set(CHECK_SUCCEEDED FALSE)
			foreach(ARG ${PARAMETER_ARCHS})
				string(TOUPPER ${ARG} ARG_UPPER)
				if (${ARG_UPPER} MATCHES ${PLATFORM_UPPER})
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
			string(TOUPPER ${ZEBUILD_PLATFORM_COMPILERS} PLATFORM_UPPER)
			set(CHECK_SUCCEEDED FALSE)
			foreach(ARG ${PARAMETER_COMPILERS})
				string(TOUPPER ${ARG} ARG_UPPER)
				if (${ARG_UPPER} MATCHES ${PLATFORM_UPPER})
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
			string(TOUPPER ${ZEBUILD_PLATFORM_ARCHITECTURE} PLATFORM_UPPER)
			set(CHECK_SUCCEEDED FALSE)
			foreach(ARG ${PARAMETER_ARCHS})
				string(TOUPPER ${ARG} ARG_UPPER)
				if (${ARG_UPPER} MATCHES ${PLATFORM_UPPER})
					set(CHECK_SUCCEEDED TRUE)
					break()
				endif()
			endforeach()
		else()
			set(CHECK_SUCCEEDED TRUE)
		endif()
	endif()
endmacro()

function(ze_add_source)
	parse_arguments(PARAMETER "${ze_check_parameters}" "ZEPP;ZEDOC" ${ARGV})

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
		add_custom_command(
			COMMAND "${ZEBUILD_TOOL_PATH}/zepp.exe" 
			ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE}
			OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE}.ZEMeta.cpp)
		list(APPEND ${PARAMETER_FILE_LIST} ${PARAMETER_FILE}.ZEMeta.cpp)
		
		source_group("Meta" FILES ${PARAMETER_FILE}.ZEMeta.cpp)
		set_property(SOURCE ${PARAMETER_FILE}.ZEMeta.cpp PROPERTY LANGUAGE CXX)
		
		list(APPEND ${PARAMETER_SOURCE_LIST} ${PARAMETER_FILE}.ZEMeta.cpp)
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
	
	# Debugging
	if (ZEBUILD_VERBOSE)
		message("ze_add_source") 
		message("\t${PARAMETER_SOURCE_LIST} = ${${PARAMETER_SOURCE_LIST}}")
		message("\tPARAMETER_ZEPP = ${PARAMETER_ZEPP}")
		message("\tPARAMETER_ZEDOC= ${tPARAMETER_ZEDOC}")
		foreach (SOURCE_LIST ${PARAMETER_SOURCE_LISTS})
			message ("\t${SOURCE_LIST} = ${${SOURCE_LIST}}")
		endforeach()
	endif()
endfunction()

function(ze_add_executable)
	parse_arguments(PARAMETER "SOURCES;LIBS;INSTALL_DESTINATION;INSTALL_COMPONENT;${ze_check_parameters}" "INSTALL;PLUGIN;CONSOLE" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
	# Compile
	if ("${ZEBUILD_PLATFORM}" MATCHES "Windows")
		if (PARAMETER_CONSOLE)
			add_executable(${PARAMETER_TARGET} ${PARAMETER_SOURCES})
		else()
			add_executable(${PARAMETER_TARGET} WIN32 ${PARAMETER_SOURCES})
		endif()
	else()
		add_executable(${PARAMETER_TARGET} ${PARAMETER_SOURCES})
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
		
	# Install
	if (PARAMETER_INSTALL)
		install(TARGETS ${PARAMERTER_TARGET} 
			DESTINATION ${PARAMETER_INSTALL_DESTINATION} 
			COMPONENT ${PARAMETER_INSTALL_COMPONENT} 
			CONFIGURATIONS Release)
	endif()

	# Copy dependecies
	if (PARAMETER_PLUGIN)
	else()
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
		
	# Debug
	if (ZEBUILD_VERBOSE)
		message("ze_add_executable") 
		message("\tPARAMETER_TARGET = ${PARAMETER_TARGET}")	
		message("\tPARAMETER_SOURCES = ${PARAMETER_SOURCES}")	
		message("\tPARAMETER_LIBS = ${PARAMETER_LIBS}")	
		message("\tPARAMETER_INSTALL = ${PARAMETER_INSTALL}")	
		message("\tPARAMETER_INSTALL_DESTINATION = ${PARAMETER_INSTALL_DESTINATION}")	
		message("\tPARAMETER_INSTALL_COMPONENT = ${PARAMETER_INSTALL_COMPONENT}")	
		message("\tZEBUILD_PROJECT_FOLDER = ${ZEBUILD_PROJECT_FOLDER}")	
	endif()
endfunction()

function(ze_add_extension)

endfunction()

function (ze_add_library)
	parse_arguments(PARAMETER "SOURCES;LIBS;HEADERS;INSTALL_DESTINATION;INSTALL_COMPONENT;${ze_check_parameters}" "INSTALL;DLL" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
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
	
	# Install
	if (PARAMETER_INSTALL)
		install(TARGETS ${PARAMETER_TARGET} 
			DESTINATION "Lib" 
			COMPONENT ${PARAMETER_INSTALL_COMPONENT} 
			CONFIGURATIONS Release)
		install(FILES ${PARAMETER_HEADERS} 
			DESTINATION Include/${PARAMETER_INSTALL_DESTINATION}
			COMPONENT ${PARAMETER_INSTALL_COMPONENT}
			CONFIGURATIONS Release)
	endif()
		
	# Debug
	if (ZEBUILD_VERBOSE)
		message("ze_add_library") 
		message("\tPARAMETER_TARGET = ${PARAMETER_TARGET}")	
		message("\tPARAMETER_SOURCES = ${PARAMETER_SOURCES}")	
		message("\tPARAMETER_LIBS = ${PARAMETER_LIBS}")	
		message("\tPARAMETER_INSTALL = ${PARAMETER_INSTALL}")	
		message("\tPARAMETER_INSTALL_DESTINATION = ${PARAMETER_INSTALL_DESTINATION}")	
		message("\tPARAMETER_INSTALL_COMPONENT = ${PARAMETER_INSTALL_COMPONENT}")
		message("\tZEBUILD_PROJECT_FOLDER = ${ZEBUILD_PROJECT_FOLDER}")	
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
			
	if (ZEBUILD_VERBOSE)
		message("ze_add_test") 
		message("\tPARAMETER_TARGET = ${PARAMETER_TARGET}")	
		message("\tPARAMETER_SOURCES = ${PARAMETER_SOURCES}")	
		message("\tPARAMETER_TEST_TARGET = ${PARAMETER_TEST_TARGET}")	
		message("\tPARAMETER_LIBS = ${PARAMETER_LIBS}")	
		message("\tTEST_TARGET_LINKS = ${TEST_TARGET_LINKS}")	
	endif()
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
	
	# Debug	
	if (ZEBUILD_VERBOSE)	
		message("ze_add_module") 
		message("\tPARAMETER_TITLE = ${PARAMETER_TITLE}")
		message("\tPARAMETER_PLATFORMS = ${tPARAMETER_PLATFORMS}")
		message("\tPARAMETER_DEFAULT = ${PARAMETER_DEFAULT}")
		message("\tPARAMETER_OPTIONAL = ${PARAMETER_OPTIONAL}")
		message("\tPARAMETER_NAME = ${PARAMETER_NAME}")
		message("\tPARAMETER_DIRECTORTY = ${PARAMETER_DIRECTORTY}")
	endif()
endfunction()

function(ze_combine_libraries)
	parse_arguments(PARAMETER "TARGETS;INSTALL_DESTINATION;INSTALL_COMPONENT" "INSTALL" ${ARGV})
	if (ZEBUILD_PLATFORM_WINDOWS)
	elseif (ZEBUILF_PLATFORM_LINUX OR ZEBUILD_PLATFORM_UNIX)
	endif()
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
	string(REGEX REPLACE "^[ \\t\\r\\n]*Version[ \\t]*:[ \\t]*([0-9]+).*" "\\1" VERSION_MAJOR "${VERSION_STRING}")

	set(VERSION_MINOR 0)
	string(REGEX REPLACE "^[ \\t\\r\\n]*Version[ \\t]*:[ \\t]*[0-9]+[ \\t]*\\.[ \\t]*([0-9]+).*" "\\1" VERSION_MINOR "${VERSION_STRING}")

	set(VERSION_INTERNAL 0)
	string(REGEX REPLACE "^[ \\t\\r\\n]*Version[ \\t]*:[ \\t]*[0-9]+[ \\t]*\\.[ \\t]*[0-9]+\\.[ \\t]*([0-9]+).*" "\\1" VERSION_INTERNAL "${VERSION_STRING}")
endmacro()

macro(ze_get_version_revision_number)
	set (VERSION_REVISION "0")
	include(FindSubversion)
	Subversion_WC_INFO(${ARGV0} SVN_INFO)
	set(VERSION_REVISION ${SVN_INFO_WC_REVISION})
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
