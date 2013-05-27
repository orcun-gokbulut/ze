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
	parse_arguments(PARAMETER "TARGET;HEADERS;${ze_check_parameters}" "" ${ARGV})

	foreach(CURRENT_FILE ${PARAMETER_HEADERS})
		get_filename_component(MODIFIED_FILENAME ${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT_FILE} NAME)
		add_custom_command(
			TARGET ${PARAMETER_TARGET}
			POST_BUILD			
			COMMAND ${CMAKE_COMMAND} 
			ARGS -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT_FILE}" "${CMAKE_SOURCE_DIR}/Output/Include/${MODIFIED_FILENAME}")
	endforeach()
endfunction()

function(ze_file_compiler)
	parse_arguments(PARAMETER "TARGET;FILES;OUTPUTS" "" ${ARGV})

	foreach(VARIABLE_FILE ${PARAMETER_FILES})
		get_filename_component(VARIABLE_FILE_NORMALIZED ${VARIABLE_FILE} ABSOLUTE)
		get_filename_component(VARIABLE_FILE_NAME ${VARIABLE_FILE} NAME)
		string(REPLACE  "." "_" PARAMETER_CLASS ${VARIABLE_FILE_NAME})
		add_custom_command(
			OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.h"
			COMMAND "$<TARGET_FILE:ZEFileCompiler>"
			ARGS "-q" "\"${VARIABLE_FILE_NORMALIZED}\"" "-c" "\"${PARAMETER_CLASS}\"" "-os" "\"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.cpp\"" "-oh" "\"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.h\""
			MAIN_DEPENDENCY "${VARIABLE_FILE_NORMALIZED}"
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
			DEPENDS ZEFileCompiler)
		set(VARIABLE_OUTPUTS ${VARIABLE_OUTPUTS} "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.h")
	endforeach()
	
	set(${PARAMETER_OUTPUTS} ${VARIABLE_OUTPUTS} PARENT_SCOPE)
endfunction()

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

	foreach(VARIABLE_FILE ${PARAMETER_FILES})
		get_filename_component(VARIABLE_FILE_NORMALIZED ${VARIABLE_FILE} ABSOLUTE)
		get_filename_component(VARIABLE_FILE_NAME_WE "${VARIABLE_FILE_NORMALIZED}" NAME_WE)
	
		set(VARIABLE_ARGS_FILE "-q" "-o" "\"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.cpp\"")
		set(VARIABLE_ARGS_FILE ${VARIABLE_ARGS_FILE} "-r" "\"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.register\"")
		set(VARIABLE_ARGS_FILE ${VARIABLE_ARGS_FILE} "\"${VARIABLE_FILE_NORMALIZED}\"")

		add_custom_command(
			OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.register"
			COMMAND "$<TARGET_FILE:ZEMetaCompiler>"
			ARGS ${VARIABLE_ARGS} ${VARIABLE_ARGS_FILE}
			MAIN_DEPENDENCY "${VARIABLE_FILE_NORMALIZED}"
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
			DEPENDS ZEMetaCompiler)
		set(VARIABLE_OUTPUTS ${VARIABLE_OUTPUTS} "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.register")
	endforeach()
	
	set(VARIABLE_ARGS "-q" "-c" "\"${PARAMETER_TARGET}MetaRegister\"")
	set(VARIABLE_ARGS ${VARIABLE_ARGS} "-cs" "\"${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaRegister.cpp\"")
	set(VARIABLE_ARGS ${VARIABLE_ARGS} "-ch" "\"${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaRegister.h\"")
	if (ZEBUILD_PLATFORM_WINDOWS)
		set(VARIABLE_ARGS ${VARIABLE_ARGS} "-msvc")
	endif()
	
	foreach(VARIABLE_FILE ${PARAMETER_FILES})
		get_filename_component(VARIABLE_FILE_NAME_WE "${VARIABLE_FILE}" NAME_WE)
		set(VARIABLE_ARGS ${VARIABLE_ARGS} "-r" "\"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.register\"")
		set(VARIABLE_DEPENDENCIES ${VARIABLE_DEPENDENCIES} "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME_WE}.ZEMeta.register")
	endforeach()
	
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaRegister.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaRegister.h"
		COMMAND "$<TARGET_FILE:ZEMetaCompiler>"
		ARGS ${VARIABLE_ARGS}
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		DEPENDS ZEMetaCompiler ${VARIABLE_DEPENDENCIES})

	set(VARIABLE_OUTPUTS ${VARIABLE_OUTPUTS} "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaRegister.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaRegister.h")
	
	set(${PARAMETER_OUTPUTS} ${VARIABLE_OUTPUTS} PARENT_SCOPE)
endfunction()

