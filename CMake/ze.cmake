#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze.cmake
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

macro (ze_configure_init)
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

	include(build_dir_check)
	include(parse_arguments)
	include(ze_functions)

	macro_ensure_out_of_source_build("[ZEBuild] Error : Project requires an out of source build.")
endmacro()

macro (ze_configure_version)
	message(STATUS "[ZEBuild] Collecting version information...")

	ze_get_version(${CMAKE_SOURCE_DIR})
	ze_get_version_revision_number(${CMAKE_SOURCE_DIR})
	ze_get_version_branch_name(${CMAKE_SOURCE_DIR})

	set(ZEBUILD_VERSION_MAJOR ${VERSION_MAJOR})
	set(ZEBUILD_VERSION_MINOR ${VERSION_MINOR})
	set(ZEBUILD_VERSION_INTERNAL ${VERSION_INTERNAL})
	set(ZEBUILD_VERSION_BUILD ${VERSION_REVISION})
	set(ZEBUILD_VERSION_BRANCH_NAME ${VERSION_BRANCH_NAME})
	message(STATUS "[ZEBuild] Version information has been collected.")
	message(STATUS "")

	message(STATUS "[ZEBuild] Version           : " "v${ZEBUILD_VERSION_MAJOR}.${ZEBUILD_VERSION_MINOR}.${ZEBUILD_VERSION_INTERNAL} Build ${ZEBUILD_VERSION_BUILD}")
	message(STATUS "[ZEBuild] Major Version     : " ${ZEBUILD_VERSION_MAJOR})
	message(STATUS "[ZEBuild] Minor Version     : " ${ZEBUILD_VERSION_MINOR})
	message(STATUS "[ZEBuild] Internal Version  : " ${ZEBUILD_VERSION_INTERNAL})
	message(STATUS "[ZEBuild] Build             : " ${ZEBUILD_VERSION_BUILD})
	message(STATUS "[ZEBuild] Branch Name       : " ${ZEBUILD_VERSION_BRANCH_NAME})
	message(STATUS "")
	message(STATUS "")
	
	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS 
		ZE_ZINEK
		ZE_VERSION_MAJOR=${ZEBUILD_VERSION_MAJOR}
		ZE_VERSION_MINOR=${ZEBUILD_VERSION_MINOR}
		ZE_VERSION_INTERNAL=${ZEBUILD_VERSION_INTERNAL}
		ZE_VERSION_BUILD=${ZEBUILD_VERSION_BUILD}
		ZE_VERSION_BRANCH_NAME=${ZEBUILD_VERSION_BRANCH_NAME})
endmacro()

