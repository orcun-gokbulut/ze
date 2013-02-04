//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUISceneStatisticsControl.cpp
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

#include "ZEUISceneStatisticsControl.h"
#include "ZEDS/ZEString.h"
#include "ZEUI/ZEFontResource.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZECamera.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEWindow.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEDS/ZEFormat.h"

#define ACTIONID_VISIBILITY 1

#define FPSText								"Frame Per Second : {0:d}"
#define CamPosText							"Camera Position : X: {0:.5}, Y: {1:.5}, Z: {2:.5}"

#define SceneTotalEntityText				"Total Entity Count : {0:d}"
#define SceneDrawableEntityText				"Drawable Entity Count : {0:d}"
#define SceneCulledEntityText				"Culled Entity Count : {0:d}"
#define SceneDrawedEntityText				"Drawn Entity Count : {0:d}"
#define SceneTotalLightText					"Total Light Count : {0:d}"
#define SceneCulledLightText				"Culled Light Count : {0:d}"
#define SceneDrawedLightText				"Drawn Light Count : {0:d}"

#define RendererMeshText					"Mesh Count : {0:d}"
#define RendererInstancedMeshText			"Instanced Mesh Count : {0:d}"
#define RendererStageText					"Stage Count : {0:d}"
#define RendererPostProcessorText			"Post Processor Count : {0:d}" 
#define RendererPrimitiveText				"Primitive Count : {0:d}"
#define RendererMinBatchText				"Min Batch Count : {0:d}"
#define RendererMaxBatchText				"Max Batch Count : {0:d}"
#define RendererAvgBatchText				"Avg Batch Count : {0:d}"
#define RendererVertexText					"Vertex Count : {0:d}"
#define RendererDrawCallText				"Draw Call Count : {0:d}"
#define RendererDrawPrimitiveText			"Draw Primitive Count : {0:d}"

#define	GraphicsVertexBufferText			"Vertex Buffer Count : {0:d}"
#define	GraphicsVertexDeclarationText		"Vertex Declaration Count : {0:d}"
#define	GraphicsTotalVertexBufferSizeText	"Total Vertex Buffer Size : {0:d}"
#define	GraphicsShaderText					"Shader Count : {0:d}"
#define	GraphicsTextureText					"Texture Count : {0:d}"
#define	GraphicsTexture2DText				"Texture2D Count : {0:d}"
#define	GraphicsTexture3DText				"Texture3D Count : {0:d}"
#define	GraphicsTextureCubeText				"TextureCube Count : {0:d}"
#define	GraphicsTotalTextureSizeText		"Total Texture Size : {0:d}"
#define	GraphicsRenderTargetText			"Render Target Count : {0:d}"
#define	GraphicsTotalRenderTargetSizeText	"Total Render Target Size : {0:d}"
#define	GraphicsDepthBufferText				"Depth Buffer Count : {0:d}"
#define	GraphicsTotalDepthBufferSizeText	"Total Depth Buffer Size : {0:d}"

#define	ModelTotalMeshText					"Total Mesh Count : {0:d}"
#define	ModelCulledMeshText					"Culled Mesh Count : {0:d}"
#define	ModelDrawnMeshText					"Drawn Mesh Count : {0:d}"

#define InteriorTotalRoomText				"Total Room Count : {0:d}"
#define InteriorCulledRoomText				"Culled Room Count : {0:d}"
#define InteriorDrawedRoomText				"Drawn Room Count : {0:d}"
#define InteriorTotalDoorText				"Total Door Count : {0:d}"
#define InteriorSeenDoorText				"Viewed Door Count : {0:d}"
#define InteriorTotalPolygonText			"Total Interior Polygon Count : {0:d}"
#define InteriorCulledPolygonText			"Culled Interior Polygon Count : {0:d}"
#define InteriorDrawedPolygonText			"Drawn  Interior Polygon Count : {0:d}"

#define	ParticleEmitterText					"Emitter Count : {0:d}"
#define	ParticleTotalParticleText			"Total Particle Count : {0:d}"
#define	ParticleDrawedParticleText			"Drawn Particle Count : {0:d}"

#define GreenTextColor						ZEVector4(0.0f, 0.7f, 0.0f, 1.0f)
#define RedTextColor						ZEVector4(7.0f, 0.0f, 0.0f, 1.0f)

void ZEUISceneStatisticsControl::SetMaterial(ZEMaterial* Material)
{

}

ZEMaterial* ZEUISceneStatisticsControl::GetMaterial() const
{
	return NULL;
}

