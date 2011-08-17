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

project (ZEditior)

find_package(Qt4 REQUIRED)

include_directories (
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/../Source
	${PROJECT_SOURCE_DIR}/../ZEFoundation
	${PROJECT_SOURCE_DIR}/../ZEToolSDK
	${PROJECT_SOURCE_DIR}/../Include 
	${PROJECT_SOURCE_DIR}/../Include/DirectX
	${PROJECT_SOURCE_DIR}/../Include/OpenAL
	${PROJECT_SOURCE_DIR}/../Include/DirectX
	${PROJECT_SOURCE_DIR}/../Include/PhysX/Cooking
	${PROJECT_SOURCE_DIR}/../Include/PhysX/Foundation
	${PROJECT_SOURCE_DIR}/../Include/PhysX/NxCharacter
	${PROJECT_SOURCE_DIR}/../Include/PhysX/Physics
	${PROJECT_SOURCE_DIR}/../Include/Curl
	${PROJECT_SOURCE_DIR}/../ZEFoundation
	${PROJECT_BINARY_DIR}/ZEDSceneEditor
	${PROJECT_BINARY_DIR}/ZEDMaterialEditor
	${PROJECT_BINARY_DIR}/ZEDBrowser
	${PROJECT_BINARY_DIR}/ZEDSoundExtensionsPlugIn
	${QT_INCLUDE_DIR}
	${QT_INCLUDE_DIR}/QtGui
	${QT_INCLUDE_DIR}/QtCore)
	
link_directories(
	"${PROJECT_SOURCE_DIR}/../Lib/Win x86/$(Configuration)" 
	"${PROJECT_SOURCE_DIR}/../Lib/Win x86/DirectX" 
	"${PROJECT_SOURCE_DIR}/../Lib/Win x86/OpenAL" 
	"${PROJECT_SOURCE_DIR}/../Lib/Win x86/PhysX"
	"${PROJECT_SOURCE_DIR}/../Lib/Win x86/Qt")

add_definitions(${QT_DEFINITIONS})

add_subdirectory (ZEDBrowser)
add_subdirectory (ZEDCommonControls)
add_subdirectory (ZEDCommonEntities)
add_subdirectory (ZEDCore)
add_subdirectory (ZEDSceneEditor)
add_subdirectory (ZEDMaterialEditor)
add_subdirectory (ZEDScriptEditor)
add_subdirectory (ZEDUndoRedo)
add_subdirectory (ZEDSoundExtensionsPlugIn)
add_subdirectory (ZEDImageExtensionsPlugIn)

add_source (ZEDMain.cpp		Sources)

add_executable (ZEditor WIN32 ${Sources})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ZEditor PROPERTY FOLDER "ZEditor")

target_link_libraries (ZEditor ZECore ZEFoundation ZEDS ZEGame ZEGraphics ZEInput ZEMap ZEMath ZEMeta ZEModel ZEParticle ZEPhysics ZESound ZEUI ZESDK)
target_link_libraries (ZEditor ZEDirect3D9 ZEDirectInput ZEDirectSound ZEOpenAL ZEPhysX ZEDummyInput ZEVirtualInput ZEPortalMap)
target_link_libraries (ZEditor ZEDCommonControls ZEDCommonEntities ZEDCore ZEDPropertyWindowManager ZEDSceneEditor ZEDMaterialEditor ZEDScriptEditor ZEDUndoRedo ZEDBrowser ZEDSoundExtensionsPlugIn ZEDImageExtensionsPlugIn)
target_link_libraries (ZEditor QtCore4.lib QtGui4.lib)

install (TARGETS ZEditor RUNTIME DESTINATION bin COMPONENT "Runtime")
