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


find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

ze_add_source(ZETEBlock.cpp					Sources)
ze_add_source(ZETEBlock.h					Sources)
ze_add_source(ZETEBlockDatabase.cpp			Sources)
ze_add_source(ZETEBlockDatabase.h			Sources)
ze_add_source(ZETEPatch.cpp					Sources)
ze_add_source(ZETEPatch.h					Sources)
ze_add_source(ZETEPatchFile.cpp				Sources)
ze_add_source(ZETEPatchFile.h				Sources)
ze_add_source(ZETEPatchGDAL.cpp				Sources)
ze_add_source(ZETEPatchGDAL.h				Sources)
ze_add_source(ZETEPatchDatabase.cpp			Sources)
ze_add_source(ZETEPatchDatabase.h			Sources)
ze_add_source(ZETEResamplerIPP.cpp			Sources)
ze_add_source(ZETEResamplerIPP.h			Sources)
ze_add_source(ZETEProcessor.cpp				Sources)
ze_add_source(ZETEProcessor.h				Sources)
ze_add_source(ZETEMain.cpp					Sources)

ze_add_executable(TARGET ZETerrainExporter
	CONSOLE
	SOURCES ${Sources}
	LIBS ZEFoundation libFreeImage libIPP libGDAL)
