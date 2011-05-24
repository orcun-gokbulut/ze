//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9FrameRenderer.cpp
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

#ifdef ZE_DEBUG_ENABLED
#define D3D_DEBUG_INFO
#endif

#include "ZED3D9FrameRenderer.h"
#include "ZED3D9VertexBuffer.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9Texture3D.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9Module.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9VertexDeclaration.h"
#include "ZED3D9Shader.h"
#include "ZED3D9Profiler.h"

#include "ZEGraphics\ZEMaterial.h"
#include "ZEGraphics\ZERenderOrder.h"
#include "ZEGraphics\ZEPointLight.h"
#include "ZEGraphics\ZEDirectionalLight.h"
#include "ZEGraphics\ZEProjectiveLight.h"
#include "ZEGraphics\ZEOmniProjectiveLight.h"
#include "ZEGraphics\ZECamera.h"
#include "ZEMath/ZEMathDefinitions.h"

#include "ZECore\ZEError.h"

#pragma warning(disable:4267)

void ZED3D9FrameRenderer::PumpStreams(ZERenderOrder* RenderOrder)
{
	ZEVertexBuffer* VertexBuffer = RenderOrder->VertexBuffer;

	((ZED3D9VertexDeclaration*)RenderOrder->VertexDeclaration)->SetupVertexDeclaration();

	// Figure out primitive type
	D3DPRIMITIVETYPE PrimitiveType;
	switch(RenderOrder->PrimitiveType)
	{
		default:
		case ZE_ROPT_POINT:
			PrimitiveType = D3DPT_POINTLIST;
			break;
		case ZE_ROPT_LINE:
			PrimitiveType = D3DPT_LINELIST;
			break;
		case ZE_ROPT_TRIANGLE:
			PrimitiveType = D3DPT_TRIANGLELIST;
			break;
	}

	// Make draw call
	// Check wheater render order is indexed or not
	if (RenderOrder->IndexBuffer != NULL)
	{
		/* NOT SUPPORTED !!! */
		/*
		// Check wheater render order has static vertex buffer or not
		if (VertexBuffer->IsStatic())
		GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, VertexBuffer->GetVertexCount(), 0, VertexBuffer->GetPolygonCount());
		else
		GetDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, VertexBuffer->GetVertexCount(), RenderOrder->PrimitiveCount, RenderOrder->IndexBuffer, D3DFMT_INDEX32, VertexBuffer->GetVertexBuffer(), RenderOrder->VertexDeclaration->GetVertexSize());
		*/
	} 
	else
	{
		// Check wheater render order has static vertex buffer or not
		if (VertexBuffer->IsStatic())
		{
			GetDevice()->SetStreamSource(0, ((ZED3D9StaticVertexBuffer*)RenderOrder->VertexBuffer)->StaticBuffer, 0, RenderOrder->VertexDeclaration->GetVertexSize());
			GetDevice()->DrawPrimitive(PrimitiveType, RenderOrder->VertexBufferOffset, RenderOrder->PrimitiveCount);
		}
		else
			GetDevice()->DrawPrimitiveUP(PrimitiveType, RenderOrder->PrimitiveCount, 
			(char*)((ZEDynamicVertexBuffer*)VertexBuffer)->GetVertexBuffer() + RenderOrder->VertexBufferOffset * RenderOrder->VertexDeclaration->GetVertexSize(),  
			RenderOrder->VertexDeclaration->GetVertexSize());
	}
}

