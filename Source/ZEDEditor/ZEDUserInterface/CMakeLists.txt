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

ze_add_source(ZEDCommand.cpp							Sources)
ze_add_source(ZEDCommand.h								Sources ZEMC QtMOCs)
ze_add_source(ZEDCommandManager.cpp						Sources)
ze_add_source(ZEDCommandManager.h						Sources ZEMC QtMOCs)

ze_add_source(ZEDUserInterfaceComponent.cpp				Sources)
ze_add_source(ZEDUserInterfaceComponent.h				Sources ZEMC QtMOCs)
ze_add_source(ZEDWindow.cpp								Sources)
ze_add_source(ZEDWindow.h								Sources ZEMC QtMOCs)
ze_add_source(ZEDMainWindow.cpp							Sources)
ze_add_source(ZEDMainWindow.h							Sources QtMOCs)
ze_add_source(ZEDMainWindow.ui							Sources QtUIs)
ze_add_source(ZEDUIUtils.cpp							Sources)
ze_add_source(ZEDUIUtils.h								Sources)

ze_add_source(ZEDMenu.cpp								Sources)
ze_add_source(ZEDMenu.h									Sources ZEMC)
ze_add_source(ZEDMenuItem.cpp							Sources)
ze_add_source(ZEDMenuItem.h								Sources ZEMC QtMOCs)
ze_add_source(ZEDMenuManager.cpp						Sources)
ze_add_source(ZEDMenuManager.h							Sources ZEMC)
ze_add_source(ZEDMenuOptionsPage.cpp					Sources)
ze_add_source(ZEDMenuOptionsPage.h						Sources QtMOCs)
ze_add_source(ZEDMenuOptionsPageWidget.ui				Sources QtUIs)

ze_add_source(ZEDToolbar.cpp							Sources)
ze_add_source(ZEDToolbar.h								Sources ZEMC)
ze_add_source(ZEDToolbarItem.cpp						Sources)
ze_add_source(ZEDToolbarItem.h							Sources ZEMC QtMOCs)
ze_add_source(ZEDToolbarManager.cpp						Sources)
ze_add_source(ZEDToolbarManager.h						Sources ZEMC)
ze_add_source(ZEDToolbarOptionsPage.cpp					Sources)
ze_add_source(ZEDToolbarOptionsPage.h					Sources QtMOCs)
ze_add_source(ZEDToolbarOptionsPageWidget.ui			Sources QtUIs)

ze_add_source(ZEDObjectModel.cpp						Sources)
ze_add_source(ZEDObjectModel.h							Sources ZEMC)
ze_add_source(ZEDObjectModelColumn.cpp					Sources)
ze_add_source(ZEDObjectModelColumn.h					Sources)
ze_add_source(ZEDObjectSelectionModel.cpp				Sources)
ze_add_source(ZEDObjectSelectionModel.h					Sources QtMOCs)
ze_add_source(ZEDObjectBrowser.cpp						Sources)
ze_add_source(ZEDObjectBrowser.h						Sources QtMOCs)
ze_add_source(ZEDObjectBrowser.ui						Sources QtUIs)

ze_add_source(ZEDAssetModel.cpp							Sources)
ze_add_source(ZEDAssetModel.h							Sources ZEMC)
ze_add_source(ZEDAssetModelColumn.cpp					Sources)
ze_add_source(ZEDAssetModelColumn.h						Sources)
ze_add_source(ZEDAssetItemDelegate.cpp					Sources)
ze_add_source(ZEDAssetItemDelegate.h					Sources)
ze_add_source(ZEDAssetBrowser.cpp						Sources)
ze_add_source(ZEDAssetBrowser.h							Sources QtMOCs)
ze_add_source(ZEDAssetBrowser.ui						Sources QtUIs)

ze_add_source(ZEDClassModel.cpp							Sources)
ze_add_source(ZEDClassModel.h							Sources ZEMC)
ze_add_source(ZEDClassBrowser.cpp						Sources)
ze_add_source(ZEDClassBrowser.h							Sources QtMOCs)
ze_add_source(ZEDClassBrowser.ui						Sources QtUIs)

ze_add_source(ZEDPropertyEditor.cpp						Sources)
ze_add_source(ZEDPropertyEditor.h						Sources Headers QtMOCs)
ze_add_source(ZEDPropertyEditorItem.cpp					Sources)
ze_add_source(ZEDPropertyEditorItem.h					Sources Headers)
ze_add_source(ZEDPropertyEditorItemNumeric.cpp			Sources)
ze_add_source(ZEDPropertyEditorItemNumeric.h			Sources QtMOCs)
ze_add_source(ZEDPropertyEditorItemBoolean.cpp			Sources)
ze_add_source(ZEDPropertyEditorItemBoolean.h			Sources QtMOCs)
ze_add_source(ZEDPropertyEditorItemString.cpp			Sources)
ze_add_source(ZEDPropertyEditorItemString.h				Sources QtMOCs)
ze_add_source(ZEDPropertyEditorItemEnumurator.cpp		Sources)
ze_add_source(ZEDPropertyEditorItemEnumurator.h			Sources QtMOCs)
ze_add_source(ZEDPropertyEditorItemVector.cpp			Sources)
ze_add_source(ZEDPropertyEditorItemVector.h				Sources QtMOCs)
ze_add_source(ZEDPropertyWindow.cpp						Sources)
ze_add_source(ZEDPropertyWindow.h						Sources Headers QtMOCs)


ze_add_source(ZEDOptionsPage.cpp						Sources)
ze_add_source(ZEDOptionsPage.h							Sources ZEMC)
ze_add_source(ZEDOptionsManager.cpp						Sources)
ze_add_source(ZEDOptionsManager.h						Sources ZEMC)
ze_add_source(ZEDOptionsWindow.cpp						Sources)
ze_add_source(ZEDOptionsWindow.h						Sources Headers QtMOCs)
ze_add_source(ZEDOptionsWindow.ui						Sources QtUIs)
ze_add_source(ZEDOptionsWindowSingle.cpp				Sources)
ze_add_source(ZEDOptionsWindowSingle.h					Sources Headers QtMOCs)
ze_add_source(ZEDOptionsWindowSingle.ui					Sources QtUIs)
ze_add_source(ZEDOptionsWindowMultiple.cpp				Sources)
ze_add_source(ZEDOptionsWindowMultiple.h				Sources Headers QtMOCs)
ze_add_source(ZEDOptionsWindowMultiple.ui				Sources QtUIs)

ze_add_source(ZEDCommon.qrc								Sources QtResources)

qt5_wrap_cpp(QtMOCOutputs ${QtMOCs})
qt5_wrap_ui(QtUIOutputs ${QtUIs})
qt5_add_resources(QtResouceOutputs ${QtResources})

include_directories(${Qt5Widgets_INCLUDE_DIRS})

ze_add_library(TARGET ZEDUserInterface
	SOURCES ${Sources} ${QtMOCOutputs} ${QtUIOutputs} ${QtResouceOutputs}
	ZEMC ${ZEMC}
	LIBS ZEFoundation ZEDCore)

qt5_use_modules(ZEDUserInterface Widgets)

source_group("Generated" FILES ${QtMOCOutputs} ${QtUIOutputs} ${QtResouceOutputs})
