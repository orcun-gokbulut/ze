//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9ShadowRenderer.cpp
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
#include "ZETexture/ZETextureOptions.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZERandom.h"


#include "ZEError.h"
#include "ZED3D9Shader.h"
#include "ZED3D9Module.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9TextureCube.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZED3D9ShadowRenderer.h"
#include "ZEGraphics/ZEMaterial.h"
#include "ZEGraphics/ZEPointLight.h"
#include "ZEGraphics/ZERenderCommand.h"
#include "ZEGraphics/ZEProjectiveLight.h"
#include "ZEGraphics/ZEDirectionalLight.h"
#include "ZEGraphics/ZEOmniProjectiveLight.h"

#include <time.h>

ZED3D9VertexShader* OmniLightVS = NULL;
ZED3D9PixelShader* OmniLightPS = NULL;
ZED3D9VertexShader*	ProjectiveLightVS = NULL;
ZED3D9PixelShader* ProjectiveLightPS = NULL;
ZED3D9VertexShader*	DirectionalLightVS = NULL;
ZED3D9PixelShader* DirectionalLightPS = NULL;

void ZED3D9ShadowRenderer::SetViewPort(ZEViewPort* ViewPort)
{
	this->ViewPort = (ZED3D9ViewPort*)ViewPort;
}

ZEViewPort* ZED3D9ShadowRenderer::GetViewPort()
{
	return ViewPort;
}

void ZED3D9ShadowRenderer::SetFace(bool Front)
{
	Face = Front;
}

bool ZED3D9ShadowRenderer::GetFace()
{
	return Face;
}

void ZED3D9ShadowRenderer::SetLight(ZELight* Light)
{
	this->Light = Light;
}

ZELight* ZED3D9ShadowRenderer::GetLight()
{
	return Light;
}

void ZED3D9ShadowRenderer::SetDrawParameters(ZEDrawParameters* Parameters)
{
	DrawParameters = Parameters;
}

ZEDrawParameters* ZED3D9ShadowRenderer::GetDrawParameters()
{
	return DrawParameters;
}

void ZED3D9ShadowRenderer::SetShadowResolution(ZEUInt Resolution)
{
	ShadowResolution = Resolution;
}

ZEUInt ZED3D9ShadowRenderer::GetShadowResolution() const
{
	return ShadowResolution;
}

const ZETexture2D* ZED3D9ShadowRenderer::GetRandomRotationMap() const
{
	return RandomRotationMap;
}

ZEArray<ZEPostProcessor*>& ZED3D9ShadowRenderer::GetPostProcessors()
{
	return PostProcessors;
}

void ZED3D9ShadowRenderer::AddPostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.Add(PostProcessor);
}

void ZED3D9ShadowRenderer::RemovePostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.DeleteValue(PostProcessor);
}

void ZED3D9ShadowRenderer::DeviceLost()
{
	Deinitialize();
}

bool ZED3D9ShadowRenderer::DeviceRestored()
{
	Initialize();
	return true;
}

#define RANDOM_ROTATION_TEXTURE_DIMENSION	64
#define SCALE_FLOAT_POSITIVE_RANDOM(Rand, Min, Max) ((Min) + ((Max) * (Rand)) - ((Min) * (Rand)))

