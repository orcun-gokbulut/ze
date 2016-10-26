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

ze_add_source(ZECamera.cpp							Sources)
ze_add_source(ZECamera.h							Sources Headers ZEMC)

ze_add_source(ZERNRenderer.cpp 						Sources)
ze_add_source(ZERNRenderer.h 						Sources Headers ZEMC)
ze_add_source(ZERNCommand.cpp 						Sources)
ze_add_source(ZERNCommand.h 						Sources Headers ZEMC)
ze_add_source(ZERNInstanceTag.cpp 					Sources)
ze_add_source(ZERNInstanceTag.h 					Sources Headers ZEMC)
ze_add_source(ZERNView.cpp							Sources)
ze_add_source(ZERNView.h							Sources Headers ZEMC)
ze_add_source(ZERNRenderParameters.cpp				Sources)
ze_add_source(ZERNRenderParameters.h				Sources Headers ZEMC)
ze_add_source(ZERNFilter.cpp						Sources)
ze_add_source(ZERNFilter.h							Sources Headers)
ze_add_source(ZERNScreenUtilities.h 				Sources Headers ZEMC)
ze_add_source(ZERNScreenUtilities.cpp				Sources)

ze_add_source(ZELight.cpp 							Sources)
ze_add_source(ZELight.h 							Sources Headers ZEMC)
ze_add_source(ZELightPoint.cpp 						Sources)
ze_add_source(ZELightPoint.h 						Sources Headers ZEMC)
ze_add_source(ZELightProjective.cpp 				Sources)
ze_add_source(ZELightProjective.h 					Sources Headers ZEMC)
ze_add_source(ZELightDirectional.cpp	 			Sources)
ze_add_source(ZELightDirectional.h					Sources Headers ZEMC)
ze_add_source(ZELightOmniProjective.cpp 			Sources)
ze_add_source(ZELightOmniProjective.h 				Sources Headers ZEMC)

ze_add_source(ZERNShaderSlots.cpp 					Sources)
ze_add_source(ZERNShaderSlots.h 					Sources Headers)
ze_add_source(ZERNMaterial.cpp 						Sources)
ze_add_source(ZERNMaterial.h 						Sources Headers ZEMC)
ze_add_source(ZERNSimpleMaterial.cpp 				Sources)
ze_add_source(ZERNSimpleMaterial.h 					Sources Headers ZEMC)
ze_add_source(ZERNStandardMaterial.cpp 				Sources)
ze_add_source(ZERNStandardMaterial.h 				Sources Headers ZEMC)
ze_add_source(ZERNParticleMaterial.cpp 				Sources)
ze_add_source(ZERNParticleMaterial.h 				Sources Headers ZEMC)

ze_add_source(ZERNStage.h 							Sources Headers ZEMC)
ze_add_source(ZERNStage.cpp 						Sources)
ze_add_source(ZERNStageID.h 						Sources Headers ZEMC)
ze_add_source(ZERNStageID.cpp 						Sources)
ze_add_source(ZERNStageManager.h 					Sources Headers ZEMC)
ze_add_source(ZERNStageManager.cpp 					Sources)
ze_add_source(ZERNStageInputTexture.h 				Sources Headers ZEMC)
ze_add_source(ZERNStageInputTexture.cpp				Sources)
ze_add_source(ZERNStageOutput.h 					Sources Headers ZEMC)
ze_add_source(ZERNStageOutput.cpp					Sources)
ze_add_source(ZERNStageOutputTexture.h 				Sources Headers ZEMC)
ze_add_source(ZERNStageOutputTexture.cpp			Sources)
ze_add_source(ZERNStageGBuffer.h 					Sources Headers ZEMC)
ze_add_source(ZERNStageGBuffer.cpp 					Sources)
ze_add_source(ZERNStageAO.h 						Sources Headers ZEMC)
ze_add_source(ZERNStageAO.cpp 						Sources)
ze_add_source(ZERNStageLighting.h 					Sources Headers ZEMC)
ze_add_source(ZERNStageLighting.cpp					Sources)
ze_add_source(ZERNStageHDR.h 						Sources Headers ZEMC)
ze_add_source(ZERNStageHDR.cpp 						Sources)
ze_add_source(ZERNStageForward.h 					Sources Headers ZEMC)
ze_add_source(ZERNStageForward.cpp 					Sources)
ze_add_source(ZERNStageMultiplexer.h 				Sources Headers ZEMC)
ze_add_source(ZERNStageMultiplexer.cpp				Sources)
ze_add_source(ZERNStageDisplay.h 					Sources Headers ZEMC)
ze_add_source(ZERNStageDisplay.cpp					Sources)
ze_add_source(ZERNStagePostProcess.h 				Sources Headers ZEMC)
ze_add_source(ZERNStagePostProcess.cpp				Sources)
ze_add_source(ZERNStageShadowmapGeneration.h		Sources Headers ZEMC)
ze_add_source(ZERNStageShadowmapGeneration.cpp		Sources)
ze_add_source(ZERNStageShadowing.h					Sources Headers ZEMC)
ze_add_source(ZERNStageShadowing.cpp				Sources)
ze_add_source(ZERNStageParticleRendering.h			Sources Headers ZEMC)
ze_add_source(ZERNStageParticleRendering.cpp		Sources)
ze_add_source(ZERNStageAntiAliasing.h				Sources Headers ZEMC)
ze_add_source(ZERNStageAntiAliasing.cpp				Sources)
ze_add_source(ZERNStageDebug.h 						Sources Headers ZEMC)
ze_add_source(ZERNStageDebug.cpp					Sources)
ze_add_source(ZERNStage2D.h 						Sources Headers ZEMC)
ze_add_source(ZERNStage2D.cpp						Sources)
ze_add_source(ZERNStageResolving.h 					Sources Headers ZEMC)
ze_add_source(ZERNStageResolving.cpp				Sources)
ze_add_source(ZERNStageRenderDepth.h 				Sources Headers ZEMC)
ze_add_source(ZERNStageRenderDepth.cpp				Sources)
ze_add_source(ZERNStageAtmosphere.h 				Sources Headers ZEMC)
ze_add_source(ZERNStageAtmosphere.cpp				Sources)

ze_add_library(TARGET ZERenderer 
	SOURCES ${Sources}
	HEADERS ${Headers}
	ZEMC ${ZEMC}
	LIBS ZEFoundation libFreeImage)
