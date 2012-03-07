//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9CloudMaterial.cpp
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


#include "..\ZECamera.h"
#include "ZED3D9Shader.h"
#include "ZEMath\ZEAngle.h"
#include "ZEMath\ZEMatrix.h"
#include "ZED3D9CommonTools.h"
#include "..\ZEFrameRenderer.h"
#include "..\ZERenderCommand.h"
#include "ZED3D9CloudMaterial.h"
#include "ZEGame\ZEScene.h"
#include "..\ZETexture2D.h"
#include "ZED3D9Texture2D.h"

#include <d3d9.h>
#include "ZEError.h"
#include "ZETypes.h"
#include "..\ZETextureOptions.h"
#include "..\ZETexture2DResource.h"
#include "ZEGame\ZEEntity.h"
#include "ZEMap\ZEPortalMap\ZEPortalMap.h"
#include "..\ZEVertexDeclaration.h"
#include "ZED3D9VertexDeclaration.h"
#include "..\ZEVertexBuffer.h"
#include "ZED3D9VertexBuffer.h"
#include "..\ZEIndexBuffer.h"
#include "ZED3D9IndexBuffer.h"
#include "ZED3D9FrameRenderer.h"

#define	 ZE_CLOUD_SHADOW_BUFFER_WIDTH		256
#define	 ZE_CLOUD_SHADOW_BUFFER_HEIGHT		256


void ZED3D9CloudMaterial::UpdateShadowTransformations()
{
	ZEAABBox	CloudInView;
	ZEAABBox	GroundInView;
	ZEAABBox	BoundingBox;
	ZEMatrix4x4	WorldToLightMtrx;
	
	// Look at the scene from light
	ZEVector3	Up(0.0f, 1.0f, 0.0f);
	ZEVector3	Light(0.0f, 0.0f, 0.0f);	// Light Position
	ZEVector3	LookAt(0.0f, 0.0f, 0.0f);
	ZEVector3::Add(LookAt, Light, SunLightDirection);
	ZEMatrix4x4::CreateLookAtTransform(WorldToLightMtrx, Light, LookAt, Up);

	// Transform Cloud Bounding box to light coordinate system
	ZEAABBox::Transform(CloudInView, CloudBoundingBox, WorldToLightMtrx);
	ZEAABBox::Transform(GroundInView, GroundBoundingBox, WorldToLightMtrx);

	// Minimize bounding box
	ZEVector3::Min(BoundingBox.Max, CloudInView.Max, GroundInView.Max);
	ZEVector3::Min(BoundingBox.Min, CloudInView.Min, GroundInView.Min);
	BoundingBox.Min.z = CloudInView.Min.z;

	// If there is a valid volume
	if (BoundingBox.Min.x < BoundingBox.Max.x && BoundingBox.Min.y < BoundingBox.Max.y && BoundingBox.Min.z < BoundingBox.Max.z)
	{
		ZEVector3 BoundingBoxDiagonal;
		ZEVector3 BoundingBoxCenter = BoundingBox.GetCenter();
		ZEVector3::Sub(BoundingBoxDiagonal, BoundingBox.Max, BoundingBox.Min);

		// Move the view position to the center of the bounding box
		// Z is located behind the volume
		ZEVector3 EyePosition(BoundingBoxCenter);
		EyePosition.z = BoundingBoxCenter.z - 0.5f * BoundingBoxDiagonal.z;
		ZEVector3 Move;
		ZEVector3::Sub(Move, Light, EyePosition);
		ZEMatrix4x4 Translation;
		ZEMatrix4x4::CreateTranslation(Translation, Move.x, Move.y, Move.z);

		// Orthogonal projection matrix
		ZEMatrix4x4 OrthProj;
		ZEMatrix4x4::CreateOrthographicProjection(OrthProj, BoundingBoxDiagonal.x, BoundingBoxDiagonal.y, 0.0f, BoundingBoxDiagonal.z);

		ZEMatrix4x4 Temp;
		// Create world to shadow map projection matrix (normal projection)
		ZEMatrix4x4::Multiply(Temp, WorldToLightMtrx, Translation);
		ZEMatrix4x4::Multiply(WorldToShadowTexProjMatrix, OrthProj, Temp);

		// Compute world to shadow texture coordinate matrix (Projection to texture coordinate matrix)
		ZEMatrix4x4	ProjToTexCoord(	0.5f,  0.0f, 0.0f, 0.0f,
									0.0f, -0.5f, 0.0f, 0.0f,
									0.0f,  0.0f, 1.0f, 0.0f,
									0.5f,  0.5f, 0.0f, 1.0f);

		ZEMatrix4x4::Multiply(WorldToShadowTexCoordMatrix, ProjToTexCoord, WorldToShadowTexProjMatrix);
		
	}
}