void ZEUISceneStatisticsControl::Draw(ZEUIRenderer* Renderer)
{
	ZEDrawStatistics Stats = zeScene->GetRenderer()->GetDrawParameters()->Statistics;
	ZEVariant Value;

	//Scene Statistics

	Value.SetUInt32(Stats.SceneStatistics.TotalEntityCount);
	SceneTotalEntityCount->SetText(ZEFormat::Format(SceneTotalEntityText, Value));

	Value.SetUInt32(Stats.SceneStatistics.DrawableEntityCount);
	SceneDrawableEntityCount->SetText(ZEFormat::Format(SceneDrawableEntityText, Value));

	Value.SetUInt32(Stats.SceneStatistics.CulledEntityCount);
	SceneCulledEntityCount->SetText(ZEFormat::Format(SceneCulledEntityText, Value));

	Value.SetUInt32(Stats.SceneStatistics.DrawedEntityCount);
	SceneDrawedEntityCount->SetText(ZEFormat::Format(SceneDrawedEntityText, Value));

	Value.SetUInt32(Stats.SceneStatistics.TotalLightCount);
	SceneTotalLightCount->SetText(ZEFormat::Format(SceneTotalLightText, Value));

	Value.SetUInt32(Stats.SceneStatistics.CulledLightCount);
	SceneCulledLightCount->SetText(ZEFormat::Format(SceneCulledLightText, Value));

	Value.SetUInt32(Stats.SceneStatistics.DrawedLightCount);
	SceneDrawedLightCount->SetText(ZEFormat::Format(SceneDrawedLightText, Value));

	//Renderer Statistics

	Value.SetUInt32(Stats.RendererStatistics.MeshCount);
	RendererMeshCount->SetText(ZEFormat::Format(RendererMeshText, Value));

	Value.SetUInt32(Stats.RendererStatistics.InstancedMeshCount);
	RendererInstancedMeshCount->SetText(ZEFormat::Format(RendererInstancedMeshText, Value));

	Value.SetUInt32(Stats.RendererStatistics.StageCount);
	RendererStageCount->SetText(ZEFormat::Format(RendererStageText, Value));

	Value.SetUInt32(Stats.RendererStatistics.PostProcessorCount);
	RendererPostProcessorCount->SetText(ZEFormat::Format(RendererPostProcessorText, Value));

	Value.SetUInt32((ZEUInt32)Stats.RendererStatistics.PrimitiveCount);
	RendererPrimitiveCount->SetText(ZEFormat::Format(RendererDrawPrimitiveText, Value));

	Value.SetUInt32((ZEUInt32)Stats.RendererStatistics.MinBatchCount);
	RendererMinBatchCount->SetText(ZEFormat::Format(RendererMinBatchText, Value));

	Value.SetUInt32((ZEUInt32)Stats.RendererStatistics.MaxBatchCount);
	RendererMaxBatchCount->SetText(ZEFormat::Format(RendererMaxBatchText, Value));

	Value.SetUInt32((ZEUInt32)Stats.RendererStatistics.AvgBatchCount);
	RendererAvgBatchCount->SetText(ZEFormat::Format(RendererAvgBatchText, Value));

	Value.SetUInt32((ZEUInt32)Stats.RendererStatistics.VertexCount);
	RendererVertexCount->SetText(ZEFormat::Format(RendererVertexText, Value));

	Value.SetUInt32(Stats.RendererStatistics.DrawCallCount);
	RendererDrawCallCount->SetText(ZEFormat::Format(RendererDrawCallText, Value));

	Value.SetUInt32(Stats.RendererStatistics.DrawPrimitiveCount);
	RendererDrawPrimitiveCount->SetText(ZEFormat::Format(RendererDrawPrimitiveText, Value));

	//Graphics Statistics

	Value.SetUInt32(Stats.GraphicsStatistics.VertexBufferCount);
	GraphicsVertexBufferCount->SetText(ZEFormat::Format(GraphicsVertexBufferText, Value));

	Value.SetUInt32(Stats.GraphicsStatistics.VertexDeclarationCount);
	GraphicsVertexDeclarationCount->SetText(ZEFormat::Format(GraphicsVertexDeclarationText, Value));

	Value.SetUInt32((ZEUInt32)Stats.GraphicsStatistics.TotalVertexBufferSize);
	GraphicsTotalVertexBufferSize->SetText(ZEFormat::Format(GraphicsTotalVertexBufferSizeText, Value));

	Value.SetUInt32(Stats.GraphicsStatistics.ShaderCount);
	GraphicsShaderCount->SetText(ZEFormat::Format(GraphicsShaderText, Value));

	Value.SetUInt32(Stats.GraphicsStatistics.TextureCount);
	GraphicsTextureCount->SetText(ZEFormat::Format(GraphicsTextureText, Value));

	Value.SetUInt32(Stats.GraphicsStatistics.Texture2DCount);
	GraphicsTexture2DCount->SetText(ZEFormat::Format(GraphicsTexture2DText, Value));

	Value.SetUInt32(Stats.GraphicsStatistics.Texture3DCount);
	GraphicsTexture3DCount->SetText(ZEFormat::Format(GraphicsTexture3DText, Value));

	Value.SetUInt32(Stats.GraphicsStatistics.TextureCubeCount);
	GraphicsTextureCubeCount->SetText(ZEFormat::Format(GraphicsTextureCubeText, Value));

	Value.SetUInt32((ZEUInt32)Stats.GraphicsStatistics.TotalTextureSize);
	GraphicsTotalTextureSize->SetText(ZEFormat::Format(GraphicsTotalTextureSizeText, Value));

	Value.SetUInt32(Stats.GraphicsStatistics.RenderTargetCount);
	GraphicsRenderTargetCount->SetText(ZEFormat::Format(GraphicsRenderTargetText, Value));

	Value.SetUInt32((ZEUInt32)Stats.GraphicsStatistics.TotalRenderTargetSize);
	GraphicsTotalRenderTargetSize->SetText(ZEFormat::Format(GraphicsTotalRenderTargetSizeText, Value));

	Value.SetUInt32(Stats.GraphicsStatistics.DepthBufferCount);
	GraphicsDepthBufferCount->SetText(ZEFormat::Format(GraphicsDepthBufferText, Value));

	Value.SetUInt32((ZEUInt32)Stats.GraphicsStatistics.TotalDepthBufferSize);
	GraphicsTotalDepthBufferSize->SetText(ZEFormat::Format(GraphicsTotalDepthBufferSizeText, Value));

	//Model Statistics

	Value.SetUInt32(Stats.ModelStatistics.TotalMeshCount);
	ModelTotalMeshCount->SetText(ZEFormat::Format(ModelTotalMeshText, Value));

	Value.SetUInt32(Stats.ModelStatistics.CulledMeshCount);
	ModelCulledMeshCount->SetText(ZEFormat::Format(ModelCulledMeshText, Value));

	Value.SetUInt32(Stats.ModelStatistics.DrawnMeshCount);
	ModelDrawnMeshCount->SetText(ZEFormat::Format(ModelDrawnMeshText, Value));

	//Interior Statistics

	Value.SetUInt32(Stats.InteriorStatistics.TotalRoomCount);
	InteriorTotalRoomCount->SetText(ZEFormat::Format(InteriorTotalRoomText, Value));

	Value.SetUInt32(Stats.InteriorStatistics.CulledRoomCount);
	InteriorCulledRoomCount->SetText(ZEFormat::Format(InteriorCulledRoomText, Value));

	Value.SetUInt32(Stats.InteriorStatistics.DrawedRoomCount);
	InteriorDrawedRoomCount->SetText(ZEFormat::Format(InteriorDrawedRoomText, Value));

	Value.SetUInt32(Stats.InteriorStatistics.TotalDoorCount);
	InteriorTotalDoorCount->SetText(ZEFormat::Format(InteriorTotalDoorText, Value));

	Value.SetUInt32(Stats.InteriorStatistics.SeenDoorCount);
	InteriorSeenDoorCount->SetText(ZEFormat::Format(InteriorSeenDoorText, Value));

	Value.SetUInt32((ZEUInt32)Stats.InteriorStatistics.TotalInteriorPolygonCount);
	InteriorTotalPolygonCount->SetText(ZEFormat::Format(InteriorTotalPolygonText, Value));

	Value.SetUInt32((ZEUInt32)Stats.InteriorStatistics.CulledInteriorPolygonCount);
	InteriorCulledPolygonCount->SetText(ZEFormat::Format(InteriorCulledPolygonText, Value));

	Value.SetUInt32((ZEUInt32)Stats.InteriorStatistics.DrawedInteriorPolygonCount);
	InteriorDrawedPolygonCount->SetText(ZEFormat::Format(InteriorDrawedPolygonText, Value));

	// Particle Statistics

	Value.SetUInt32(Stats.ParticleStatistics.EmitterCount);
	ParticleEmitterCount->SetText(ZEFormat::Format(ParticleEmitterText, Value));

	Value.SetUInt32((ZEUInt32)Stats.ParticleStatistics.TotalParticleCount);
	ParticleTotalParticleCount->SetText(ZEFormat::Format(ParticleTotalParticleText, Value));

	Value.SetUInt32((ZEUInt32)Stats.ParticleStatistics.DrawedParticleCount);
	ParticleDrawedParticleCount->SetText(ZEFormat::Format(ParticleDrawedParticleText, Value));


	ZEUIControl::Draw(Renderer);
}

