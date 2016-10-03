#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_merge_libraries.cmake
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

function(ze_merge_static_libraries)
	parse_arguments(PARAMETER "TARGET;LIBS;${ze_check_parameters}" "COMBINABLE" ${ARGV})
	
	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()
	
	if (ZEBUILD_PLATFORM_WINDOWS)
		string(REPLACE ";" "\" \"" VARIABLE_ADDITIONAL_FLAGS "${PARAMETER_LIBS}")
		set(VARIABLE_ADDITIONAL_FLAGS \"${VARIABLE_ADDITIONAL_FLAGS}\")
		set_property(TARGET ${TARGET} APPEND PROPERTY STATIC_LIBRARY_FLAGS "${VARIABLE_ADDITIONAL_FLAGS}")
	elseif(ZEBUILD_PLATFORM_DARWIN)
		string (REPLACE ";" " " VARIABLE_ADDITIONAL_FLAGS "${PARAMETER_LIBS}")
		set_property (TARGET ${TARGET} APPEND PROPERTY STATIC_LIBRARY_FLAGS "${VARIABLE_ADDITIONAL_FLAGS}")
	elseif(ZE_BUILD_PLATFORM_UNIX)
		message(FATAL_ERROR "Library combination. Not implamented.")
	endif()
endfunction()

function(ze_combine_libraries)
	parse_arguments(PARAMETER "TARGET;LIBS;${ze_check_parameters}" "" ${ARGV})
	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

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
						list(APPEND LIBRARY_PATHS "${VARIABLE_LIB_DIRECTORY}/${VARIABLE_CURRENT_DEPENDENCY}")
					endforeach()
					
					foreach(VARIABLE_PLATFORM_LIB ${VARIABLE_LIBS_CONFIGURATION})
						list(APPEND LIBRARY_PATHS "${VARIABLE_LIB_DIRECTORY}/$<CONFIGURATION>/${VARIABLE_CURRENT_DEPENDENCY}")
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
