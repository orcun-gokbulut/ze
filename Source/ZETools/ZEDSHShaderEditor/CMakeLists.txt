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


ze_add_source(ZEDSHErrorsWindow.cpp				Sources)
ze_add_source(ZEDSHErrorsWindow.h				Sources QtMocs)
ze_add_source(ZEDSHOutputWindow.cpp				Sources)
ze_add_source(ZEDSHOutputWindow.h				Sources QtMocs)
ze_add_source(ZEDSHEditorWidget.cpp				Sources)
ze_add_source(ZEDSHEditorWidget.h				Sources QtMocs)
ze_add_source(ZEDSHEditorLineNumbersWidget.cpp	Sources)
ze_add_source(ZEDSHEditorLineNumbersWidget.h	Sources)
ze_add_source(ZEDSHSyntaxHighlighter.cpp		Sources)
ze_add_source(ZEDSHSyntaxHighlighter.h			Sources QtMocs)
ze_add_source(ZEDSHMainWindow.cpp				Sources)
ze_add_source(ZEDSHMainWindow.h					Sources QtMocs)
ze_add_source(ZEDSHMainWindow.ui				Sources QtUIs)
ze_add_source(ZEDSHCompileOptionsWindow.cpp		Sources)
ze_add_source(ZEDSHCompileOptionsWindow.h		Sources QtMocs)
ze_add_source(ZEDSHCompileOptionsWindow.ui		Sources QtUIs)
ze_add_source(ZEDSHMain.cpp						Sources)

qt5_wrap_cpp(QtMocFiles ${QtMocs})
qt5_wrap_ui(QtUIFiles ${QtUIs})

source_group("Generated" FILES ${QtMocFiles} ${QtUIFiles})

ze_add_executable(TARGET ZEDSHShaderEditor
	SOURCES ${Sources} ${QtMocFiles} ${QtUIFiles}
	LIBS ZEFoundation ZEEngine ZEDirect3D11 Qt5::WinMain)

qt5_use_modules(ZEDSHShaderEditor Widgets)

ze_add_source(ZEDSHShaderEditor.def SourcesDLL)
ze_add_library(TARGET ZEDSHShaderEditorDLL
	SOURCES ${SourcesDLL} ${Sources} ${QtMocFiles} ${QtUIFiles}
	LIBS ZEFoundation ZEEngine ZEDirect3D11
	DLL)

qt5_use_modules(ZEDSHShaderEditorDLL Widgets)

set_property(TARGET ZEDSHShaderEditorDLL PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Rundir")
set_property(TARGET ZEDSHShaderEditorDLL PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Rundir")
set_property(TARGET ZEDSHShaderEditorDLL PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Rundir")
set_property(TARGET ZEDSHShaderEditorDLL PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/Rundir")
