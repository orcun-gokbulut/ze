#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_compiler_linker.cmake
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

macro (ze_compiler_linker_msvc)
	message(STATUS "[ZEBuild] Microsoft Visual C++ compiler detected. Adjusting compiler and linker parameters.")

	# /EH Exception Handling
	# /EHsc- Disable C++ Exceptions
	# /GL Whole Program Optimization
	# /GS Security Checks
	# /GR Runtime Type Information
	# /GT Support Fiber-Safe Thread-Local Storage
	# /GF Eliminate Duplicate Strings
	# /Gy Enable Function Level Linking
	# /Gs[n] Control Stack Checking Calls. /Gs == /Gs0 == Disable
	# /O2 Optimization Performance - /Ob2 /Og /Oi /Ot /Oy /Gs /GF /Gy
	# /Ox Full Optmization - /Ob2 /Ot /Oi /Ot /Oy
	# /Ob[n] Inline Function Expension (0 - none, 1 - marked with inline or __forceinline, 2 - any applicaple)
	# /Ot Favor for Fast Code
	# /Og Global Optimization
	# /Oy Omit Frame-Pointer
	# /Oi Enable Instrint Functions
	# /Zi Debug Information Format (PDB) 
	# /ZI Debug Information Format (PDB with Edit and Continue)
	# /d2Zi+ Debug Optimized Code (Also /Zo in VS2013 Update3+)
	# /IGNORE:4099 Ignore PDB file not found problem
	# /wd4251 Ignore Member Variable DLL Interface Problem
	# /MP Multi-Processor compilation
	
	if (ZEBUILD_PLATFORM_ARCHITECTURE_X86)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")	
	endif()

	string(REPLACE "/GR" 	"" CMAKE_C_FLAGS 	"${CMAKE_C_FLAGS}")
	string(REPLACE "/GS" 	"" CMAKE_C_FLAGS 	"${CMAKE_C_FLAGS}")
	string(REPLACE "/GT" 	"" CMAKE_C_FLAGS 	"${CMAKE_C_FLAGS}")
	string(REPLACE "/GR" 	"" CMAKE_CXX_FLAGS 	"${CMAKE_CXX_FLAGS}")
	string(REPLACE "/GS" 	"" CMAKE_CXX_FLAGS 	"${CMAKE_CXX_FLAGS}")
	string(REPLACE "/GT" 	"" CMAKE_CXX_FLAGS 	"${CMAKE_CXX_FLAGS}")
	string(REPLACE "/EHsc" 	"" CMAKE_CXX_FLAGS 	"${CMAKE_CXX_FLAGS}")

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /wd4251")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc- /wd4251")
	
	set(ZEBUID_COMPILER_FLAGS_RELEASE "/fp:fast /GL /GT- /GS- /GR- /Zi /d2Zi+ /Ox /Ob2 /Oi /Ot /Oy /Gs /GF /Gy")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${ZEBUID_COMPILER_FLAGS_RELEASE}")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${ZEBUID_COMPILER_FLAGS_RELEASE}")
	
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /IGNORE:4099")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /DEBUG /DEBUGTYPE:cv /OPT:REF /OPT:ICF")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /IGNORE:4099")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG /DEBUG /DEBUGTYPE:cv /OPT:REF /OPT:ICF")
	set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /LTCG")

	add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endmacro()

macro (ze_compiler_linker_msvc_icc)
	message(STATUS "[ZEBuild] Intel C++ Compiler detected. Adjusting compiler and linker parameters.")
	
	# /O3 Intel Compiler Max Optimization
	# /Qipo Interprocess Optimization (Whole Program Optimization
	# /Qunroll Loop unrolling optimization
	# /GA Optimize for Windwows Application (Thread Local Storage optimization)
	# /QaxSSE4.2 Generated SSE4.2 code
	# /QxSSE4.2 Generate code for Intel CPUs with SSE4.2
	# /Quse-intel-optimized-headers Use intel Optmized standard library headers
	# /Qdiag-disable:10120 Ignore "overriding '/O2' with '/O3'" warning
	# /Qdiag-disable:11022 Ignore not compiled with whole program optimization warning
	# /Qcheck-pointers:rw Check pointer boundries for read and write accesses
	# /Qcheck-pointers-undimensioned- Disables pointer checks on undimensioned. (Causes linker errors)
	# /Qcheck-pointers-dangling:all Find dangling pointers pointing to stack and heap
	# /ignore:4006 Suppress "Already defined in xxx; second definition ignored." warning. (This warning occures when pointer check enabled)
	
	string(REPLACE "/O2" "" CMAKE_C_RELEASE "${CMAKE_C_RELEASE}")
	string(REPLACE "/O2" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O3 /Qipo /Qunroll /GA /Qdiag-disable:10120 /Quse-intel-optimized-headers /QaxSSE4.2 /QxSSE4.2")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")


	set(ZEBUILD_DEBUG_INTEL_COMPILER_POINTER_CHECKS	FALSE CACHE BOOL "Enable Intel C++ compiler pointer checks.")
	if (ZEBUILD_DEBUG_INTEL_COMPILER_POINTER_CHECKS)
		message(STATUS "[ZEBuild] Enabiling Intel C++ Compiler pointer checks.")
		set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /ignore:4006")
		set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /ignore:4006")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Qcheck-pointers:rw /Qcheck-pointers-dangling:all /Qcheck-pointers-undimensioned-")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Qcheck-pointers:rw /Qcheck-pointers-dangling:all /Qcheck-pointers-undimensioned-")
	endif()

	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /Qipo /Qdiag-disable:11022")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /Qipo /Qdiag-disable:11022")
	set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /Qipo")
endmacro()

macro (ze_compiler_linker_print_flags)
	message(STATUS "[ZEBuild] Compiler and linker flags")
	message(STATUS "[ZEBuild] Debug Configuration")
	message(STATUS "[ZEBuild]   C Compiler Flags   :${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
	message(STATUS "[ZEBuild]   C++ Compiler Flags :${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
	message(STATUS "[ZEBuild]   EXE Linker Flags   :${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
	message(STATUS "[ZEBuild]   DLL Linker Flags   :${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
	message(STATUS "[ZEBuild]   LIB Linker Flags   :${CMAKE_STATIC_LINKER_FLAGS} ${CMAKE_STATIC_LINKER_FLAGS_DEBUG}")
	message(STATUS "[ZEBuild] Release Configuration")
	message(STATUS "[ZEBuild]   C Compiler Flags   :${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
	message(STATUS "[ZEBuild]   C++ Compiler Flags :${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
	message(STATUS "[ZEBuild]   EXE Linker Flags   :${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
	message(STATUS "[ZEBuild]   DLL Linker Flags   :${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
	message(STATUS "[ZEBuild]   LIB Linker Flags   :${CMAKE_STATIC_LINKER_FLAGS} ${CMAKE_STATIC_LINKER_FLAGS_RELEASE}")
endmacro()

macro (ze_compiler_linker_init)
	message(STATUS "[ZEBuild] Determining compiler and linker parameters...")
	
	if (ZEBUILD_PLATFORM_COMPILER STREQUAL "MSVC")
		ze_compiler_linker_msvc()
		if (CMAKE_GENERATOR_TOOLSET MATCHES "Intel")
			ze_compiler_linker_msvc_icc()
		endif()
	endif()
	
	ze_compiler_linker_print_flags()
	
	message(STATUS "")
	message(STATUS "")
endmacro()

ze_compiler_linker_init()
