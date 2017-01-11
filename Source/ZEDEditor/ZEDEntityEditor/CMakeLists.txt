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

ze_add_source(ZEDEntityEditor.cpp						Sources)
ze_add_source(ZEDEntityEditor.h							Sources ZEMC)
ze_add_source(ZEDSceneWrapper.cpp						Sources)
ze_add_source(ZEDSceneWrapper.h							Sources ZEMC)
ze_add_source(ZEDEntityWrapper.cpp						Sources)
ze_add_source(ZEDEntityWrapper.h						Sources ZEMC)
ze_add_source(ZEDSectorWrapper.cpp						Sources)
ze_add_source(ZEDSectorWrapper.h						Sources ZEMC)

ze_add_source(ZEDMaterialAsset.cpp						Sources)
ze_add_source(ZEDMaterialAsset.h						Sources)
ze_add_source(ZEDMaterialAssetType.cpp					Sources)
ze_add_source(ZEDMaterialAssetType.h					Sources ZEMC)
ze_add_source(ZEDModelAsset.cpp							Sources)
ze_add_source(ZEDModelAsset.h							Sources)
ze_add_source(ZEDModelAssetType.cpp						Sources)
ze_add_source(ZEDModelAssetType.h						Sources ZEMC)
ze_add_source(ZEDPrefabAsset.cpp						Sources)
ze_add_source(ZEDPrefabAsset.h							Sources)
ze_add_source(ZEDPrefabAssetType.cpp					Sources)
ze_add_source(ZEDPrefabAssetType.h						Sources ZEMC)
ze_add_source(ZEDSceneAsset.cpp							Sources)
ze_add_source(ZEDSceneAsset.h							Sources)
ze_add_source(ZEDSceneAssetType.cpp						Sources)
ze_add_source(ZEDSceneAssetType.h						Sources ZEMC)
ze_add_source(ZEDSectorAsset.cpp						Sources)
ze_add_source(ZEDSectorAsset.h							Sources)
ze_add_source(ZEDSectorAssetType.cpp					Sources)
ze_add_source(ZEDSectorAssetType.h						Sources ZEMC)
ze_add_source(ZEDSoundAsset.cpp							Sources)
ze_add_source(ZEDSoundAsset.h							Sources)
ze_add_source(ZEDSoundAssetType.cpp						Sources)
ze_add_source(ZEDSoundAssetType.h						Sources ZEMC)
ze_add_source(ZEDTextureAsset.cpp						Sources)
ze_add_source(ZEDTextureAsset.h							Sources)
ze_add_source(ZEDTextureAssetType.cpp					Sources)
ze_add_source(ZEDTextureAssetType.h						Sources ZEMC)

ze_add_library(TARGET ZEDEntityEditor 
	ZEMC ${ZEMC}
	SOURCES ${Sources} ${QtMocFiles} ${QtUIFiles} 
	LIBS ZEFoundation ZEDCore ZEDUserInterface)

source_group("Generated" FILES ${QtMocFiles} ${QtUIFiles})
