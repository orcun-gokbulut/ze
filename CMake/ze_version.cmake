#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_version.cmake
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

macro(ze_version_get)
	file(READ "${ARGV0}/Version.txt" VERSION_STRING)

	set(VERSION_MAJOR 0)
	if ("${VERSION_STRING}" MATCHES "[ \\t]*Version[ \\t]*:[ \\t]*([0-9]+).*")
		set(VERSION_MAJOR ${CMAKE_MATCH_1})
	endif()

	set(VERSION_MINOR 0)
	if ("${VERSION_STRING}" MATCHES "[ \\t]*Version[ \\t]*:[ \\t]*[0-9]+[ \\t]*\\.[ \\t]*([0-9]+).*")
		set(VERSION_MINOR ${CMAKE_MATCH_1})
	endif()

	set(VERSION_INTERNAL 0)
	if ("${VERSION_STRING}" MATCHES "[ \\t]*Version[ \\t]*:[ \\t]*[0-9]+[ \\t]*\\.[ \\t]*[0-9]+\\.[ \\t]*([0-9]+).*")
		set(VERSION_INTERNAL ${CMAKE_MATCH_1})
	endif()
endmacro()

macro(ze_version_get_branch)
	set (VERSION_BRANCH "")
	include(External/FindSubversion)
	Subversion_WC_INFO(${ARGV0} SVN_INFO)
	if (SVN_INFO_WC_RESULT)
		if ("${SVN_INFO_WC_URL}" MATCHES ".*/(.*)[ \\t]*")
			set(VERSION_BRANCH ${CMAKE_MATCH_1})
		endif()
	endif()
endmacro()

macro(ze_version_get_revision_number)
	set (VERSION_REVISION "0")
	include(External/FindSubversion)
	Subversion_WC_INFO(${ARGV0} SVN_INFO)
	if (SVN_INFO_WC_RESULT)
		set(VERSION_REVISION ${SVN_INFO_WC_REVISION})
	else()
		set(VERSION_REVISION "0")
	endif()
endmacro()

macro(ze_version_generate_version_txt)
	message(STATUS "[ZEBuild] Generating Version.txt.")
	file(WRITE "${CMAKE_SOURCE_DIR}/Rundir/Version.txt"
		"Company: Zinek Code House\n"
		"Product: Zinek Engine\n"
		"Version: ${ZEBUILD_VERSION_MAJOR}.${ZEBUILD_VERSION_MINOR}.${ZEBUILD_VERSION_INTERNAL} - Rev:${ZEBUILD_VERSION_REVISION} (Branch:${ZEBUILD_VERSION_BRANCH})")
endmacro()

macro(ze_version_generate_zeversiondata_h)
	message(STATUS "[ZEBuild] Version: Generating ZEVersionData.h.")
	file(WRITE "${CMAKE_BINARY_DIR}/ZEVersionData.h"
		"// ZEBuild - Auto Generated Version File - DO NOT MODIFY ! \n"
		"////////////////////////////////////////////////////////////////////////\n\n"
		"#pragma once\n\n"
		"#define ZE_VERSION \"${ZEBUILD_VERSION}\"\n"
		"#define ZE_VERSION_MAJOR ${ZEBUILD_VERSION_MAJOR}\n"
		"#define ZE_VERSION_MINOR ${ZEBUILD_VERSION_MINOR}\n"
		"#define ZE_VERSION_INTERNAL ${ZEBUILD_VERSION_INTERNAL}\n"
		"#define ZE_VERSION_REVISION ${ZEBUILD_VERSION_REVISION}\n"
		"#define ZE_VERSION_BRANCH \"${ZEBUILD_VERSION_BRANCH}\"")
endmacro()

