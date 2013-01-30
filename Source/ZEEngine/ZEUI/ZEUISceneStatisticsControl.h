//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUISceneStatisticsControl.h
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
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#pragma once
#ifndef __ZE_SCENE_STATISTICS_CONTROL_H__
#define __ZE_SCENE_STATISTICS_CONTROL_H__

#include "ZEUI/ZEUIControl.h"
#include "ZEUI/ZEUITextControl.h"
#include "ZEInput/ZEInputMap.h"

class ZEUISceneStatisticsControl : public ZEUIControl
{
	private:

		ZEInputMap				InputMap;

		ZEUITextControl*		FPSCount;
		ZEUITextControl*		CameraPosition;

		ZEUITextControl*		SceneStatisticsDescription;
		ZEUITextControl*		SceneTotalEntityCount;
		ZEUITextControl*		SceneDrawableEntityCount;
		ZEUITextControl*		SceneCulledEntityCount;
		ZEUITextControl*		SceneDrawedEntityCount;

		ZEUITextControl*		SceneTotalLightCount;
		ZEUITextControl*		SceneCulledLightCount;
		ZEUITextControl*		SceneDrawedLightCount;

		ZEUITextControl*		RendererStatisticsDescription;
		ZEUITextControl*		RendererMeshCount;
		ZEUITextControl*		RendererInstancedMeshCount;

		ZEUITextControl*		RendererStageCount;
		ZEUITextControl*		RendererPostProcessorCount;

		ZEUITextControl*		RendererPrimitiveCount;
		ZEUITextControl*		RendererMinBatchCount;
		ZEUITextControl*		RendererMaxBatchCount;
		ZEUITextControl*		RendererAvgBatchCount;
		ZEUITextControl*		RendererVertexCount;
		ZEUITextControl*		RendererDrawCallCount;
		ZEUITextControl*		RendererDrawPrimitiveCount;

		ZEUITextControl*		GraphicsStatisticsDescription;
		ZEUITextControl*		GraphicsVertexBufferCount;
		ZEUITextControl*		GraphicsVertexDeclarationCount;
		ZEUITextControl*		GraphicsTotalVertexBufferSize;
		ZEUITextControl*		GraphicsShaderCount;
		ZEUITextControl*		GraphicsTextureCount;
		ZEUITextControl*		GraphicsTexture2DCount;
		ZEUITextControl*		GraphicsTexture3DCount;
		ZEUITextControl*		GraphicsTextureCubeCount;
		ZEUITextControl*		GraphicsTotalTextureSize;
		ZEUITextControl*		GraphicsRenderTargetCount;
		ZEUITextControl*		GraphicsTotalRenderTargetSize;
		ZEUITextControl*		GraphicsDepthBufferCount;
		ZEUITextControl*		GraphicsTotalDepthBufferSize;

		ZEUITextControl*		ModelStatisticsDescription;
		ZEUITextControl*		ModelTotalMeshCount;
		ZEUITextControl*		ModelCulledMeshCount;
		ZEUITextControl*		ModelDrawnMeshCount;

		ZEUITextControl*		InteriorStatisticsDescription;
		ZEUITextControl*		InteriorTotalRoomCount;
		ZEUITextControl*		InteriorCulledRoomCount;
		ZEUITextControl*		InteriorDrawedRoomCount;

		ZEUITextControl*		InteriorTotalDoorCount;
		ZEUITextControl*		InteriorSeenDoorCount;

		ZEUITextControl*		InteriorTotalPolygonCount;
		ZEUITextControl*		InteriorCulledPolygonCount;
		ZEUITextControl*		InteriorDrawedPolygonCount;

		ZEUITextControl*		ParticleStatisticsDescription;
		ZEUITextControl*		ParticleTotalParticleCount;
		ZEUITextControl*		ParticleDrawedParticleCount;
		ZEUITextControl*		ParticleEmitterCount;

	public:

		void					SetMaterial(ZEMaterial* Material);
		ZEMaterial*				GetMaterial() const;

		void					Draw(ZEUIRenderer* Renderer);
		void					Tick(float ElapsedTime);

								ZEUISceneStatisticsControl();


};

#endif
