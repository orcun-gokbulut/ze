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

ze_add_source(ZEApplicationModule.cpp				Sources)
ze_add_source(ZEApplicationModule.h					Sources Headers ZEMC)
ze_add_source(ZECommand.cpp							Sources)
ze_add_source(ZECommand.h							Sources Headers)
ze_add_source(ZECommandManager.cpp					Sources)
ze_add_source(ZECommandManager.h					Sources Headers)
ze_add_source(ZECommandSection.cpp					Sources)
ze_add_source(ZECommandSection.h					Sources Headers)
ze_add_source(ZEConsole.cpp							Sources)
ze_add_source(ZEConsole.h							Sources Headers)
ze_add_source(ZEConsoleWindow.cpp					Sources)
ze_add_source(ZEConsoleWindow.h						Sources Headers)
ze_add_source(ZECore.cpp							Sources)
ze_add_source(ZECore.h								Sources Headers ZEMC)
ze_add_source(ZEErrorManager.cpp					Sources)
ze_add_source(ZEErrorManager.h						Sources Headers)
ze_add_source(ZEModule.cpp							Sources)
ze_add_source(ZEModule.h							Sources Headers ZEMC)
ze_add_source(ZEOption.cpp							Sources)
ze_add_source(ZEOption.h							Sources Headers)
ze_add_source(ZEOptionManager.cpp					Sources)
ze_add_source(ZEOptionManager.h						Sources Headers)
ze_add_source(ZEOptionSection.cpp					Sources)
ze_add_source(ZEOptionSection.h						Sources Headers)
ze_add_source(ZEProfiler.cpp						Sources)
ze_add_source(ZEProfiler.h							Sources Headers)
ze_add_source(ZEProfilerCounter.cpp					Sources)
ze_add_source(ZEProfilerCounter.h					Sources Headers)
ze_add_source(ZEResource.cpp						Sources)
ze_add_source(ZEResource.h							Sources Headers ZEMC)
ze_add_source(ZEResourceManager.cpp					Sources)
ze_add_source(ZEResourceManager.h					Sources Headers)
ze_add_source(ZESystemMessageManager.cpp			Sources)
ze_add_source(ZESystemMessageManager.h				Sources Headers)
ze_add_source(ZESystemMessageHandler.cpp			Sources)
ze_add_source(ZESystemMessageHandler.h				Sources Headers)
ze_add_source(ZEWindowResources.h					Sources)
ze_add_source(ZETimer.cpp							Sources)
ze_add_source(ZETimer.h								Sources Headers ZEMC)
ze_add_source(ZETimeManager.cpp						Sources)
ze_add_source(ZETimeManager.h						Sources Headers ZEMC)
ze_add_source(ZETimeParameters.cpp					Sources)
ze_add_source(ZETimeParameters.h					Sources Headers ZEMC)
ze_add_source(ZECrashHandler.cpp					Sources)
ze_add_source(ZECrashHandler.h						Sources Headers)
ze_add_source(ZECrashHandler_Windows.cpp			Sources PLATFORMS Windows)
ze_add_source(ZESplashWindow.cpp					Sources)
ze_add_source(ZESplashWindow.h						Sources)

ze_add_component(COMPONENT ZECore
	TARGET ZEEngine 
	SOURCES ${Sources}
	HEADERS ${Headers}
	ZEMC ${ZEMC})
