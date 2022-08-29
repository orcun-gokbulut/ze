#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - CMakeLists.txt
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


project (ZEFoundation)

ze_set_project_folder("ZEFoundation")

set(FoundationMetaHeaders)
macro(ze_add_foundation_zemc)
	foreach (VARIABLE_FILE ${ARGN})
		set(TEMP_LIST ${TEMP_LIST} "${CMAKE_CURRENT_SOURCE_DIR}/${VARIABLE_FILE}")
	endforeach()
	set(FoundationMetaHeaders ${FoundationMetaHeaders} ${TEMP_LIST} PARENT_SCOPE)
endmacro()

add_subdirectory(ZEDS)
add_subdirectory(ZEMath)
add_subdirectory(ZESerialization)
add_subdirectory(ZEStateMachine)
add_subdirectory(ZEFile)
add_subdirectory(ZEMacro)
add_subdirectory(ZESpatial)
add_subdirectory(ZESerialPort)
add_subdirectory(ZETest)
add_subdirectory(ZEThread)
add_subdirectory(ZESocket)
add_subdirectory(ZEML)
add_subdirectory(ZERegEx)
add_subdirectory(ZECompression)
add_subdirectory(ZEProtect)
add_subdirectory(ZEMeta)
add_subdirectory(ZEPointer)
add_subdirectory(ZEITIntegrity)

ze_add_source(ZEData.h 							Sources Headers)
ze_add_source(ZEData.cpp 						Sources)
ze_add_source(ZEDLL.h 							Sources Headers)
ze_add_source(ZEDLL.cpp 						Sources)
ze_add_source(ZECommon.h 						Sources Headers)
ze_add_source(ZECommon.cpp 						Sources)
ze_add_source(ZEDestroyable.cpp					Sources Headers)
ze_add_source(ZEDestroyable.h					Sources)
ze_add_source(ZEDestroyableCollector.cpp		Sources Headers)
ze_add_source(ZEDestroyableCollector.h			Sources)
ze_add_source(ZEGUID.h 							Sources Headers)
ze_add_source(ZEGUID.cpp 						Sources)
ze_add_source(ZEGUIDTests.cpp					Tests)
ze_add_source(ZETypes.h 						Sources Headers)
ze_add_source(ZETypes.cpp 						Sources)
ze_add_source(ZEEndian.h 						Sources Headers)
ze_add_source(ZEEndian.cpp 						Sources)
ze_add_source(ZEEndianTests.cpp					Tests)
ze_add_source(ZESwap.h 							Sources Headers)
ze_add_source(ZESwap.cpp 						Sources)
ze_add_source(ZEError.h 						Sources Headers)
ze_add_source(ZEError.cpp 						Sources)
ze_add_source(ZELog.h							Sources Headers)
ze_add_source(ZELog.cpp							Sources)
ze_add_source(ZELogSession.h					Sources Headers)
ze_add_source(ZELogSession.cpp					Sources)
ze_add_source(ZETerminalUtils.h					Sources Headers)
ze_add_source(ZETerminalUtils.cpp				Sources)
ze_add_source(ZEPacking.h 						Sources Headers)
ze_add_source(ZEPacking.cpp 					Sources)
ze_add_source(ZERandom.h 						Sources Headers)
ze_add_source(ZERandom.cpp 						Sources)
ze_add_source(ZERandomTests.cpp					Tests)
ze_add_source(ZETimeCounter.h					Headers)
ze_add_source(ZETimeCounter.cpp					Sources)
ze_add_source(ZETimeCounterTests.cpp			Tests)
ze_add_source(ZETimeStamp.h						Headers)
ze_add_source(ZETimeStamp.cpp					Sources)
ze_add_source(ZEBase64.h						Sources Headers)
ze_add_source(ZEBase64.cpp						Sources)
ze_add_source(ZEVersion.cpp						Sources)
ze_add_source(ZEVersion.h						Sources Headers)
ze_add_source(ZEInitializable.cpp				Sources)
ze_add_source(ZEInitializable.h					Sources Headers)
ze_add_source(ZEEncryption.cpp					Sources)
ze_add_source(ZEEncryption.h					Sources Headers)

ze_add_source(ZEAutoExp.txt						FoundationSources FoundationHeaders)
ze_add_source(ZE.natvis							FoundationSources FoundationHeaders)

ze_add_library(TARGET ZEFoundationBase
	SOURCES ${Sources}
	HEADERS ${Headers}
	COMBINE
	LIBS ZEDS ZEMath ZESerialization ZEStateMachine ZEFile ZEMacro ZESpatial ZESerialPort ZETest ZEThread ZESocket ZEML ZERegEx ZECompression ZEPointer ZEMeta libCryptopp)

ze_add_library(TARGET ZEFoundationMeta
	SOURCES ${FoundationMetaHeaders}
	ZEMC ${FoundationMetaHeaders})

ze_add_library(TARGET ZEFoundation
	SOURCES ${FoundationSources} ZEFoundation.cpp
	HEADERS ${FoundationHeaders}
	COMBINE
	LIBS ZEFoundationBase ZEFoundationMeta)

ze_add_test(TARGET ZEFoundationTests
	SOURCES ${Tests}
	EXTRA_SOURCES
	TEST_TARGET ZEFoundation)

ze_link(ZEFoundation LIBS rt PLATFORMS Unix)