bool ZED3D9FrameRenderer::CheckRenderOrder(ZERenderOrder* RenderOrder)
{
	#ifdef ZE_DEBUG_ENABLED
		// Check render order material is available
		if (RenderOrder->Material == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's material does not have valid material.");
			return false;
		}

		// Check vertex declaration is available
		if (RenderOrder->VertexDeclaration == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's vertex declaration is invalid.");
			return false;
		}

		// Check render order has valid primitive type
		if (RenderOrder->PrimitiveType != ZE_ROPT_POINT &&
			RenderOrder->PrimitiveType != ZE_ROPT_LINE &&
			RenderOrder->PrimitiveType != ZE_ROPT_TRIANGLE &&
			RenderOrder->PrimitiveType != ZE_ROPT_TRIANGLESTRIPT)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's primitive type is not valid.");
			return false;
		}

		// Check render order has vertex buffer
		if (RenderOrder->VertexBuffer == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's vertex buffer is not valid.");
			return false;
		}

		// Check render order has one or more primitive
		/*if (RenderOrder->PrimitiveCount == 0)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's primitive count is zero.");
			return false;
		}*/
	#else
		if (RenderOrder->PrimitiveCount == 0)
			return false;
	#endif

	return true;
}

#include "ZEGraphics/ZECanvas.h"

void ZED3D9FrameRenderer::InitializeLightning()
{
	// Create Static Vertex Buffer;
	/*
	Primitive	Start		Start(Byte)	Vertex Count	Prim. Count		Size (Bytes)
	Quad		0			0			6				2				288
	Sphere12	6			288			936				312				44928
	Sphere24	942			45216		3600			1200			172800
	Cone12		4542		218016		18				6				864
	End			4650				
	*/

	ZECanvas Canvas;
	// Directional
	Canvas.AddQuad(
		ZEVector3( 1.0f,  1.0f, 1.0f),
		ZEVector3(-1.0f,  1.0f, 1.0f),
		ZEVector3(-1.0f, -1.0f, 1.0f),
		ZEVector3( 1.0f, -1.0f, 1.0f));

	// Point/OmniProjective
	Canvas.AddSphere(1.0f, 12, 12);
	Canvas.AddSphere(1.0f, 24, 24);

	// Projective
	Canvas.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3::UnitX));
	Canvas.SetTranslation(ZEVector3::UnitZ);
	Canvas.AddPyramid(1.0f, 1.0f, 1.0f);

	if (LightningComponents.LightMeshVB == NULL)
		LightningComponents.LightMeshVB = (ZED3D9StaticVertexBuffer*)Canvas.CreateStaticVertexBuffer();

	LightningComponents.PointLightVS = ZED3D9VertexShader::CreateShader("Lights.hlsl", "ZEPointLight_VertexShader", 0, "vs_3_0");
	LightningComponents.PointLightPS = ZED3D9PixelShader::CreateShader("Lights.hlsl", "ZEPointLight_PixelShader", 0, "ps_3_0");

	LightningComponents.DirectionalLightVS = ZED3D9VertexShader::CreateShader("Lights.hlsl", "ZEDirectionalLight_VertexShader", 0, "vs_3_0");
	LightningComponents.DirectionalLightPS = ZED3D9PixelShader::CreateShader("Lights.hlsl", "ZEDirectionalLight_PixelShader", 0, "ps_3_0");

	LightningComponents.OmniProjectiveLightVS = ZED3D9VertexShader::CreateShader("Lights.hlsl", "ZEOmniProjectiveLight_VertexShader", 0, "vs_3_0");
	LightningComponents.OmniProjectiveLightPS = ZED3D9PixelShader::CreateShader("Lights.hlsl", "ZEOmniProjectiveLight_PixelShader", 0, "ps_3_0");

	LightningComponents.ProjectiveLightVS = ZED3D9VertexShader::CreateShader("Lights.hlsl", "ZEProjectiveLight_VertexShader", 0, "vs_3_0");
	LightningComponents.ProjectiveLightPS = ZED3D9PixelShader::CreateShader("Lights.hlsl", "ZEProjectiveLight_PixelShader", 0, "ps_3_0");

}

