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
#include "ZEUI/ZEUILabel.h"
#include "ZEInput/ZEInputMap.h"

class ZEUISceneStatisticsControl : public ZEUIControl
{
	private:

		ZEInputMap				InputMap;

		ZEUILabel*				FPSCount;
		ZEUILabel*				CameraPosition;

		ZEUILabel*				SceneStatisticsDescription;
		ZEUILabel*				SceneTotalEntityCount;
		ZEUILabel*				SceneDrawableEntityCount;
		ZEUILabel*				SceneCulledEntityCount;
		ZEUILabel*				SceneDrawedEntityCount;

		ZEUILabel*				SceneTotalLightCount;
		ZEUILabel*				SceneCulledLightCount;
		ZEUILabel*				SceneDrawedLightCount;

		ZEUILabel*				RendererStatisticsDescription;
		ZEUILabel*				RendererMeshCount;
		ZEUILabel*				RendererInstancedMeshCount;

		ZEUILabel*				RendererStageCount;
		ZEUILabel*				RendererPostProcessorCount;

		ZEUILabel*				RendererPrimitiveCount;
		ZEUILabel*				RendererMinBatchCount;
		ZEUILabel*				RendererMaxBatchCount;
		ZEUILabel*				RendererAvgBatchCount;
		ZEUILabel*				RendererVertexCount;
		ZEUILabel*				RendererDrawCallCount;
		ZEUILabel*				RendererDrawPrimitiveCount;

		ZEUILabel*				GraphicsStatisticsDescription;
		ZEUILabel*				GraphicsVertexBufferCount;
		ZEUILabel*				GraphicsVertexDeclarationCount;
		ZEUILabel*				GraphicsTotalVertexBufferSize;
		ZEUILabel*				GraphicsShaderCount;
		ZEUILabel*				GraphicsTextureCount;
		ZEUILabel*				GraphicsTexture2DCount;
		ZEUILabel*				GraphicsTexture3DCount;
		ZEUILabel*				GraphicsTextureCubeCount;
		ZEUILabel*				GraphicsTotalTextureSize;
		ZEUILabel*				GraphicsRenderTargetCount;
		ZEUILabel*				GraphicsTotalRenderTargetSize;
		ZEUILabel*				GraphicsDepthBufferCount;
		ZEUILabel*				GraphicsTotalDepthBufferSize;

		ZEUILabel*				ModelStatisticsDescription;
		ZEUILabel*				ModelTotalMeshCount;
		ZEUILabel*				ModelCulledMeshCount;
		ZEUILabel*				ModelDrawnMeshCount;

		ZEUILabel*				InteriorStatisticsDescription;
		ZEUILabel*				InteriorTotalRoomCount;
		ZEUILabel*				InteriorCulledRoomCount;
		ZEUILabel*				InteriorDrawedRoomCount;

		ZEUILabel*				InteriorTotalDoorCount;
		ZEUILabel*				InteriorSeenDoorCount;

		ZEUILabel*				InteriorTotalPolygonCount;
		ZEUILabel*				InteriorCulledPolygonCount;
		ZEUILabel*				InteriorDrawedPolygonCount;

		ZEUILabel*				ParticleStatisticsDescription;
		ZEUILabel*				ParticleTotalParticleCount;
		ZEUILabel*				ParticleDrawedParticleCount;
		ZEUILabel*				ParticleEmitterCount;

	public:

		void					SetMaterial(ZERNMaterial* Material);
		ZERNMaterial*				GetMaterial() const;

		void					Draw(ZEUIRenderer* Renderer);
		void					Tick(float ElapsedTime);

								ZEUISceneStatisticsControl();


};

#endif