void ZEUISceneStatisticsControl::Tick(float ElapsedTime)
{
	//FPS Calculation

	for(ZESize I = 0; I < GetChildControls().GetCount(); I++)
		GetChildControls()[I]->SetZOrder(GetZOrder());

	static float TotalElapsedTime = 0;
	static ZEUInt32 AverageFPS = 0;
	static ZEUInt32 FrameCount = 0;

	FrameCount++;
	TotalElapsedTime += ElapsedTime;
	if (TotalElapsedTime > 0.5f)
	{
		TotalElapsedTime = 0.0f;
		AverageFPS = FrameCount * 2;
		FrameCount = 0;
	}

	ZEScene* TempScene = zeScene;
	
	if (TempScene->GetActiveCamera() != NULL)
	{
		ZEVector3 CamPos = TempScene->GetActiveCamera()->GetWorldPosition();

		FPSCount->SetText(ZEFormat::Format(FPSText, AverageFPS));
		CameraPosition->SetText(ZEFormat::Format(CamPosText, CamPos.x, CamPos.y, CamPos.z));
	}

	zeInput->ProcessInputMap(&InputMap);

	for (ZESize I = 0; I < InputMap.InputActionCount; I++)
	{
		switch(InputMap.InputActions[I].Id)
		{
			case ACTIONID_VISIBILITY:
				if (GetVisiblity())
					SetVisiblity(false);
				else
					SetVisiblity(true);
				break;
		}
	}

	ZEUIControl::Tick(ElapsedTime);

}