void ZED3D9FrameRenderer::DeinitializeLightning()
{
	ZED3D_RELEASE(LightningComponents.LightMeshVB);
	ZED3D_RELEASE(LightningComponents.PointLightVS);
	ZED3D_RELEASE(LightningComponents.PointLightPS);
	ZED3D_RELEASE(LightningComponents.DirectionalLightVS);
	ZED3D_RELEASE(LightningComponents.DirectionalLightPS);
	ZED3D_RELEASE(LightningComponents.ProjectiveLightVS);
	ZED3D_RELEASE(LightningComponents.ProjectiveLightPS);
	ZED3D_RELEASE(LightningComponents.OmniProjectiveLightVS);
	ZED3D_RELEASE(LightningComponents.OmniProjectiveLightPS);
}

void ZED3D9FrameRenderer::DrawPointLight(ZEPointLight* Light)
{
	zeProfilerStart("Light Pass");

	// Light Parameters
	struct
	{
		ZEVector3		Position;
		float			Range;
		ZEVector3		Color;
		float			Intensity;
		ZEVector3		Attenuation;
		float			Reserved1;
	} LightParameters;
	ZEMatrix4x4::Transform(LightParameters.Position, Camera->GetViewTransform(), Light->GetWorldPosition());
	LightParameters.Color = Light->GetColor();
	LightParameters.Attenuation = Light->GetAttenuation();
	LightParameters.Range = Light->GetRange();
	LightParameters.Intensity = Light->GetIntensity();
	
	GetDevice()->SetPixelShaderConstantF(0, (float*)&LightParameters, 3);
	
	// Transformation
	ZEMatrix4x4 WorldTransform;
	ZEMatrix4x4 WorldViewProjTransform;
	ZEMatrix4x4::CreateOrientation(WorldTransform, Light->GetWorldPosition(), 
		ZEQuaternion::Identity, 
		ZEVector3(LightParameters.Range, LightParameters.Range, LightParameters.Range));
	ZEMatrix4x4::Multiply(WorldViewProjTransform, Camera->GetViewProjectionTransform(), WorldTransform);
	GetDevice()->SetVertexShaderConstantF(4, (float*)&WorldViewProjTransform, 4);
	
	//float DistanceToCamera =  ZEVector3::Distance(Light->GetWorldPosition(), Camera->GetWorldPosition());

	GetDevice()->SetVertexShader(LightningComponents.PointLightVS->GetVertexShader());
	GetDevice()->SetPixelShader(LightningComponents.PointLightPS->GetPixelShader());
	
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 312); // Sphere 1

	zeProfilerEnd();
}

void ZED3D9FrameRenderer::DrawDirectionalLight(ZEDirectionalLight* Light)
{
	zeProfilerStart("Directional Light Pass");

	// Light Parameters
	struct
	{
		ZEVector3		Reverved0;
		float			Range;
		ZEVector3		Color;
		float			Intensity;
		ZEVector3		Attenuation;
		float			Reserved1;
		ZEVector3		Direction;
		float			Reserverd2;
	} LightParameters;
	ZEMatrix4x4::Transform3x3(LightParameters.Direction, Camera->GetViewTransform(), Light->GetWorldDirection());
	LightParameters.Color = Light->GetColor();
	LightParameters.Attenuation = Light->GetAttenuation();
	LightParameters.Range = Light->GetRange();
	LightParameters.Intensity = Light->GetIntensity();
	
	GetDevice()->SetPixelShaderConstantF(0, (float*)&LightParameters, 4);
	
	// Transformation
	/*ZEMatrix4x4 WorldTransform;
	ZEMatrix4x4 WorldViewProjTransform;
	ZEMatrix4x4::CreateOrientation(WorldTransform, Light->GetWorldPosition(), 
		ZEQuaternion::Identity, 
		ZEVector3(LightParameters.Range, LightParameters.Range, LightParameters.Range));
	ZEMatrix4x4::Multiply(WorldViewProjTransform, Camera->GetViewProjectionTransform(), WorldTransform);
	GetDevice()->SetVertexShaderConstantF(4, (float*)&WorldViewProjTransform, 4);*/
	
	//float DistanceToCamera =  ZEVector3::Distance(Light->GetWorldPosition(), Camera->GetWorldPosition());

	GetDevice()->SetVertexShader(LightningComponents.DirectionalLightVS->GetVertexShader());
	GetDevice()->SetPixelShader(LightningComponents.DirectionalLightPS->GetPixelShader());
	
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2); // Quad

	zeProfilerEnd();
}

