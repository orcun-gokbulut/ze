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

cmake_minimum_required (VERSION 2.8)

project (ZEFoundation)

ze_set_project_folder("ZEFoundation")

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
add_subdirectory(ZETask)
add_subdirectory(ZESocket)
add_subdirectory(ZEML)
add_subdirectory(ZERegEx)

ze_add_source(ZEAutoExp.txt				Sources Headers)
ze_add_source(ZEData.h 					Sources Headers)
ze_add_source(ZEData.cpp 				Sources)
ze_add_source(ZEDLL.h 					Sources Headers)
ze_add_source(ZEDLL.cpp 				Sources)
ze_add_source(ZECommon.h 				Sources Headers)
ze_add_source(ZECommon.cpp 				Sources)
ze_add_source(ZEGUID.h 					Sources Headers)
ze_add_source(ZEGUID.cpp 				Sources)
ze_add_source(ZEGUIDTests.cpp			Tests)
ze_add_source(ZETypes.h 				Sources Headers)
ze_add_source(ZETypes.cpp 				Sources)
ze_add_source(ZEEndian.h 				Sources Headers)
ze_add_source(ZEEndian.cpp 				Sources)
ze_add_source(ZEEndianTests.cpp			Tests)
ze_add_source(ZESwap.h 					Sources Headers)
ze_add_source(ZESwap.cpp 				Sources)
ze_add_source(ZEError.h 				Sources Headers)
ze_add_source(ZEError.cpp 				Sources)
ze_add_source(ZELog.h					Sources Headers)
ze_add_source(ZELog.cpp					Sources)
ze_add_source(ZEPacking.h 				Sources Headers)
ze_add_source(ZEPacking.cpp 			Sources)
ze_add_source(ZERandom.h 				Sources Headers)
ze_add_source(ZERandom.cpp 				Sources)
ze_add_source(ZERandomTests.cpp			Tests)
ze_add_source(ZETimeCounter.h			Headers)
ze_add_source(ZETimeCounter.cpp			Sources)
ze_add_source(ZETimeCounterTests.cpp	Tests)

ze_add_library(ZEFoundation
	LIBS ZEDS ZEMath ZESerialization ZEStateMachine ZEFile ZEMacro ZESpatial ZESerialPort ZETest ZEThread ZETask ZESocket ZEML ZERegEx
	SOURCES ${Sources} 
	HEADERS ${Headers})

ze_combine_libraries(ZEFoundation LIBS ZEFoundation ZEDS ZEMath ZESerialization ZEStateMachine ZEFile ZEMacro ZESpatial ZESerialPort ZETest ZEThread ZETask ZESocket ZEML ZERegEx)

ze_add_test(ZEFoundationTests
	SOURCES ${Tests}
	EXTRA_SOURCES
	TEST_TARGET ZEFoundation)

ze_link(ZEFoundation LIBS rt PLATFORMS Unix)
