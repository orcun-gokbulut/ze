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

ze_add_source(ZEDUserInterfaceComponent.cpp				Sources)
ze_add_source(ZEDUserInterfaceComponent.h				Sources ZEMC QtMocs)
ze_add_source(ZEDWindow.cpp								Sources)
ze_add_source(ZEDWindow.h								Sources ZEMC QtMocs)
ze_add_source(ZEDMenu.cpp								Sources)
ze_add_source(ZEDMenu.h									Sourcesr)
ze_add_source(ZEDToolbar.cpp							Sources)
ze_add_source(ZEDToolbar.h								Sources ZEMC QtMocs)
ze_add_source(ZEDMainWindow.cpp							Sources)
ze_add_source(ZEDMainWindow.h							Sources QtMocs)
ze_add_source(ZEDMainWindow.ui							Sources QtUI)

# Common Controls
ze_add_source(ZEDTransformationToolbar.cpp				Sources)
ze_add_source(ZEDTransformationToolbar.h				Sources QtMocs)
ze_add_source(ZEDSelectionToolbar.cpp					Sources)
ze_add_source(ZEDSelectionToolbar.h						Sources QtMocs)
ze_add_source(ZEDViewportToolbar.cpp					Sources)
ze_add_source(ZEDViewportToolbar.h						Sources QtMocs)

ze_add_source(ZEDObjectTree.cpp							Sources)
ze_add_source(ZEDObjectTree.h							Sources QtMocs)
ze_add_source(ZEDObjectTreeColumn.cpp					Sources)
ze_add_source(ZEDObjectTreeColumn.h						Sources)
ze_add_source(ZEDObjectBrowser.cpp						Sources)
ze_add_source(ZEDObjectBrowser.h						Sources QtMocs)
ze_add_source(ZEDObjectBrowser.ui						Sources QtUI)

ze_add_source(ZEDAssetTree.cpp							Sources)
ze_add_source(ZEDAssetTree.h							Sources QtMocs)
ze_add_source(ZEDAssetBrowser.cpp						Sources)
ze_add_source(ZEDAssetBrowser.h							Sources QtMocs)
ze_add_source(ZEDAssetBrowser.ui						Sources QtUI)

ze_add_source(ZEDClassTree.cpp							Sources)
ze_add_source(ZEDClassTree.h							Sources QtMocs)
ze_add_source(ZEDClassBrowser.cpp						Sources)
ze_add_source(ZEDClassBrowser.h							Sources QtMocs)
ze_add_source(ZEDClassBrowser.ui						Sources QtUI)

ze_add_source(ZEDPropertyEditor.cpp						Sources)
ze_add_source(ZEDPropertyEditor.h						Sources Headers QtMocs)
ze_add_source(ZEDPropertyEditorItem.cpp					Sources)
ze_add_source(ZEDPropertyEditorItem.h					Sources Headers)
ze_add_source(ZEDPropertyEditorItemNumeric.cpp			Sources)
ze_add_source(ZEDPropertyEditorItemNumeric.h			Sources QtMocs)
ze_add_source(ZEDPropertyEditorItemBoolean.cpp			Sources)
ze_add_source(ZEDPropertyEditorItemBoolean.h			Sources QtMocs)
ze_add_source(ZEDPropertyEditorItemString.cpp			Sources)
ze_add_source(ZEDPropertyEditorItemString.h				Sources QtMocs)
ze_add_source(ZEDPropertyEditorItemEnumurator.cpp		Sources)
ze_add_source(ZEDPropertyEditorItemEnumurator.h			Sources QtMocs)
ze_add_source(ZEDPropertyEditorItemVector.cpp			Sources)
ze_add_source(ZEDPropertyEditorItemVector.h				Sources QtMocs)

qt5_wrap_cpp (QtMocFiles ${QtMocs})
qt5_wrap_ui (QtUIFiles ${QtUI})

ze_add_library(TARGET ZEDUserInterface
	ZEMC ${ZEMC}
	SOURCES ${Sources} ${QtMocFiles} ${QtUIFiles} 
	LIBS ZEFoundation ZEDCore)

source_group("Generated" FILES ${QtMocFiles} ${QtUIFiles})

qt5_use_modules(ZEDUserInterface Widgets)
