//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9MoonMaterial.cpp
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

#include "ZED3D9Shader.h"
#include "ZED3D9MoonMaterial.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9VertexBuffer.h"
#include "ZED3D9VertexDeclaration.h"
#include "ZEGraphics/ZEFrameRenderer.h"
#include "ZEGraphics/ZERenderCommand.h"
#include "ZEGraphics/ZEVertexDeclaration.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZETexture3D.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"


ZEMoonDynamicVertexBuffer::ZEMoonDynamicVertexBuffer()
{

}

ZEMoonDynamicVertexBuffer::~ZEMoonDynamicVertexBuffer()
{

}

ZESize ZEMoonDynamicVertexBuffer::GetBufferSize()
{
	return sizeof(MoonQuadVertex) * 4;
}

void* ZEMoonDynamicVertexBuffer::GetVertexBuffer()
{
	return &Vertices[0];
}

void ZED3D9MoonMaterial::CreateShaders()
{
	if (MoonVertexShader == NULL)
	{
		MoonVertexShader = ZED3D9VertexShader::CreateShader("MoonMaterial.hlsl", "vs_main_render_moon", 0, "vs_3_0");
	}
	
	if (MoonPixelShader == NULL)
	{
		MoonPixelShader = ZED3D9PixelShader::CreateShader("MoonMaterial.hlsl", "ps_main_render_moon", 0, "ps_3_0");
	}
}

void ZED3D9MoonMaterial::ReleaseShaders()
{
	ZED3D_RELEASE(MoonVertexShader);
	ZED3D_RELEASE(MoonPixelShader);
}

void ZED3D9MoonMaterial::CreateBuffers()
{
	VertexCount			= 4;
	PrimitiveCount		= 2;

	if (VertexDecleration == NULL)
	{
		// Vertex declaration for screen aligned quad
		VertexDecleration = (ZED3D9VertexDeclaration*)ZEVertexDeclaration::CreateInstance();
		static const ZEVertexElement Declaration[] = 
		{
			{ZE_VES_POSITION, ZE_VET_FLOAT4, 0}, 
			{ZE_VES_TEXTCOORD, ZE_VET_FLOAT3, 0}
		};

		if (!VertexDecleration->Create(Declaration, 2))
		{
			VertexDecleration->Destroy();
			VertexDecleration = NULL;
		}
	}

	if (VertexBuffer == NULL)
	{
		VertexBuffer = new ZEMoonDynamicVertexBuffer();
		
		if (VertexBuffer == NULL)
			zeCriticalError("Cannot Create Dynamic Vertex Buffer");

		MoonQuadVertex* VertexData = (MoonQuadVertex*)VertexBuffer->GetVertexBuffer();

		// Setup texture Coordinates that wont change at render time
		// TopRight
		VertexData[0].TexCoord[0] = 1.0f;
		VertexData[0].TexCoord[1] = 0.0f;
		
		
		// BottomRight
		VertexData[1].TexCoord[0] = 1.0f;
		VertexData[1].TexCoord[1] = 1.0f;
		VertexData[1].TexCoord[2] = 0.3f;
		
		// TopLeft
		VertexData[2].TexCoord[0] = 0.0f;
		VertexData[2].TexCoord[1] = 0.0f;
		VertexData[2].TexCoord[2] = 0.3f;
		
		// BottomLeft
		VertexData[3].TexCoord[0] = 0.0f;
		VertexData[3].TexCoord[1] = 1.0f;
		VertexData[3].TexCoord[2] = 0.3f;
	}
}

void ZED3D9MoonMaterial::ReleaseBuffers()
{
	if (VertexBuffer != NULL)
	{
		delete VertexBuffer;
		VertexBuffer = NULL;
	}
	if (VertexDecleration)
	{
		VertexDecleration->Destroy();
		VertexDecleration = NULL;
	}
}