void ZED3D9FrameRenderer::DrawProjectiveLight(ZEProjectiveLight* Light)
{
	if (Light->GetProjectionTexture() == NULL)
		return;

	zeProfilerStart("Projective Light Pass");

	// Light Parameters
	struct
	{
		ZEVector3		Position;
		float			Range;
		ZEVector3		Color;
		float			Intensity;
		ZEVector3		Attenuation;
		ZEVector3		Direction;
		float			FOV;
	} LightParameters;

	ZEMatrix4x4::Transform(LightParameters.Position, Camera->GetViewTransform(), Light->GetWorldPosition());
	LightParameters.Color = Light->GetColor();
	LightParameters.Attenuation = Light->GetAttenuation();
	LightParameters.Range = Light->GetRange();
	LightParameters.Intensity = Light->GetIntensity();
	LightParameters.FOV = Light->GetFOV();
	ZEMatrix4x4::Transform3x3(LightParameters.Direction, Camera->GetViewTransform(), Light->GetWorldDirection());

	GetDevice()->SetPixelShaderConstantF(0, (float*)&LightParameters, 3);

	// Transformation
	ZEMatrix4x4 WorldViewProjTransform, WorldTransform;
	float TanFovRange = tanf(Light->GetFOV() * 0.5f) * Light->GetRange();
	ZEMatrix4x4::CreateOrientation(WorldTransform, Light->GetWorldPosition(), Light->GetWorldRotation(), ZEVector3(TanFovRange * Light->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, Light->GetRange()));
	ZEMatrix4x4::Multiply(WorldViewProjTransform, Camera->GetViewProjectionTransform(), WorldTransform);
	GetDevice()->SetVertexShaderConstantF(4, (float*)&WorldViewProjTransform, 4);

	// Projection Transformation
	ZEMatrix4x4 LightViewProjectionMatrix;
	ZEMatrix4x4 LightViewMatrix;
	ZEMatrix4x4::CreateViewTransform(LightViewMatrix, Light->GetWorldPosition(), Light->GetWorldRotation());
	ZEMatrix4x4 LightProjectionMatrix;
	ZEMatrix4x4::CreatePerspectiveProjection(LightProjectionMatrix, Light->GetFOV(), Light->GetAspectRatio(), zeGraphics->GetNearZ(), Light->GetRange());
	ZEMatrix4x4::Multiply(LightViewProjectionMatrix, LightProjectionMatrix, LightViewMatrix);

	ZEMatrix4x4 TextureMatrix;
	ZEMatrix4x4::Create(TextureMatrix, 
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.001f, 1.0f);

	ZEMatrix4x4 InvCameraViewMatrix;
	ZEMatrix4x4::Inverse(InvCameraViewMatrix, Camera->GetViewTransform());
	ZEMatrix4x4 ProjectionMatrix, Temp;
	ZEMatrix4x4::Multiply(Temp, LightViewProjectionMatrix, InvCameraViewMatrix);
	ZEMatrix4x4::Multiply(ProjectionMatrix, TextureMatrix, Temp);

	GetDevice()->SetPixelShaderConstantF(16, (float*)&ProjectionMatrix, 4);

	//float DistanceToCamera =  ZEVector3::Distance(Light->GetWorldPosition(), Camera->GetWorldPosition());
	GetDevice()->SetVertexShader(LightningComponents.ProjectiveLightVS->GetVertexShader());
	GetDevice()->SetPixelShader(LightningComponents.ProjectiveLightPS->GetPixelShader());
	

	// Stencil Test
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	GetDevice()->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_REPLACE);
	GetDevice()->SetRenderState(D3DRS_STENCILREF, LightStencilMaskValue);
	GetDevice()->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	GetDevice()->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	LightStencilMaskValue++;

	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 4542, 6); // Cone 2
	
	// Draw Light
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFF);

	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);
	GetDevice()->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
	GetDevice()->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	ZED3D9CommonTools::SetTexture(2, (ZETextureCube*)Light->GetProjectionTexture(), D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	if (Light->GetCastsShadow() && Light->GetShadowMap() != NULL)
		ZED3D9CommonTools::SetTexture(4, Light->GetShadowMap(), D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 4542, 6); // Cone 2

	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	zeProfilerEnd();
}

