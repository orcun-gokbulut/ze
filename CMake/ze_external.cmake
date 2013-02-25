#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_external.cmake
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

function (ze_external_register_library)
	parse_arguments(PARAMETER "LIBS;DLLS;LIB_DIRECTORY;INCLUDE_DIRECTORY;DLL_DIRECTORY;SYSTEM_LIBS;${ze_check_parameters}" "NON_COMBINABLE" ${ARGV})

	ze_check()
	if (NOT CHECK_SUCCEEDED)
		return()
	endif()

	# Get Target Name
	list(GET PARAMETER_DEFAULT_ARGS 0 PARAMETER_NAME)
	
	if (NOT PARAMETER_LIB_DIRECTORY)
		set(PARAMETER_LIB_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Lib")
	else()
		set(PARAMETER_LIB_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_LIB_DIRECTORY}")
	endif()
	
	if (NOT PARAMETER_INCLUDE_DIRECTORY)
		set(PARAMETER_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Include")
	else()
		set(PARAMETER_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_INCLUDE_DIRECTORY}")
	endif()
	
	if (NOT PARAMETER_DLL_DIRECTORY)
		set(PARAMETER_DLL_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/DLL")
	else()
		set(PARAMETER_DLL_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PARAMETER_DLL_DIRECTORY}")
	endif()
	

	if (NOT PARAMETER_LIBS)
		file(GLOB PARAMETER_LIBS "${PARAMETER_LIB_DIRECTORY}/*.${ZEBUILD_PLATFORM_LIB_EXTENSION}")
		file(GLOB PARAMETER_LIBS_CONFIGURATION RELATIVE "${PARAMETER_LIB_DIRECTORY}/Release" "${PARAMETER_LIB_DIRECTORY}/Release/*.${ZEBUILD_PLATFORM_LIB_EXTENSION}")
		foreach(CURRENT_LIB ${PARAMETER_LIBS_CONFIGURATION})
			set(PARAMETER_LIBS ${PARAMETER_LIBS} "${PARAMETER_LIB_DIRECTORY}/$(CONFIGURATION)/${CURRENT_LIB}")
		endforeach()
	else()
		foreach(CURRENT_LIB ${PARAMETER_LIBS})
			if (EXISTS "${PARAMETER_LIB_DIRECTORY}/Release/${CURRENT_LIB}.${ZEBUILD_PLATFORM_LIB_EXTENSION}")
				set(PARAMETER_LIBS_NEW ${PARAMETER_LIBS_NEW} "${PARAMETER_LIB_DIRECTORY}/$(CONFIGURATION)/${CURRENT_LIB}.${ZEBUILD_PLATFORM_LIB_EXTENSION}")
			else()
				set(PARAMETER_LIBS_NEW ${PARAMETER_LIBS_NEW} "${PARAMETER_LIB_DIRECTORY}/${CURRENT_LIB}.${ZEBUILD_PLATFORM_LIB_EXTENSION}")		
			endif()
		endforeach()
		set(PARAMETER_LIBS ${PARAMETER_LIBS_NEW})
	endif()
	
	if (NOT PARAMETER_DLLS)
		file(GLOB PARAMETER_DLLS "${PARAMETER_DLL_DIRECTORY}/*.${ZEBUILD_PLATFORM_DLL_EXTENSION}")
		file(GLOB PARAMETER_DLLS_CONFIGURATION RELATIVE "${PARAMETER_DLL_DIRECTORY}/Release" "${PARAMETER_DLL_DIRECTORY}/Release/*.${ZEBUILD_PLATFORM_DLL_EXTENSION}")
		foreach (CURRENT_DLL ${PARAMETER_DLLS_CONFIGURATION})
			set(PARAMETER_DLLS ${PARAMETER_DLLS} "${PARAMETER_DLL_DIRECTORY}/$<CONFIGURATION>/${CURRENT_DLL}")
		endforeach()
	else()
		foreach(CURRENT_DLL ${PARAMETER_DLLS})
			if (EXISTS "${PARAMETER_DLL_DIRECTORY}/Release/${CURRENT_DLL}.${ZEBUILD_PLATFORM_DLL_EXTENSION}")
				set(PARAMETER_DLLS_NEW ${PARAMETER_DLLS_NEW} "${PARAMETER_DLL_DIRECTORY}/$(CONFIGURATION)/${CURRENT_DLL}.${ZEBUILD_PLATFORM_DLL_EXTENSION}")
			else()
				set(PARAMETER_DLLS_NEW ${PARAMETER_DLLS_NEW} "${PARAMETER_DLL_DIRECTORY}/${CURRENT_DLL}.${ZEBUILD_PLATFORM_DLL_EXTENSION}")
			endif()
		endforeach()
		set(PARAMETER_DLLS ${PARAMETER_DLLS_NEW})
	endif()

	add_custom_target(${PARAMETER_NAME})
	set_property(TARGET ${PARAMETER_NAME} PROPERTY ZEBUILD_TYPE "EXTERNAL")
	set_property(TARGET ${PARAMETER_NAME} PROPERTY ZEBUILD_ROOT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	set_property(TARGET ${PARAMETER_NAME} PROPERTY ZEBUILD_LIB_DIRECTORY ${PARAMETER_LIB_DIRECTORY})
	set_property(TARGET ${PARAMETER_NAME} PROPERTY ZEBUILD_INCLUDE_DIRECTORY ${PARAMETER_INCLUDE_DIRECTORY})
	set_property(TARGET ${PARAMETER_NAME} PROPERTY ZEBUILD_DLL_DIRECTORY ${PARAMETER_DLL_DIRECTORY})
	set_property(TARGET ${PARAMETER_NAME} PROPERTY ZEBUILD_LIBS ${PARAMETER_LIBS})
	set_property(TARGET ${PARAMETER_NAME} PROPERTY ZEBUILD_DLLS ${PARAMETER_DLLS})
	set_property(TARGET ${PARAMETER_NAME} PROPERTY ZEBUILD_SYSTEM_LIBS ${PARAMETER_SYSTEM_LIBS})
	set_property(TARGET ${PARAMETER_NAME} PROPERTY FOLDER "External")

	if (NON_COMBINABLE)
		set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_COMBINABLE FALSE)
	else()
		set_property(TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_COMBINABLE TRUE)
	endif()
	
	message(STATUS "[ZEBuild] External library \"${PARAMETER_NAME}\" has been registered.")
endfunction()

function(ze_external_initialize)
	ze_check()
	if (NOT CHECK_SUCCEEDED)
		message(FATAL_ERROR "[ZEBUILD] Error: Wrong external libraries. These external libraries are not build for ${ZEBUILD_PLATFORM} platform and ${ZEBUILD_PLATFORM_ARCHITECTURE} architecture. Please check your External directory.")
	endif()
	
	ze_set_project_folder("External")
endfunction()