bool ZED3D9MoonMaterial::SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	GetDevice()->SetVertexShader(MoonVertexShader->GetVertexShader());
	GetDevice()->SetPixelShader(MoonPixelShader->GetPixelShader());

	// Alpha Blending
	// GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	// GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	
	// Additive Color Blending
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);

	ZED3D9CommonTools::SetTexture(0, (ZETexture3D*)MoonTexture, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP);

	ZEMatrix4x4	WorldViewProjMatrix;
	ZEMatrix4x4::Multiply(WorldViewProjMatrix, Renderer->GetCamera()->GetViewProjectionTransform(), RenderCommand->WorldMatrix);

	ZEVector3 CameraUp = Renderer->GetCamera()->GetWorldUp();
	ZEVector3 CameraRight = Renderer->GetCamera()->GetWorldRight();
	ZEVector3 CameraPosition = Renderer->GetCamera()->GetWorldPosition();
	
	// Find Moon Up
	ZEQuaternion Rotation;
	ZEVector3 MoonQuadRotUp, MoonQuadRotRight;
	ZEVector3 MoonQuadUp, MoonQuadLookAt, MoonQuadRight;
	ZEQuaternion::CreateFromDirection(Rotation, MoonDirection);
	ZEQuaternion::ConvertToLookAndUp(MoonQuadLookAt, MoonQuadUp, Rotation);
	
	// ZEQuaternion gives opposite of up
	MoonQuadUp = -MoonQuadUp;

	// Rotate MoonQuadUp around MoonQuadLookAt
	ZEVector3 NegMoonDir(-MoonDirection);

	float ZxZ = MoonDirection.z * MoonDirection.z;
	float YxY = MoonDirection.y * MoonDirection.y;
	float XxX = MoonDirection.x * MoonDirection.x;

	float XY = MoonDirection.x * MoonDirection.y;
	float XZ = MoonDirection.x * MoonDirection.z;
	float YZ = MoonDirection.y * MoonDirection.z;

	ZEMatrix3x3 L(	 0.0f,				MoonDirection.z, -MoonDirection.y,
					-MoonDirection.z,	0.0f,			  MoonDirection.x,
					 MoonDirection.y,  -MoonDirection.x,  0.0f );

	ZEMatrix3x3 LL(	-ZxZ - YxY,  XY,		 XZ,
					 XY,		-ZxZ - XxX,  YZ,
					 XZ,		 YZ,		-YxY - XxX );

	MoonQuadRotUp = (ZEMatrix3x3::Identity + ZEAngle::Cos(MoonRotation) * L + ((1 - (ZEAngle::Sin(MoonRotation))) * LL)) * MoonQuadUp;
	ZEVector3::CrossProduct(MoonQuadRotRight, MoonQuadRotUp, MoonQuadLookAt);

	// Calculate Vertex Positions with the initial quad width = height = 1 * MoonScale
	ZEVector3 QuadCenter(-MoonDirection);
	ZEVector3 QuadWidthOver2(MoonQuadRotUp * MoonScale);
	ZEVector3 QuadHeightOver2(MoonQuadRotRight * MoonScale);

	ZEVector3 QuadTopRight		(QuadCenter +  QuadHeightOver2 + QuadWidthOver2);
	ZEVector3 QuadBottomRight	(QuadCenter -  QuadHeightOver2 + QuadWidthOver2);
	ZEVector3 QuadTopLeft		(QuadCenter +  QuadHeightOver2 - QuadWidthOver2);
	ZEVector3 QuadBottomLeft	(QuadCenter -  QuadHeightOver2 - QuadWidthOver2);


	MoonQuadVertex* VertexData = (MoonQuadVertex*)VertexBuffer->GetVertexBuffer();
	// Update vertex positions based on direction scale and rotation
	// TopRight
	VertexData[0].Position[0] = QuadTopRight.x;
	VertexData[0].Position[1] = QuadTopRight.y;
	VertexData[0].Position[2] = QuadTopRight.z;
	VertexData[0].Position[3] = 1.0f;

	// BottomRight
	VertexData[1].Position[0] = QuadBottomRight.x;
	VertexData[1].Position[1] = QuadBottomRight.y;
	VertexData[1].Position[2] = QuadBottomRight.z;
	VertexData[1].Position[3] = 1.0f;

	// TopLeft
	VertexData[2].Position[0] = QuadTopLeft.x;
	VertexData[2].Position[1] = QuadTopLeft.y;
	VertexData[2].Position[2] = QuadTopLeft.z;
	VertexData[2].Position[3] = 1.0f;

	// BottomLeft
	VertexData[3].Position[0] = QuadBottomLeft.x;
	VertexData[3].Position[1] = QuadBottomLeft.y;
	VertexData[3].Position[2] = QuadBottomLeft.z;
	VertexData[3].Position[3] = 1.0f;
	
	// TextureCoords
	VertexData[0].TexCoord[2] = MoonPhase;
	VertexData[1].TexCoord[2] = MoonPhase;
	VertexData[2].TexCoord[2] = MoonPhase;
	VertexData[3].TexCoord[2] = MoonPhase;

	struct
	{
		float MoonAmbientColor[3];
		float MoonLightIntensity;

	} PixelShaderParameters = {

		{MoonAmbientColor.x, MoonAmbientColor.y, MoonAmbientColor.z}, MoonAmbientFactor
	};

	GetDevice()->SetVertexShaderConstantF(0, (const float*)&WorldViewProjMatrix, 4);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelShaderParameters, sizeof(PixelShaderParameters) / 16);

	// ---------------------------------------

	RenderCommand->Pipeline				= ZE_RORP_3D;
	RenderCommand->PrimitiveCount		= 2;
	RenderCommand->PrimitiveType		= ZE_ROPT_TRIANGLE_STRIPT;
	RenderCommand->Flags				= ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	RenderCommand->VertexBufferOffset	= 0;
	RenderCommand->IndexBuffer			= NULL;
	RenderCommand->VertexBuffer			= VertexBuffer;
	RenderCommand->VertexDeclaration	= VertexDecleration;

	// ---------------------------------------

	return true;
}

void ZED3D9MoonMaterial::UpdateMaterial()
{
	this->CreateShaders();
	this->CreateBuffers();
}

void ZED3D9MoonMaterial::Release()
{
	this->ReleaseShaders();
}

ZED3D9MoonMaterial::ZED3D9MoonMaterial()
{
	MoonVertexShader	= NULL;
	MoonPixelShader		= NULL;

	VertexBuffer		= NULL;
	VertexDecleration	= NULL;

	VertexCount			= 0;
	PrimitiveCount		= 0;
}

ZED3D9MoonMaterial::~ZED3D9MoonMaterial()
{
	this->Release();
}
