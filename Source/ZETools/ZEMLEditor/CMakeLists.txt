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

ze_add_source(ZEMain.cpp						Sources)
ze_add_source(ZEDOperation.cpp					Sources)
ze_add_source(ZEDOperation.h					Sources)
ze_add_source(ZEDOperationManager.cpp			Sources)
ze_add_source(ZEDOperationManager.h				Sources)
ze_add_source(ZEMLEditorWindow.cpp				Sources)
ze_add_source(ZEMLEditorWindow.h				Sources QtMocs)
ze_add_source(ZEMLEditorWindow.ui				Sources QtUIs)
ze_add_source(ZEMLEditorElementWidget.cpp		Sources)
ze_add_source(ZEMLEditorElementWidget.h			Sources QtMocs)
ze_add_source(ZEMLEditorElementWidget.ui		Sources QtUIs)
ze_add_source(ZEOperationCopy.cpp				Sources)
ze_add_source(ZEOperationCopy.h					Sources)
ze_add_source(ZEOperationCut.cpp				Sources)
ze_add_source(ZEOperationCut.h					Sources)
ze_add_source(ZEOperationPaste.cpp				Sources)
ze_add_source(ZEOperationPaste.h				Sources)
ze_add_source(ZEOperationAddElement.cpp			Sources)
ze_add_source(ZEOperationAddElement.h			Sources)
ze_add_source(ZEOperationChangeElementName.cpp	Sources)
ze_add_source(ZEOperationChangeElementName.h	Sources)
ze_add_source(ZEOperationChangePropertyValue.cpp		Sources)
ze_add_source(ZEOperationChangePropertyValue.h		Sources)
ze_add_source(ZEOperationDeleteElements.cpp		Sources)
ze_add_source(ZEOperationDeleteElements.h		Sources)
ze_add_source(ZEOperationSelection.cpp			Sources)
ze_add_source(ZEOperationSelection.h			Sources)

qt4_wrap_ui(QtUIOutputs ${QtUIs})
qt4_wrap_cpp(QtMocOutputs  ${QtMocs})
qt4_add_resources(QtResourceOutputs ${QtResources})

ze_add_executable(TARGET ZEMLEditor 
	SOURCES ${Sources} ${QtMocOutputs} ${QtUIOutputs} ${QtResourceOutputs}
	LIBS ZEFoundation libQt)
