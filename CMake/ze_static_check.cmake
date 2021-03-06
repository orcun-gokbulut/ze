#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_static_check.cmake
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

macro(ze_static_check_init)
	set(ZEBUILD_STATIC_CHECKS_ENABLE          FALSE CACHE BOOL "Enable static checks.")
	set(ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK FALSE CACHE BOOL "Enable cppcheck static checker.")
	set(ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT   FALSE CACHE BOOL "Enable pc-lint static checker.")
	
	if (ZEBUILD_STATIC_CHECKS_ENABLE AND ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK)
		include(External/cppcheck.cmake)
	endif()

	if (ZEBUILD_STATIC_CHECKS_ENABLE AND ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT)
		include(External/pc-lint.cmake)
	endif()
endmacro()

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

ze_static_check_init()