void ZED3D9FrameRenderer::DrawOmniProjectiveLight(ZEOmniProjectiveLight* Light)
{
	if (Light->GetProjectionTexture() == NULL)
		return;

	zeProfilerStart("Omni Projective Light Pass");

	// Light Parameters
	struct
	{
		ZEVector3		Position;
		float			Range;
		ZEVector3		Color;
		float			Intensity;
		ZEVector3		Attenuation;
		float			Reserved1;
	} LightParameters;
	ZEMatrix4x4::Transform(LightParameters.Position, Camera->GetViewTransform(), Light->GetWorldPosition());
	LightParameters.Color = Light->GetColor();
	LightParameters.Attenuation = Light->GetAttenuation();
	LightParameters.Range = Light->GetRange();
	LightParameters.Intensity = Light->GetIntensity();

	GetDevice()->SetPixelShaderConstantF(0, (float*)&LightParameters, 3);

	// Transformation
	ZEMatrix4x4 WorldTransform;
	ZEMatrix4x4 WorldViewProjTransform;
	ZEMatrix4x4::CreateOrientation(WorldTransform, Light->GetWorldPosition(), 
		ZEQuaternion::Identity, 
		ZEVector3(LightParameters.Range, LightParameters.Range, LightParameters.Range));
	ZEMatrix4x4::Multiply(WorldViewProjTransform, Camera->GetViewProjectionTransform(), WorldTransform);
	GetDevice()->SetVertexShaderConstantF(4, (float*)&WorldViewProjTransform, 4);

	// Projection Transform
	ZEQuaternion ProjectionRotation;
	ZEQuaternion::Product(ProjectionRotation, Light->GetWorldRotation().Conjugate(), Camera->GetWorldRotation());
	
	ZEQuaternion::Normalize(ProjectionRotation, ProjectionRotation);

	ZEMatrix4x4 ProjectionMatrix;
	ZEMatrix4x4::CreateRotation(ProjectionMatrix, ProjectionRotation);
	GetDevice()->SetPixelShaderConstantF(12, (float*)&ProjectionMatrix, 3);

	//float DistanceToCamera =  ZEVector3::Distance(Light->GetWorldPosition(), Camera->GetWorldPosition());
	GetDevice()->SetVertexShader(LightningComponents.OmniProjectiveLightVS->GetVertexShader());
	GetDevice()->SetPixelShader(LightningComponents.OmniProjectiveLightPS->GetPixelShader());

	ZED3D9CommonTools::SetTexture(3, (ZETextureCube*)Light->GetProjectionTexture(), D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 312); // Sphere 1

	zeProfilerEnd();
}

