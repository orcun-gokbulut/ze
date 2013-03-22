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

	include(External/build_dir_check)
	include(External/parse_arguments)
	include(ze_check)
	include(ze_common)
	include(ze_dependency)
	include(ze_utility)
	include(ze_target)
	include(ze_platform)
	include(ze_version)
	include(ze_external)
	include(ze_merge_libraries)
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


	message(STATUS "[ZEBuild] Version           : v${ZEBUILD_VERSION_MAJOR}.${ZEBUILD_VERSION_MINOR}.${ZEBUILD_VERSION_INTERNAL} Build ${ZEBUILD_VERSION_BUILD}")
	message(STATUS "[ZEBuild] Major Version     : ${ZEBUILD_VERSION_MAJOR}")
	message(STATUS "[ZEBuild] Minor Version     : ${ZEBUILD_VERSION_MINOR}")
	message(STATUS "[ZEBuild] Internal Version  : ${ZEBUILD_VERSION_INTERNAL}")
	message(STATUS "[ZEBuild] Build             : ${ZEBUILD_VERSION_BUILD}")
	message(STATUS "[ZEBuild] Branch Name       : ${ZEBUILD_VERSION_BRANCH_NAME}")
	message(STATUS "[ZEBuild] Version information has been collected.")
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
	elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(ZEBUILD_PLATFORM "MacOSX")
	elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD") 
		set(ZEBUILD_PLATFORM "BSD")
	else()
		message(FATAL_ERROR "[ZEBuild] Error: Platform is not supported. Operating System : ${CMAKE_SYSTEM_NAME}, Achitecture : ${CMAKE_SYSTEM_PROCESSOR}, Compiler : ${CMAKE_CXX_COMPILER}")
	endif()
	
	if (ANDROID)
		set(ZEBUILD_PLATFORM "Android")
		set(ZEBUILD_PLATFORM_WORD_SIZE "32")
		set(ZEBUILD_PLATFORM_ENDIANNESS "Little")
		set(ZEBUILD_PLATFORM_COMPILER "gcc")
		set(ZEBUILD_PLATFORM_ARCHITECTURE "ARM")
	elseif (XBOX360)
		set(ZEBUILD_PLATFORM "Xbox360")
		set(ZEBUILD_PLATFORM_WORD_SIZE "32")
		set(ZEBUILD_PLATFORM_ENDIANNESS "Little")
		set(ZEBUILD_PLATFORM_COMPILER "MSVC")
		set(ZEBUILD_PLATFORM_ARCHITECTURE "PPC")	
	elseif (PS3)
		set(ZEBUILD_PLATFORM "PS3")
		set(ZEBUILD_PLATFORM_WORD_SIZE "32")
		set(ZEBUILD_PLATFORM_ENDIANNESS "Big")
		set(ZEBUILD_PLATFORM_COMPILER "gcc")
		set(ZEBUILD_PLATFORM_ARCHITECTURE "PPC")
	elseif (IOS)
		if (${IOS_PLATFORM} STREQUAL "OS")
			set(ZEBUILD_PLATFORM "iOS")
			set(ZEBUILD_PLATFORM_WORD_SIZE "32")
			set(ZEBUILD_PLATFORM_ENDIANNESS "Little")
			set(ZEBUILD_PLATFORM_COMPILER "clang")
			set(ZEBUILD_PLATFORM_ARCHITECTURE "ARM")
		else()
			set(ZEBUILD_PLATFORM "iOS_Simulator")
			set(ZEBUILD_PLATFORM_WORD_SIZE "32")
			set(ZEBUILD_PLATFORM_ENDIANNESS "Little")
			set(ZEBUILD_PLATFORM_COMPILER "clang")
			if (CMAKE_SIZEOF_VOID_P EQUAL 4)
				set(ZEBUILD_PLATFORM_WORD_SIZE "32")
				set(ZEBUILD_PLATFORM_ARCHITECTURE "x86")
			else()
				set(ZEBUILD_PLATFORM_WORD_SIZE "64")
				set(ZEBUILD_PLATFORM_ARCHITECTURE "x64")
			endif()
		endif()
	else()
		if (CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(ZEBUILD_PLATFORM_WORD_SIZE "32")
		else()
			set(ZEBUILD_PLATFORM_WORD_SIZE "64")
		endif()
	
		if (WIN32 OR
		    (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_32") OR
			(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86") OR
			(${CMAKE_SYSTEM_PROCESSOR} MATCHES "i386") OR
			(${CMAKE_SYSTEM_PROCESSOR} MATCHES "i486") OR
			(${CMAKE_SYSTEM_PROCESSOR} MATCHES "i586") OR
			(${CMAKE_SYSTEM_PROCESSOR} MATCHES "i686"))
			if (${ZEBUILD_PLATFORM_WORD_SIZE} MATCHES "32")
				set(ZEBUILD_PLATFORM_ARCHITECTURE "x86")
			else()
				set(ZEBUILD_PLATFORM_ARCHITECTURE "x64")
			endif()
		elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
			set(ZEBUILD_PLATFORM_ARCHITECTURE "x64")
		elseif(IOS)
			set(ZEBUILD_PLATFORM_ARCHITECTURE "ARM")
		else()
			message(SEND_ERROR "[ZEBuild] Error: Unsupported architecture. Processor Name : ${CMAKE_SYSTEM_PROCESSOR}")
		endif()
		
		if (CMAKE_COMPILER_IS_GNUCC)
			set(ZEBUILD_PLATFORM_COMPILER "GCC")
		elseif (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
			set(ZEBUILD_PLATFORM_COMPILER "Clang")
		else()
			if (CMAKE_CXX_COMPILER MATCHES "cl")
				set(ZEBUILD_PLATFORM_COMPILER "MSVC")
			else()
				message(WARNING "[ZEBuild] Warning: Compiler is not supported. Compiler Name : ${CMAKE_CXX_COMPILER}")
				set(ZEBUILD_PLATFORM_COMPILER "UNKNOWN")
			endif()
		endif()

		include(TestBigEndian)
		TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
		if (IS_BIG_ENDIAN)
			set(ZEBUILD_PLATFORM_ENDIANNESS "Big")
		else()
			set(ZEBUILD_PLATFORM_ENDIANNESS "Little")
		endif()
		
		if (ZEBUILD_PLATFORM_ARCHITECTURE MATCHES "x86" OR ZEBUILD_PLATFORM_ARCHITECTURE MATCHES "x64")
			set(ZEBUILD_PLATFORM_ARCHITECTURE_SSE3 TRUE CACHE BOOL "Enable SSE/SSE2/SSE3 support for x86 and x64 architectures.")
			set(ZEBUILD_PLATFORM_ARCHITECTURE_SSE4 FALSE CACHE BOOL "Enable SSE/SSE2/SSE3/SSE4 support for x86 and x64 architectures.")

			if (ZEBUILD_PLATFORM_COMPILER MATCHES "gcc" OR ZEBUILD_PLATFORM_COMPILER MATCHES "clang")
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
	endif()

	if (ZEBUIDL_PLATFORM_IOS_SIMULATOR)
		set(ZEBUILD_PLATFORM_IOS TRUE)
	endif()
	
	if (ZEBUILD_PLATFORM MATCHES "XBOX360")
		set(ZEBUILD_PLATFORM_WINDOWS TRUE)
	elseif (ZEBUILD_PLATFORM MATCHES "MACOSX")
		set(ZEBUILD_PLATFORM_BSD TRUE)
		set(ZEBUILD_PLATFORM_UNIX TRUE)
		set(ZEBUILD_PLATFORM_DARWIN TRUE)
	elseif (ZEBUILD_PLATFORM MATCHES "iOS_Simulator")
		set(ZEBUILD_PLATFORM_IOS TRUE)
		set(ZEBUILD_PLATFORM_BSD TRUE)
		set(ZEBUILD_PLATFORM_UNIX TRUE)
		set(ZEBUILD_PLATFORM_DARWIN TRUE)
	elseif (ZEBUILD_PLATFORM MATCHES "iOS")
		set(ZEBUILD_PLATFORM_BSD TRUE)
		set(ZEBUILD_PLATFORM_UNIX TRUE)
		set(ZEBUILD_PLATFORM_DARWIN TRUE)
	elseif (ZEBUILD_PLATFORM MATCHES "Android")
		set(ZEBUILD_PLATFORM_LINUX TRUE)
		set(ZEBUILD_PLATFORM_UNIX TRUE)
	elseif (ZEBUILD_PLATFORM MATCHES "Linux")
		set(ZEBUILD_PLATFORM_UNIX TRUE)
	elseif (ZEBUILD_PLATFORM MATCHES "BSD")
		set(ZEBUILD_PLATFORM_UNIX TRUE)
	endif()
		

	string(TOUPPER ${ZEBUILD_PLATFORM} TEMP)
	set(ZEBUILD_PLATFORM_${TEMP} TRUE)

	string(TOUPPER ${ZEBUILD_PLATFORM_ARCHITECTURE} TEMP)
	set(ZEBUILD_PLATFORM_ARCHITECTURE_${TEMP} TRUE)

	string(TOUPPER ${ZEBUILD_PLATFORM_WORD_SIZE} TEMP)
	set(ZEBUILD_PLATFORM_WORD_SIZE_${TEMP} TRUE)

	string(TOUPPER ${ZEBUILD_PLATFORM_ENDIANNESS} TEMP)
	set(ZEBUILD_PLATFORM_ENDIANNESS_${TEMP} TRUE)

	string(TOUPPER ${ZEBUILD_PLATFORM_COMPILER} TEMP)
	set(ZEBUILD_PLATFORM_COMPILER_${TEMP} TRUE)
	
	string(TOUPPER ${ZEBUILD_PLATFORM} ZEBUILD_PLATFORM_UPPER_CASE)
	string(TOUPPER ${ZEBUILD_PLATFORM_ARCHITECTURE} ZEBUILD_PLATFORM_ARCHITECTURE_UPPER_CASE)
	string(TOUPPER ${ZEBUILD_PLATFORM_COMPILER} ZEBUILD_PLATFORM_COMPILER_UPPER_CASE)
	string(TOUPPER ${ZEBUILD_PLATFORM_ENDIANNESS} ZEBUILD_PLATFORM_ENDIANNESS_UPPER_CASE)


	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS 	
		"ZE_PLATFORM_${ZEBUILD_PLATFORM_UPPER_CASE}"
		ZE_PLATFORM="${ZEBUILD_PLATFORM}"
		"ZE_PLATFORM_ARCHITECTURE_${ZEBUILD_PLATFORM_ARCHITECTURE_UPPER_CASE}"
		ZE_PLATFORM_ARCHITECTURE="${ZEBUILD_PLATFORM_ARCHITECTURE}"
		"ZE_PLATFORM_COMPILER_${ZEBUILD_PLATFORM_COMPILER_UPPER_CASE}"
		ZE_PLATFORM_COMPILER="${ZEBUILD_PLATFORM_COMPILER}"
		"ZE_PLATFORM_ENDIANNESS_${ZEBUILD_PLATFORM_ENDIANNESS_UPPER_CASE}"
		ZE_PLATFORM_ENDIANNESS="${ZEBUILD_PLATFORM_ENDIANNESS}"
		"ZE_PLATFORM_WORD_SIZE=${ZEBUILD_PLATFORM_WORD_SIZE}"
		ZE_PLATFORM_WORD_SIZE_${ZEBUILD_PLATFORM_WORD_SIZE})

	if (ZEBUILD_PLATFORM_UNIX)
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_UNIX)
	endif()
	
	if (ZEBUILD_PLATFORM_WINDOWS)
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_WINDOWS)
	endif()
    
    if (ZEBUILD_PLATFORM_DARWIN)      
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_DARWIN)
    endif()

	if (ZEBUILD_PLATFORM_BSD)      
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_BSD)
    endif()

	if (ZEBUILD_PLATFORM_LINUX)      
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_LINUX)
    endif()

	if (ZEBUILD_PLATFORM_ANDROID)      
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_ANDROID)
    endif()

	if (ZEBUILD_PLATFORM_IOS)      
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_IOS)
    endif()

	if (ZEBUILD_PLATFORM_XBOX360)      
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_XBOX360)
    endif()

	if (ZEBUILD_PLATFORM_PS3)      
		ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_PLATFORM_PS3)
    endif()
	
	if (ZEBUILD_PLATFORM_WINDOWS)
		set(ZEBUILD_PLATFORM_DLL_EXTENSION "dll")
		set(ZEBUILD_PLATFORM_LIB_EXTENSION "lib")
		set(ZEBUILD_PLATFORM_BIN_EXTENSION "exe")
	else (ZEBUILD_PLATFORM_UNIX)
		set(ZEBUILD_PLATFORM_DLL_EXTENSION "so")
		set(ZEBUILD_PLATFORM_LIB_EXTENSION "a")
		set(ZEBUILD_PLATFORM_BIN_EXTENSION "")
	endif()
	
	set(ZEBUILD_PLATFORM_NAME "${ZEBUILD_PLATFORM}-${ZEBUILD_PLATFORM_ARCHITECTURE}")
	
	message(STATUS "[ZEBuild] Platform      : ${ZEBUILD_PLATFORM}")
	if (ZEBUILD_PLATFORM_IOS)
		message(STATUS "[ZEBuild] Base Platform : iOS")
	endif()
	if (ZEBUILD_PLATFORM_WINDOWS)
		message(STATUS "[ZEBuild] Base Platform : Windows")
	endif()
	if (ZEBUILD_PLATFORM_LINUX)
		message(STATUS "[ZEBuild] Base Platform : Linux")
	endif()
	if (ZEBUILD_PLATFORM_DARWIN)
		message(STATUS "[ZEBuild] Base Platform : Darwin")
	endif()
	if (ZEBUILD_PLATFORM_BSD)
		message(STATUS "[ZEBuild] Base Platform : BSD")
	endif()
	if (ZEBUILD_PLATFORM_UNIX)
		message(STATUS "[ZEBuild] Base Platform : Unix")
	endif()
		
	message(STATUS "[ZEBuild] Architecture  : ${ZEBUILD_PLATFORM_ARCHITECTURE}")
	message(STATUS "[ZEBuild] Platform Name : ${ZEBUILD_PLATFORM_NAME}")
	message(STATUS "[ZEBuild] Word Size     : ${ZEBUILD_PLATFORM_WORD_SIZE}")
	message(STATUS "[ZEBuild] Endianness    : ${ZEBUILD_PLATFORM_ENDIANNESS}")
	message(STATUS "[ZEBuild] Compiler      : ${ZEBUILD_PLATFORM_COMPILER}")
	message(STATUS "[ZEBuild] Lib Extension : ${ZEBUILD_PLATFORM_LIB_EXTENSION}")
	message(STATUS "[ZEBuild] DLL Extension : ${ZEBUILD_PLATFORM_DLL_EXTENSION}")
	message(STATUS "[ZEBuild] Bin Extension : ${ZEBUILD_PLATFORM_BIN_EXTENSION}")
	message(STATUS "[ZEBuild] Platform has been detected.")
	message(STATUS "")
	message(STATUS "")