bool ZED3D9ShadowRenderer::Initialize()
{
	Deinitialize();

	OmniLightVS = ZED3D9VertexShader::CreateShader("ShadowMaterial.hlsl", "OmniProjectiveLightShadowVS", 0, "vs_3_0");
	OmniLightPS = ZED3D9PixelShader::CreateShader("ShadowMaterial.hlsl", "OmniProjectiveLightShadowPS", 0, "ps_3_0");

	ProjectiveLightVS = ZED3D9VertexShader::CreateShader("ShadowMaterial.hlsl", "ProjectiveLightShadowVS", 0, "vs_3_0");
	ProjectiveLightPS = ZED3D9PixelShader::CreateShader("ShadowMaterial.hlsl", "ProjectiveLightShadowPS", 0, "ps_3_0");

	DirectionalLightVS = ZED3D9VertexShader::CreateShader("ShadowMaterial.hlsl", "DirectionalLightShadowVS", 0, "vs_3_0");
	DirectionalLightPS = ZED3D9PixelShader::CreateShader("ShadowMaterial.hlsl", "DirectionalLightShadowPS", 0, "ps_3_0");

	srand(time(NULL));

	if (RandomRotationMap == NULL)
	{
		RandomRotationMap = ZETexture2D::CreateInstance();
		RandomRotationMap->Create(RANDOM_ROTATION_TEXTURE_DIMENSION, RANDOM_ROTATION_TEXTURE_DIMENSION, 1, ZE_TPF_RGBA8, false);
		ZESize Pitch = 0;
		ZEUInt32* Data = NULL;
		RandomRotationMap->Lock(((void**)&Data), &Pitch, 0);

		for (ZESize Height = 0; Height < RANDOM_ROTATION_TEXTURE_DIMENSION; ++Height)
		{
			for (ZESize Width = 0; Width < RANDOM_ROTATION_TEXTURE_DIMENSION; ++Width)
			{
				float Random = ZERandom::GetFloatPositive();
				float ScaledRandom = SCALE_FLOAT_POSITIVE_RANDOM(Random, 0.0f, ZE_PIx2);
				float Sin = ZEAngle::Sin(ScaledRandom);
				float Cos = ZEAngle::Cos(ScaledRandom);
				ZEUInt SinComp = (ZEUInt)(((Sin + 1.0f) / 2.0f) * 255.0f);
				ZEUInt CosComp = (ZEUInt)(((Cos + 1.0f) / 2.0f) * 255.0f);

				ZEUInt32* Pixel = Data + Height * 64 + Width;
				*Pixel = D3DCOLOR_ARGB(0, SinComp, CosComp, 0);
			}
		}

		RandomRotationMap->Unlock(0);
	}

	if (NULLRenderTarget == NULL)
	{
		NULLRenderTarget = (ZED3D9Texture2D*)ZED3D9Texture2D::CreateInstance();
		NULLRenderTarget->Create(ShadowResolution, ShadowResolution, 1, ZE_TPF_NULL, true);
	}
	else
	{
		if (NULLRenderTarget->GetWidth() != ShadowResolution || NULLRenderTarget->GetHeight() != ShadowResolution)
		{
			NULLRenderTarget->Destroy();
			NULLRenderTarget = (ZED3D9Texture2D*)ZED3D9Texture2D::CreateInstance();
			NULLRenderTarget->Create(ShadowResolution, ShadowResolution, 1, ZE_TPF_NULL, true);
		}
	}

	if (DepthSurface == NULL)
	{
		if (FAILED(GetDevice()->CreateDepthStencilSurface(ShadowResolution, ShadowResolution, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &DepthSurface, NULL)))
		{
			zeError("Can not create shadow map z buffer.");
			return false;
		}
	}

	return true;
}

void ZED3D9ShadowRenderer::Deinitialize()
{
	RandomRotationMap = NULL;

	ZED3D_RELEASE(DepthSurface);
	ZED3D_RELEASE(NULLRenderTarget);
	ZED3D_RELEASE(OmniLightVS);
	ZED3D_RELEASE(OmniLightVS);
	ZED3D_RELEASE(DirectionalLightVS);
	ZED3D_RELEASE(DirectionalLightPS);
	ZED3D_RELEASE(ProjectiveLightVS);
	ZED3D_RELEASE(ProjectiveLightPS);
}

void ZED3D9ShadowRenderer::Destroy()
{
	GetModule()->ShadowRenderers.DeleteValue((ZED3D9ShadowRenderer*)this);
	ZERenderer::Destroy();
}

void ZED3D9ShadowRenderer::ClearRenderList()
{
	CommandList.Clear(true);
}

