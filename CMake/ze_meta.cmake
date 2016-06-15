#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_meta.cmake
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

set(ZEBUILD_USE_STOCK_META_COMPILER TRUE CACHE BOOL "Use the ZEMetaCompiler at Tools directory which is already compiled in release configuration. (For faster builds)")

function(ze_meta_compiler)
	parse_arguments(PARAMETER "TARGET;FILES;GENERATED;OUTPUTS" "" ${ARGV})
	
	get_property(VARIABLE_DEFINITIONS DIRECTORY PROPERTY COMPILE_DEFINITIONS)
	get_property(VARIABLE_INCLUDE_DIRECTORIES DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
		
	set(VARIABLE_ARGS "")
	foreach(VARIABLE_DEFINITION ${VARIABLE_DEFINITIONS})
		set(VARIABLE_ARGS ${VARIABLE_ARGS} "-D" "\"${VARIABLE_DEFINITION}\"")
	endforeach()

	foreach(VARIABLE_INCLUDE_DIRECTORY ${VARIABLE_INCLUDE_DIRECTORIES})
		set(VARIABLE_ARGS ${VARIABLE_ARGS} "-I" "\"${VARIABLE_INCLUDE_DIRECTORY}\"")
	endforeach()
	
	if (ZEBUILD_PLATFORM_WINDOWS)
		set(VARIABLE_ARGS ${VARIABLE_ARGS} "-msvc")
	endif()

	set(VARIABLE_MODULE_REGISTER_FILE_CONTENT "")
	foreach(VARIABLE_FILE ${PARAMETER_FILES})
		get_filename_component(VARIABLE_FILE_NORMALIZED ${VARIABLE_FILE} ABSOLUTE)
		get_filename_component(VARIABLE_FILE_NAME_WE "${VARIABLE_FILE_NORMALIZED}" NAME_WE)
	
		set(VARIABLE_ARGS_FILE "-q" "-o" "\"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.cpp\"")
		set(VARIABLE_ARGS_FILE ${VARIABLE_ARGS_FILE} "-r" "\"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.h\"")
		set(VARIABLE_ARGS_FILE ${VARIABLE_ARGS_FILE} "\"${VARIABLE_FILE_NORMALIZED}\"")

		if (ZEBUILD_USE_STOCK_META_COMPILER)
			if (ZEBUILD_PLATFORM_WINDOWS)
				set(META_COMPILER_PATH "${CMAKE_SOURCE_DIR}/Tool/ZEMetaCompiler.exe")
			else()
				set(META_COMPILER_PATH "${CMAKE_SOURCE_DIR}/Tool/ZEMetaCompiler")
			endif()
			
			add_custom_command(
				OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.h"
				COMMAND ${META_COMPILER_PATH}
				ARGS ${VARIABLE_ARGS} ${VARIABLE_ARGS_FILE}
				MAIN_DEPENDENCY "${VARIABLE_FILE_NORMALIZED}"
				WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
				DEPENDS "${META_COMPILER_PATH}")
		else()
			# FUCK YOU CMAKE !
			add_custom_command(
				OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.h"
				COMMAND "$<TARGET_FILE:ZEMetaCompiler>"
				ARGS ${VARIABLE_ARGS} ${VARIABLE_ARGS_FILE}
				MAIN_DEPENDENCY "${VARIABLE_FILE_NORMALIZED}"
				WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
				DEPENDS "${META_COMPILER_PATH}")
		endif()

		set(VARIABLE_MODULE_REGISTER_FILE_CONTENT "${VARIABLE_MODULE_REGISTER_FILE_CONTENT}#include \"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.h\"\n")
		set(VARIABLE_OUTPUTS ${VARIABLE_OUTPUTS} "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.h")
	endforeach()
	
	# Module Register File
	set(VARIABLE_MODULE_REGISTER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaRegister.h")
	file(WRITE ${VARIABLE_MODULE_REGISTER_FILE} "// ZEBuild ZEMeta module\n// Auto-generated ZEMeta register file. Do not edit by hand.\n${VARIABLE_MODULE_REGISTER_FILE_CONTENT}")
	set(VARIABLE_OUTPUTS ${VARIABLE_OUTPUTS} "${VARIABLE_MODULE_REGISTER_FILE}")
	
	set(${PARAMETER_OUTPUTS} ${VARIABLE_OUTPUTS} PARENT_SCOPE)
endfunction()

function(ze_meta_register)
	parse_arguments(PARAMETER "TARGET;LIBS;OUTPUT_FILE" "" ${ARGV})
	if(NOT PARAMETER_OUTPUT_FILE)
		set(PARAMETER_OUTPUT_FILE "ZEMetaRegister.h")
	endif()
	set(PARAMETER_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_OUTPUT_FILE}")
	
	set(VARIABLE_OUTPUT_FILE_CONTENT "//ZEMeta auto generated file. Do not change by hand.\n")
	foreach(VARIABLE_CURRENT ${PARAMETER_LIBS})
		if (TARGET ${VARIABLE_CURRENT})
			get_property(VARIABLE_RETURN TARGET ${VARIABLE_CURRENT} PROPERTY ZEBUILD_BINARY_DIRECTORY)
			if (EXISTS "${VARIABLE_RETURN}/${VARIABLE_CURRENT}.ZEMetaRegister.h")
				set(VARIABLE_OUTPUT_FILE_CONTENT "${VARIABLE_OUTPUT_FILE_CONTENT}#include \"${VARIABLE_RETURN}/${VARIABLE_CURRENT}.ZEMetaRegister.h\"\n")
			endif()
			if (EXISTS "${VARIABLE_RETURN}/ZEMetaRegister.h")
				set(VARIABLE_OUTPUT_FILE_CONTENT "${VARIABLE_OUTPUT_FILE_CONTENT}#include \"${VARIABLE_RETURN}/ZEMetaRegister.h\"\n")
			endif()
		endif()
	endforeach()
	file(WRITE "${PARAMETER_OUTPUT_FILE}" "${VARIABLE_OUTPUT_FILE_CONTENT}")
endfunction()
