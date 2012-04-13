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

macro(ze_platform_check)
	if (PARAMETER_PLATFORM)
		string(TOUPPER ${ZEBUILD_PLATFORM} PLATFORM_UPPER)
		set(PLATFORM_FOUND FALSE)
		foreach(ARG ${PARAMETER_PLATFORMS})
			string(TOUPPER ${ARG} ARG_UPPER)
			if (${ARG_UPPER} EQUAL ${PLATFORM_UPPER})
				set(PLATFORM_FOUND TRUE)
				break()
			endif()
		endforeach()
	else()
		set(PLATFORM_FOUND TRUE)
	endif()
endmacro()

function(ze_add_source)
	parse_arguments(PARAMETER "PLATFORMS" "ZEPP;ZEDOC" ${ARGV})

	ze_platform_check()
	if (NOT PLATFORM_FOUND)
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
	parse_arguments(PARAMETER "SOURCES;LIBS;INSTALL_DESTINATION;INSTALL_COMPONENT;PLATFORMS" "INSTALL;PLUGIN" ${ARGV})

	ze_platform_check()
	if (NOT PLATFORM_FOUND)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
	# Compile
	if ("${ZEBUILD_PLATFORM}" MATCHES "Windows")
		add_executable(${PARAMETER_TARGET} WIN32 ${PARAMETER_SOURCES})
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
		FILE(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/Platform/${ZEBUILD_PLATFORM}/${ZEBUILD_PLATFORM_ARCHITECTURE}/Dll/*.dll" A)
		FILE(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/Platform/${ZEBUILD_PLATFORM}/${ZEBUILD_PLATFORM_ARCHITECTURE}/Dll/$(ConfigurationName)/*.dll" B)
		FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/$(ConfigurationName)" C)
		add_custom_command(
			TARGET ${PARAMETER_TARGET}
			COMMAND "copy" 
			ARGS "${A}" "${C}")
		add_custom_command(
			TARGET ${PARAMETER_TARGET}
			COMMAND "copy" 
			ARGS "${B}" "${C}")
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
	parse_arguments(PARAMETER "SOURCES;LIBS;HEADERS;INSTALL_DESTINATION;INSTALL_COMPONENT;PLATFORMS" "INSTALL;SHARED" ${ARGV})

	ze_platform_check()
	if (NOT PLATFORM_FOUND)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
	# Compile
	add_library(${PARAMETER_TARGET} ${PARAMETER_SOURCES})

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

	parse_arguments(PARAMETER "SOURCES;TEST_TARGET;EXTRA_SOURCES;LIBS;PLATFORMS" "" ${ARGV})
		
	ze_platform_check()
	if (NOT PLATFORM_FOUND)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
			
	add_executable(${PARAMETER_TARGET} ${PARAMETER_SOURCES} ${PARAMETER_EXTRA_SOURCES})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER "${ZEBUILD_PROJECT_FOLDER}/Tests")
		
	if (PARAMETER_TEST_TARGET)
		get_property(TEST_TARGET_LINKS TARGET ${PARAMETER_TEST_TARGET} PROPERTY ZEBUILD_LIBS)
		target_link_libraries(${PARAMETER_TARGET} ${PARAMETER_TEST_TARGET} ${TEST_TARGET_LINKS})
	endif()

	target_link_libraries(${PARAMETER_TARGET} ZETest)
	
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
	parse_arguments(PARAMETER "TITLE;PLATFORMS" "DEFAULT;OPTIONAL" ${ARGV})

	ze_platform_check()
	if (NOT PLATFORM_FOUND)
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
		endif()
	else()
		add_subdirectory(${PARAMETER_DIRECTORY})
	endif()

	# Debug	
	if (ZEBUILD_VERBOSE)	
		message("ze_add_module") 
		message("\tPARAMETER_TITLE = ${PARAMETER_TITLE}")
		message("\tPARAMETER_PLATFORMS = ${tPARAMETER_PLATFORMS}")
		message("\tPARAMETER_DEFAULT = ${PARAMETER_DEFAULT}")
		message("\PARAMETER_OPTIONAL = ${PARAMETER_OPTIONAL}")
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
	parse_arguments(PARAMETER "SOURCESS" "" ${ARGV})
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_NAME)

	ze_platform_check()
	if (NOT PLATFORM_FOUND)
		return()
	endif()

	add_custom_target(${PARAMETER_NAME}	SOURCES ${PARAMETER_SOURCES})
endfunction()
