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

ze_add_source(ZELock.cpp         		Sources)
ze_add_source(ZELock.h          	 	Sources Headers)
ze_add_source(ZELockTests.cpp			Tests)
ze_add_source(ZEMutex.cpp   			Sources)
ze_add_source(ZEMutex_Unix.cpp   		Sources PLATFORMS Unix)
ze_add_source(ZEMutex_Windows.cpp  		Sources PLATFORMS Windows)
ze_add_source(ZEMutex.h    	 			Sources Headers)
ze_add_source(ZEMutexTests.cpp			Tests)
ze_add_source(ZESignal.cpp   			Sources)
ze_add_source(ZESignal_Unix.cpp   		Sources PLATFORMS Unix)
ze_add_source(ZESignal_Windows.cpp  	Sources PLATFORMS Windows)
ze_add_source(ZESignal.h    	 		Sources Headers)
ze_add_source(ZESignalTests.cpp			Tests)
ze_add_source(ZEThread.cpp       		Sources)
ze_add_source(ZEThread_Unix.cpp 		Sources PLATFORMS Unix)
ze_add_source(ZEThread_Windows.cpp		Sources PLATFORMS Windows)
ze_add_source(ZEThread.h				Sources Headers)
ze_add_source(ZEThreadTests.cpp			Tests)
ze_add_source(ZETask.cpp         		Sources)
ze_add_source(ZETask.h           		Sources Headers)
ze_add_source(ZETaskThread.cpp         	Sources)
ze_add_source(ZETaskThread.h           	Sources Headers)
ze_add_source(ZETaskPool.cpp       		Sources)
ze_add_source(ZETaskPool.h         		Sources Headers)
ze_add_source(ZETaskManager.cpp   		Sources)
ze_add_source(ZETaskManager.h     		Sources Headers)

set(ZEBUILD_VTUNE_ENABLED FALSE CACHE BOOL "Enable VTune notify API for better VTune results")
if (ZEBUILD_VTUNE_ENABLED)
	find_package(VTune REQUIRED)
	if (VTUNE_FOUND)
		add_definitions(-DZE_VTUNE_ENABLED)
		include_directories(${VTune_INCLUDE_DIRS})
	endif()
endif()

ze_add_library(TARGET ZEThread
	SOURCES ${Sources} 
	HEADERS ${Headers}
	LIBS ZEDS "${VTune_LIBRARIES}")

ze_add_test(TARGET ZEThreadTests
	SOURCES ${Tests}
	EXTRA_SOURCES
	TEST_TARGET ZEThread)

ze_link(ZEThread PLATFORMS Unix LIBS pthread)
