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

ze_add_source(ZE3dsMapExporterDLLEntry.cpp		Sources)
ze_add_source(ZE3dsMapExporterDLLEntry.h		Sources)
ze_add_source(ZE3dsMapExporter.h				Sources)
ze_add_source(ZE3dsMapExporter.cpp				Sources)
ze_add_source(ZE3dsMapExporterDescription.h		Sources ZEFC)
ze_add_source(ZE3dsMapExporterDescription.cpp	Sources)
ze_add_source(ZE3dsMapExporterProcess.cpp		Sources)
ze_add_source(ZE3dsMapExporterOptions.h			Sources)
ze_add_source(ZE3dsMapExporterOptions.cpp		Sources)
ze_add_source(ZEMapExporterOptionsDialog.h		Sources QtMocs)
ze_add_source(ZEMapExporterOptionsDialog.cpp	Sources)
ze_add_source(ZE3dsMapExporterResources.rc		Sources)
ze_add_source(ZE3dsMapExporterResources.h		Sources)
ze_add_source(ZE3dsMapExporter.def				Sources)
ze_add_source(ZEMapExporterOptionsWidget.ui		Sources QtUI)
#ze_add_source(../ZE3dsProgressDialog/ZE3dsProgressDialogResources.rc	Sources)

qt4_add_resources (QtResourceFiles ${QtResources})
qt4_wrap_ui (QtUIFiles ${QtUI})
qt4_wrap_cpp (QtMocFiles  ${QtMocs})

ze_add_library(ZE3dsMapExporter 
	SOURCES ${Sources} ${QtMocFiles} ${QtUIFiles} ${QtResourceFiles}
	LIBS ZEFoundation ZEToolAPI ZE3dsProgressDialog
		core geom gfx mesh maxutil maxscrpt paramblk2 igame QtCore4 QtGui4 QWinWidget ZEProgressDialog
	INSTALL
	DLL)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
	
set_property(TARGET ZE3dsMapExporter PROPERTY FOLDER "ZE3dsMax")
set_property(TARGET ZE3dsMapExporter PROPERTY SUFFIX ".dle")
set_property(TARGET ZE3dsMapExporter PROPERTY CLEAN_DIRECT_OUTPUT 1)

set(LOCAL_WARNING_FLAGS /W3)
set(LOCAL_RTTI_FLAGS /GR)
	
set(MAX_BINARY_DIRECTORY "C:\\Program Files (x86)\\Autodesk\\3ds Max 2010")
get_property(TargetPath TARGET ZE3dsMapExporter PROPERTY LOCATION)
file(TO_NATIVE_PATH ${TargetPath} NativeTargetPath)
add_custom_command (TARGET ZE3dsMapExporter POST_BUILD COMMAND copy "${NativeTargetPath}" "${ZEBUILD_3DS_MAX_BINARY_DIRECTORY}\\plugins")
