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

include_directories(
	${PROJECT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/Source)
	
ze_add_source(ZEDirectoryInfo.cpp		Sources)
ze_add_source(ZEDirectoryInfo.h			Sources Headers)
ze_add_source(ZEFile.cpp				Sources)
ze_add_source(ZEFile.h					Sources Headers)
ze_add_source(ZEFileUtils_Unix.cpp		Sources PLATFORMS Unix)
ze_add_source(ZEFileUtils_Windows.cpp	Sources PLATFORMS Windows)
ze_add_source(ZEFileUtils.h				Sources)
ze_add_source(ZEFileInfo.cpp			Sources)
ze_add_source(ZEFileInfo.h				Sources Headers)
ze_add_source(ZEFileTests.cpp			Tests)
ze_add_source(ZEPartialFile.cpp			Sources)
ze_add_source(ZEPartialFile.h			Sources Headers)
ze_add_source(ZEPathManager_Unix.cpp	Sources PLATFORMS Unix)
ze_add_source(ZEPathManager_Windows.cpp	Sources PLATFORMS Windows)
ze_add_source(ZEPathManager.h			Sources)
ze_add_source(ZEPartialFileTests.cpp	Tests)
ze_add_source(ZEFileCache.cpp			Sources)
ze_add_source(ZEFileCache.h				Sources Headers)
ze_add_source(ZEFileCacheTests.cpp		Tests)

ze_add_library(ZEFile 
	SOURCES ${Sources} 
	HEADERS ${Headers}
    LIBS ZEFoundation
	INSTALL
	INSTALL_DESTINATION ZEFoundation/ZEFile
	INSTALL_COMPONENT ZESDK)

ze_add_test(ZEFileTests
	SOURCES ${Tests}
	EXTRA_SOURCES
	TEST_TARGET ZEFile)
