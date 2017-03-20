#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_unit_test.cmake
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

function (ze_add_test)
	if (NOT ZEBUILD_UNIT_TESTS_ENABLE)
		return()
	endif()

	parse_arguments(PARAMETER "TARGET;SOURCES;TEST_TARGET;EXTRA_SOURCES;LIBS;${ze_process_source_parameters};${ze_check_parameters}" "" ${ARGV})
		
	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()
	
	add_executable(${PARAMETER_TARGET} ${PARAMETER_SOURCES} ${PARAMETER_EXTRA_SOURCES})
	
	ze_process_sources()
	
	if (PARAMETER_TARGET)
		set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER "Tests/${ZEBUILD_PROJECT_FOLDER}")
	else()
		set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER "Tests/${ZEBUILD_PROJECT_FOLDER}/${PARAMETER_TARGET}")	
	endif()
	
	set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
		
	if (PARAMETER_TEST_TARGET)
		get_property(TEST_TARGET_LINKS TARGET ${PARAMETER_TEST_TARGET} PROPERTY ZEBUILD_LIBS)
		ze_link(TARGET ${PARAMETER_TARGET} LIBS ${TEST_TARGET_LINKS})

		get_property(TEST_TARGET_TYPE TARGET ${PARAMETER_TEST_TARGET} PROPERTY TYPE)
		if (TEST_TARGET_TYPE EQUAL STATIC_LIBRARY)
			ze_link(TARGET ${PARAMETER_TARGET} LIBS ${PARAMETER_TEST_TARGET})
		endif()
	endif()

	ze_link(TARGET ${PARAMETER_TARGET} LIBS ZETest ${PARAMETER_LIBS})

	# Copy dependent DLLs
	ze_copy_runtime_dlls(TARGET ${PARAMETER_TARGET})

	source_group("" FILES ${PARAMETER_SOURCES})		
	add_test(${PARAMETER_TARGET}Test ${PARAMETER_TARGET})
endfunction()

macro(ze_unit_test_init)
	set(ZEBUILD_UNIT_TESTS_ENABLE             FALSE CACHE BOOL "Enable unit tests.")
	set(ZEBUILD_UNIT_TESTS_COVERAGE_ENABLE    FALSE CACHE BOOL "Enables code coverage instrumentation.")
	
	if (ZEBUILD_UNIT_TESTS_ENABLE)
		ENABLE_TESTING()
	endif()
	
	if (ZEBUILD_UNIT_TESTS_ENABLE AND ZEBUILD_UNIT_TESTS_COVERAGE_ENABLE)
		include(External/test_cocoon)
	endif()
endmacro()

ze_unit_test_init()
