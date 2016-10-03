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

macro(ze_append_property SCOPE PROPERTY_IDEN PROPERTY_NAME)
	get_property(OLD_VALUES ${SCOPE} PROPERTY ${PROPERTY_NAME})
	set_property(${SCOPE} PROPERTY ${PROPERTY_NAME} ${OLD_VALUES} ${ARGN})
endmacro()

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
	
function(ze_copy_runtime_dlls)
	parse_arguments(PARAMETER "TARGET" "" ${ARGV})

	ze_get_dependency_list_combined(TARGET ${PARAMETER_TARGET} RETURN VARIABLE_DEPENDENCIES)
	foreach(VARIABLE_DEPENDENCY ${VARIABLE_DEPENDENCIES})
		if (TARGET ${VARIABLE_DEPENDENCY})
			get_property(VARIABLE_TYPE TARGET ${VARIABLE_DEPENDENCY} PROPERTY ZEBUILD_TYPE)
			if (VARIABLE_TYPE MATCHES "EXTERNAL")
				get_property(VARIABLE_DLL_DIRECTORY TARGET ${VARIABLE_DEPENDENCY} PROPERTY ZEBUILD_DLL_DIRECTORY)
				get_property(VARIABLE_DLLS TARGET ${VARIABLE_DEPENDENCY} PROPERTY ZEBUILD_DLLS)
				get_property(VARIABLE_DLLS_DEBUG TARGET ${VARIABLE_DEPENDENCY} PROPERTY ZEBUILD_DLLS_DEBUG)	 
				get_property(VARIABLE_DLLS_RELEASE TARGET ${VARIABLE_DEPENDENCY} PROPERTY ZEBUILD_DLLS_RELEASE)	 
					
				foreach(VARIABLE_DLL ${VARIABLE_DLLS})
					get_filename_component(VARIABLE_DLL_BASENAME "${VARIABLE_DLL}" NAME)
					add_custom_command(
						TARGET ${PARAMETER_TARGET}
						POST_BUILD
						COMMAND ${CMAKE_COMMAND}
						ARGS -E copy_if_different "${VARIABLE_DLL}" "$<TARGET_FILE_DIR:${PARAMETER_TARGET}>/${VARIABLE_DLL_BASENAME}")
				endforeach()
				foreach(VARIABLE_DLL ${VARIABLE_DLLS_DEBUG})
					get_filename_component(VARIABLE_DLL_BASENAME "${VARIABLE_DLL}" NAME)
					add_custom_command(
						TARGET ${PARAMETER_TARGET}
						POST_BUILD
						COMMAND if 1==$<CONFIG:Debug> ${CMAKE_COMMAND}
						ARGS -E copy_if_different "${VARIABLE_DLL}" "$<TARGET_FILE_DIR:${PARAMETER_TARGET}>/${VARIABLE_DLL_BASENAME}")
				endforeach()
				foreach(VARIABLE_DLL ${VARIABLE_DLLS_RELEASE})
					get_filename_component(VARIABLE_DLL_BASENAME "${VARIABLE_DLL}" NAME)
					add_custom_command(
						TARGET ${PARAMETER_TARGET}
						POST_BUILD
						COMMAND if 1==$<CONFIG:Release> ${CMAKE_COMMAND}
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