const ZESmartArray<ZERenderCommand>& ZED3D9ShadowRenderer::GetRenderList() const
{
	return CommandList;
}

void ZED3D9ShadowRenderer::AddToRenderList(ZERenderCommand* RenderCommand)
{
	#ifdef ZE_DEBUG_ENABLE
		if (!ZED3D9FrameRenderer::CheckRenderCommand(RenderCommand))
			return;
	#endif

	CommandList.Add(*RenderCommand);
}

void ZED3D9ShadowRenderer::RenderProjectiveLight()
{
	zeCriticalError("Not implemented yet");

// 	ZEProjectiveLight* Light = (ZEProjectiveLight*)this->Light;
// 	GetDevice()->SetDepthStencilSurface(((ZED3D9ViewPort*)Light->GetShadowMap()->GetViewPort())->FrameBuffer);
// 	float MaxFloat = 1.0f;
// 	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, *(ZEInt*)&MaxFloat, 1.0f, 0x00);
// 
// 	GetDevice()->SetVertexShader(ProjectiveLightVS->GetVertexShader());
// 	GetDevice()->SetPixelShader(ProjectiveLightPS->GetPixelShader());
// 	
// 	ZEMatrix4x4 ViewTransform, ProjectionTransform, ViewProjectionTransform;
// 	ZEMatrix4x4::CreateViewTransform(ViewTransform, Light->GetPosition(), Light->GetRotation());
// 	ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, Light->GetFOV(), Light->GetAspectRatio(), zeGraphics->GetNearZ(), Light->GetRange());
// 
// 	ZEMatrix4x4::Multiply(ViewProjectionTransform, ProjectionTransform, ViewTransform);
// 	GetDevice()->BeginScene();
// 	for (ZESize I = 0; I < CommandList.GetCount(); I++)
// 	{
// 		ZERenderCommand* RenderCommand = &CommandList[I];
// 
// 		ZEMatrix4x4 ViewProjMatrix;
// 		if ((RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM) == ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
// 		{
// 			ViewProjMatrix = ViewProjectionTransform;
// 		}
// 		else if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
// 		{
// 			ViewProjMatrix = ViewTransform;
// 		}
// 		else if (RenderCommand->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
// 		{
// 			ViewProjMatrix = ProjectionTransform;
// 		}
// 		else
// 		{
// 			ViewProjMatrix = ZEMatrix4x4::Identity;
// 		}
// 
// 		ZEMatrix4x4 WorldViewProjMatrix;
// 		if (RenderCommand->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
// 		{
// 			ZEMatrix4x4::Multiply(WorldViewProjMatrix, ViewProjMatrix, RenderCommand->WorldMatrix);
// 		}
// 		else
// 		{
// 			WorldViewProjMatrix = ViewProjectionTransform;
// 		}
// 
// 		GetDevice()->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
// 
// 		// Setup ZCulling
// 		if (RenderCommand->Flags & ZE_ROF_ENABLE_Z_CULLING)
// 		{
// 			GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
// 			GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
// 			GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, RenderCommand->Flags & ZE_ROF_ENABLE_NO_Z_WRITE ? FALSE : TRUE);
// 		}
// 		else
// 		{
// 			GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
// 		}
// 
// 		// Setup Bone Transforms
// 		if (RenderCommand->Flags & ZE_ROF_SKINNED && RenderCommand->BoneTransforms.GetCount() < 58)
// 		{
// 			GetDevice()->SetVertexShaderConstantF(32, (float*)RenderCommand->BoneTransforms.GetCArray(), (UINT)RenderCommand->BoneTransforms.GetCount() * 4);
// 		}
// 
// 		ZED3D9FrameRenderer::PumpStreams(RenderCommand);
// 	}
// 	GetDevice()->EndScene();
}

void ZED3D9ShadowRenderer::RenderOmniProjectiveLight()
{
	zeCriticalError("Not implemented yet");
}

