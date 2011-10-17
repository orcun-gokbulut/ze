#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - test_cocoon.cmake
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

set(ZEBUILD_UNIT_TESTS_COVERAGE_ENABLE_TESTCOCOON TRUE CACHE BOOL "Enables test cocoon code coverage instrumentation.")
set(ZEBUILD_TESTCOCOON_COVERAGE_FLAGS	"--cs-on --cs-count"	CACHE STRING "Additional Testcocoon flags.")
set(ZEBUILD_TESTCOCOON_PATH				"$ENV{TESTCOCOON}"		CACHE STRING "Testcocoon path.")
	
if(ZEBUILD_UNIT_TESTS_COVERAGE_ENABLE AND ZEBUILD_UNIT_TESTS_COVERAGE_ENABLE_TESTCOCOON)

	set(CMAKE_CXX_FLAGS_COVERAGE 
		 "${CMAKE_C_FLAGS_RELEASE} ${ZEBUILD_TESTCOCOON_COVERAGE_FLAGS}" CACHE STRING
		"Flags used by the C++ compiler during coverage builds."
		FORCE)
	set(CMAKE_C_FLAGS_COVERAGE 
		 "${CMAKE_CXX_FLAGS_RELEASE} ${ZEBUILD_TESTCOCOON_COVERAGE_FLAGS}" CACHE STRING
		"Flags used by the C compiler during coverage builds."
		FORCE)
	set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
		"${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${ZEBUILD_TESTCOCOON_COVERAGE_FLAGS}" CACHE STRING
		"Flags used for linking binaries during coverage builds."
		FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
		"${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${ZEBUILD_TESTCOCOON_COVERAGE_FLAGS}" CACHE STRING
		"Flags used by the shared libraries linker during coverage builds."
		FORCE)
	set(CMAKE_STATIC_LINKER_FLAGS_COVERAGE
		"${CMAKE_STATIC_LINKER_FLAGS_RELEASE} ${ZEBUILD_TESTCOCOON_COVERAGE_FLAGS}" CACHE STRING
		"Flags used by the static libraries linker during coverage builds."
		FORCE)
	mark_as_advanced(
		CMAKE_CXX_FLAGS_COVERAGE
		CMAKE_C_FLAGS_COVERAGE
		CMAKE_EXE_LINKER_FLAGS_COVERAGE
		CMAKE_SHARED_LINKER_FLAGS_COVERAGE 
		CMAKE_STATIC_LINKER_FLAGS_COVERAGE 
		COMPILE_DEFINITIONS_COVERAGE)

	set(CMAKE_SYSTEM_NAME Windows)

	file(TO_CMAKE_PATH "${ZEBUILD_TESTCOCOON_PATH}/visualstudio" TESTCOCOON)
	set(CMAKE_C_COMPILER ${TESTCOCOON}/cl.exe 
		CACHE FILEPATH "CoverageScanner wrapper" FORCE)
	set(CMAKE_CXX_COMPILER ${TESTCOCOON}/cl.exe 
		CACHE FILEPATH "CoverageScanner wrapper" FORCE)
	set(CMAKE_LINKER ${TESTCOCOON}/link.exe 
		CACHE FILEPATH "CoverageScanner wrapper" FORCE)
		
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES};COVERAGE")
endif()
