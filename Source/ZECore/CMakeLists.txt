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

cmake_minimum_required(VERSION 2.8)

add_source(ZECommand.cpp ZECore_SOURCE_FILES)
add_source(ZECommand.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECommandManager.cpp ZECore_SOURCE_FILES)
add_source(ZECommandManager.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECommandSection.cpp ZECore_SOURCE_FILES)
add_source(ZECommandSection.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEConsole.cpp ZECore_SOURCE_FILES)
add_source(ZEConsole.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEConsoleWindow.cpp ZECore_SOURCE_FILES)
add_source(ZEConsoleWindow.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECore.cpp ZECore_SOURCE_FILES)
add_source(ZECore.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEError.cpp ZECore_SOURCE_FILES)
add_source(ZEError.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEModule.cpp ZECore_SOURCE_FILES)
add_source(ZEModule.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEModuleManager.cpp ZECore_SOURCE_FILES)
add_source(ZEModuleManager.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEOption.cpp ZECore_SOURCE_FILES)
add_source(ZEOption.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEOptionManager.cpp ZECore_SOURCE_FILES)
add_source(ZEOptionManager.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEOptionSection.cpp ZECore_SOURCE_FILES)
add_source(ZEOptionSection.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEResource.cpp ZECore_SOURCE_FILES)
add_source(ZEResource.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEResourceFile.cpp ZECore_SOURCE_FILES)
add_source(ZEResourceFile.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEResourceManager.cpp ZECore_SOURCE_FILES)
add_source(ZEResourceManager.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEWindow.cpp ZECore_SOURCE_FILES)
add_source(ZEWindow.h ZECore_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEWindowResource.h ZECore_SOURCE_FILES)

add_library(ZECore ${ZECore_SOURCE_FILES})

install(TARGETS ZECore COMPONENT "Libraries" CONFIGURATIONS Release ARCHIVE DESTINATION Lib)
install(FILES ${zesdk_INCLUDES} COMPONENT "Includes" DESTINATION Includes/ZECore)