ZEUISceneStatisticsControl::ZEUISceneStatisticsControl()
{
	CameraPosition = new ZEUITextControl();
	CameraPosition->SetWidth(350.0f);
	CameraPosition->SetHeight(25.0f);
	CameraPosition->SetPosition(ZEVector2(0.0f, 0.0f));
	CameraPosition->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	CameraPosition->SetTextColor(GreenTextColor);
	AddChildControl(CameraPosition);

	FPSCount = new ZEUITextControl();
	FPSCount->SetWidth(250.0f);
	FPSCount->SetHeight(25.0f);
	FPSCount->SetPosition(ZEVector2(355.0f, 0.0f));
	FPSCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	FPSCount->SetTextColor(GreenTextColor);
	AddChildControl(FPSCount);

	//Scene Statistics

	SceneStatisticsDescription = new ZEUITextControl();
	SceneStatisticsDescription->SetWidth(150.0f);
	SceneStatisticsDescription->SetHeight(25.0f);
	SceneStatisticsDescription->SetPosition(ZEVector2(0.0f, 25.0f));
	SceneStatisticsDescription->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	SceneStatisticsDescription->SetTextColor(RedTextColor);
	SceneStatisticsDescription->SetText("- Scene Statistics -");
	AddChildControl(SceneStatisticsDescription);

	SceneTotalEntityCount = new ZEUITextControl();
	SceneTotalEntityCount->SetWidth(150.0f);
	SceneTotalEntityCount->SetHeight(25.0f);
	SceneTotalEntityCount->SetPosition(ZEVector2(0.0f, 50.0f));
	SceneTotalEntityCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	SceneTotalEntityCount->SetTextColor(GreenTextColor);
	AddChildControl(SceneTotalEntityCount);

	SceneDrawableEntityCount= new ZEUITextControl();
	SceneDrawableEntityCount->SetWidth(180.0f);
	SceneDrawableEntityCount->SetHeight(25.0f);
	SceneDrawableEntityCount->SetPosition(ZEVector2(155.0f, 50.0f));
	SceneDrawableEntityCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	SceneDrawableEntityCount->SetTextColor(GreenTextColor);
	AddChildControl(SceneDrawableEntityCount);

	SceneCulledEntityCount = new ZEUITextControl();
	SceneCulledEntityCount->SetWidth(150.0f);
	SceneCulledEntityCount->SetHeight(25.0f);
	SceneCulledEntityCount->SetPosition(ZEVector2(0.0f, 75.0f));
	SceneCulledEntityCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	SceneCulledEntityCount->SetTextColor(GreenTextColor);
	AddChildControl(SceneCulledEntityCount);

	SceneDrawedEntityCount = new ZEUITextControl();
	SceneDrawedEntityCount->SetWidth(150.0f);
	SceneDrawedEntityCount->SetHeight(25.0f);
	SceneDrawedEntityCount->SetPosition(ZEVector2(155.0f, 75.0f));
	SceneDrawedEntityCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	SceneDrawedEntityCount->SetTextColor(GreenTextColor);
	AddChildControl(SceneDrawedEntityCount);

	SceneTotalLightCount = new ZEUITextControl();
	SceneTotalLightCount->SetWidth(150.0f);
	SceneTotalLightCount->SetHeight(25.0f);
	SceneTotalLightCount->SetPosition(ZEVector2(0.0f, 100.0f));
	SceneTotalLightCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	SceneTotalLightCount->SetTextColor(GreenTextColor);
	AddChildControl(SceneTotalLightCount);

	SceneCulledLightCount = new ZEUITextControl();
	SceneCulledLightCount->SetWidth(150.0f);
	SceneCulledLightCount->SetHeight(25.0f);
	SceneCulledLightCount->SetPosition(ZEVector2(155.0f, 100.0f));
	SceneCulledLightCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	SceneCulledLightCount->SetTextColor(GreenTextColor);
	AddChildControl(SceneCulledLightCount);

	SceneDrawedLightCount = new ZEUITextControl();
	SceneDrawedLightCount->SetWidth(150.0f);
	SceneDrawedLightCount->SetHeight(25.0f);
	SceneDrawedLightCount->SetPosition(ZEVector2(310.0f, 100.0f));
	SceneDrawedLightCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	SceneDrawedLightCount->SetTextColor(GreenTextColor);
	AddChildControl(SceneDrawedLightCount);

	//Model Statistics

	ModelStatisticsDescription = new ZEUITextControl();
	ModelStatisticsDescription->SetWidth(150.0f);
	ModelStatisticsDescription->SetHeight(25.0f);
	ModelStatisticsDescription->SetPosition(ZEVector2(0.0f, 125.0f));
	ModelStatisticsDescription->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	ModelStatisticsDescription->SetTextColor(RedTextColor);
	ModelStatisticsDescription->SetText("- Model Statistics -");
	AddChildControl(ModelStatisticsDescription);

	ModelTotalMeshCount = new ZEUITextControl();
	ModelTotalMeshCount->SetWidth(150.0f);
	ModelTotalMeshCount->SetHeight(25.0f);
	ModelTotalMeshCount->SetPosition(ZEVector2(0.0f, 150.0f));
	ModelTotalMeshCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	ModelTotalMeshCount->SetTextColor(GreenTextColor);
	AddChildControl(ModelTotalMeshCount);

	ModelCulledMeshCount = new ZEUITextControl();
	ModelCulledMeshCount->SetWidth(150.0f);
	ModelCulledMeshCount->SetHeight(25.0f);
	ModelCulledMeshCount->SetPosition(ZEVector2(155.0f, 150.0f));
	ModelCulledMeshCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	ModelCulledMeshCount->SetTextColor(GreenTextColor);
	AddChildControl(ModelCulledMeshCount);

	ModelDrawnMeshCount = new ZEUITextControl();
	ModelDrawnMeshCount->SetWidth(150.0f);
	ModelDrawnMeshCount->SetHeight(25.0f);
	ModelDrawnMeshCount->SetPosition(ZEVector2(310.0f, 150.0f));
	ModelDrawnMeshCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	ModelDrawnMeshCount->SetTextColor(GreenTextColor);
	AddChildControl(ModelDrawnMeshCount);

	// Renderer Statistics

	RendererStatisticsDescription = new ZEUITextControl();
	RendererStatisticsDescription->SetWidth(150.0f);
	RendererStatisticsDescription->SetHeight(25.0f);
	RendererStatisticsDescription->SetPosition(ZEVector2(0.0f, 175.0f));
	RendererStatisticsDescription->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererStatisticsDescription->SetTextColor(RedTextColor);
	RendererStatisticsDescription->SetText("- Renderer Statistics -");
	AddChildControl(RendererStatisticsDescription);

	RendererMeshCount = new ZEUITextControl();
	RendererMeshCount->SetWidth(150.0f);
	RendererMeshCount->SetHeight(25.0f);
	RendererMeshCount->SetPosition(ZEVector2(0.0f, 200.0f));
	RendererMeshCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererMeshCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererMeshCount);

	RendererInstancedMeshCount = new ZEUITextControl();
	RendererInstancedMeshCount->SetWidth(150.0f);
	RendererInstancedMeshCount->SetHeight(25.0f);
	RendererInstancedMeshCount->SetPosition(ZEVector2(155.0f, 200.0f));
	RendererInstancedMeshCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererInstancedMeshCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererInstancedMeshCount);

	RendererStageCount = new ZEUITextControl();
	RendererStageCount->SetWidth(150.0f);
	RendererStageCount->SetHeight(25.0f);
	RendererStageCount->SetPosition(ZEVector2(310.0f, 200.0f));
	RendererStageCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererStageCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererStageCount);

	RendererPostProcessorCount = new ZEUITextControl();
	RendererPostProcessorCount->SetWidth(150.0f);
	RendererPostProcessorCount->SetHeight(25.0f);
	RendererPostProcessorCount->SetPosition(ZEVector2(465.0f, 200.0f));
	RendererPostProcessorCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererPostProcessorCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererPostProcessorCount);

	RendererPrimitiveCount = new ZEUITextControl();
	RendererPrimitiveCount->SetWidth(150.0f);
	RendererPrimitiveCount->SetHeight(25.0f);
	RendererPrimitiveCount->SetPosition(ZEVector2(0.0f, 225.0f));
	RendererPrimitiveCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererPrimitiveCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererPrimitiveCount);

	RendererMinBatchCount = new ZEUITextControl();
	RendererMinBatchCount->SetWidth(150.0f);
	RendererMinBatchCount->SetHeight(25.0f);
	RendererMinBatchCount->SetPosition(ZEVector2(155.0f, 225.0f));
	RendererMinBatchCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererMinBatchCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererMinBatchCount);

	RendererMaxBatchCount = new ZEUITextControl();
	RendererMaxBatchCount->SetWidth(150.0f);
	RendererMaxBatchCount->SetHeight(25.0f);
	RendererMaxBatchCount->SetPosition(ZEVector2(310.0f, 225.0f));
	RendererMaxBatchCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererMaxBatchCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererMaxBatchCount);

	RendererAvgBatchCount = new ZEUITextControl();
	RendererAvgBatchCount->SetWidth(150.0f);
	RendererAvgBatchCount->SetHeight(25.0f);
	RendererAvgBatchCount->SetPosition(ZEVector2(465.0f, 225.0f));
	RendererAvgBatchCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererAvgBatchCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererAvgBatchCount);

	RendererVertexCount = new ZEUITextControl();
	RendererVertexCount->SetWidth(150.0f);
	RendererVertexCount->SetHeight(25.0f);
	RendererVertexCount->SetPosition(ZEVector2(0.0f, 250.0f));
	RendererVertexCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererVertexCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererVertexCount);

	RendererDrawCallCount = new ZEUITextControl();
	RendererDrawCallCount->SetWidth(150.0f);
	RendererDrawCallCount->SetHeight(25.0f);
	RendererDrawCallCount->SetPosition(ZEVector2(155.0f, 250.0f));
	RendererDrawCallCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererDrawCallCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererDrawCallCount);

	RendererDrawPrimitiveCount = new ZEUITextControl();
	RendererDrawPrimitiveCount->SetWidth(150.0f);
	RendererDrawPrimitiveCount->SetHeight(25.0f);
	RendererDrawPrimitiveCount->SetPosition(ZEVector2(310.0f, 250.0f));
	RendererDrawPrimitiveCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	RendererDrawPrimitiveCount->SetTextColor(GreenTextColor);
	AddChildControl(RendererDrawPrimitiveCount);

	//Graphics Statistics

	GraphicsStatisticsDescription = new ZEUITextControl();
	GraphicsStatisticsDescription->SetWidth(150.0f);
	GraphicsStatisticsDescription->SetHeight(25.0f);
	GraphicsStatisticsDescription->SetPosition(ZEVector2(0.0f, 275.0f));
	GraphicsStatisticsDescription->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsStatisticsDescription->SetTextColor(RedTextColor);
	GraphicsStatisticsDescription->SetText("- Graphics Statistics -");
	AddChildControl(GraphicsStatisticsDescription);

	GraphicsVertexBufferCount = new ZEUITextControl();
	GraphicsVertexBufferCount->SetWidth(150.0f);
	GraphicsVertexBufferCount->SetHeight(25.0f);
	GraphicsVertexBufferCount->SetPosition(ZEVector2(0.0f, 300.0f));
	GraphicsVertexBufferCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsVertexBufferCount->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsVertexBufferCount);

	GraphicsVertexDeclarationCount = new ZEUITextControl();
	GraphicsVertexDeclarationCount->SetWidth(180.0f);
	GraphicsVertexDeclarationCount->SetHeight(25.0f);
	GraphicsVertexDeclarationCount->SetPosition(ZEVector2(155.0f, 300.0f));
	GraphicsVertexDeclarationCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsVertexDeclarationCount->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsVertexDeclarationCount);

	GraphicsTotalVertexBufferSize = new ZEUITextControl();
	GraphicsTotalVertexBufferSize->SetWidth(180.0f);
	GraphicsTotalVertexBufferSize->SetHeight(25.0f);
	GraphicsTotalVertexBufferSize->SetPosition(ZEVector2(340.0f, 300.0f));
	GraphicsTotalVertexBufferSize->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsTotalVertexBufferSize->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsTotalVertexBufferSize);

	GraphicsShaderCount = new ZEUITextControl();
	GraphicsShaderCount->SetWidth(150.0f);
	GraphicsShaderCount->SetHeight(25.0f);
	GraphicsShaderCount->SetPosition(ZEVector2(0.0f, 325.0f));
	GraphicsShaderCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsShaderCount->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsShaderCount);

	GraphicsTextureCount = new ZEUITextControl();
	GraphicsTextureCount->SetWidth(150.0f);
	GraphicsTextureCount->SetHeight(25.0f);
	GraphicsTextureCount->SetPosition(ZEVector2(0.0f, 350.0f));
	GraphicsTextureCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsTextureCount->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsTextureCount);

	GraphicsTexture2DCount = new ZEUITextControl();
	GraphicsTexture2DCount->SetWidth(150.0f);
	GraphicsTexture2DCount->SetHeight(25.0f);
	GraphicsTexture2DCount->SetPosition(ZEVector2(155.0f, 350.0f));
	GraphicsTexture2DCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsTexture2DCount->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsTexture2DCount);

	GraphicsTexture3DCount = new ZEUITextControl();
	GraphicsTexture3DCount->SetWidth(150.0f);
	GraphicsTexture3DCount->SetHeight(25.0f);
	GraphicsTexture3DCount->SetPosition(ZEVector2(310.0f, 350.0f));
	GraphicsTexture3DCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsTexture3DCount->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsTexture3DCount);

	GraphicsTextureCubeCount = new ZEUITextControl();
	GraphicsTextureCubeCount->SetWidth(150.0f);
	GraphicsTextureCubeCount->SetHeight(25.0f);
	GraphicsTextureCubeCount->SetPosition(ZEVector2(465.0f, 350.0f));
	GraphicsTextureCubeCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsTextureCubeCount->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsTextureCubeCount);

	GraphicsTotalTextureSize = new ZEUITextControl();
	GraphicsTotalTextureSize->SetWidth(150.0f);
	GraphicsTotalTextureSize->SetHeight(25.0f);
	GraphicsTotalTextureSize->SetPosition(ZEVector2(620.0f, 350.0f));
	GraphicsTotalTextureSize->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsTotalTextureSize->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsTotalTextureSize);

	GraphicsRenderTargetCount = new ZEUITextControl();
	GraphicsRenderTargetCount->SetWidth(150.0f);
	GraphicsRenderTargetCount->SetHeight(25.0f);
	GraphicsRenderTargetCount->SetPosition(ZEVector2(0.0f, 375.0f));
	GraphicsRenderTargetCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsRenderTargetCount->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsRenderTargetCount);

	GraphicsTotalRenderTargetSize = new ZEUITextControl();
	GraphicsTotalRenderTargetSize->SetWidth(180.0f);
	GraphicsTotalRenderTargetSize->SetHeight(25.0f);
	GraphicsTotalRenderTargetSize->SetPosition(ZEVector2(155.0f, 375.0f));
	GraphicsTotalRenderTargetSize->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsTotalRenderTargetSize->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsTotalRenderTargetSize);

	GraphicsDepthBufferCount = new ZEUITextControl();
	GraphicsDepthBufferCount->SetWidth(150.0f);
	GraphicsDepthBufferCount->SetHeight(25.0f);
	GraphicsDepthBufferCount->SetPosition(ZEVector2(340.0f, 375.0f));
	GraphicsDepthBufferCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsDepthBufferCount->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsDepthBufferCount);

	GraphicsTotalDepthBufferSize = new ZEUITextControl();
	GraphicsTotalDepthBufferSize->SetWidth(180.0f);
	GraphicsTotalDepthBufferSize->SetHeight(25.0f);
	GraphicsTotalDepthBufferSize->SetPosition(ZEVector2(525.0f, 375.0f));
	GraphicsTotalDepthBufferSize->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	GraphicsTotalDepthBufferSize->SetTextColor(GreenTextColor);
	AddChildControl(GraphicsTotalDepthBufferSize);

	// Particle Statistics

	ParticleStatisticsDescription = new ZEUITextControl();
	ParticleStatisticsDescription->SetWidth(150.0f);
	ParticleStatisticsDescription->SetHeight(25.0f);
	ParticleStatisticsDescription->SetPosition(ZEVector2(0.0f, 400.0f));
	ParticleStatisticsDescription->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	ParticleStatisticsDescription->SetTextColor(RedTextColor);
	ParticleStatisticsDescription->SetText("- Particle Statistics -");
	AddChildControl(ParticleStatisticsDescription);

	ParticleEmitterCount = new ZEUITextControl();
	ParticleEmitterCount->SetWidth(150.0f);
	ParticleEmitterCount->SetHeight(25.0f);
	ParticleEmitterCount->SetPosition(ZEVector2(0.0f, 425.0f));
	ParticleEmitterCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	ParticleEmitterCount->SetTextColor(GreenTextColor);
	AddChildControl(ParticleEmitterCount);

	ParticleTotalParticleCount = new ZEUITextControl();
	ParticleTotalParticleCount->SetWidth(180.0f);
	ParticleTotalParticleCount->SetHeight(25.0f);
	ParticleTotalParticleCount->SetPosition(ZEVector2(155.0f, 425.0f));
	ParticleTotalParticleCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	ParticleTotalParticleCount->SetTextColor(GreenTextColor);
	AddChildControl(ParticleTotalParticleCount);

	ParticleDrawedParticleCount = new ZEUITextControl();
	ParticleDrawedParticleCount->SetWidth(180.0f);
	ParticleDrawedParticleCount->SetHeight(25.0f);
	ParticleDrawedParticleCount->SetPosition(ZEVector2(340.0f, 425.0f));
	ParticleDrawedParticleCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	ParticleDrawedParticleCount->SetTextColor(GreenTextColor);
	AddChildControl(ParticleDrawedParticleCount);

	// Interior Statistics

	InteriorStatisticsDescription = new ZEUITextControl();
	InteriorStatisticsDescription->SetWidth(150.0f);
	InteriorStatisticsDescription->SetHeight(25.0f);
	InteriorStatisticsDescription->SetPosition(ZEVector2(0.0f, 450.0f));
	InteriorStatisticsDescription->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	InteriorStatisticsDescription->SetTextColor(RedTextColor);
	InteriorStatisticsDescription->SetText("- Interior Statistics -");
	AddChildControl(InteriorStatisticsDescription);

	InteriorTotalRoomCount = new ZEUITextControl();
	InteriorTotalRoomCount->SetWidth(150.0f);
	InteriorTotalRoomCount->SetHeight(25.0f);
	InteriorTotalRoomCount->SetPosition(ZEVector2(0.0f, 475.0f));
	InteriorTotalRoomCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	InteriorTotalRoomCount->SetTextColor(GreenTextColor);
	AddChildControl(InteriorTotalRoomCount);

	InteriorCulledRoomCount = new ZEUITextControl();
	InteriorCulledRoomCount->SetWidth(150.0f);
	InteriorCulledRoomCount->SetHeight(25.0f);
	InteriorCulledRoomCount->SetPosition(ZEVector2(155.0f, 475.0f));
	InteriorCulledRoomCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	InteriorCulledRoomCount->SetTextColor(GreenTextColor);
	AddChildControl(InteriorCulledRoomCount);

	InteriorDrawedRoomCount = new ZEUITextControl();
	InteriorDrawedRoomCount->SetWidth(150.0f);
	InteriorDrawedRoomCount->SetHeight(25.0f);
	InteriorDrawedRoomCount->SetPosition(ZEVector2(310.0f, 475.0f));
	InteriorDrawedRoomCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	InteriorDrawedRoomCount->SetTextColor(GreenTextColor);
	AddChildControl(InteriorDrawedRoomCount);

	InteriorTotalDoorCount = new ZEUITextControl();
	InteriorTotalDoorCount->SetWidth(150.0f);
	InteriorTotalDoorCount->SetHeight(25.0f);
	InteriorTotalDoorCount->SetPosition(ZEVector2(0.0f, 500.0f));
	InteriorTotalDoorCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	InteriorTotalDoorCount->SetTextColor(GreenTextColor);
	AddChildControl(InteriorTotalDoorCount);

	InteriorSeenDoorCount = new ZEUITextControl();
	InteriorSeenDoorCount->SetWidth(150.0f);
	InteriorSeenDoorCount->SetHeight(25.0f);
	InteriorSeenDoorCount->SetPosition(ZEVector2(155.0f, 500.0f));
	InteriorSeenDoorCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	InteriorSeenDoorCount->SetTextColor(GreenTextColor);
	AddChildControl(InteriorSeenDoorCount);

	InteriorTotalPolygonCount = new ZEUITextControl();
	InteriorTotalPolygonCount->SetWidth(200.0f);
	InteriorTotalPolygonCount->SetHeight(25.0f);
	InteriorTotalPolygonCount->SetPosition(ZEVector2(0.0f, 525.0f));
	InteriorTotalPolygonCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	InteriorTotalPolygonCount->SetTextColor(GreenTextColor);
	AddChildControl(InteriorTotalPolygonCount);

	InteriorCulledPolygonCount = new ZEUITextControl();
	InteriorCulledPolygonCount->SetWidth(200.0f);
	InteriorCulledPolygonCount->SetHeight(25.0f);
	InteriorCulledPolygonCount->SetPosition(ZEVector2(205.0f, 525.0f));
	InteriorCulledPolygonCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	InteriorCulledPolygonCount->SetTextColor(GreenTextColor);
	AddChildControl(InteriorCulledPolygonCount);

	InteriorDrawedPolygonCount = new ZEUITextControl();
	InteriorDrawedPolygonCount->SetWidth(200.0f);
	InteriorDrawedPolygonCount->SetHeight(25.0f);
	InteriorDrawedPolygonCount->SetPosition(ZEVector2(410.0f, 525.0f));
	InteriorDrawedPolygonCount->SetFont(ZEFontResource::LoadResource("ZEEngine/ZEGUI/Fonts/Arial16Bitmap/Arial16.zeFont"));
	InteriorDrawedPolygonCount->SetTextColor(GreenTextColor);
	AddChildControl(InteriorDrawedPolygonCount);

	this->SetHeight(600.0f);
	this->SetWidth(900.0f);

// 	int WindowWidth = 0;
// 	int WindowHeight = 0;
// 	zeCore->GetWindow()->GetWindowSize(WindowWidth, WindowHeight);
//	WindowWidth - GetWidth()

	this->SetPosition(ZEVector2(3.0f, 0.0f));

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_VISIBILITY, ZEInputEvent("Keyboard", ZE_IKB_F1, ZE_IBS_PRESSED)));
	
}
	
	