function (ze_meta_generate_module_register)
	parse_arguments(PARAMETER "TARGET" "" ${ARGV})

	set(VARIABLE_REGISTER_CODE "")
	set(VARIABLE_REGISTER_INCLUDES "")
	set(VARIABLE_REGISTER_COUNT 0)
	ze_get_dependency_list_combined(TARGET ${PARAMETER_TARGET} RETURN VARIABLE_DEPENDENCIES)
	foreach(VARIABLE_DEPENDENCY ${VARIABLE_DEPENDENCIES})
		if (TARGET ${VARIABLE_DEPENDENCY})
			get_property(VARIABLE_REGISTER_FILE TARGET ${VARIABLE_DEPENDENCY} PROPERTY ZEBUILD_ZE_META_REGISTER_FILE)
			if (VARIABLE_REGISTER_FILE)
				set(VARIABLE_REGISTER_CODE "${VARIABLE_REGISTER_CODE}\t\t${VARIABLE_DEPENDENCY}MetaRegister::GetInstance(),\n")
				set(VARIABLE_REGISTER_INCLUDES "${VARIABLE_REGISTER_INCLUDES}#include \"${VARIABLE_REGISTER_FILE}.h\"\n")
				MATH(EXPR VARIABLE_REGISTER_COUNT "${VARIABLE_REGISTER_COUNT} + 1")
			endif()
		endif()
	endforeach()
	
	file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaModuleRegister.h"
		"#pragma once\n"
		"#include \"ZEMeta/ZEMetaModuleRegister.h\"\n"
		"\n"
		"class ${PARAMETER_TARGET}_MetaModuleRegister : public ZEMetaModuleRegister\n"
		"{\n"
		"\tpublic:\n"
		"\t\tvirtual ZEMetaRegister**\t\tGetRegisters();\n"
		"\t\tvirtual ZESize\t\t\t\t\tGetRegisterCount();\n"
		"\n"
		"\t\tstatic ${PARAMETER_TARGET}_MetaModuleRegister*\tGetInstance();\n"
		"};\n")
	
	if (VARIABLE_REGISTER_COUNT EQUAL 0)
		file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaModuleRegister.cpp"		
			"#include \"${PARAMETER_TARGET}.ZEMetaModuleRegister.h\"\n"
			"\n"
			"ZEMetaRegister** ${PARAMETER_TARGET}_MetaModuleRegister::GetRegisters()\n"
			"{\n"
			"\treturn NULL;\n"
			"}\n"
			"\n"
			"ZESize ${PARAMETER_TARGET}_MetaModuleRegister::GetRegisterCount()\n"
			"{\n"
			"\treturn 0;\n"
			"}\n"
			"\n"
			"${PARAMETER_TARGET}_MetaModuleRegister* ${PARAMETER_TARGET}_MetaModuleRegister::GetInstance()\n"
			"{\n"
			"\tstatic ${PARAMETER_TARGET}_MetaModuleRegister ModuleRegister;\n"
			"\treturn &ModuleRegister;\n"
			"}\n")
	else()
		file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PARAMETER_TARGET}.ZEMetaModuleRegister.cpp"		
			"#include \"${PARAMETER_TARGET}.ZEMetaModuleRegister.h\"\n"
			"${VARIABLE_REGISTER_INCLUDES}"
			"\n"
			"ZEMetaRegister** ${PARAMETER_TARGET}_MetaModuleRegister::GetRegisters()\n"
			"{\n"
			"\tstatic ZEMetaRegister* Registers[] =\n"
			"\t{\n"
			"${VARIABLE_REGISTER_CODE}"
			"\t};\n"
			"\t\n"
			"\treturn Registers;\n"
			"}\n"
			"\n"
			"ZESize ${PARAMETER_TARGET}_MetaModuleRegister::GetRegisterCount()\n"
			"{\n"
			"\treturn ${VARIABLE_REGISTER_COUNT};\n"
			"}\n"
			"\n"
			"${PARAMETER_TARGET}_MetaModuleRegister* ${PARAMETER_TARGET}_MetaModuleRegister::GetInstance()\n"
			"{\n"
			"\tstatic ${PARAMETER_TARGET}_MetaModuleRegister ModuleRegister;\n"
			"\treturn &ModuleRegister;\n"
			"}\n")
	endif()
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
	
