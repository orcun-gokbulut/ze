#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_file_compiler.cmake
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

function(ze_file_compiler)
	parse_arguments(PARAMETER "TARGET;FILES;OUTPUTS" "" ${ARGV})

	foreach(VARIABLE_FILE ${PARAMETER_FILES})
		get_filename_component(VARIABLE_FILE_NORMALIZED "${VARIABLE_FILE}" ABSOLUTE)
		get_filename_component(VARIABLE_FILE_NAME "${VARIABLE_FILE}" NAME)
		string(REPLACE  "." "_" PARAMETER_CLASS "${VARIABLE_FILE_NAME}")
		add_custom_command(
			OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.h"
			COMMAND "$<TARGET_FILE:ZEFileCompiler>"
			ARGS "-q" "\"${VARIABLE_FILE_NORMALIZED}\"" "-c" "\"${PARAMETER_CLASS}\"" "-os" "\"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.cpp\"" "-oh" "\"${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.h\""
			MAIN_DEPENDENCY "${VARIABLE_FILE_NORMALIZED}"
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
			DEPENDS ZEFileCompiler)
		set(VARIABLE_OUTPUTS ${VARIABLE_OUTPUTS} "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${VARIABLE_FILE_NAME}.h")
	endforeach()
	
	if (PARAMETER_TARGET)
		target_sources(${PARAMETER_TARGET} PRIVATE ${VARIABLE_OUTPUTS})
		source_group("Generated" FILES ${VARIABLE_OUTPUTS})
	endif()
	
	if (PARAMETER_OUTPUTS)
		set(${PARAMETER_OUTPUTS} ${VARIABLE_OUTPUTS} PARENT_SCOPE)
	endif()
endfunction()