macro (ze_configure_platform)
	message(STATUS "[ZEBuild] Detecting platform...")

	if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		set(ZEBUILD_PLATFORM "Windows")
	elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(ZEBUILD_PLATFORM "Linux")
	elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR 
		${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD") 
		set(ZEBUILD_PLATFORM "BSD")
	else()
		message(SEND_ERROR "[ZEBuild] Error: Platform is not supported. Operating System : ${CMAKE_SYSTEM_NAME}, Achitecture : ${CMAKE_SYSTEM_PROCESSOR}, Compiler : ${CMAKE_CXX_COMPILER}")
	endif()

	if (ZEBUILD_PLATFORM MATCHES "Linux" OR
		ZEBUILD_PLATFORM MATCHES "BSD" OR
		ZEBUILD_PLATFORM MATCHES "MacOSX" OR
		ZEBUILD_PLATFORM MATCHES "iOS")
		set(ZEBUILD_PLATFORM_BASE "Unix")
	elseif(ZEBUILD_PLATFORM MATCHES "Windows")
		set(ZEBUILD_PLATFORM_BASE "Windows")
	endif()

	if (CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(ZEBUILD_PLATFORM_WORD_SIZE "32")
	else()
		set(ZEBUILD_PLATFORM_WORD_SIZE "64")
	endif()
		
	if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_32" OR
		${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86" OR
		${CMAKE_SYSTEM_PROCESSOR} MATCHES "i386" OR
		${CMAKE_SYSTEM_PROCESSOR} MATCHES "i486" OR
		${CMAKE_SYSTEM_PROCESSOR} MATCHES "i586" OR
		${CMAKE_SYSTEM_PROCESSOR} MATCHES "i686")
		if (${ZEBUILD_PLATFORM_WORD_SIZE} MATCHES "32")
			set(ZEBUILD_PLATFORM_ARCHITECTURE "x86")
		else()
			set(ZEBUILD_PLATFORM_ARCHITECTURE "x64")
		endif()
	elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
		set(ZEBUILD_PLATFORM_ARCHITECTURE "x64")
	else()
		message(SEND_ERROR "[ZEBuild] Error: Unsupported architecture. Processor Name : ${CMAKE_SYSTEM_PROCESSOR}")
	endif()
		
	if (CMAKE_COMPILER_IS_GNUCC)
		set(ZEBUILD_PLATFORM_COMPILER "GCC")
	else()
		if (CMAKE_CXX_COMPILER MATCHES "cl")
			set(ZEBUILD_PLATFORM_COMPILER "MSVC")
		else()
			message(SEND_ERROR "[ZEBuild] Error: Compiler is not supported. Compiler Name : ${CMAKE_CXX_COMPILER}")
		endif()
	endif()

	include(TestBigEndian)
	TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
	if (IS_BIG_ENDIAN)
		set(ZEBUILD_PLATFORM_ENDIANNESS "Big")
	else()
		set(ZEBUILD_PLATFORM_ENDIANNESS "Little")
	endif()

	message(STATUS "[ZEBuild] Platform has been detected.")
	message(STATUS "")

	message(STATUS "[ZEBuild] Platform      : " ${ZEBUILD_PLATFORM})
	message(STATUS "[ZEBuild] Base Platform : " ${ZEBUILD_PLATFORM_BASE})
	message(STATUS "[ZEBuild] Architecture  : " ${ZEBUILD_PLATFORM_ARCHITECTURE})
	message(STATUS "[ZEBuild] Word Size     : " ${ZEBUILD_PLATFORM_WORD_SIZE})
	message(STATUS "[ZEBuild] Endianness    : " ${ZEBUILD_PLATFORM_ENDIANNESS})
	message(STATUS "[ZEBuild] Compiler      : " ${ZEBUILD_PLATFORM_COMPILER})
	message(STATUS "")
	message(STATUS "")

	string(TOUPPER ${ZEBUILD_PLATFORM} TEMP)
	set(ZEBUILD_PLATFORM_${TEMP} "${ZEBUILD_PLATFORM}")

	string(TOUPPER ${ZEBUILD_PLATFORM_BASE} TEMP)
	set(ZEBUILD_PLATFORM_${TEMP} "${ZEBUILD_PLATFORM_BASE}")

	string(TOUPPER ${ZEBUILD_PLATFORM_ARCHITECTURE} TEMP)
	set(ZEBUILD_PLATFORM_ARCHITECTURE_${TEMP} "${ZEBUILD_PLATFORM_ARCHITECTURE}")

	string(TOUPPER ${ZEBUILD_PLATFORM_WORD_SIZE} TEMP)
	set(ZEBUILD_PLATFORM_WORD_SIZE_${TEMP} "${ZEBUILD_PLATFORM_WORD_SIZE}")

	string(TOUPPER ${ZEBUILD_PLATFORM_ENDIANNESS} TEMP)
	set(ZEBUILD_PLATFORM_ENDIANNESS_${TEMP} "${ZEBUILD_PLATFORM_ENDIANNESS}")

	string(TOUPPER ${ZEBUILD_PLATFORM_COMPILER} TEMP)
	set(ZEBUILD_PLATFORM_COMPILER_${TEMP} "${ZEBUILD_PLATFORM_COMPILER}")
	
	string(TOUPPER ${ZEBUILD_PLATFORM} ZEBUILD_PLATFORM_UPPER_CASE)
	string(TOUPPER ${ZEBUILD_PLATFORM_BASE} ZEBUILD_PLATFORM_BASE_UPPER_CASE)
	string(TOUPPER ${ZEBUILD_PLATFORM_ARCHITECTURE} ZEBUILD_PLATFORM_ARCHITECTURE_UPPER_CASE)
	string(TOUPPER ${ZEBUILD_PLATFORM_COMPILER} ZEBUILD_PLATFORM_COMPILER_UPPER_CASE)
	string(TOUPPER ${ZEBUILD_PLATFORM_ENDIANNESS} ZEBUILD_PLATFORM_ENDIANNESS_UPPER_CASE)


	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS 	
		"ZE_PLATFORM_${ZEBUILD_PLATFORM_UPPER_CASE}"
		"ZE_PLATFORM_${ZEBUILD_PLATFORM_BASE_UPPER_CASE}"
		"ZE_PLATFORM_ARCHITECTURE_${ZEBUILD_PLATFORM_ARCHITECTURE_UPPER_CASE}"
		"ZE_PLATFORM_COMPILER_${ZEBUILD_PLATFORM_COMPILER_UPPER_CASE}"
		"ZE_PLATFORM_ENDIANNESS_${ZEBUILD_PLATFORM_ENDIANNESS_UPPER_CASE}"

		ZE_PLATFORM="${ZEBUILD_PLATFORM}"
		ZE_PLATFORM_ARCHITECTURE="${ZEBUILD_PLATFORM_ARCHITECTURE}"
		ZE_PLATFORM_COMPILER="${ZEBUILD_PLATFORM_COMPILER}"
		ZE_PLATFORM_ENDIANNESS="${ZEBUILD_PLATFORM_ENDIANNESS}"
		ZE_PLATFORM_WORD_SIZE=${ZEBUILD_PLATFORM_WORD_SIZE})

	if (ZEBUILD_PLATFORM_UNIX)
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_UNIX)
	elseif (ZEBUILD_PLATFORM_WINDOWS)
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_WINDOWS)
	endif()
	
	if(ZEBUILD_PLATFORM_ARCHITECTURE_X86 OR ZEBUILD_PLATFORM_ARCHITECTURE_X64)
		set(ZEBUILD_PLATFORM_ARCHITECTURE_SSE3             TRUE CACHE BOOL "Enable SSE/SSE2/SSE3 support for x86 and x64 architectures.")
		set(ZEBUILD_PLATFORM_ARCHITECTURE_SSE4             FALSE CACHE BOOL "Enable SSE/SSE2/SSE3/SSE4 support for x86 and x64 architectures.")

		if (ZEBUILD_PLATFORM_COMPILER_GCC)
			if (ZEBUILD_PLATFORM_ARCHITECTURE_SSE4)
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3 -msse4")
			elseif (ZEBUILD_PLATFORM_ARCHITECTURE_SSE3)
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3")
			endif()
		endif()

		if (ZEBUILD_PLATFORM_ARCHITECTURE_SSE4)
			ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS 
				ZE_PLATFORM_ARCHITECTURE_SSE4 
				ZE_PLATFORM_ARCHITECTURE_SSE3)
		elseif (ZEBUILD_PLATFORM_ARCHITECTURE_SSE3)
			ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS 
				ZE_PLATFORM_ARCHITECTURE_SSE3)
		endif()
	endif()