macro(ze_version_generate_version_info PARAMETER_TARGET PARAMETER_EXTENSION PARAMETER_DESCRIPTION)
	if(ZEBUILD_PLATFORM_WINDOWS)
		if ("${CMAKE_BINARY_DIR}/LastRevision.txt" IS_NEWER_THAN "${CMAKE_CURRENT_BINARY_DIR}/ZEVersion.rc")
			message(STATUS "[ZEBuild] Generating ZEVersion.rc for ${PARAMETER_TARGET}.")
			configure_file(
				"${CMAKE_SOURCE_DIR}/CMake/ze_version.rc.in"
				"${CMAKE_CURRENT_BINARY_DIR}/ZEVersion.rc"
				@ONLY)
		endif()
		target_sources(${PARAMETER_TARGET} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/ZEVersion.rc")
		source_group("Generated" FILES "${CMAKE_CURRENT_BINARY_DIR}/ZEVersion.rc")
	endif()
endmacro()

macro(ze_version_generate_files)
	if (EXISTS "${CMAKE_BINARY_DIR}/LastRevision.txt")
		file(READ "${CMAKE_BINARY_DIR}/LastRevision.txt" LAST_REVISION)
		if (NOT (LAST_REVISION EQUAL ZEBUILD_VERSION_REVISION))
			ze_version_generate_version_txt()
			ze_version_generate_zeversiondata_h()
			file(WRITE "${CMAKE_BINARY_DIR}/LastRevision.txt" ${ZEBUILD_VERSION_REVISION})
		endif()	
	else()
		ze_version_generate_version_txt()
		ze_version_generate_zeversiondata_h()
		file(WRITE "${CMAKE_BINARY_DIR}/LastRevision.txt" ${ZEBUILD_VERSION_REVISION})
	endif()
endmacro()

macro(ze_version_generate_definitions)
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_DEFINITIONS 
		ZE_VERSION="${ZEBUILD_VERSION}"
		ZE_VERSION_MAJOR=${ZEBUILD_VERSION_MAJOR}
		ZE_VERSION_MINOR=${ZEBUILD_VERSION_MINOR}
		ZE_VERSION_INTERNAL=${ZEBUILD_VERSION_INTERNAL}
		ZE_VERSION_REVISION=${ZEBUILD_VERSION_REVISION}
		ZE_VERSION_BRANCH="${ZEBUILD_VERSION_BRANCH}")
endmacro()

macro(ze_version_init)
	message(STATUS "[ZEBuild] Collecting version information...")

	ze_version_get(${CMAKE_SOURCE_DIR})
	ze_version_get_revision_number(${CMAKE_SOURCE_DIR})
	ze_version_get_branch(${CMAKE_SOURCE_DIR})

	set(ZEBUILD_VERSION_MAJOR ${VERSION_MAJOR})
	set(ZEBUILD_VERSION_MINOR ${VERSION_MINOR})
	set(ZEBUILD_VERSION_INTERNAL ${VERSION_INTERNAL})
	set(ZEBUILD_VERSION_REVISION ${VERSION_REVISION})
	set(ZEBUILD_VERSION_BRANCH ${VERSION_BRANCH})
	set(ZEBUILD_VERSION "${ZEBUILD_VERSION_MAJOR}.${ZEBUILD_VERSION_MINOR}.${ZEBUILD_VERSION_INTERNAL} - Rev:${ZEBUILD_VERSION_REVISION} (Branch:${ZEBUILD_VERSION_BRANCH})")

	message(STATUS "[ZEBuild] Version           : ${ZEBUILD_VERSION}")
	message(STATUS "[ZEBuild] Major Version     : ${ZEBUILD_VERSION_MAJOR}")
	message(STATUS "[ZEBuild] Minor Version     : ${ZEBUILD_VERSION_MINOR}")
	message(STATUS "[ZEBuild] Internal Version  : ${ZEBUILD_VERSION_INTERNAL}")
	message(STATUS "[ZEBuild] Build             : ${ZEBUILD_VERSION_REVISION}")
	message(STATUS "[ZEBuild] Branch            : ${ZEBUILD_VERSION_BRANCH}")
	message(STATUS "[ZEBuild] Version information has been collected.")
	
	ze_version_generate_files()
	
	message(STATUS "")
	message(STATUS "")
endmacro()

ze_version_init()
