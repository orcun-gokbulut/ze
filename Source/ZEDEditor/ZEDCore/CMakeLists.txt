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

ze_add_source(ZEDEditorCore.cpp							Sources)
ze_add_source(ZEDEditorCore.h							Sources ZEMC)
ze_add_source(ZEDEditor.cpp								Sources)
ze_add_source(ZEDEditor.h								Sources ZEMC)
ze_add_source(ZEDEditorEvent.cpp						Sources)
ze_add_source(ZEDEditorEvent.h							Sources ZEMC)
ze_add_source(ZEDEvent.cpp								Sources)
ze_add_source(ZEDEvent.h								Sources ZEMC)
ze_add_source(ZEDComponent.cpp							Sources)
ze_add_source(ZEDComponent.h							Sources ZEMC)

ze_add_source(ZEDOperation.cpp							Sources)
ze_add_source(ZEDOperation.h							Sources ZEMC)
ze_add_source(ZEDOperationManager.cpp					Sources)
ze_add_source(ZEDOperationManager.h						Sources ZEMC)

ze_add_source(ZEDObjectWrapper.cpp						Sources)
ze_add_source(ZEDObjectWrapper.h						Sources ZEMC)
ze_add_source(ZEDObjectEvent.cpp						Sources)
ze_add_source(ZEDObjectEvent.h							Sources ZEMC)
ze_add_source(ZEDObjectManager.cpp						Sources)
ze_add_source(ZEDObjectManager.h						Sources ZEMC)

ze_add_source(ZEDCreateOperation.cpp					Sources)
ze_add_source(ZEDCreateOperation.h						Sources ZEMC)
ze_add_source(ZEDDeleteOperation.cpp					Sources)
ze_add_source(ZEDDeleteOperation.h						Sources ZEMC)
ze_add_source(ZEDRelocateOperation.cpp					Sources)
ze_add_source(ZEDRelocateOperation.h					Sources ZEMC)
ze_add_source(ZEDCloneOperation.cpp						Sources)
ze_add_source(ZEDCloneOperation.h						Sources ZEMC)
ze_add_source(ZEDPropertyOperation.cpp					Sources)
ze_add_source(ZEDPropertyOperation.h					Sources ZEMC)

ze_add_source(ZEDTransformationManager.cpp				Sources)
ze_add_source(ZEDTransformationManager.h				Sources ZEMC)
ze_add_source(ZEDTransformationEvent.cpp				Sources)
ze_add_source(ZEDTransformationEvent.h					Sources ZEMC)
ze_add_source(ZEDTransformationOperation.cpp			Sources)
ze_add_source(ZEDTransformationOperation.h				Sources ZEMC)

ze_add_source(ZEDSelectionManager.cpp					Sources)
ze_add_source(ZEDSelectionManager.h						Sources ZEMC)
ze_add_source(ZEDSelectionEvent.cpp						Sources)
ze_add_source(ZEDSelectionEvent.h						Sources ZEMC)

ze_add_source(ZEDViewport.cpp							Sources)
ze_add_source(ZEDViewport.h								Sources QtMocs)
ze_add_source(ZEDViewportEvent.cpp						Sources)
ze_add_source(ZEDViewportEvent.h						Sources ZEMC)
ze_add_source(ZEDViewportManager.cpp					Sources)
ze_add_source(ZEDViewportManager.h						Sources ZEMC)
ze_add_source(ZEDViewportController.cpp					Sources)
ze_add_source(ZEDViewportController.h					Sources ZEMC)

ze_add_source(ZEDAsset.cpp								Sources)
ze_add_source(ZEDAsset.h								Sources ZEMC)
ze_add_source(ZEDAssetType.cpp							Sources)
ze_add_source(ZEDAssetType.h							Sources ZEMC)
ze_add_source(ZEDAssetManager.cpp						Sources)
ze_add_source(ZEDAssetManager.h							Sources ZEMC)

ze_add_source(ZEDGizmo.cpp								Sources)
ze_add_source(ZEDGizmo.h								Sources ZEMC)
ze_add_source(ZEDGrid.cpp								Sources)
ze_add_source(ZEDGrid.h									Sources ZEMC)


qt5_wrap_cpp (QtMocFiles ${QtMocs})
qt5_wrap_ui (QtUIFiles ${QtUI})

ze_add_library(TARGET ZEDCore 
	ZEMC ${ZEMC}
	SOURCES ${Sources} ${QtMocFiles} ${QtUIFiles} 
	LIBS ZEFoundation)

source_group("Generated" FILES ${QtMocFiles} ${QtUIFiles})

qt5_use_modules(ZEDCore Widgets)
