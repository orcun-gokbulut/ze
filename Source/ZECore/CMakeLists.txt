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
ze_add_source(ZEApplicationModule.h					Sources Headers)
ze_add_source(ZEApplicationModuleDescription.cpp	Sources)
ze_add_source(ZEApplicationModuleDescription.h		Sources Headers)
ze_add_source(ZEBase.cpp							Sources)
ze_add_source(ZEBase.h								Sources Headers)
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
ze_add_source(ZECore.h								Sources Headers)
ze_add_source(ZEErrorManager.cpp					Sources)
ze_add_source(ZEErrorManager.h						Sources Headers)
ze_add_source(ZEExtension.cpp						Sources)
ze_add_source(ZEExtension.h							Sources Headers)
ze_add_source(ZEExtensionDescription.cpp			Sources)
ze_add_source(ZEExtensionDescription.h				Sources Headers)
ze_add_source(ZEExtensionManager.cpp				Sources)
ze_add_source(ZEExtensionManager.h					Sources Headers)
ze_add_source(ZEModule.cpp							Sources)
ze_add_source(ZEModule.h							Sources Headers)
ze_add_source(ZEModuleDescription.cpp				Sources)
ze_add_source(ZEModuleDescription.h					Sources Headers)
ze_add_source(ZEModuleManager.cpp					Sources)
ze_add_source(ZEModuleManager.h						Sources Headers)
ze_add_source(ZEOption.cpp							Sources)
ze_add_source(ZEOption.h							Sources Headers)
ze_add_source(ZEOptionManager.cpp					Sources)
ze_add_source(ZEOptionManager.h						Sources Headers)
ze_add_source(ZEOptionSection.cpp					Sources)
ze_add_source(ZEOptionSection.h						Sources Headers)
ze_add_source(ZEPlugin.cpp							Sources)
ze_add_source(ZEPlugin.h							Sources Headers)
ze_add_source(ZEPluginManager.cpp					Sources)
ze_add_source(ZEPluginManager.h						Sources Headers)
ze_add_source(ZEProfiler.cpp						Sources)
ze_add_source(ZEProfiler.h							Sources Headers)
ze_add_source(ZEProfilerManager.cpp					Sources)
ze_add_source(ZEProfilerManager.h					Sources Headers)
ze_add_source(ZERealTimeClock.cpp					Sources)
ze_add_source(ZERealTimeClock.h						Sources Headers)
ze_add_source(ZEResource.cpp						Sources)
ze_add_source(ZEResource.h							Sources Headers)
ze_add_source(ZEResourceManager.cpp					Sources)
ze_add_source(ZEResourceManager.h					Sources Headers)
ze_add_source(ZESystemMessageManager.cpp			Sources)
ze_add_source(ZESystemMessageManager.h				Sources Headers)
ze_add_source(ZESystemMessageHandler.cpp			Sources)
ze_add_source(ZESystemMessageHandler.h				Sources Headers)
ze_add_source(ZEWindow.cpp							Sources)
ze_add_source(ZEWindow.h							Sources Headers)
ze_add_source(ZEWindowResources.h					Sources)
ze_add_source(ZEWindowResources.rc					Sources)
ze_add_source(ZEVersion.cpp							Sources)
ze_add_source(ZEVersion.h							Sources Headers)

ze_add_library(ZECore 
	SOURCES ${Sources}
	HEADERS ${Headers}
	INSTALL
	INSTALL_DESTINATION "ZEEngineAPI/ZECore"
	INSTALL_COMPONENT ZESDK)