void ZED3D9ShadowRenderer::RenderDirectionalLight()
{
	GetDevice()->BeginScene();

	GetDevice()->SetDepthStencilSurface(((ZED3D9ViewPort*)DrawParameters->ViewPort)->FrameBuffer);
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)NULLRenderTarget->GetViewPort())->FrameBuffer);
	GetDevice()->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0x00);

// 	GetDevice()->SetDepthStencilSurface(DepthSurface);
// 	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)DrawParameters->ViewPort)->FrameBuffer);
// 	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0x00);

	ZEDirectionalLightCascade* CustomData = (ZEDirectionalLightCascade*)DrawParameters->CustomData;
	for (ZESize CommandN = 0; CommandN < CommandList.GetCount(); CommandN++)
	{
		ZERenderCommand* RenderCommand = &CommandList[CommandN];

 		if (!RenderCommand->Material->GetShadowCaster())
 			continue;

		BOOL SkinEnabled = FALSE;
  		if (RenderCommand->Flags & ZE_ROF_SKINNED && RenderCommand->BoneTransforms.GetCount() < 58)
  		{
  			SkinEnabled = TRUE;
  			UINT MatrixCount = (UINT)RenderCommand->BoneTransforms.GetCount();
  			const float* BoneMatrices = (const float*)RenderCommand->BoneTransforms.GetConstCArray();
 
  			GetDevice()->SetVertexShaderConstantF(32, BoneMatrices, MatrixCount * 4);
  		}

		ZEMatrix4x4 LightShadowMatrix = CustomData->ShadowTransform * RenderCommand->WorldMatrix;
		
		GetDevice()->SetVertexShaderConstantB(0, &SkinEnabled, 1);
		GetDevice()->SetVertexShaderConstantF(0, LightShadowMatrix.MA, 4);

		ZED3D9FrameRenderer::PumpStreams(RenderCommand);
	}

	GetDevice()->EndScene();
	
}

