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


ze_set_project_folder("ZETools")

set(ZEBUILD_3DS_MAX_BINARY_DIRECTORY "C:\\Program Files\\Autodesk\\3ds Max 2015" CACHE STRING "Directory of 3ds max.")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-D_UNICODE)

ze_add_source(ZE3dsMaxPlugin.h								Sources)
ze_add_source(ZE3dsMaxPlugin.cpp							Sources)
ze_add_source(ZE3dsMaxPlugin.def							Sources)
ze_add_source(ZE3dsMaxUtils.h								Sources)
ze_add_source(ZE3dsMaxUtils.cpp								Sources)
ze_add_source(ZE3dsMaxDefinitions.h							Sources)
ze_add_source(ZE3dsMaxDefinitions.cpp						Sources)

ze_add_source(ZE3dsMaxInteriorExporter.h					Sources)
ze_add_source(ZE3dsMaxInteriorExporter.cpp					Sources)
ze_add_source(ZE3dsMaxInteriorExporterDescription.h			Sources)
ze_add_source(ZE3dsMaxInteriorExporterDescription.cpp		Sources)
ze_add_source(ZE3dsMaxInteriorExporterProcess.cpp			Sources)
ze_add_source(ZE3dsMaxInteriorExporterOptionsDialog.h		Sources QtMocs)
ze_add_source(ZE3dsMaxInteriorExporterOptionsDialog.cpp		Sources)
ze_add_source(ZE3dsMaxInteriorExporterOptionsWidget.ui		Sources QtUI)

ze_add_source(ZE3dsMaxModelExporter.h						Sources)
ze_add_source(ZE3dsMaxModelExporter.cpp						Sources)
ze_add_source(ZE3dsMaxModelExporterDescription.h			Sources)
ze_add_source(ZE3dsMaxModelExporterDescription.cpp			Sources)
ze_add_source(ZE3dsMaxModelExporterProcess.cpp				Sources)
ze_add_source(ZE3dsMaxModelExporterOptionsDialog.h			Sources QtMocs)
ze_add_source(ZE3dsMaxModelExporterOptionsDialog.cpp		Sources)
ze_add_source(ZE3dsMaxModelExporterOptionsWidget.ui			Sources QtUI)
ze_add_source(ZE3dsMaxUI.h									Sources)
ze_add_source(ZE3dsMaxUI.cpp								Sources)
ze_add_source(ZE3dsMaxUIDescription.h						Sources)
ze_add_source(ZE3dsMaxUIDescription.cpp						Sources)
ze_add_source(ZE3dsMaxScriptCommonUtilities.ms 				Sources ZEFC)
ze_add_source(ZE3dsMaxScriptHelper.ms						Sources ZEFC)
ze_add_source(ZE3dsMaxScriptRemove.ms 						Sources ZEFC)
ze_add_source(ZE3dsMaxInteriorActions.cpp					Sources)
ze_add_source(ZE3dsMaxInteriorActions.h						Sources)
ze_add_source(ZE3dsMaxInteriorScriptRoom.ms					Sources ZEFC)
ze_add_source(ZE3dsMaxInteriorScriptDoor.ms					Sources ZEFC)
ze_add_source(ZE3dsMaxModelActions.h						Sources)
ze_add_source(ZE3dsMaxModelActions.cpp						Sources)
ze_add_source(ZE3dsMaxModelScriptMesh.ms					Sources ZEFC)
ze_add_source(ZE3dsMaxModelScriptBone.ms					Sources ZEFC)
ze_add_source(ZE3dsMaxModelScriptPhysicalBody.ms			Sources ZEFC)
ze_add_source(ZE3dsMaxModelScriptBoundingBox.ms				Sources ZEFC)
ze_add_source(ZE3dsMaxScriptMeshBatcher.ms					Sources ZEFC)
ze_add_source(ZEToolbarIcons_16a.bmp						Sources ZEFC)
ze_add_source(ZEToolbarIcons_16i.bmp						Sources ZEFC)
ze_add_source(ZEToolbarIcons_24a.bmp						Sources ZEFC)
ze_add_source(ZEToolbarIcons_24i.bmp						Sources ZEFC)
ze_add_source(zineklogo.bmp									Sources ZEFC)

ze_add_source(ZEResourceOption.h			 				Sources)
ze_add_source(ZEResourceOption.cpp			  				Sources)
ze_add_source(ZEResourceOptionWidget.ui						Sources QtUI)
ze_add_source(ZEResourceOptionWidget.h						Sources QtMocs)
ze_add_source(ZEResourceOptionWidget.cpp					Sources)
ze_add_source(ZEResourceOptionTreeItem.cpp					Sources)
ze_add_source(ZEResourceOptionTreeItem.h					Sources)
ze_add_source(ZEResourceConfigurator.h						Sources)
ze_add_source(ZEResourceConfigurator.cpp					Sources)
ze_add_source(ZEResourceConfiguratorWidget.ui				Sources QtUI)
ze_add_source(ZEResourceConfiguratorWidget.h				Sources QtMocs)
ze_add_source(ZEResourceConfiguratorWidget.cpp				Sources)

ze_add_source(ZEProgressDialog.h							Sources)
ze_add_source(ZEProgressDialog.cpp							Sources)
ze_add_source(ZEProgressDialogTask.h						Sources)
ze_add_source(ZEProgressDialogTask.cpp						Sources)
ze_add_source(ZEProgressDialog.ui							Sources QtUI)
ze_add_source(ZEProgresDialogSignalHandler.h				Sources QtMocs)
ze_add_source(ZEProgresDialogSignalHandler.cpp				Sources)
ze_add_source(ZEProgressDialogTreeWidget.h					Sources)
ze_add_source(ZEProgressDialogTreeWidget.cpp				Sources)

qt5_add_resources(QtResourceFiles ${QtResources})
qt5_wrap_ui(QtUIFiles ${QtUI})
qt5_wrap_cpp(QtMocFiles ${QtMocs})

source_group("Generated" FILES ${QtResourceFiles} ${QtMocFiles} ${QtUIFiles})

ze_add_library(TARGET ZE3dsMaxPlugin
	SOURCES ${Sources} ${QtMocFiles} ${QtUIFiles} ${QtResourceFiles}
	ZEFC ${ZEFC}
	LIBS ZEFoundation lib3dsMax2015
	DLL)

qt5_use_modules(ZE3dsMaxPlugin Widgets)

set_property(TARGET ZE3dsMaxPlugin APPEND PROPERTY COMPILE_FLAGS "/GR")
set_property(TARGET ZE3dsMaxPlugin PROPERTY SUFFIX ".gup")