void ZED3D9CloudMaterial::CreateBuffers()
{
	/************************************************************************/
	/*                    Cloud Shadow Pass Buffers                         */
	/************************************************************************/

	// Create vertex declaration
	if (CloudShadowVertexDeclaration == NULL)
	{
		CloudShadowVertexDeclaration = (ZED3D9VertexDeclaration*)ZEVertexDeclaration::CreateInstance();
		ZEVertexElement Declaration0[] = 
		{
			{ZE_VES_POSITION, ZE_VET_FLOAT4, 0}
		};

		if (!CloudShadowVertexDeclaration->Create(Declaration0, 1))
		{
			CloudShadowVertexDeclaration->Destroy();
			CloudShadowVertexDeclaration = NULL;
		}

	}

	CloudShadowVertexCount = ((ZEUInt)CellCountXZ.x + 1) * ((ZEUInt)CellCountXZ.y + 1);
	ZEUInt	IndexCount0 = ((ZEUInt)CellCountXZ.x + 2) * 2 * (ZEUInt)CellCountXZ.y - 2;

	if (CloudShadowVertexBuffer == NULL)
	{
		
		CloudShadowVertexBuffer = (ZED3D9StaticVertexBuffer*)ZEStaticVertexBuffer::CreateInstance();
		if (!CloudShadowVertexBuffer->Create(sizeof(Vertex0) * CloudShadowVertexCount))
		{
			CloudShadowVertexBuffer->Destroy();
			CloudShadowVertexBuffer = NULL;
		}

		Vertex0* Data = NULL;
		Data = (Vertex0*)CloudShadowVertexBuffer->Lock();

		float ScaleX = 1.0f / (CellCountXZ.x + 1.0f);
		float ScaleZ = 1.0f / (CellCountXZ.y + 1.0f);
		
		for (ZEUInt Z = 0; Z < ((ZEUInt)CellCountXZ.y) + 1; ++Z)
		{
			for (ZEUInt X = 0; X < ((ZEUInt)CellCountXZ.x) + 1; ++X)
			{
				Data->X = (float)X;
				Data->Z	= (float)Z;
				Data->U = (float)X * ScaleX;
				Data->V = (float)Z * ScaleZ;
				++Data;
			}
		}

		CloudShadowVertexBuffer->Unlock();
	}

	// Create bounding box
	ZEVector2 EndXZ(CellSizeXZ.x * CellCountXZ.x, CellSizeXZ.y * CellCountXZ.y);
	ZEVector2::Add(EndXZ, EndXZ, StartXZ);

	CloudBoundingBox.Min = ZEVector3(StartXZ.x, 0.0f, StartXZ.y);
	CloudBoundingBox.Max = ZEVector3(EndXZ.x, 0.0f, EndXZ.y);


	if (CloudShadowIndexBuffer == NULL)
	{
		CloudShadowIndexBuffer = (ZED3D9StaticIndexBuffer*)ZEStaticIndexBuffer::CreateInstance();

		if (!CloudShadowIndexBuffer->Create(sizeof(ZEUInt16) * IndexCount0, ZE_IBF_INDEX16))
		{
			CloudShadowIndexBuffer->Destroy();
			CloudShadowIndexBuffer = NULL;
		}

		ZEUInt16* Data = NULL;
		Data = (ZEUInt16*)CloudShadowIndexBuffer->Lock();

		ZEUInt16 VertexNumX = (ZEUInt16)(CellCountXZ.x + 1);
		for (ZEInt16 X = (ZEInt16)CellCountXZ.x; 0 <= X; --X)
		{
			*Data++ = X;
			*Data++ = VertexNumX + X;
		}

		for(ZEUInt16 Z = 1; Z < (ZEInt16)CellCountXZ.y; ++Z)
		{
			*Data++ = Z * VertexNumX;
			*Data++ = Z * VertexNumX + (ZEUInt16)CellCountXZ.x;

			for (ZEInt16 X = (ZEInt16)CellCountXZ.x; 0 <= X; --X)
			{
				*Data++ = Z * VertexNumX + X;
				*Data++ = (Z + 1) * VertexNumX + X;
			}
		}

		CloudShadowIndexBuffer->Unlock();

		CloudShadowTriangleCount = IndexCount0 - 2;
	}


	/************************************************************************/
	/*                    Cloud Density Pass Buffers                        */
	/************************************************************************/
	// Density pass uses the same buffers with cloud shadow pass(previous one)

	
	/************************************************************************/
	/*                    Density Blur Pass Buffers                         */
	/************************************************************************/

	// Vertex declaration for screen aligned quad
	if (DensityBlurVertexDeclaration == NULL)
	{
		DensityBlurVertexDeclaration = (ZED3D9VertexDeclaration*)ZEVertexDeclaration::CreateInstance();
		ZEVertexElement Declaration1[] = 
		{
			{ZE_VES_POSITION,  ZE_VET_FLOAT4, 0},
			{ZE_VES_TEXTCOORD, ZE_VET_FLOAT2, 0}

		};

		if (!DensityBlurVertexDeclaration->Create(Declaration1, 2))
		{
			DensityBlurVertexDeclaration->Destroy();
			DensityBlurVertexDeclaration = NULL;
		}
	}

	if (DensityBlurVertexBuffer == NULL)
	{
		Vertex1 Vertices[] =
		{
			{{ 1.0f,  1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{ 1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{-1.0f,  1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{-1.0f, -1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};

		DensityBlurVertexBuffer = (ZED3D9StaticVertexBuffer*)ZEStaticVertexBuffer::CreateInstance();
		if (!DensityBlurVertexBuffer->Create(sizeof(Vertex1) * 4))
		{
			DensityBlurVertexBuffer->Destroy();
			DensityBlurVertexBuffer = NULL;
		}

		Vertex1* Data = NULL;
		Data = (Vertex1*)DensityBlurVertexBuffer->Lock();

		memcpy(Data, Vertices, sizeof(Vertex1) * 4);
		
		DensityBlurVertexBuffer->Unlock();

		DensityBlurTriangleCount = 2;
	}
	
	// No need for index buffer in density blur pass
	
	/************************************************************************/
	/*                    Render Cloud Pass Buffers                         */
	/************************************************************************/
	
	ZEUInt DivX = 4;
	ZEUInt DivY = 4;
	RenderCloudVertexCount = (DivX + 1) * (DivY + 1);
	ZEUInt IndexCount1 = 2 * DivY * (DivX + 1) + (DivY - 1) * 2;
	RenderCloudTriangleCount = IndexCount1 - 2;
	
	// Same vertex declaration of Density Blur Pass is used in this pass
	if (RenderCloudVertexDeclaration == NULL)
	{
		RenderCloudVertexDeclaration = (ZED3D9VertexDeclaration*)ZEVertexDeclaration::CreateInstance();
		ZEVertexElement Declaration2[] = // Same as previous declaration
		{
			{ZE_VES_POSITION,  ZE_VET_FLOAT4, 0},
			{ZE_VES_TEXTCOORD, ZE_VET_FLOAT2, 0},

		};

		if (!RenderCloudVertexDeclaration->Create(Declaration2, 2))
		{
			RenderCloudVertexDeclaration->Destroy();
			RenderCloudVertexDeclaration = NULL;
		}

	}

	// Same structure of vertex1 is used here
	if (RenderCloudVertexBuffer == NULL)
	{
		RenderCloudVertexBuffer = (ZED3D9StaticVertexBuffer*)ZEStaticVertexBuffer::CreateInstance();
		if (!RenderCloudVertexBuffer->Create(sizeof(Vertex1) * RenderCloudVertexCount))
		{
			RenderCloudVertexBuffer->Destroy();
			RenderCloudVertexBuffer = NULL;
		}

		Vertex1* Data = NULL;
		Data = (Vertex1*)RenderCloudVertexBuffer->Lock();

		float Depth = 0.999999f;
		for (ZEUInt I = 0; I <= DivY; ++I)
		{
			for (ZEUInt J = 0; J <= DivX; ++J)
			{
				float X = 1.0f - J / (float)DivX;
				float Y = I / (float)DivY;

				Data->Position[0] = X * 2.0f - 1.0f;
				Data->Position[1] = -1.0f * (Y * 2.0f - 1.0f);
				Data->Position[2] = Depth;
				Data->Position[3] = 1.0f;
				
				Data->UV[0] = X;
				Data->UV[1] = Y;

				++Data;
			}
		}

		RenderCloudVertexBuffer->Unlock();
	}

	if (RenderCloudIndexBuffer == NULL)
	{
		RenderCloudIndexBuffer = (ZED3D9StaticIndexBuffer*)ZEStaticIndexBuffer::CreateInstance();

		if (!RenderCloudIndexBuffer->Create(sizeof(ZEUInt16) * IndexCount1, ZE_IBF_INDEX16))
		{
			RenderCloudIndexBuffer->Destroy();
			RenderCloudIndexBuffer = NULL;
		}

		ZEUInt16* Data = NULL;
		Data = (ZEUInt16*)RenderCloudIndexBuffer->Lock();

		for (ZEUInt16 I = 0; I < DivY; ++I)
		{
			for (ZEUInt16 J = 0; J <= DivX; ++J)
			{
				*Data = I * (DivX + 1) + J;
				++Data;
				*Data = (I + 1) * (DivX + 1) + J;
				++Data;
			}

			if (I + 1 < (ZEUInt16)DivY)
			{
				*Data = (I + 1) * (DivX + 1) + DivX;
				++Data;
				*Data = (I + 1) * (DivX + 1);
				++Data;
			}
		}

		RenderCloudIndexBuffer->Unlock();
	}

}

void ZED3D9CloudMaterial::ReleaseBuffers()
{
	
	if (CloudShadowIndexBuffer != NULL)
	{
		CloudShadowIndexBuffer->Release();
		CloudShadowIndexBuffer->Destroy();
		CloudShadowIndexBuffer = NULL;
	}
	if (CloudShadowVertexBuffer != NULL)
	{
		CloudShadowVertexBuffer->Release();
		CloudShadowVertexBuffer->Destroy();
		CloudShadowVertexBuffer = NULL;
	}
	if (CloudShadowIndexBuffer != NULL)
	{
		CloudShadowIndexBuffer->Release();
		CloudShadowIndexBuffer->Destroy();
		CloudShadowIndexBuffer = NULL;
	}
	if (CloudShadowVertexDeclaration != NULL)
	{
		CloudShadowVertexDeclaration->Release();
		CloudShadowVertexDeclaration->Destroy();
		CloudShadowVertexDeclaration = NULL;
	}
	if (DensityBlurVertexDeclaration != NULL)
	{
		DensityBlurVertexDeclaration->Release();
		DensityBlurVertexDeclaration->Destroy();
		DensityBlurVertexDeclaration = NULL;
	}
	if (CloudShadowIndexBuffer != NULL)
	{
		RenderCloudIndexBuffer->Release();
		RenderCloudIndexBuffer->Destroy();
		RenderCloudIndexBuffer = NULL;
	}
	if (RenderCloudVertexBuffer != NULL)
	{
		RenderCloudVertexBuffer->Release();
		RenderCloudVertexBuffer->Destroy();
		RenderCloudVertexBuffer = NULL;
	}
	if (RenderCloudVertexDeclaration != NULL)
	{
		RenderCloudVertexDeclaration->Release();
		RenderCloudVertexDeclaration->Destroy();
		RenderCloudVertexDeclaration = NULL;
	}
	
}

void ZED3D9CloudMaterial::CreateRenderTargets()
{
	bool Result = false;
	ZEUInt TargetWidth = zeScene->GetRenderer()->GetViewPort()->GetWidth() / 2;
	ZEUInt TargetHeight = zeScene->GetRenderer()->GetViewPort()->GetHeight() / 2;

	if(CloudShadowBuffer == NULL)
	{
		CloudShadowBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Result = CloudShadowBuffer->Create(ZE_CLOUD_SHADOW_BUFFER_WIDTH, ZE_CLOUD_SHADOW_BUFFER_HEIGHT, ZE_TPF_I8_4, true, 1);
		zeAssert(!Result, "Cannot Create ZED3D9Texture2D: \"CloudShadowBuffer at ZED3D9CloudMaterial\".");
	}
	
	if(CloudDensityBuffer == NULL)
	{
		CloudDensityBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Result = CloudDensityBuffer->Create(TargetWidth, TargetHeight, ZE_TPF_I8_4, true, 1);
		zeAssert(!Result, "Cannot Create ZED3D9Texture2D: \"CloudDensityBuffer at ZED3D9CloudMaterial\".");
	}

	if(CloudDensityBlurBuffer == NULL)
	{
		CloudDensityBlurBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		Result = CloudDensityBlurBuffer->Create(TargetWidth, TargetHeight, ZE_TPF_I8_4, true, 1);
		zeAssert(!Result, "Cannot Create ZED3D9Texture2D: \"CloudDensityBlurBuffer at ZED3D9CloudMaterial\".");
	}

	if(CloudTexture == NULL)
	{
		ZETextureOptions TextureOptions = { ZE_TCT_NONE, ZE_TCQ_AUTO, ZE_TDS_NONE,
											ZE_TFC_ENABLED,	ZE_TMM_DISABLED, 1 };
		
		// Load cloud texture
		ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource("Cloud.bmp", &TextureOptions);
		CloudTexture = (ZED3D9Texture2D*)Resource->GetTexture();
	}
}

void ZED3D9CloudMaterial::ReleaseRenderTargets()
{
	CloudTexture = NULL;
	ZED3D_DESTROY(CloudShadowBuffer);
	ZED3D_DESTROY(CloudDensityBuffer);
	ZED3D_DESTROY(CloudDensityBlurBuffer);

}

void ZED3D9CloudMaterial::CreateShaders()
{
	if (VertexShaderCloudShadow == NULL)
		VertexShaderCloudShadow = ZED3D9VertexShader::CreateShader("CloudMaterial.hlsl", "vs_main_cloud_shadow", 0, "vs_3_0");
		
	if (PixelShaderCloudShadow == NULL)
		PixelShaderCloudShadow = ZED3D9PixelShader::CreateShader("CloudMaterial.hlsl", "ps_main_cloud_shadow", 0, "ps_3_0");

	if (VertexShaderCloudDensity == NULL)
		VertexShaderCloudDensity = ZED3D9VertexShader::CreateShader("CloudMaterial.hlsl", "vs_main_cloud_density", 0, "vs_3_0");
		
	if (PixelShaderCloudDensity == NULL)
		PixelShaderCloudDensity = ZED3D9PixelShader::CreateShader("CloudMaterial.hlsl", "ps_main_cloud_density", 0, "ps_3_0");

	if (VertexShaderDensityBlur == NULL)
		VertexShaderDensityBlur = ZED3D9VertexShader::CreateShader("CloudMaterial.hlsl", "vs_main_density_blur", 0, "vs_3_0");
		
	if (PixelShaderDensityBlur == NULL)
		PixelShaderDensityBlur = ZED3D9PixelShader::CreateShader("CloudMaterial.hlsl", "ps_main_density_blur", 0, "ps_3_0");

	if (VertexShaderDrawCloud == NULL)
		VertexShaderDrawCloud = ZED3D9VertexShader::CreateShader("CloudMaterial.hlsl", "vs_main_draw_cloud", 0, "vs_3_0");
		
	if (PixelShaderDrawCloud == NULL)
		PixelShaderDrawCloud = ZED3D9PixelShader::CreateShader("CloudMaterial.hlsl", "ps_main_draw_cloud", 0, "ps_3_0");
}

void ZED3D9CloudMaterial::ReleaseShaders()
{
	ZED3D_RELEASE(VertexShaderCloudShadow);
	ZED3D_RELEASE(PixelShaderCloudShadow);
	ZED3D_RELEASE(VertexShaderCloudDensity);
	ZED3D_RELEASE(PixelShaderCloudDensity);
	ZED3D_RELEASE(VertexShaderDensityBlur);
	ZED3D_RELEASE(PixelShaderDensityBlur);
	ZED3D_RELEASE(VertexShaderDrawCloud);
	ZED3D_RELEASE(PixelShaderDrawCloud);

}

ZED3D9CloudMaterial::ZED3D9CloudMaterial()
{
	CellCountXZ		= ZEVector2(16.0f, 16.0f);
	PlaneDimensions	= ZEVector2(40000.0f, 40000.0f);
	StartXZ			= ZEVector2(-PlaneDimensions.x / 2.0f, -PlaneDimensions.y / 2.0f);
	CellSizeXZ		= ZEVector2(PlaneDimensions.x / CellCountXZ.x, PlaneDimensions.y / CellCountXZ.y);

	VertexShaderCloudShadow			= NULL;
	PixelShaderCloudShadow			= NULL;

	VertexShaderCloudDensity		= NULL;
	PixelShaderCloudDensity			= NULL;

	VertexShaderDensityBlur			= NULL;
	PixelShaderDensityBlur			= NULL;

	VertexShaderDrawCloud			= NULL;
	PixelShaderDrawCloud			= NULL;

	CloudShadowVertexCount			= 0;
	CloudShadowTriangleCount		= 0;
	CloudShadowIndexBuffer			= NULL;
	CloudShadowVertexBuffer			= NULL;
	CloudShadowVertexDeclaration	= NULL;

	DensityBlurTriangleCount		= 0;
	DensityBlurVertexBuffer			= NULL;
	DensityBlurVertexDeclaration	= NULL;

	RenderCloudVertexCount			= 0;
	RenderCloudTriangleCount		= 0;
	RenderCloudIndexBuffer			= NULL;
	RenderCloudVertexBuffer			= NULL;
	RenderCloudVertexDeclaration	= NULL;

	CloudTexture					= NULL;
	CloudShadowBuffer				= NULL;
	CloudDensityBuffer				= NULL;
	CloudDensityBlurBuffer			= NULL;

}

ZED3D9CloudMaterial::~ZED3D9CloudMaterial()
{
	this->Release();
}

bool ZED3D9CloudMaterial::SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	// Update material if its changed. (Recompile shader, etc.)
	// ((ZED3D9CloudMaterial*)this)->UpdateMaterial();
	
	((ZED3D9CloudMaterial*)this)->UpdateShadowTransformations();

	IDirect3DSurface9* RenderTarget;
	GetDevice()->GetRenderTarget(0, &RenderTarget);

	// This cloud shadow pass is not functional right now

	/************************************************************************/
	/*               Pass 1: Create Cloud Shadow Texture                    */
	/************************************************************************/
	/*
	GetDevice()->SetVertexShader(VertexShaderCloudShadow->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderCloudShadow->GetPixelShader());

	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);

	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)CloudShadowBuffer);
	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x00FFFFFF, 1.0f, 0x00);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)CloudTexture, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_WRAP);
	*/
	ZEVector3 CameraPosition = Camera->GetWorldPosition();

	struct
	{
		float	HeightParameters[2];
		float	Reserved0;
		float	Reserved1;

		float	CameraPos[3];
		float	Reserved2;

		float	UVParameters[4];

		float	XZParameters[4];

	}
	VertexShaderParameters = 
	{
		{CloudPlaneHeightFallOff, CloudPlaneHeight}, 0.0f, 0.0f,
		{CameraPosition.x, CameraPosition.y, CameraPosition.z}, 0.0f,
		{5.0f, 5.0f, UVOffset.x, UVOffset.y},
		{CellSizeXZ.x, CellSizeXZ.y, StartXZ.x, StartXZ.y}
	};

	struct
	{
		float	CloudCover;
		float	Reserved0;
		float	Reserved1;
		float	Reserved2;
	}
	PixelShaderParameters =
	{
		CloudCover, 0.0f, 0.0f, 0.0f
	};
	/*
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&WorldToShadowTexProjMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(4, (const float*)&VertexShaderParameters, sizeof(VertexShaderParameters) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelShaderParameters, sizeof(PixelShaderParameters) / 16);

	CloudShadowVertexDeclaration->SetupVertexDeclaration();
	GetDevice()->SetIndices(CloudShadowIndexBuffer->StaticBuffer);
	GetDevice()->SetStreamSource(0, CloudShadowVertexBuffer->StaticBuffer, 0, sizeof(Vertex0));
	GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, CloudShadowVertexCount, 0, CloudShadowTriangleCount);
	*/
	/************************************************************************/
	/*                 Pass 2: Create Cloud Density Texture                 */
	/************************************************************************/
	GetDevice()->SetVertexShader(VertexShaderCloudDensity->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderCloudDensity->GetPixelShader());

	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);

	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)CloudDensityBuffer);
	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0x00);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)CloudTexture, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_WRAP);

	ZEMatrix4x4	WorldViewProjMatrix;
	ZEMatrix4x4::Multiply(WorldViewProjMatrix, Renderer->GetCamera()->GetViewProjectionTransform(), RenderCommand->WorldMatrix);

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&WorldViewProjMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(4, (const float*)&VertexShaderParameters, sizeof(VertexShaderParameters) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelShaderParameters, sizeof(PixelShaderParameters) / 16);


	CloudShadowVertexDeclaration->SetupVertexDeclaration();
	GetDevice()->SetIndices(CloudShadowIndexBuffer->StaticBuffer);
	GetDevice()->SetStreamSource(0, CloudShadowVertexBuffer->StaticBuffer, 0, sizeof(Vertex0));
	GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, CloudShadowVertexCount, 0, CloudShadowTriangleCount);
	
	/************************************************************************/
	/*                   Pass 3: Blur Cloud Density Texture                 */
	/************************************************************************/
	GetDevice()->SetVertexShader(VertexShaderDensityBlur->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderDensityBlur->GetPixelShader());

	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);

	
	ZED3D9CommonTools::SetRenderTarget(0, (ZETexture2D*)CloudDensityBlurBuffer);
	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0x00);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)CloudDensityBuffer, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	// Transform screen coordinate to World coordinate
	ZEMatrix4x4	InvWorldViewProjMat;
	ZEMatrix4x4::Inverse(InvWorldViewProjMat, WorldViewProjMatrix);

	// Transform Light into projection space
	ZEVector4 Light(SunLightDirection.x, SunLightDirection.y, SunLightDirection.z, 0.0f);
	if (Light.y	> 0.0f)
	{
		// assuming light direction is horizontal when sunset or sunrise.
		// otherwise, shadow of clouds converges at a point on the screen opposite to the light.
		Light.y = 0.0f;
		Light.NormalizeSelf();
	}

	ZEVector4 TransformedLight;
	ZEMatrix4x4::Transform(TransformedLight, WorldViewProjMatrix, Light);
	
	// blur vector = vBlurDir.xy * uv + vBlurDir.zw 
	ZEVector4 BlurDirection;
	if (ZEMath::Abs(TransformedLight.w) < ((float)ZE_ZERO_THRESHOLD * 100.0f) || ZEMath::Abs(TransformedLight.z) < ((float)ZE_ZERO_THRESHOLD * 100.0f))
	{
		// if dot( litdir, ray ) == 0.0f : directional light is stil directional in projection space.
		TransformedLight.w = TransformedLight.z = 0.0f;
		TransformedLight.NormalizeSelf();
		TransformedLight.y = -TransformedLight.y;

		BlurDirection = ZEVector4(0.0f, 0.0f, -TransformedLight.x, -TransformedLight.y);
	}
	else
	{
		// otherwise : point blur. light direction is a position in projection space.
		if (0.0f < TransformedLight.w)
		{	
			// transform screen position to texture coordinate
			ZEVector4::Scale(TransformedLight, TransformedLight, 1.0f / TransformedLight.w);
			TransformedLight.x =  0.5f * TransformedLight.x + 0.5f;
			TransformedLight.y = -0.5f * TransformedLight.y + 0.5f;

			// Light in front case
			BlurDirection = ZEVector4(1.0f, 1.0f, -TransformedLight.x, -TransformedLight.y);
		}
		else
		{
			// transform screen position to texture coordinate
			ZEVector4::Scale(TransformedLight, TransformedLight, 1.0f / TransformedLight.w);
			TransformedLight.x =  0.5f * TransformedLight.x + 0.5f;
			TransformedLight.y = -0.5f * TransformedLight.y + 0.5f;

			// invert vector if light comes from behind the camera.
			BlurDirection = ZEVector4(-1.0f, -1.0f, TransformedLight.x, TransformedLight.y);
		}
	}
	
	ZEVector4 LocalRayleigh;
	ZEVector4::Scale(LocalRayleigh, ZEVector4(Rayleigh.x, Rayleigh.y, Rayleigh.z, 0.0f), 3.0f / (16.0f * (float)ZE_PI));
	LocalRayleigh.w = -2.0f * G;
	

	float LocalG = 1.0f - G;
	ZEVector4 LocalMie;
	ZEVector4::Scale(LocalMie, ZEVector4(Mie.x, Mie.y, Mie.z, 0.0f), LocalG * LocalG / (4.0f * (float)ZE_PI));
	LocalMie.w = 1.0f + G * G;

	ZEVector4 LocalMieRayleighSum;
	ZEVector4::Add(LocalMieRayleighSum, ZEVector4(Rayleigh, 0.0f), ZEVector4(Mie, 0.0f));

	ZEVector4 LocalESun;
	LocalESun.x = LightScale * SunLightColor.x / LocalMieRayleighSum.x;
	LocalESun.y = LightScale * SunLightColor.y / LocalMieRayleighSum.y;
	LocalESun.z = LightScale * SunLightColor.z / LocalMieRayleighSum.z;
	LocalESun.w = EarthRadius;

	ZEVector4::Scale(LocalMieRayleighSum, LocalMieRayleighSum, 1.0f / Camera->GetFarZ());
	LocalMieRayleighSum.w = AtmosphereHeight * (2.0f * EarthRadius + AtmosphereHeight);

	ZEVector4 LocalAmbient;
	ZEVector4::Scale(LocalAmbient, ZEVector4(AmbientColor, 0.0f), AmbientScale);
	LocalAmbient.w = 1.0f / ZEMath::Sqrt(LocalMieRayleighSum.w);

	struct
	{
		float	PixelSize[2];
		float	Reserved0;
		float	Reserved1;

		float	BlurDirection[4];

	}
	VertexShaderParametersBlur = 
	{
		{0.5f / CloudDensityBlurBuffer->GetWidth(), 0.5f / CloudDensityBlurBuffer->GetHeight()}, 0.0f, 0.0f,
		{BlurDirection.x, BlurDirection.y, BlurDirection.z, BlurDirection.w}

	};

	struct
	{
		float	InverseMax[2];
		float	Reserved0;
		float	Reserved1;

		float	CameraPosition[3];
		float	Reserved2;

		float	DistanceParamters[3];
		float	Reserved3;

		float	FallofParameters[4];

	}
	PixelShaderParametersBlur = 
	{
		{1.0f / (0.1f / 16.0f), 1.0f / (0.1f / 16.0f)}, 0.0f, 0.0f,
		{CameraPosition.x, CameraPosition.y, CameraPosition.z}, 0.0f,
		{LocalESun.w, LocalMieRayleighSum.w, AtmosphereHeight}, 0.0f,
		{-5000.0f, -1.5f, -1.5f, -1000.0f}
	};

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&InvWorldViewProjMat, 4);
	GetDevice()->SetVertexShaderConstantF(4, (const float*)&VertexShaderParametersBlur, sizeof(VertexShaderParametersBlur) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelShaderParametersBlur, sizeof(PixelShaderParametersBlur) / 16);

	DensityBlurVertexDeclaration->SetupVertexDeclaration();
	GetDevice()->SetStreamSource(0, DensityBlurVertexBuffer->StaticBuffer, 0, sizeof(Vertex1));
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, DensityBlurTriangleCount);
	
	/************************************************************************/
	/*         Pass 4: Setup Forward Pass for Rendering Clouds              */
	/************************************************************************/
	GetDevice()->SetVertexShader(VertexShaderDrawCloud->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShaderDrawCloud->GetPixelShader());

	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	struct
	{
		float	CameraPosition[3];
		float	Reserved0;

		float	SunLightDirection[3];
		float	Reserved1;


	}
	VertexShaderParametersCloudRender = 
	{
		{CameraPosition.x, CameraPosition.y, CameraPosition.z}, 0.0f,
		{SunLightDirection.x, SunLightDirection.y, SunLightDirection.z}, 0.0f
	};

	struct
	{
		float	CameraPosition[3];
		float	Reserved0;

		float	CloudDistance[2];
		float	Reserved1;
		float	Reserved2;

		float	SunLightDirection[3];
		float	Reserved3;

		float	SunLightColor[3];
		float	Reserved4;

		float	AmbientColor[3];
		float	Reserved5;

		float	Rayleigh[4];

		float	Mie[4];

		float	ESun[4];

		float	RayleighMieSum[4];

		float	Ambient[4];


	}
	PixelShaderParametersCloudRender = 
	{
		{CameraPosition.x, CameraPosition.y, CameraPosition.z}, 0.0f,
		{EarthRadius, CloudPlaneHeight * (2.0f * EarthRadius + CloudPlaneHeight)}, 0.0f, 0.0f,
		{SunLightDirection.x, SunLightDirection.y, SunLightDirection.z}, 0.0f,
		{SunLightColor.x, SunLightColor.y, SunLightColor.z}, 0.0f,
		{AmbientColor.x, AmbientColor.y, AmbientColor.z}, 0.0f,
		{LocalRayleigh. x,LocalRayleigh.y, LocalRayleigh.z, LocalRayleigh.w},
		{LocalMie.x, LocalMie.y, LocalMie.z, LocalMie.w},
		{LocalESun.x, LocalESun.y, LocalESun.z, LocalESun.w},
		{LocalMieRayleighSum.x, LocalMieRayleighSum.y, LocalMieRayleighSum.z, LocalMieRayleighSum.w},
		{LocalAmbient.x, LocalAmbient.y, LocalAmbient.z, LocalAmbient.w}
		
	};

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&InvWorldViewProjMat, 4);
	GetDevice()->SetVertexShaderConstantF(4, (const float*)&VertexShaderParametersCloudRender, sizeof(VertexShaderParametersCloudRender) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelShaderParametersCloudRender, sizeof(PixelShaderParametersCloudRender) / 16);
	
	// Restore render target
	GetDevice()->SetRenderTarget(0, RenderTarget);
	ZED3D9CommonTools::SetTexture(0, (ZETexture2D*)CloudDensityBuffer, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);
	ZED3D9CommonTools::SetTexture(1, (ZETexture2D*)CloudDensityBlurBuffer, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	RenderCommand->PrimitiveType		= ZE_ROPT_TRIANGLE_STRIPT;
	RenderCommand->PrimitiveCount		= RenderCloudTriangleCount;
	RenderCommand->IndexBuffer			= RenderCloudIndexBuffer;
	RenderCommand->VertexBuffer			= RenderCloudVertexBuffer;
	RenderCommand->VertexDeclaration	= RenderCloudVertexDeclaration;

	return true;
}

void ZED3D9CloudMaterial::UpdateMaterial()
{
	this->CreateShaders();
	this->CreateBuffers();
	this->CreateRenderTargets();
}

void ZED3D9CloudMaterial::Release()
{
	this->ReleaseShaders();
	this->ReleaseBuffers();
	this->ReleaseRenderTargets();
}