function(ze_copy_runtime_dlls)
	parse_arguments(PARAMETER "TARGET" "" ${ARGV})

	ze_get_dependency_list_combined(TARGET ${PARAMETER_TARGET} RETURN VARIABLE_DEPENDENCIES)
	foreach(VARIABLE_DEPENDENCY ${VARIABLE_DEPENDENCIES})
		if (TARGET ${VARIABLE_DEPENDENCY})
			get_property(VARIABLE_TYPE TARGET ${VARIABLE_DEPENDENCY} PROPERTY ZEBUILD_TYPE)
			if (VARIABLE_TYPE MATCHES "EXTERNAL")
				get_property(VARIABLE_DLL_DIRECTORY TARGET ${VARIABLE_DEPENDENCY} PROPERTY ZEBUILD_DLL_DIRECTORY)
				get_property(VARIABLE_DLLS TARGET ${VARIABLE_DEPENDENCY} PROPERTY ZEBUILD_DLLS)
				
				foreach(VARIABLE_DLL ${VARIABLE_DLLS})
					get_filename_component(VARIABLE_DLL_BASENAME "${VARIABLE_DLL}" NAME)
					add_custom_command(
						TARGET ${PARAMETER_TARGET}
						POST_BUILD
						COMMAND ${CMAKE_COMMAND}
						ARGS -E copy_if_different "${VARIABLE_DLL}" "$<TARGET_FILE_DIR:${PARAMETER_TARGET}>/${VARIABLE_DLL_BASENAME}")
				endforeach()
			elseif (VARIABLE_TYPE MATCHES "DLL")
				add_custom_command(
					TARGET ${PARAMETER_TARGET}
					POST_BUILD
					COMMAND ${CMAKE_COMMAND}
					ARGS -E copy_if_different "$<TARGET_FILE:${VARIABLE_DEPENDENCY}>" "$<TARGET_FILE_DIR:${PARAMETER_TARGET}>")
			endif()
		endif()
	endforeach()
endfunction()

function(ze_combine_libraries)
	parse_arguments(PARAMETER "LIBS;${ze_check_parameters}" "" ${ARGV})
	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_TARGET)

	# Gather Libraries
	foreach(VARIABLE_CURRENT_LIB ${PARAMETER_LIBS})
		if (TARGET ${VARIABLE_CURRENT_LIB})
			ze_get_dependency_list(TARGET ${VARIABLE_CURRENT_LIB} RETURN VARIABLE_CURRENT_LIB_DEPENDENCIES)
			foreach(VARIABLE_CURRENT_DEPENDENCY ${VARIABLE_CURRENT_LIB_DEPENDENCIES})
				list(FIND VARIABLE_LIBS ${VARIABLE_CURRENT_DEPENDENCY} VARIABLE_FOUND)
				if (VARIABLE_FOUND EQUAL -1)				
					list(APPEND VARIABLE_LIBS ${VARIABLE_CURRENT_DEPENDENCY})
				endif()
			endforeach()		
		else()
			list(APPEND VARIABLE_LIBS ${VARIABLE_CURRENT_DEPENDENCY})
		endif()
	endforeach()
	
	# Normalize Libraries
	foreach(VARIABLE_LIB ${VARIABLE_LIBS})
		if (TARGET ${VARIABLE_LIB})
			get_property(VARIABLE_TYPE TARGET ${VARIABLE_LIB} PROPERTY ZEBUILD_TYPE)
			if (VARIABLE_TYPE STREQUAL "PLATFORMLIB")
				get_property(VARIABLE_COMBINABLE TARGET ${VARIABLE_LIB} PROPERTY ZEBUILD_COMBINABLE)
				if (VARIABLE_COMBINABLE)
					get_property(VARIABLE_LIBS TARGET ${VARIABLE_LIB} PROPERTY ZEBUILD_LIBS)
					get_property(VARIABLE_LIBS_CONFIGURATION TARGET ${VARIABLE_LIB} PROPERTY ZEBUILD_LIBS_CONFIGURATION)
					get_property(VARIABLE_LIB_DIRECTORY TARGET ${VARIABLE_LIB} PROPERTY ZEBUILD_LIB_DIRECTORY)
					
					foreach(VARIABLE_PLATFORM_LIB ${VARIABLE_LIBS})
						list(APPEND LIBRARY_PATHS "${VARIABLE_LIB_DIRECTORY}/${VARIABLE_CURRENT_DEPENDENCY})
					endforeach()
					
					foreach(VARIABLE_PLATFORM_LIB ${VARIABLE_LIBS_CONFIGURATION})
						list(APPEND LIBRARY_PATHS "${VARIABLE_LIB_DIRECTORY}/$<CONFIGURATION>/${VARIABLE_CURRENT_DEPENDENCY})
					endforeach()
				endif()
			elseif(VARIABLE_TYPE STREQUAL "LIB")
				list(APPEND LIBRARY_PATHS "$<TARGET_FILE_DIR:${VARIABLE_LIB}>")
				ze_set_property_all_config(TARGET ${VARIABLE_LIB} PROPERTY PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Output/Symbol)
			else()
				message(SEND_ERROR "Can not combine library. ${VARIABLE_LIB} is not static library or platform library.")
				return()
			endif()
		else()
			message(SEND_ERROR "Can not combine library. ${VARIABLE_LIB} is not static library or platform library.")
			return()
		endif()
	endforeach()
	
	if (ZEBUILD_PLATFORM_WINDOWS)
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
