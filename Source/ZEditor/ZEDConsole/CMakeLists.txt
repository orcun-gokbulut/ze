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

ze_add_source(ZEDConsoleWidget.cpp						Sources)
ze_add_source(ZEDConsoleWidget.h						Sources Headers QtMocs)
ze_add_source(ZEDConsoleWidget.ui						Sources QtUI)
ze_add_source(ZECommandEngine.cpp						Sources)
ze_add_source(ZECommandEngine.h							Sources Headers)
ze_add_source(ZECommandParser.cpp						Sources)
ze_add_source(ZECommandParser.h							Sources Headers)
ze_add_source(ZECommandParser.y							Sources)
ze_add_source(ZECommandLexer.cpp						Sources)
ze_add_source(ZECommandLexer.h							Sources)
ze_add_source(ZECommandLexer.l							Sources Headers)
ze_add_source(ZEMain.cpp								Sources)

qt5_wrap_cpp (QtMocFiles ${QtMocs})
qt5_wrap_ui (QtUiFiles ${QtUI})


# RE2C
#######################################################

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ZECommandLexer.l.cpp"
	COMMAND "${CMAKE_SOURCE_DIR}/Tool/re2c.exe" 
	ARGS "-o" "${CMAKE_CURRENT_BINARY_DIR}/ZECommandLexer.l.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/ZECommandLexer.l"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/ZECommandLexer.l"
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

ze_add_source("${CMAKE_CURRENT_BINARY_DIR}/ZECommandLexer.l.cpp" Sources)


# LEMON
#######################################################

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ZECommandParser.y.tmp" "${CMAKE_CURRENT_BINARY_DIR}/ZECommandParser.y.c" "${CMAKE_CURRENT_BINARY_DIR}/ZECommandParser.y.h"
	COMMAND "${CMAKE_COMMAND}"
		ARGS "-E" "copy_if_different" "${CMAKE_CURRENT_SOURCE_DIR}/ZECommandParser.y" "${CMAKE_CURRENT_BINARY_DIR}/ZECommandParser.y.tmp"
	COMMAND "${CMAKE_SOURCE_DIR}/Tool/lemon.exe" 
		ARGS "${CMAKE_CURRENT_BINARY_DIR}/ZECommandParser.y.tmp"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/ZECommandParser.y"
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
	
ze_add_source("${CMAKE_CURRENT_BINARY_DIR}/ZECommandParser.y.c" Sources)
ze_add_source("${CMAKE_CURRENT_BINARY_DIR}/ZECommandParser.y.h" Sources)
	
ze_add_executable(TARGET ZEDConsole 
	ZEMC ${ZEMC}
	SOURCES ${Sources} ${QtMocFiles} ${QtUiFiles} 
	LIBS ZEFoundation ZEEngine)

source_group("Generated" FILES 
	${QtMocFiles} 
	${QtUiFiles}
	"${CMAKE_CURRENT_BINARY_DIR}/ZECommandLexer.l.cpp"
	"${CMAKE_CURRENT_BINARY_DIR}/ZECommandParser.y.c"
	"${CMAKE_CURRENT_BINARY_DIR}/ZECommandParser.y.h")

qt5_use_modules(ZEDConsole Widgets)