void ZED3D9FrameRenderer::DoPreZPass()
{
	zeProfilerStart("PreZ Pass");

	GetDevice()->SetRenderTarget(0, ViewPort->FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	for (size_t I = 0; I < RenderList.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &RenderList[I];
		
		if ((RenderOrder->Material->GetMaterialFlags() & ZE_MTF_PRE_Z_PASS) == 0)
			continue;

		if (!RenderOrder->Material->SetupPreZPass(this, RenderOrder))
			zeCriticalError("Renderer", "Can not set material's Pre-Z pass. (Material Type : \"%s\")", RenderOrder->Material->GetClassDescription()->GetName());

		PumpStreams(RenderOrder);
	}

	zeProfilerEnd();
}

void ZED3D9FrameRenderer::DoGBufferPass()
{
	zeProfilerStart("GBuffer Pass");
	
	GetDevice()->SetRenderTarget(0, ((ZED3D9ViewPort*)GBuffer1->GetViewPort())->FrameBuffer);
	GetDevice()->SetRenderTarget(1, ((ZED3D9ViewPort*)GBuffer2->GetViewPort())->FrameBuffer);
	GetDevice()->SetRenderTarget(2, ((ZED3D9ViewPort*)GBuffer3->GetViewPort())->FrameBuffer);

	GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0x00);

	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	for (size_t I = 0; I < RenderList.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &RenderList[I];

		if (RenderOrder->Pipeline != ZE_RORP_3D)
			continue;

		if ((RenderOrder->Material->GetMaterialFlags() & ZE_MTF_G_BUFFER_PASS) == 0)
			continue;

		zeProfilerStart("Object Pass");
		if (!RenderOrder->Material->SetupGBufferPass(this, RenderOrder))
			zeCriticalError("Renderer", "Can not set material's GBuffer pass. (Material Type : \"%s\")", RenderOrder->Material->GetClassDescription()->GetName());

		PumpStreams(RenderOrder);

		zeProfilerEnd();
	}
	
	GetDevice()->SetRenderTarget(1, NULL);
	GetDevice()->SetRenderTarget(2, NULL);
	zeProfilerEnd();
}

void ZED3D9FrameRenderer::DoLightningPass()
{
	zeProfilerStart("Lightning Pass");

	ZED3D9CommonTools::SetRenderTarget(0, LBuffer1);
	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x00, 0.0f, 0);
	
	if (Lights.GetCount() == 0)
		return;

	// Z-Buffer
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// Alpha blending
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	// Stencil Buffer
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	// GBuffers
	ZED3D9CommonTools::SetTexture(0, GBuffer1, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, GBuffer2, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(2, GBuffer3, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	// ViewVector & PixelSize
	ZEVector4 ViewVector;
	ViewVector.y = tanf(Camera->GetFOV() * 0.5f);
	ViewVector.x = ViewVector.y * ViewPort->GetAspectRatio();
	ViewVector.z = 1.0f;
	ViewVector.w = 0.0f;
	//ZEVector4::Normalize(ViewVector, ViewVector);
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&ViewVector, 1);
	GetDevice()->SetPixelShaderConstantF(5, (const float*)&ZEVector4(1.0f / ViewPort->GetWidth(), 1.0f / ViewPort->GetHeight(), 0.5f / ViewPort->GetWidth(), 0.5f / ViewPort->GetHeight()), 1);

	// Vertex Buffer and Vertex Shader
	GetDevice()->SetStreamSource(0, LightningComponents.LightMeshVB->StaticBuffer, 0, sizeof(ZECanvasVertex));
	ZECanvasVertex::GetVertexDeclaration()->SetupVertexDeclaration();

	// Draw lights
	for (size_t I = 0; I < Lights.GetCount(); I++)
		switch(Lights[I]->GetLightType())
		{
			case ZE_LT_POINT:
				DrawPointLight((ZEPointLight*)Lights[I]);
				break;
			case ZE_LT_DIRECTIONAL:
				DrawDirectionalLight((ZEDirectionalLight*)Lights[I]);
				break;
			case ZE_LT_PROJECTIVE:
				DrawProjectiveLight((ZEProjectiveLight*)Lights[I]);
				break;
			case ZE_LT_OMNIPROJECTIVE:
				DrawOmniProjectiveLight((ZEOmniProjectiveLight*)Lights[I]);
				break;
		}

	zeProfilerEnd();
}