endmacro()

macro (ze_configure_external)
	message(STATUS "[ZEBUILD] Checking Externals...")
	
	if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/External/${ZEBUILD_PLATFORM_NAME}")
		message(WARNING "External/${ZEBUILD_PLATFORM_NAME} folder is missing.")
	else()
		ze_get_version(${CMAKE_SOURCE_DIR})
		ze_get_version_revision_number(${CMAKE_SOURCE_DIR})
	
		message(STATUS "[ZEBuild] External Version             : " "v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_INTERNAL} Build ${VERSION_BUILD}")
		message(STATUS "[ZEBuild] External Major Version       : " ${VERSION_MAJOR})
		message(STATUS "[ZEBuild] External Minor Version       : " ${VERSION_MINOR})
		message(STATUS "[ZEBuild] External Internal Version    : " ${VERSION_INTERNAL})
		message(STATUS "[ZEBuild] External Build               : " ${VERSION_BUILD})
	
		set(ZEBUILD_EXTERNALS_DISABLE_VERSION_CHECK FALSE CACHE BOOL "Disables external version check.")
		if (NOT ZEBUILD_EXTERNALS_DISABLE_VERSION_CHECK)
			if (NOT (VERSION_MAJOR EQUAL ZEBUILD_VERSION_MAJOR))
				message(FATAL_ERROR "[ZEBuild] Error : External major version mismatch.")
			endif()
			
			if (NOT (VERSION_MINOR EQUAL ZEBUILD_VERSION_MINOR))
				message(WARNING "[ZEBuild] Error : External minor version mismatch.")
			endif()
		endif()

		set(ZEBUILD_EXTERNALS_PATH "${CMAKE_SOURCE_DIR}/External/${ZEBUILD_PLATFORM_NAME}")

		message(STATUS "[ZEBuild] Externals has been checked.")
		message(STATUS "")
		message(STATUS "")

		message(STATUS "[ZEBuild] Initializing Externals...")
		file(GLOB VARIABLE_DIRECTORIES "${ZEBUILD_EXTERNALS_PATH}/*")
		foreach(VARIABLE_DIRECTORY ${VARIABLE_DIRECTORIES})
			if ((IS_DIRECTORY "${VARIABLE_DIRECTORY}") AND (EXISTS "${VARIABLE_DIRECTORY}/CMakeLists.txt"))
				add_subdirectory(${VARIABLE_DIRECTORY})
			endif()		
		endforeach()

		message(STATUS "[ZEBuild] Externals initialized.")
		message(STATUS "")
		message(STATUS "")
	endif()
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
		include(External/test_cocoon)
	endif()
endmacro()

macro(ze_configure_static_checks)
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

macro(ze_configure_symbol_server)
	set(ZEBUILD_SYMBOL_SERVER_ENABLE FALSE CACHE BOOL "Upload symbols to symbol server.")
	set(ZEBUILD_SYMBOL_SERVER_ADDRESS "\\\\Server\\Symbols" CACHE STRING "Symbol server address.")
	set(ZEBUILD_SYMBOL_SERVER_SOURCE_INDEX FALSE CACHE BOOL "Index code in sybols with svn address.")
endmacro()
