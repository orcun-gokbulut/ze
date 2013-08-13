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

macro (ze_version_init)
	message(STATUS "[ZEBuild] Collecting version information...")

	ze_version_get(${CMAKE_SOURCE_DIR})
	ze_version_get_revision_number(${CMAKE_SOURCE_DIR})
	ze_version_get_branch(${CMAKE_SOURCE_DIR})

	set(ZEBUILD_VERSION_MAJOR ${VERSION_MAJOR})
	set(ZEBUILD_VERSION_MINOR ${VERSION_MINOR})
	set(ZEBUILD_VERSION_INTERNAL ${VERSION_INTERNAL})
	set(ZEBUILD_VERSION_REVISION ${VERSION_REVISION})
	set(ZEBUILD_VERSION_BRANCH ${VERSION_BRANCH})


	message(STATUS "[ZEBuild] Version           : v${ZEBUILD_VERSION_MAJOR}.${ZEBUILD_VERSION_MINOR}.${ZEBUILD_VERSION_INTERNAL} Build ${ZEBUILD_VERSION_BUILD}")
	message(STATUS "[ZEBuild] Major Version     : ${ZEBUILD_VERSION_MAJOR}")
	message(STATUS "[ZEBuild] Minor Version     : ${ZEBUILD_VERSION_MINOR}")
	message(STATUS "[ZEBuild] Internal Version  : ${ZEBUILD_VERSION_INTERNAL}")
	message(STATUS "[ZEBuild] Build             : ${ZEBUILD_VERSION_REVISION}")
	message(STATUS "[ZEBuild] Branch            : ${ZEBUILD_VERSION_BRANCH}")
	message(STATUS "[ZEBuild] Version information has been collected.")
	message(STATUS "")
	message(STATUS "")
	
	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS 
		ZE_ZINEK
		ZE_VERSION_MAJOR=${ZEBUILD_VERSION_MAJOR}
		ZE_VERSION_MINOR=${ZEBUILD_VERSION_MINOR}
		ZE_VERSION_INTERNAL=${ZEBUILD_VERSION_INTERNAL}
		ZE_VERSION_REVISION=${ZEBUILD_VERSION_REVISION}
		ZE_VERSION_BRANCH="${ZEBUILD_VERSION_BRANCH}")
endmacro()