void ZED3D9FrameRenderer::DoForwardPass()
{
	zeProfilerStart("Forward Pass");

	// GBuffers
	ZED3D9CommonTools::SetRenderTarget(0, ABuffer);

	ZED3D9CommonTools::SetTexture(0, GBuffer1, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, GBuffer2, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(2, GBuffer3, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(3, LBuffer1, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(4, SSAOBuffer, D3DTEXF_POINT, D3DTEXF_NONE, D3DTADDRESS_CLAMP);

	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x333333, 1.0f, 0x00);

	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	
	for (size_t I = 0; I < RenderList.GetCount(); I++)
	{		
		ZERenderOrder* RenderOrder = &RenderList[I];
		if (RenderOrder->Pipeline != ZE_RORP_3D)
			continue;

		zeProfilerStart("Object Pass");
		
		if (!RenderOrder->Material->SetupForwardPass(this, RenderOrder))
			zeCriticalError("Renderer", "Can not set material's Forward pass. (Material Type : \"%s\")", RenderOrder->Material->GetClassDescription()->GetName());
		PumpStreams(RenderOrder);
		
		zeProfilerEnd();
	}

	GetDevice()->SetTexture(0, NULL);
	GetDevice()->SetTexture(1, NULL);
	GetDevice()->SetTexture(2, NULL);
	GetDevice()->SetTexture(3, NULL);
	GetDevice()->SetTexture(4, NULL);

	zeProfilerEnd();
}


void ZED3D9FrameRenderer::Do2DPass()
{
	zeProfilerStart("Forward Pass");

	// GBuffers
	ZED3D9CommonTools::SetRenderTarget(0, ViewPort);

	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	for (size_t I = 0; I < RenderList.GetCount(); I++)
	{		
		ZERenderOrder* RenderOrder = &RenderList[I];
		if (RenderOrder->Pipeline != ZE_RORP_2D)
			continue;

		zeProfilerStart("Object Pass");

		if (!RenderOrder->Material->SetupForwardPass(this, RenderOrder))
			zeCriticalError("Renderer", "Can not set material's Forward pass. (Material Type : \"%s\")", RenderOrder->Material->GetClassDescription()->GetName());
		PumpStreams(RenderOrder);

		zeProfilerEnd();
	}

	zeProfilerEnd();
}

void ZED3D9FrameRenderer::InitializeRenderTargets()
{
	if (GBuffer1 == NULL)
		GBuffer1 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	GBuffer1->Create(ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_DEPTH, true);
	
	if (GBuffer2 == NULL)
		GBuffer2 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	GBuffer2->Create(ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_INT32, true);

	if (GBuffer3 == NULL)
		GBuffer3 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	GBuffer3->Create(ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_INT32, true);

	if (LBuffer1 == NULL)
		LBuffer1 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	LBuffer1->Create(ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR, true);

	if (LBuffer2 == NULL)
		LBuffer2 = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	LBuffer2->Create(ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR, true);

	if (SSAOBuffer == NULL)
		SSAOBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	SSAOBuffer->Create(ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_INT32, true);

	if (ABuffer == NULL)
		ABuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
	ABuffer->Create(ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR, true);
}

void ZED3D9FrameRenderer::DeinitializeRenderTargets()
{
	ZED3D_DESTROY(GBuffer1);
	ZED3D_DESTROY(GBuffer2);
	ZED3D_DESTROY(GBuffer3);
	ZED3D_DESTROY(LBuffer1);
	ZED3D_DESTROY(LBuffer2);
	ZED3D_DESTROY(SSAOBuffer);
	ZED3D_DESTROY(ABuffer);
}

ZED3D9FrameRenderer::ZED3D9FrameRenderer()
{
	GBuffer1 = NULL;
	GBuffer2 = NULL;
	GBuffer3 = NULL;
	LBuffer1 = NULL;
	LBuffer2 = NULL;
	ABuffer = NULL;
	SSAOBuffer = NULL;

	LightningComponents.LightMeshVB = NULL;
	LightningComponents.PointLightVS = NULL;
	LightningComponents.PointLightPS = NULL;
	LightningComponents.DirectionalLightVS = NULL;
	LightningComponents.DirectionalLightPS = NULL;
	LightningComponents.ProjectiveLightVS = NULL;
	LightningComponents.ProjectiveLightPS = NULL;
	LightningComponents.OmniProjectiveLightVS = NULL;
	LightningComponents.OmniProjectiveLightPS = NULL;

	SetViewPort(zeGraphics->GetFrameBufferViewPort());
}

ZED3D9FrameRenderer::~ZED3D9FrameRenderer()
{
	Deinitialize();
}

void ZED3D9FrameRenderer::SetViewPort(ZEViewPort* ViewPort)
{
	this->ViewPort = (ZED3D9ViewPort*)ViewPort;
}

ZEViewPort* ZED3D9FrameRenderer::GetViewPort()
{
	return ViewPort;
}

bool ZED3D9FrameRenderer::Initialize() 
{ 
	InitializeRenderTargets();
	SSAOProcessor.SetRenderer(this);
	HDRProcessor.Initialize();
	SSAOProcessor.Initialize();
	
	InitializeLightning();

	return true; 
} 

void ZED3D9FrameRenderer::Deinitialize()
{
	HDRProcessor.Deinitialize();
	SSAOProcessor.Deinitialize();
	DeinitializeLightning();
	DeinitializeRenderTargets();
}

void ZED3D9FrameRenderer::DeviceLost()
{

}

bool ZED3D9FrameRenderer::DeviceRestored()
{
	return true;
}

void ZED3D9FrameRenderer::Destroy()
{
	// Remove renderer from modules renderer list
	GetModule()->Renderers.DeleteValue((ZED3D9FrameRenderer*)this);
	delete this;
}

void ZED3D9FrameRenderer::SetCamera(ZECamera* Camera)
{
	this->Camera = Camera;
}

ZECamera* ZED3D9FrameRenderer::GetCamera()
{
	return Camera;
}

ZEArray<ZEPostProcessor*>& ZED3D9FrameRenderer::GetPostProcessors()
{
	return PostProcessors;
}

void ZED3D9FrameRenderer::AddPostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.Add(PostProcessor);
}

void ZED3D9FrameRenderer::RemovePostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.DeleteValue(PostProcessor);
}

