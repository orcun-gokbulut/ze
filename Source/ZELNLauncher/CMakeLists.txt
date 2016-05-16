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

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

ze_add_source(ZELNMain.cpp						Sources)
ze_add_source(ZELNModule.cpp					Sources)
ze_add_source(ZELNModule.h						Sources Headers)
ze_add_source(ZELNLauncher.cpp					Sources)
ze_add_source(ZELNLauncher.h					Sources)
ze_add_source(ZELNLauncherWindow.cpp			Sources)
ze_add_source(ZELNLauncherWindow.h				Sources QtMocs)
ze_add_source(ZELNLauncherWindow.ui				Sources QtUIs)
ze_add_source(ZELNLauncher.qrc					Sources QtResources)

ze_add_source(ZELNLogModule.cpp					Sources)
ze_add_source(ZELNLogModule.h					Sources)
ze_add_source(ZELNLogWidget.cpp					Sources)
ze_add_source(ZELNLogWidget.h					Sources QtMocs)
ze_add_source(ZELNLogWidget.ui					Sources QtUIs)

ze_add_source(ZELNLicenseModule.cpp				Sources)
ze_add_source(ZELNLicenseModule.h				Sources)
ze_add_source(ZELNLicenseWidget.cpp				Sources)
ze_add_source(ZELNLicenseWidget.h				Sources QtMocs)
ze_add_source(ZELNLicenseWidget.ui				Sources QtUIs)
ze_add_source(ZELNLicenseManagerDialog.cpp		Sources)
ze_add_source(ZELNLicenseManagerDialog.h		Sources QtMocs)
ze_add_source(ZELNLicenseManagerDialog.ui		Source QtUIs)
ze_add_source(ZELNLicenseEditorDialog.cpp		Sources)
ze_add_source(ZELNLicenseEditorDialog.h			Sources QtMocs)
ze_add_source(ZELNLicenseEditorDialog.ui		Source QtUIs)

ze_add_source(ZELNIntegrityModule.cpp			Sources)
ze_add_source(ZELNIntegrityModule.h				Sources QtMocs)
ze_add_source(ZELNIntegrityWidget.ui			Sources QtUIs)

ze_add_source(ZELNUpdateModule.cpp				Sources)
ze_add_source(ZELNUpdateModule.h				Sources)
ze_add_source(ZELNUpdateWidget.cpp				Sources)
ze_add_source(ZELNUpdateWidget.h				Sources QtMocs)
ze_add_source(ZELNUpdateWidget.ui				Sources QtUIs)

ze_add_source(ZELNContactModule.cpp				Sources)
ze_add_source(ZELNContactModule.h				Sources)
ze_add_source(ZELNContactWidget.cpp				Sources)
ze_add_source(ZELNContactWidget.h				Sources QtMocs)
ze_add_source(ZELNContactWidget.ui				Sources QtUIs)

ze_add_source(../ZEEngine/ZECore/ZEWindowResources.rc 	Sources)

set(CMAKE_LIBRARY_PATH "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x64")

find_package(Qt5Core)
find_package(Qt5Widgets)

qt5_wrap_ui(QtUIOutputs ${QtUIs})
qt5_wrap_cpp(QtMocOutputs  ${QtMocs})
qt5_add_resources(QtResourceOutputs ${QtResources})

ze_add_executable(TARGET ZELNLauncher 
	SOURCES ${Sources} ${QtMocOutputs} ${QtUIOutputs} ${QtResourceOutputs}
	LIBS ZEFoundation ZEProtect ZEITIntegrity libCryptopp Qt5::WinMain
	DESCRIPTION "Zinek Launcher")

qt5_use_modules(ZELNLauncher Core Widgets)

add_custom_command(TARGET ZELNLauncher POST_BUILD
	# DEBUG
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Cored.dll 		${CMAKE_CURRENT_BINARY_DIR}/Debug
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Guid.dll		${CMAKE_CURRENT_BINARY_DIR}/Debug
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Widgetsd.dll 	${CMAKE_CURRENT_BINARY_DIR}/Debug
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/icuin51.dll		${CMAKE_CURRENT_BINARY_DIR}/Debug
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/icuuc51.dll		${CMAKE_CURRENT_BINARY_DIR}/Debug
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/icudt51.dll		${CMAKE_CURRENT_BINARY_DIR}/Debug
	# RELEASE
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Core.dll		${CMAKE_CURRENT_BINARY_DIR}/Release
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Gui.dll			${CMAKE_CURRENT_BINARY_DIR}/Release
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/Qt5Widgets.dll		${CMAKE_CURRENT_BINARY_DIR}/Release
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/icuin51.dll		${CMAKE_CURRENT_BINARY_DIR}/Release
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/icuuc51.dll		${CMAKE_CURRENT_BINARY_DIR}/Release
	COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Core_DIR}/../../../bin/icudt51.dll		${CMAKE_CURRENT_BINARY_DIR}/Release
	# Output Message
	COMMENT "Copying Qt binaries" VERBATIM)
	
source_group("Generated" FILES ${QtMocOutputs} ${QtUIOutputs} ${QtResourceOutputs})
