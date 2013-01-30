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

#define ACTIONID_VISIBILITY 1

#define FPSText								ZEString("Frame Per Second : ")
#define CamPosText							ZEString("Camera Position : ")

#define SceneTotalEntityText				ZEString("Total Entity Count : ")
#define SceneDrawableEntityText				ZEString("Drawable Entity Count : ")
#define SceneCulledEntityText				ZEString("Culled Entity Count : ")
#define SceneDrawedEntityText				ZEString("Drawn Entity Count : ")
#define SceneTotalLightText					ZEString("Total Light Count : ")
#define SceneCulledLightText				ZEString("Culled Light Count : ")
#define SceneDrawedLightText				ZEString("Drawn Light Count : ")

#define RendererMeshText					ZEString("Mesh Count : ")
#define RendererInstancedMeshText			ZEString("Instanced Mesh Count : ")
#define RendererStageText					ZEString("Stage Count : ")
#define RendererPostProcessorText			ZEString("Post Processor Count : ")	    
#define RendererPrimitiveText				ZEString("Primitive Count : ")
#define RendererMinBatchText				ZEString("Min Batch Count : ")
#define RendererMaxBatchText				ZEString("Max Batch Count : ")
#define RendererAvgBatchText				ZEString("Avg Batch Count : ")
#define RendererVertexText					ZEString("Vertex Count : ")
#define RendererDrawCallText				ZEString("Draw Call Count : ")
#define RendererDrawPrimitiveText			ZEString("Draw Primitive Count : ")

#define	GraphicsVertexBufferText			ZEString("Vertex Buffer Count : ")
#define	GraphicsVertexDeclarationText		ZEString("Vertex Declaration Count : ")
#define	GraphicsTotalVertexBufferSizeText	ZEString("Total Vertex Buffer Size : ")
#define	GraphicsShaderText					ZEString("Shader Count : ")	    
#define	GraphicsTextureText					ZEString("Texture Count : ")
#define	GraphicsTexture2DText				ZEString("Texture2D Count : ")
#define	GraphicsTexture3DText				ZEString("Texture3D Count : ")
#define	GraphicsTextureCubeText				ZEString("TextureCube Count : ")
#define	GraphicsTotalTextureSizeText		ZEString("Total Texture Size : ")
#define	GraphicsRenderTargetText			ZEString("Render Target Count : ")
#define	GraphicsTotalRenderTargetSizeText	ZEString("Total Render Target Size : ")
#define	GraphicsDepthBufferText				ZEString("Depth Buffer Count : ")
#define	GraphicsTotalDepthBufferSizeText	ZEString("Total Depth Buffer Size : ")

#define	ModelTotalMeshText					ZEString("Total Mesh Count : ")
#define	ModelCulledMeshText					ZEString("Culled Mesh Count : ")
#define	ModelDrawnMeshText					ZEString("Drawn Mesh Count : ")

#define InteriorTotalRoomText				ZEString("Total Room Count : ")
#define InteriorCulledRoomText				ZEString("Culled Room Count : ")
#define InteriorDrawedRoomText				ZEString("Drawn Room Count : ")
#define InteriorTotalDoorText				ZEString("Total Door Count : ")
#define InteriorSeenDoorText				ZEString("Viewed Door Count : ")
#define InteriorTotalPolygonText			ZEString("Total Interior Polygon Count : ")
#define InteriorCulledPolygonText			ZEString("Culled Interior Polygon Count : ")
#define InteriorDrawedPolygonText			ZEString("Drawn  Interior Polygon Count : ")

