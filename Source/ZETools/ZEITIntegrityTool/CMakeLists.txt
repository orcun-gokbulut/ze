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


find_package(Qt5Core)
find_package(Qt5Widgets)

ze_add_source(ZEITMain.cpp							Sources)
ze_add_source(ZEITGeneratorWorker.cpp				Sources)
ze_add_source(ZEITGeneratorWorker.h					Sources QtMocs)
ze_add_source(ZEITIntegrityToolWindow.cpp			Sources)
ze_add_source(ZEITIntegrityToolWindow.h				Sources QtMocs)
ze_add_source(ZEITIntegrityToolWindow.ui			Sources QtUIs)
ze_add_source(../CommonResources/ZEZinekIcon.qrc	Sources QtResources)

qt5_wrap_ui(QtUIOutputs ${QtUIs})
qt5_wrap_cpp(QtMocOutputs  ${QtMocs})
qt5_add_resources(QtResourceOutputs ${QtResources})

ze_add_executable(TARGET ZEITIntegrityTool
	SOURCES ${Sources} ${QtMocOutputs} ${QtUIOutputs} ${QtResourceOutputs}
	LIBS ZEFoundation ZEITIntegrity Qt5::WinMain)

qt5_use_modules(ZEITIntegrityTool Widgets)

source_group("Generated" FILES ${QtMocOutputs} ${QtUIOutputs} ${QtResourceOutputs})