endmacro()

macro (ze_configure_externals)
	message(STATUS "[ZEBUILD] Checking externals...")
	ze_check_externals("${CMAKE_SOURCE_DIR}/Platform" 0 0)
	if (CHECK_EXTERNALS_RESULT EQUAL 1) # Not found
		message(SEND_ERROR "[ZEBuild] Error: Could not dectect externals. Please check Platforms directory that you have the external.")
	elseif (CHECK_EXTERNALS_RESULT EQUAL 2) # version Mismatch
		message(SEND_ERROR "[ZEBuild] Error: Externals version is mismatched.")
	endif() 
	message(STATUS "[ZEBuild] Externals has been checked.")
	message(STATUS "")

	ze_get_version(${CMAKE_SOURCE_DIR})
	ze_get_version_revision_number(${CMAKE_SOURCE_DIR})

	message(STATUS "[ZEBuild] Externals Version             : " "v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_INTERNAL} Build ${VERSION_BUILD}")
	message(STATUS "[ZEBuild] Externals Major Version       : " ${VERSION_MAJOR})
	message(STATUS "[ZEBuild] Externals Minor Version       : " ${VERSION_MINOR})
	message(STATUS "[ZEBuild] Externals Internal Version    : " ${VERSION_INTERNAL})
	message(STATUS "[ZEBuild] Externals Build               : " ${VERSION_BUILD})
	message(STATUS "")
	message(STATUS "")
	
    set(ZEBUILD_EXTERNAL_PATH ${CMAKE_SOURCE_DIR}/Platform/${ZEBUILD_PLATFORM}/${ZEBUILD_PLATFORM_ARCHITECTURE})
	set(ZEBUILD_INCLUDE_PATH ${ZEBUILD_EXTERNAL_PATH}/Include)
	set(ZEBUILD_LIB_PATH ${ZEBUILD_EXTERNAL_PATH}/Lib)
	set(ZEBUILD_DLL_PATH ${ZEBUILD_EXTERNAL_PATH}/Dll)
	set(ZEBUILD_TOOL_PATH ${ZEBUILD_EXTERNAL_PATH}/Tool)
	
	include_directories(
		${ZEBUILD_INCLUDE_PATH})
		
	link_directories(
		${ZEBUILD_LIB_PATH}
		${ZEBUILD_LIB_PATH}/$(ConfigurationName))