void ZED3D9ShadowRenderer::RenderPointLight()
{
	zeCriticalError("Not implemented yet");

// 	ZEPointLight* Light = (ZEPointLight*)this->Light;
// 
// 	GetDevice()->SetDepthStencilSurface(ShadowMapZBuffer);
// 	if (Face)
// 	{
// 		GetDevice()->SetDepthStencilSurface(((ZED3D9ViewPort*)Light->GetFrontShadowMap()->GetViewPort())->FrameBuffer);
// 	}
// 	else
// 	{
// 		GetDevice()->SetDepthStencilSurface(((ZED3D9ViewPort*)Light->GetBackShadowMap()->GetViewPort())->FrameBuffer);
// 	}
// 
// 	float MaxFloat = FLT_MAX;
// 	GetDevice()->Clear(0, NULL, D3DCLEAR_ZBUFFER, *(ZEInt*)&MaxFloat, 1.0f, 0x00);
// 
// 	GetDevice()->SetVertexShader(OmniLightVS->GetVertexShader());
// 	GetDevice()->SetPixelShader(OmniLightPS->GetPixelShader());
// 	
// 	ZEMatrix4x4 ViewTransform, ProjectionTransform, ViewProjectionTransform;
// 	ZEMatrix4x4::CreateViewTransform(ViewTransform, Light->GetPosition(), (Face ? Light->GetRotation() : Light->GetRotation() * ZEQuaternion(ZE_PI, ZEVector3(0.0f, 1.0f, 0.0f))));
// //	ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, Light->GetFOV(), Light->GetAspectRatio(), zeGraphics->GetNearZ(), Light->GetRange());
// 	ProjectionTransform = ZEMatrix4x4::Identity;
// 
// 	ZEMatrix4x4::Multiply(ViewProjectionTransform, ProjectionTransform, ViewTransform);
// 
// 	ZEVector4 Options = ZEVector4(Light->GetRange(), 0.0f, 0.0f, 0.0f);
// 	GetDevice()->SetVertexShaderConstantF(4, (float*)&Options, 1);
// 
// 	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFF);
// 	GetDevice()->BeginScene();
// 	for (ZESize I = 0; I < CommandList.GetCount(); I++)
// 	{
// 		ZERenderCommand* RenderCommand = &CommandList[I];
// 
// 		ZEMatrix4x4 ViewProjMatrix;
// 		if ((RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM) == ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
// 		{
// 			ViewProjMatrix = ViewProjectionTransform;
// 		}
// 		else if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
// 		{
// 			ViewProjMatrix = ViewTransform;
// 		}
// 		else if (RenderCommand->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
// 		{
// 			ViewProjMatrix = ProjectionTransform;
// 		}
// 		else
// 		{
// 			ViewProjMatrix = ZEMatrix4x4::Identity;
// 		}
// 
// 		ZEMatrix4x4 WorldViewProjMatrix;
// 		if (RenderCommand->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
// 		{
// 			ZEMatrix4x4::Multiply(WorldViewProjMatrix, ViewProjMatrix, RenderCommand->WorldMatrix);
// 		}
// 		else
// 		{
// 			WorldViewProjMatrix = ViewProjectionTransform;
// 		}
// 
// 		GetDevice()->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
// 
// 		// Setup ZCulling
// 		if (RenderCommand->Flags & ZE_ROF_ENABLE_Z_CULLING)
// 		{
// 			GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
// 			GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
// 			GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, RenderCommand->Flags & ZE_ROF_ENABLE_NO_Z_WRITE ? FALSE : TRUE);
// 		}
// 		else
// 		{
// 			GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
// 		}
// 
// 		// Setup Bone Transforms
// 		if (RenderCommand->Flags & ZE_ROF_SKINNED && RenderCommand->BoneTransforms.GetCount() < 58)
// 		{
// 			GetDevice()->SetVertexShaderConstantF(32, (float*)RenderCommand->BoneTransforms.GetCArray(), (UINT)RenderCommand->BoneTransforms.GetCount() * 4);
// 		}
// 
// 		ZED3D9FrameRenderer::PumpStreams(RenderCommand);
// 	}
// 	GetDevice()->EndScene();
}

void ZED3D9ShadowRenderer::Render(float ElaspedTime)
{
	D3DPERF_BeginEvent(0, L"Cascade Pass");
	
	D3DVIEWPORT9 OldViewport;
	GetDevice()->GetViewport(&OldViewport);
	
	D3DVIEWPORT9 Viewport;
	Viewport.X = 0;
	Viewport.Y = 0;
	Viewport.MaxZ = 1.0f;
	Viewport.MinZ = 0.0f;
	Viewport.Width = ShadowResolution;
	Viewport.Height = ShadowResolution;
	GetDevice()->SetViewport(&Viewport);

	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	GetDevice()->SetVertexShader(DirectionalLightVS->GetVertexShader());
	GetDevice()->SetPixelShader(DirectionalLightPS->GetPixelShader());
	
	//GetDevice()->SetPixelShader(NULL);

	switch(Light->GetLightType())
	{
		case ZE_LT_POINT:
			RenderPointLight();
			break;
		case ZE_LT_DIRECTIONAL:
			RenderDirectionalLight();
			break;
		case ZE_LT_PROJECTIVE:
			RenderProjectiveLight();
			break;
		case ZE_LT_OMNIPROJECTIVE:
			RenderOmniProjectiveLight();
			break;
	}

	DrawParameters = NULL;
	GetDevice()->SetViewport(&OldViewport);


	D3DPERF_EndEvent();
}

ZED3D9ShadowRenderer::ZED3D9ShadowRenderer()
{
	DepthSurface = NULL;
	DrawParameters = NULL;
	NULLRenderTarget = NULL;
	RandomRotationMap = NULL;
	FilterType = ZE_SMFT_PCF_POISSON_5_TAP;

	ShadowResolution = 4096;
}

ZED3D9ShadowRenderer::~ZED3D9ShadowRenderer()
{
	Deinitialize();
}
