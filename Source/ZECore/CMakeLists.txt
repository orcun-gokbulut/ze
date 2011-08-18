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

add_source (ZECommand.cpp					Sources)
add_source (ZECommand.h						Sources ZESDK)
add_source (ZECommandManager.cpp			Sources)
add_source (ZECommandManager.h				Sources ZESDK)
add_source (ZECommandSection.cpp			Sources)
add_source (ZECommandSection.h				Sources ZESDK)
add_source (ZEConsole.cpp					Sources)
add_source (ZEConsole.h						Sources ZESDK)
add_source (ZEConsoleWindow.cpp				Sources)
add_source (ZEConsoleWindow.h				Sources ZESDK)
add_source (ZECore.cpp						Sources)
add_source (ZECore.h						Sources ZESDK)
add_source (ZEApplicationModule.cpp			Sources)
add_source (ZEApplicationModule.h			Sources ZESDK)
add_source (ZEApplicationModuleDescription.cpp		Sources)
add_source (ZEApplicationModuleDescription.h		Sources ZESDK)
add_source (ZEError.cpp						Sources)
add_source (ZEError.h						Sources ZESDK)
add_source (ZEExtension.cpp					Sources)
add_source (ZEExtension.h					Sources ZESDK)
add_source (ZEExtensionDescription.cpp		Sources)
add_source (ZEExtensionDescription.h		Sources ZESDK)
add_source (ZEExtensionManager.cpp			Sources)
add_source (ZEExtensionManager.h			Sources ZESDK)
add_source (ZEModule.cpp					Sources)
add_source (ZEModule.h						Sources ZESDK)
add_source (ZEModuleDescription.cpp			Sources)
add_source (ZEModuleDescription.h			Sources ZESDK)
add_source (ZEModuleManager.cpp				Sources)
add_source (ZEModuleManager.h				Sources ZESDK)
add_source (ZEOption.cpp					Sources)
add_source (ZEOption.h						Sources ZESDK)
add_source (ZEFile.cpp			Sources)
add_source (ZEFile.h			Sources ZESDK)
add_source (ZEOptionManager.cpp				Sources)
add_source (ZEOptionManager.h				Sources ZESDK)
add_source (ZEOptionSection.cpp				Sources)
add_source (ZEOptionSection.h				Sources ZESDK)
add_source (ZEPlugin.cpp					Sources)
add_source (ZEPlugin.h						Sources ZESDK)
add_source (ZEPluginManager.cpp				Sources)
add_source (ZEPluginManager.h				Sources ZESDK)
add_source (ZEResource.cpp					Sources)
add_source (ZEResource.h					Sources ZESDK)
add_source (ZEResourceFile.cpp				Sources)
add_source (ZEResourceFile.h				Sources ZESDK)
add_source (ZEResourceManager.cpp			Sources)
add_source (ZEResourceManager.h				Sources ZESDK)
add_source (ZESystemMessageManager.cpp		Sources)
add_source (ZESystemMessageManager.h		Sources ZESDK)
add_source (ZESystemMessageHandler.cpp		Sources)
add_source (ZESystemMessageHandler.h		Sources ZESDK)
add_source (ZEWindow.cpp					Sources)
add_source (ZEWindow.h						Sources ZESDK)
add_source (ZEWindowResources.h				Sources)
add_source (ZEWindowResources.rc			Sources)
add_source (ZEVersion.cpp					Sources)
add_source (ZEVersion.h						Sources ZESDK)

add_library (ZECore ${Sources})

set_property (TARGET ZECore PROPERTY FOLDER "Zinek Engine")

install (TARGETS ZECore COMPONENT "Libraries" CONFIGURATIONS Release ARCHIVE DESTINATION Lib)
install (FILES ${ZESDK} COMPONENT "Includes" DESTINATION Include/ZECore)
