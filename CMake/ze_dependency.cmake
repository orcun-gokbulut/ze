#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_dependency.cmake
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

function (ze_add_dependency)
	parse_arguments(PARAMETER "TARGET;DEPENDENCIES" "" ${ARGV})
	set_property(TARGET ${PARAMETER_TARGET} APPEND PROPERTY ZEBUILD_DEPENDENCIES ${PARAMETER_DEPENDENCIES})
endfunction()

function (ze_get_dependency_list)
	parse_arguments(PARAMETER "TARGET;RETURN;LIST" "" ${ARGV})
	
	if (PARAMETER_LIST)
		set(RETURN_TEMP ${PARAMETER_LIST} ${PARAMETER_TARGET})
	else()
		set(RETURN_TEMP ${PARAMETER_TARGET})
	endif()
	
	if (TARGET ${PARAMETER_TARGET})
		get_property(TARGET_DEPENDENCIES TARGET ${PARAMETER_TARGET} PROPERTY ZEBUILD_DEPENDENCIES)
		foreach(CURRENT_DEPENDENCY ${TARGET_DEPENDENCIES})
			list(FIND RETURN_TEMP ${CURRENT_DEPENDENCY} RESULT)
			if (RESULT EQUAL -1)
				ze_get_dependency_list(TARGET ${CURRENT_DEPENDENCY} LIST ${RETURN_TEMP} RETURN RETURNED_VALUES)
				foreach(CURRENT_RETURN ${RETURNED_VALUES})
					list(FIND RETURN_TEMP ${CURRENT_RETURN} RESULT)
					if (RESULT EQUAL -1)
						list(APPEND RETURN_TEMP ${CURRENT_RETURN})
					endif()
				endforeach()
			endif()
		endforeach()
	endif()
	
	set(${PARAMETER_RETURN} ${RETURN_TEMP} PARENT_SCOPE)
endfunction()