endmacro()

macro (ze_configure_debugging)
	set(ZEBUILD_DEBUG_ENABLE                  TRUE  CACHE BOOL "Enable debug extensions.")
	set(ZEBUILD_DEBUG_HEAP_CHECK_ENABLE       FALSE CACHE BOOL "Enable heap checking.")
	set(ZEBUILD_DEBUG_BREAK_ON_ERROR          TRUE  CACHE BOOL "Break if an error occures.")
	set(ZEBUILD_DEBUG_BREAK_ON_WARNING        FALSE CACHE BOOL "Break if an warning occures.")
	set(ZEBUILD_VERBOSE	                      FALSE CACHE BOOL "Enable ZEBuild verbose mode.")

	if (ZEBUILD_DEBUG_ENABLE)
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG ZE_DEBUG_ENABLE)
		
		if (ZEBUILD_DEBUG_BREAK_ON_ERROR)
			ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG ZE_DEBUG_BREAK_ON_ERROR)
		endif()

		if (ZEBUILD_DEBUG_BREAK_ON_WARNING)
			ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG _ZE_DEBUG_BREAK_ON_WARNING)
		endif()

		if (ZEBUILD_DEBUG_HEAP_CHECK_ENABLE)
			ze_append_property(GLOBAL PROPERTY COMPILE_DEFINITIONS_DEBUG ZE_DEBUG_MEMORY_CHECKS_ENABLE)
		endif()
	endif()
endmacro()

macro(ze_configure_unit_tests)
	set(ZEBUILD_UNIT_TESTS_ENABLE             FALSE CACHE BOOL "Enable unit tests.")
	set(ZEBUILD_UNIT_TESTS_COVERAGE_ENABLE    FALSE CACHE BOOL "Enables code coverage instrumentation.")
	
	if (ZEBUILD_UNIT_TESTS_ENABLE)
		ENABLE_TESTING()
	endif()
	
	if (ZEBUILD_UNIT_TESTS_ENABLE AND ZEBUILD_UNIT_TESTS_COVERAGE_ENABLE)
		include(test_cocoon)
	endif()
endmacro()

macro(ze_configure_static_checks)
	set(ZEBUILD_STATIC_CHECKS_ENABLE          FALSE CACHE BOOL "Enable static checks.")
	set(ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK FALSE CACHE BOOL "Enable cppcheck static checker.")
	set(ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT   FALSE CACHE BOOL "Enable pc-lint static checker.")
	
	if (ZEBUILD_STATIC_CHECKS_ENABLE AND ZEBUILD_STATIC_CHECKS_ENABLE_CPPCHECK)
		include(cppcheck.cmake)
	endif()

	if (ZEBUILD_STATIC_CHECKS_ENABLE AND ZEBUILD_STATIC_CHECKS_ENABLE_PCLINT)
		include(pc-lint.cmake)
	endif()
endmacro()

macro(ze_configure_symbol_server)
	set(ZEBUILD_SYMBOL_SERVER_ENABLE FALSE CACHE BOOL "Upload symbols to symbol server.")
	set(ZEBUILD_SYMBOL_SERVER_ADDRESS "\\\\Server\\Symbols" CACHE STRING "Symbol server address.")
	set(ZEBUILD_SYMBOL_SERVER_SOURCE_INDEX FALSE CACHE BOOL "Index code in sybols with svn address.")
endmacro()
