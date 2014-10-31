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

ze_add_source(ZEModel.cpp								Sources)
ze_add_source(ZEModel.h									Sources Headers ZEPP)
ze_add_source(ZEModelAnimation.cpp						Sources)
ze_add_source(ZEModelAnimation.h						Sources Headers)
ze_add_source(ZEModelAnimationTrack.cpp					Sources)
ze_add_source(ZEModelAnimationTrack.h					Sources Headers)
ze_add_source(ZEModelBone.cpp							Sources)
ze_add_source(ZEModelBone.h								Sources Headers)
ze_add_source(ZEModelIKChain.cpp						Sources)
ze_add_source(ZEModelIKChain.h							Sources Headers)
ze_add_source(ZEModelIKChainNode.cpp					Sources)
ze_add_source(ZEModelIKChainNode.h						Sources Headers)
ze_add_source(ZEModelMesh.cpp							Sources)
ze_add_source(ZEModelMesh.h								Sources Headers)
ze_add_source(ZEModelMeshLOD.cpp						Sources)
ze_add_source(ZEModelMeshLOD.h							Sources Headers)
ze_add_source(ZEModelResource.cpp						Sources)
ze_add_source(ZEModelResource.h							Sources Headers)
ze_add_source(ZEModelHelper.cpp							Sources)
ze_add_source(ZEModelHelper.h							Sources Headers)
ze_add_source(ZEModelDebugDrawer.cpp					Sources)
ze_add_source(ZEModelDebugDrawer.h						Sources Headers)
ze_add_source(ZEModelAnimationNode.cpp					Sources)
ze_add_source(ZEModelAnimationNode.h					Sources Headers)
ze_add_source(ZEModelAnimationGraph.cpp					Sources)
ze_add_source(ZEModelAnimationGraph.h					Sources Headers)
ze_add_source(ZEModelAnimationClip.cpp					Sources)
ze_add_source(ZEModelAnimationClip.h					Sources Headers)
ze_add_source(ZEModelAnimationNodeMap.cpp				Sources)
ze_add_source(ZEModelAnimationNodeMap.h					Sources Headers)
ze_add_source(ZEModelAnimationNodeBlend.cpp				Sources)
ze_add_source(ZEModelAnimationNodeBlend.h				Sources Headers)
ze_add_source(ZEModelAnimationNodeAdd.cpp				Sources)
ze_add_source(ZEModelAnimationNodeAdd.h					Sources Headers)
ze_add_source(ZEModelAnimationNodeClip.cpp				Sources)
ze_add_source(ZEModelAnimationNodeClip.h				Sources Headers)
ze_add_source(ZEModelAnimationNodeStateMachine.cpp		Sources)
ze_add_source(ZEModelAnimationNodeStateMachine.h		Sources Headers)
ze_add_source(ZEModelAnimationNodeState.cpp				Sources)
ze_add_source(ZEModelAnimationNodeState.h				Sources Headers)
ze_add_source(ZEModelAnimationNodeTransition.cpp		Sources)
ze_add_source(ZEModelAnimationNodeTransition.h			Sources Headers)
ze_add_source(ZEModelAnimationController.cpp			Sources)
ze_add_source(ZEModelAnimationController.h				Sources Headers)

ze_add_library(TARGET ZEModel 
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZEFoundation)