#define	ParticleEmitterText					ZEString("Emitter Count : ")
#define	ParticleTotalParticleText			ZEString("Total Particle Count : ")
#define	ParticleDrawedParticleText			ZEString("Drawn Particle Count : ")

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
	ZEString Value;

	//Scene Statistics

	Value.SetValue(Stats.SceneStatistics.TotalEntityCount);
	SceneTotalEntityCount->SetText(SceneTotalEntityText + Value);

	Value.SetValue(Stats.SceneStatistics.DrawableEntityCount);
	SceneDrawableEntityCount->SetText(SceneDrawableEntityText + Value);

	Value.SetValue(Stats.SceneStatistics.CulledEntityCount);
	SceneCulledEntityCount->SetText(SceneCulledEntityText + Value);

	Value.SetValue(Stats.SceneStatistics.DrawedEntityCount);
	SceneDrawedEntityCount->SetText(SceneDrawedEntityText + Value);

	Value.SetValue(Stats.SceneStatistics.TotalLightCount);
	SceneTotalLightCount->SetText(SceneTotalLightText + Value);

	Value.SetValue(Stats.SceneStatistics.CulledLightCount);
	SceneCulledLightCount->SetText(SceneCulledLightText + Value);

	Value.SetValue(Stats.SceneStatistics.DrawedLightCount);
	SceneDrawedLightCount->SetText(SceneDrawedLightText + Value);

	//Renderer Statistics

	Value.SetValue(Stats.RendererStatistics.MeshCount);
	RendererMeshCount->SetText(RendererMeshText + Value);

	Value.SetValue(Stats.RendererStatistics.InstancedMeshCount);
	RendererInstancedMeshCount->SetText(RendererInstancedMeshText + Value);

	Value.SetValue(Stats.RendererStatistics.StageCount);
	RendererStageCount->SetText(RendererStageText + Value);

	Value.SetValue(Stats.RendererStatistics.PostProcessorCount);
	RendererPostProcessorCount->SetText(RendererPostProcessorText + Value);

	Value.SetValue(Stats.RendererStatistics.PrimitiveCount);
	RendererPrimitiveCount->SetText(RendererPrimitiveText + Value);

	Value.SetValue(Stats.RendererStatistics.MinBatchCount);
	RendererMinBatchCount->SetText(RendererMinBatchText + Value);

	Value.SetValue(Stats.RendererStatistics.MaxBatchCount);
	RendererMaxBatchCount->SetText(RendererMaxBatchText + Value);

	Value.SetValue(Stats.RendererStatistics.AvgBatchCount);
	RendererAvgBatchCount->SetText(RendererAvgBatchText + Value);

	Value.SetValue(Stats.RendererStatistics.VertexCount);
	RendererVertexCount->SetText(RendererVertexText + Value);

	Value.SetValue(Stats.RendererStatistics.DrawCallCount);
	RendererDrawCallCount->SetText(RendererDrawCallText + Value);

	Value.SetValue(Stats.RendererStatistics.DrawPrimitiveCount);
	RendererDrawPrimitiveCount->SetText(RendererDrawPrimitiveText + Value);

	//Graphics Statistics

	Value.SetValue(Stats.GraphicsStatistics.VertexBufferCount);
	GraphicsVertexBufferCount->SetText(GraphicsVertexBufferText + Value);

	Value.SetValue(Stats.GraphicsStatistics.VertexDeclarationCount);
	GraphicsVertexDeclarationCount->SetText(GraphicsVertexDeclarationText + Value);

	Value.SetValue(Stats.GraphicsStatistics.TotalVertexBufferSize);
	GraphicsTotalVertexBufferSize->SetText(GraphicsTotalVertexBufferSizeText + Value);

	Value.SetValue(Stats.GraphicsStatistics.ShaderCount);
	GraphicsShaderCount->SetText(GraphicsShaderText + Value);

	Value.SetValue(Stats.GraphicsStatistics.TextureCount);
	GraphicsTextureCount->SetText(GraphicsTextureText + Value);

	Value.SetValue(Stats.GraphicsStatistics.Texture2DCount);
	GraphicsTexture2DCount->SetText(GraphicsTexture2DText + Value);

	Value.SetValue(Stats.GraphicsStatistics.Texture3DCount);
	GraphicsTexture3DCount->SetText(GraphicsTexture3DText + Value);

	Value.SetValue(Stats.GraphicsStatistics.TextureCubeCount);
	GraphicsTextureCubeCount->SetText(GraphicsTextureCubeText + Value);

	Value.SetValue(Stats.GraphicsStatistics.TotalTextureSize);
	GraphicsTotalTextureSize->SetText(GraphicsTotalTextureSizeText + Value);

	Value.SetValue(Stats.GraphicsStatistics.RenderTargetCount);
	GraphicsRenderTargetCount->SetText(GraphicsRenderTargetText + Value);

	Value.SetValue(Stats.GraphicsStatistics.TotalRenderTargetSize);
	GraphicsTotalRenderTargetSize->SetText(GraphicsTotalRenderTargetSizeText + Value);

	Value.SetValue(Stats.GraphicsStatistics.DepthBufferCount);
	GraphicsDepthBufferCount->SetText(GraphicsDepthBufferText + Value);

	Value.SetValue(Stats.GraphicsStatistics.TotalDepthBufferSize);
	GraphicsTotalDepthBufferSize->SetText(GraphicsTotalDepthBufferSizeText + Value);

	//Model Statistics

	Value.SetValue(Stats.ModelStatistics.TotalMeshCount);
	ModelTotalMeshCount->SetText(ModelTotalMeshText + Value);

	Value.SetValue(Stats.ModelStatistics.CulledMeshCount);
	ModelCulledMeshCount->SetText(ModelCulledMeshText + Value);

	Value.SetValue(Stats.ModelStatistics.DrawnMeshCount);
	ModelDrawnMeshCount->SetText(ModelDrawnMeshText + Value);

	//Interior Statistics

	Value.SetValue(Stats.InteriorStatistics.TotalRoomCount);
	InteriorTotalRoomCount->SetText(InteriorTotalRoomText + Value);

	Value.SetValue(Stats.InteriorStatistics.CulledRoomCount);
	InteriorCulledRoomCount->SetText(InteriorCulledRoomText + Value);

	Value.SetValue(Stats.InteriorStatistics.DrawedRoomCount);
	InteriorDrawedRoomCount->SetText(InteriorDrawedRoomText + Value);

	Value.SetValue(Stats.InteriorStatistics.TotalDoorCount);
	InteriorTotalDoorCount->SetText(InteriorTotalDoorText + Value);

	Value.SetValue(Stats.InteriorStatistics.SeenDoorCount);
	InteriorSeenDoorCount->SetText(InteriorSeenDoorText + Value);

	Value.SetValue(Stats.InteriorStatistics.TotalInteriorPolygonCount);
	InteriorTotalPolygonCount->SetText(InteriorTotalPolygonText + Value);

	Value.SetValue(Stats.InteriorStatistics.CulledInteriorPolygonCount);
	InteriorCulledPolygonCount->SetText(InteriorCulledPolygonText + Value);

	Value.SetValue(Stats.InteriorStatistics.DrawedInteriorPolygonCount);
	InteriorDrawedPolygonCount->SetText(InteriorDrawedPolygonText + Value);

	// Particle Statistics

	Value.SetValue(Stats.ParticleStatistics.EmitterCount);
	ParticleEmitterCount->SetText(ParticleEmitterText + Value);

	Value.SetValue(Stats.ParticleStatistics.TotalParticleCount);
	ParticleTotalParticleCount->SetText(ParticleTotalParticleText + Value);

	Value.SetValue(Stats.ParticleStatistics.DrawedParticleCount);
	ParticleDrawedParticleCount->SetText(ParticleDrawedParticleText + Value);


	ZEUIControl::Draw(Renderer);
}

void ZEUISceneStatisticsControl::Tick(float ElapsedTime)
{
	//FPS Calculation

	for(ZESize I = 0; I < GetChildControls().GetCount(); I++)
		GetChildControls()[I]->SetZOrder(GetZOrder());

	static float TotalElapsedTime = 0;
	static unsigned int AverageFPS = 0;
	static unsigned int FrameCount = 0;

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
		ZEString Value;

		Value.SetValue(AverageFPS);
		FPSCount->SetText(FPSText + Value);

		static char FormatString[100];

		sprintf(FormatString, "X: %.3lf, Y: %.3lf, Z: %.3lf", CamPos.x, CamPos.y, CamPos.z);

		Value.SetValue(FormatString);
		CameraPosition->SetText(CamPosText + Value);
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
	
	
