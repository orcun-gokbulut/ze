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
		
	if (ZEBUILD_DEBUG_MODE)
		message("ze_set_project_folder") 
		message("\tZEBUILD_PROJECT_FOLDER = ${ZEBUILD_PROJECT_FOLDER}")
	endif()
endfunction()

function(ze_add_source)
	parse_arguments(PARAMETER "" "ZEPP;ZEDOC" ${ARGV})
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
			COMMAND "${CMAKE_SOURCE_DIR}/ZEPP/zepp.exe" 
			ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE}
			OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_FILE}.zpp)
		list(APPEND ${PARAMETER_FILE_LIST} ${PARAMETER_FILE}.zpp)
		
		source_group("ZPP" FILES ${PARAMETER_FILE}.zpp)
		set_property(SOURCE ${PARAMETER_FILE}.zpp PROPERTY LANGUAGE CXX)
		
		list(APPEND ${PARAMETER_SOURCE_LIST} ${PARAMETER_FILE}.zpp)
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
	if (ZEBUILD_DEBUG_MODE)
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
	parse_arguments(PARAMETER "SOURCES;LIBS;INSTALL_DESTINATION;INSTALL_COMPONENT" "INSTALL;PLUGIN;WIN32" ${ARGV})

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
	# Compile
	if (PARAMETER_PLUGIN)
			add_library(${PARAMETER_TARGET} SHARED ${PARAMETER_SOURCES})
	else()
		if (PARAMETER_WIN32)
			add_executable(${PARAMETER_TARGET} WIN32 ${PARAMETER_SOURCES})
		else()
			add_executable(${PARAMETER_TARGET} ${PARAMETER_SOURCES})
		endif()
	endif()
	
	target_link_libraries(${PARAMETER_TARGET} ${PARAMETER_LIBS})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZE_BUILD_LIBS ${PARAMETER_LIBS})

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
	
	# Debug
	if (ZEBUILD_DEBUG_MODE)
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

function (ze_add_library)
	parse_arguments(PARAMETER "SOURCES;LIBS;HEADERS;INSTALL_DESTINATION;INSTALL_COMPONENT" "INSTALL;SHARED" ${ARGV})

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
	
	# Compile
	add_library(${PARAMETER_TARGET} ${PARAMETER_SOURCES})

	target_link_libraries(${PARAMETER_TARGET} ${PARAMETER_LIBS})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
	set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZE_BUILD_LIBS ${PARAMETER_LIBS})
	
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
	if (ZEBUILD_DEBUG_MODE)
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

function (ze_add_unit_tests)
	if (ZEBUILD_UNIT_TESTS_ENABLE)
		parse_arguments(PARAMETER "SOURCES;TEST_TARGET;LIBS" "" ${ARGV})
		
		# Get Target Name
		list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)
			
		add_executable(${PARAMETER_TARGET} ${PARAMETER_SOURCES})
		set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER "${ZEBUILD_PROJECT_FOLDER}/Tests")
		
		if (PARAMETER_TEST_TARGET)
			get_property(TEST_TARGET_LINKS TARGET ${PARAMETER_TEST_TARGET} PROPERTY ZE_BUILD_LIBS)
			target_link_libraries(${PARAMETER_TARGET} ${PARAMETER_TEST_TARGET} ${TEST_TARGET_LINKS})
		endif()

		target_link_libraries(${PARAMETER_TARGET} ZETest)
	
		source_group("" FILES ${PARAMETER_SOURCES})		
		add_test(${PARAMETER_TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET})
		
		# Debug
		if (ZEBUILD_DEBUG_MODE)
			message("ze_add_unit_tests") 
			message("\tPARAMETER_TARGET = ${PARAMETER_TARGET}")	
			message("\tPARAMETER_SOURCES = ${PARAMETER_SOURCES}")	
			message("\tPARAMETER_TEST_TARGET = ${PARAMETER_TEST_TARGET}")	
			message("\tPARAMETER_LIBS = ${PARAMETER_LIBS}")	
			message("\tTEST_TARGET_LINKS = ${TEST_TARGET_LINKS}")	
		endif()
	endif()
endfunction()

function (ze_add_bulk_unit_tests)
	parse_arguments(PARAMETER "SOURCES;EXTRA_SOURCES;TEST_TARGET;LIBS" "" ${ARGV})

	foreach(TARGET_NAME ${PARAMETER_SOURCES})
		ze_add_unit_tests(${TARGET_NAME} SOURCES ${TARGET_NAME} ${PARAMETER_EXTRA_SOURCES} TEST_TARGET ${PARAMETER_TEST_TARGET} LIBS ${PARAMETER_LIBS})
	endforeach()

	# Debug	
	if (ZEBUILD_DEBUG_MODE)
		message("ze_add_bulk_unit_tests") 
		message("\tPARAMETER_EXTRA_SOURCES = ${PARAMETER_EXTRA_SOURCES}")	
		message("\tPARAMETER_SOURCES = ${PARAMETER_SOURCES}")	
		message("\tPARAMETER_TEST_TARGET = ${PARAMETER_TEST_TARGET}")	
		message("\tPARAMETER_LIBS = ${PARAMETER_LIBS}")	
	endif()
endfunction()

function(ze_add_conditional_build)
	parse_arguments(PARAMETER "TITLE" "TRUE;FALSE" ${ARGV})

	set (PARAMETER_NAME "ZEBUILD_BUILD")
	foreach(ARG ${PARAMETER_DEFAULT_ARGS})
		set(PARAMETER_DIRECTORY ${ARG})
		string(TOUPPER ${ARG} ARG)
		set(PARAMETER_NAME "${PARAMETER_NAME}_${ARG}")
	endforeach()

	if (PARAMETER_TRUE)
		set(DEFAULT TRUE)
	else()
		set(DEFAULT FALSE)
	endif()
	
	set(${PARAMETER_NAME} ${DEFAULT} CACHE BOOL "${PARAMETER_TITLE}")
	if (${PARAMETER_NAME})
		add_subdirectory(${PARAMETER_DIRECTORY})
	endif()

	# Debug	
	if (ZEBUILD_DEBUG_MODE)	
		message("ze_add_conditional_build") 
		message("\tPARAMETER_TITLE = ${PARAMETER_TITLE}")
		message("\tPARAMETER_TRUE = ${PARAMETER_TRUE}")
		message("\tPARAMETER_FALSE = ${PARAMETER_FALSE}")
		message("\tPARAMETER_NAME = ${PARAMETER_NAME}")
		message("\tPARAMETER_DIRECTORY = ${PARAMETER_DIRECTORY}")
		message("\t${PARAMETER_NAME} ${DEFAULT} CAHCHE BOOL \"${PARAMETER_TITLE}\"")
	endif()
endfunction()
