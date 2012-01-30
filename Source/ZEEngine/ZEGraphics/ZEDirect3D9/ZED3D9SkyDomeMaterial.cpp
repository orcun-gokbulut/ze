//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9SkyDomeMaterial.cpp
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
#include "ZED3D9SkyDomeMaterial.h"
#include "..\ZEFrameRenderer.h"
#include "..\ZERenderCommand.h"
#include "ZED3D9CommonTools.h"
#include "ZEMath\ZEMatrix.h"
#include "..\ZECamera.h"
#include "ZEMath\ZEAngle.h"



ZED3D9SkyDomeMaterial::ZED3D9SkyDomeMaterial()
{
	VertexShader = NULL;
	PixelShader = NULL;
	
}

ZED3D9SkyDomeMaterial::~ZED3D9SkyDomeMaterial()
{
	ReleaseShaders();
}

void ZED3D9SkyDomeMaterial::CreateShaders()
{
	if (VertexShader != NULL || PixelShader != NULL)
		ReleaseShaders();

	VertexShader = ZED3D9VertexShader::CreateShader("SkyDomeMaterial.hlsl", "vs_main", 0, "vs_3_0");
	PixelShader = ZED3D9PixelShader::CreateShader("SkyDomeMaterial.hlsl", "ps_main", 0, "ps_3_0");
}

void ZED3D9SkyDomeMaterial::ReleaseShaders()
{
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(PixelShader);
}

bool ZED3D9SkyDomeMaterial::SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	// Update material if its changed. (Recompile shader, etc.)
	((ZED3D9SkyDomeMaterial*)this)->UpdateMaterial();

	// Setup Shaders
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());

	// ZBuffer
	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// Culling
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Transparancy
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	ZEMatrix4x4	ScaleMatrix;
	ZEMatrix4x4	WorldViewProjMatrix;
	ZEMatrix4x4::CreateScale(ScaleMatrix, ZEVector3(OuterRadius, OuterRadius, OuterRadius));
	ZEMatrix4x4::Multiply(WorldViewProjMatrix, Renderer->GetCamera()->GetViewProjectionTransform(), RenderCommand->WorldMatrix);
	
	// Send data to pipeline
	// Final Projection matrix
	GetDevice()->SetVertexShaderConstantF(0, (const float*)&WorldViewProjMatrix, 4);
	// Scale matrix
	GetDevice()->SetVertexShaderConstantF(4, (const float*)&ScaleMatrix, 4);
	
	float Scale = 1.0f / (OuterRadius - InnerRadius);
	struct
	{
		float	InvWaveLenghtPow4[3];
		float	Reserved0;

		float	SunPosition[3];
		float	Reserved1;

		float	CameraPositionOffset[3];
		float	ScaleOverScaleDepth;

		float	CameraPosition[3];
		float	CameraHeight;

		float	OuterRadius;
		float	InnerRadius;
		float	MieConstSunInt;
		float	RayConstSunInt;

		float	Mie4PI;
		float	Ray4PI;
		float	Scale;
		float	ScaleDepth;
	
	}VertexShaderParameters = {
		{1.0f / ZEMath::Power(WaveLenght.x, 4.0f), 1.0f / ZEMath::Power(WaveLenght.y, 4.0f), 1.0f / ZEMath::Power(WaveLenght.z, 4.0f)}, 0.0f,
		{SunPosition.x, SunPosition.y, SunPosition.z}, 0.0f,
		{CameraPositionOffset.x, CameraPositionOffset.y, CameraPositionOffset.z}, Scale / RayleighScaleDepth,
		{CameraPosition.x, CameraPosition.y, CameraPosition.z}, ZEVector3::Length(CameraPosition + CameraPositionOffset),
		OuterRadius, InnerRadius, MieConstant * SunIntensity, RayleighConstant * SunIntensity,
		MieConstant * 4.0f * ZE_PI, RayleighConstant * 4.0f * ZE_PI, Scale = 1.0f / (OuterRadius - InnerRadius), RayleighScaleDepth
	};

	struct 
	{
		float	AmbientColor[4];

		float	G;
		float	GPow2;
		float	AmbienFactor;
		float	Reserved2;

		float	SunPosition[3];
		float	Reserved3;

	}PixelShaderParameters = {
		{AmbientColor.x, AmbientColor.y, AmbientColor.z, AmbientColor.w},
		G, G * G, AmbientFactor, 0.0f,
		{SunPosition.x, SunPosition.y, SunPosition.z}, 0.0f
	};
	
	GetDevice()->SetVertexShaderConstantF(8, (const float*)&VertexShaderParameters, sizeof(VertexShaderParameters) / 16);
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&PixelShaderParameters, sizeof(PixelShaderParameters) / 16);

	return true;
}

void ZED3D9SkyDomeMaterial::UpdateMaterial()
{
	if (VertexShader == NULL && PixelShader == NULL)
		CreateShaders();
}

void ZED3D9SkyDomeMaterial::Release()
{
	ReleaseShaders();
}
