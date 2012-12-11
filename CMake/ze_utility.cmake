#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_utility.cmake
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

function(ze_copy_headers)
	parse_arguments(PARAMETER "TARGET;HEADERS" "" ${ARGV})

	foreach(CURRENT_FILE ${PARAMETER_HEADERS})
		string(REPLACE "${CMAKE_SOURCE_DIR}/Source" "\0" MODIFIED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT_FILE})
		add_custom_command(
			TARGET ${PARAMETER_TARGET}
			POST_BUILD			
			COMMAND ${CMAKE_COMMAND} 
			ARGS "-E copy_if_different \"${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT_FILE}\" \"${CMAKE_SOURCE_DIR}/Output/Include${MODIFIED_PATH}\"")
	endforeach()
endfunction()

function(ze_copy_runtime_dlls)
	parse_arguments(PARAMETER "TARGET" "" ${ARGV})

	add_custom_command(
		TARGET ${PARAMETER_TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS "-E copy_directory \"${CMAKE_SOURCE_DIR}/Platform/${ZEBUILD_PLATFORM}/${ZEBUILD_PLATFORM_ARCHITECTURE}/Dll\" \"$<TARGET_FILE_DIR:${PARAMETER_TARGET}>\"")

	if (ZEBUILD_MULTI_CONFIGURATION)
		add_custom_command(
			TARGET ${PARAMETER_TARGET}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND}
			ARGS "-E copy_directory \"${CMAKE_SOURCE_DIR}/Platform/${ZEBUILD_PLATFORM}/${ZEBUILD_PLATFORM_ARCHITECTURE}/Dll/$<CONFIGURATION>\" \"$<TARGET_FILE_DIR:${PARAMETER_TARGET}>\"")
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

		add_custom_target(${PARAMETER_TARGET}Combined
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/Output/Lib/$<CONFIGURATION>
			COMMAND lib.exe ${LIBRARY_PATHS} /out:\"${CMAKE_SOURCE_DIR}/Output/Lib/$<CONFIGURATION>/${PARAMETER_TARGET}.lib\"
			DEPENDS ${PARAMETER_LIBS})
		add_dependencies(ZE ${PARAMETER_TARGET}Combined)
	elseif (ZEBUILD_PLATFORM_DARWIN)
	elseif (ZEBUILD_PLATFORM_UNIX)
	endif()

	set_property(TARGET ${PARAMETER_TARGET} PROPERTY FOLDER ${ZEBUILD_PROJECT_FOLDER})
endfunction()

function(ze_file_compiler)
	parse_arguments(PARAMETER "CLASS;OUTPUT" "" ${ARGV})
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_INPUT)

	get_property(ZEFC_BINARY_PATH TARGET ZEFC PROPERTY RUNTIME_OUTPUT_DIRECTORY)
	add_custom_command(
		OUTPUT "${PARAMETER_OUTPUT}.cpp" "${PARAMETER_OUTPUT}.h"
		COMMAND $<TARGET_FILE:ZEFC>
		ARGS "\"${PARAMETER_INPUT}\" -C \"${PARAMETER_CLASS}\" -S \"${PARAMETER_OUTPUT}.cpp\" -H \"${PARAMETER_OUTPUT}.h\""
		MAIN_DEPENDENCY "${PARAMETER_INPUT}"
		DEPENDS ZEFC)
	
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
		OUTPUT "${PARAMETER_OUTPUT}"
		COMMAND ${ZEBUILD_TOOL_PATH}\\zepp.exe
		ARGS "\"${PARAMETER_INPUT}\" \"${PARAMETER_OUTPUT}\""
		MAIN_DEPENDENCY "${PARAMETER_INPUT}")
endfunction()

function(ze_static_analysis PARAMETER_TARGET)
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
endfunction()