void ZED3D9FrameRenderer::SetLights(ZESmartArray<ZELight*>& Lights)
{
	this->Lights = Lights;
}

void ZED3D9FrameRenderer::AddToLightList(ZELight* Light)
{
	Lights.Add(Light);
}

void ZED3D9FrameRenderer::ClearLightList()
{
	Lights.Clear();
}

void ZED3D9FrameRenderer::AddToRenderList(ZERenderOrder* RenderOrder)
{
	#ifdef ZE_DEBUG_ENABLED
		// Check render order is valid
		if (!CheckRenderOrder(RenderOrder))
			return;
	#endif

		RenderList.Add(*RenderOrder);
}

void ZED3D9FrameRenderer::ClearRenderList()
{
	RenderList.Clear(true);
}

static int RenderOrderCompare(const void* A, const void* B)
{
	if (((ZERenderOrder*)A)->Priority == ((ZERenderOrder*)B)->Priority)
	{
		if (((ZERenderOrder*)A)->Order == ((ZERenderOrder*)B)->Order)
			return 0;
		else if (((ZERenderOrder*)A)->Order > ((ZERenderOrder*)B)->Order)
			return 1;
		else
			return -1;
	}
	else if (((ZERenderOrder*)A)->Priority > ((ZERenderOrder*)B)->Priority)
		return 1;
	else
		return -1;
}

void ZED3D9FrameRenderer::Render(float ElaspedTime)
{
	if (!GetModule()->IsEnabled() || GetModule()->IsDeviceLost())
		return;

	RenderList.Sort(RenderOrderCompare);

	zeProfilerStart("Rendering");

	GetDevice()->SetDepthStencilSurface(ViewPort->ZBuffer);

	LightStencilMaskValue = 1;
	
	InitializeRenderTargets();

	GetDevice()->BeginScene();

		//DoPreZPass();
		DoGBufferPass();
		DoLightningPass();
		DoForwardPass();

		HDRProcessor.SetInput(ABuffer);
		HDRProcessor.SetOutput(ViewPort);
		HDRProcessor.Process(ElaspedTime);

		Do2DPass();
	GetDevice()->EndScene();
	
	zeProfilerEnd();
}
