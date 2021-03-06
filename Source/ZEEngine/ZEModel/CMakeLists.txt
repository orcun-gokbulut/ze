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

ze_add_source(ZEModel.cpp					Sources)
ze_add_source(ZEModel.h						Sources Headers ZEMC)
ze_add_source(ZEMDVertex.cpp				Sources)
ze_add_source(ZEMDVertex.h					Sources Headers)
ze_add_source(ZEMDAnimationKey.cpp			Sources)
ze_add_source(ZEMDAnimationKey.h			Sources Headers)
ze_add_source(ZEMDAnimationData.cpp			Sources)
ze_add_source(ZEMDAnimationData.h			Sources Headers)
ze_add_source(ZEModelAnimation.cpp			Sources)
ze_add_source(ZEModelAnimation.h			Sources Headers ZEMC)
ze_add_source(ZEModelAnimationTrack.cpp		Sources)
ze_add_source(ZEModelAnimationTrack.h		Sources Headers ZEMC)
ze_add_source(ZEModelBone.cpp				Sources)
ze_add_source(ZEModelBone.h					Sources Headers ZEMC)
ze_add_source(ZEModelIKChain.cpp			Sources)
ze_add_source(ZEModelIKChain.h				Sources Headers)
ze_add_source(ZEModelIKChainNode.cpp		Sources)
ze_add_source(ZEModelIKChainNode.h			Sources Headers)
ze_add_source(ZEModelMesh.cpp				Sources)
ze_add_source(ZEModelMesh.h					Sources Headers ZEMC)
ze_add_source(ZEModelMeshLOD.cpp			Sources)
ze_add_source(ZEModelMeshLOD.h				Sources Headers ZEMC)
ze_add_source(ZEModelDraw.cpp				Sources)
ze_add_source(ZEModelDraw.h					Sources Headers ZEMC)
ze_add_source(ZEModelHelper.cpp				Sources)
ze_add_source(ZEModelHelper.h				Sources Headers ZEMC)

ze_add_source(ZEMDResource.cpp				Sources)
ze_add_source(ZEMDResource.h				Sources Headers ZEMC)
ze_add_source(ZEMDResourceMesh.cpp			Sources)
ze_add_source(ZEMDResourceMesh.h			Sources Headers ZEMC)
ze_add_source(ZEMDResourceLOD.cpp			Sources)
ze_add_source(ZEMDResourceLOD.h				Sources Headers ZEMC)
ze_add_source(ZEMDResourceDraw.cpp			Sources)
ze_add_source(ZEMDResourceDraw.h			Sources Headers ZEMC)
ze_add_source(ZEMDResourceBone.cpp			Sources)
ze_add_source(ZEMDResourceBone.h			Sources Headers ZEMC)
ze_add_source(ZEMDResourcePhysics.cpp		Sources)
ze_add_source(ZEMDResourcePhysics.h			Sources Headers ZEMC)
ze_add_source(ZEMDResourceAnimation.cpp		Sources)
ze_add_source(ZEMDResourceAnimation.h		Sources Headers ZEMC)
ze_add_source(ZEMDResourceHelper.cpp		Sources)
ze_add_source(ZEMDResourceHelper.h			Sources Headers ZEMC)

ze_add_library(TARGET ZEModel 
	ZEMC ${ZEMC} 
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZEFoundation)
