//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9PixelWorldPositionProcessor.cpp
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

#include "ZEMath/ZERay.h"
#include "ZED3D9Module.h"
#include "ZED3D9Shader.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9FrameRenderer.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9PixelWorldPositionProcessor.h"

#define COLOR_ALL	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA

void ZED3D9PixelWorldPositionProcessor::SetPixelCoordinate(const ZEVector2& Coord)
{
	if (PixelCoordinate != Coord)
	{
		NewInputSet = true;
		PixelCoordinate = Coord;
	}
}

ZEVector2 ZED3D9PixelWorldPositionProcessor::GetPixelCoordinate() const
{
	return PixelCoordinate;
}

const ZEVector4& ZED3D9PixelWorldPositionProcessor::GetWorldCoordinate() const
{
	return WorldCoordinate;
}

void ZED3D9PixelWorldPositionProcessor::Initialize()
{
	if (VertexDeclaration == NULL)
	{
		// Vertex declaration for screen aligned quad
		D3DVERTEXELEMENT9 Declaration[] = 
		{
			{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, 
			D3DDECL_END()
		};

		GetDevice()->CreateVertexDeclaration(Declaration, &VertexDeclaration);
	}
	
	if (VertexShader == NULL)
	{
		VertexShader = ZED3D9VertexShader::CreateShader("PixelWorldPositionProcessor.hlsl", "VSMain", 0, "vs_3_0");
	}
	if (PixelShader == NULL)
	{
		PixelShader = ZED3D9PixelShader::CreateShader("PixelWorldPositionProcessor.hlsl", "PSMain", 0, "ps_3_0");
	}

	if (ReadBackBuffer == NULL)
	{
		HRESULT Result = GetDevice()->CreateOffscreenPlainSurface(1, 1, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &ReadBackBuffer, NULL);
		if (FAILED(Result))
		{
			zeCriticalError("Cannot create offscreen surface");
			Deinitialize();
		}
	}

	if (ResultBuffer == NULL)
	{
		ResultBuffer = (ZED3D9Texture2D*)ZETexture2D::CreateInstance();
		ResultBuffer->Create(1, 1, 1, ZE_TPF_F32, true);
	}
}

void ZED3D9PixelWorldPositionProcessor::Deinitialize()
{
	ZED3D_RELEASE(PixelShader);
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(ResultBuffer);
	ZED3D_RELEASE(ReadBackBuffer);

	ZED3D_RELEASE(VertexDeclaration);
}

void ZED3D9PixelWorldPositionProcessor::SetRenderer(ZED3D9FrameRenderer* FrameRenderer)
{
	Renderer = FrameRenderer;
}

ZED3D9FrameRenderer* ZED3D9PixelWorldPositionProcessor::GetRenderer() const
{
	return Renderer;
}

void ZED3D9PixelWorldPositionProcessor::SetInputDepth(ZED3D9Texture2D* Texture)
{
	InputDepth = Texture;
}

ZED3D9Texture2D* ZED3D9PixelWorldPositionProcessor::GetInputDepth()
{
	return InputDepth;
}

void ZED3D9PixelWorldPositionProcessor::Process()
{
	if (!NewInputSet)
		return;

	if (Renderer == NULL)
		return;

	if (InputDepth == NULL)
		return;

	D3DPERF_BeginEvent(0, L"Pixel World Position Pass");

	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, COLOR_ALL);

	GetDevice()->SetVertexDeclaration(VertexDeclaration);
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());

	GetDevice()->SetRenderTarget(0, ResultBuffer->ViewPort.FrameBuffer);

	GetDevice()->SetTexture(0, InputDepth->Texture);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

	ZECamera* Camera = Renderer->GetCamera();

	ZEVector2 ScreenSize = ZEVector2((float)zeGraphics->GetScreenWidth(), (float)zeGraphics->GetScreenHeight());
	ZEVector2 PixelSize = ZEVector2::One / ScreenSize;
	ZEVector2 SampleCoord = PixelCoordinate * PixelSize + PixelSize * 0.5f;

	ZEVector4 ParameterSampleCoord = ZEVector4(SampleCoord, 0.0f, 0.0f);

	GetDevice()->SetPixelShaderConstantF(0, ParameterSampleCoord.M, 1);

	GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertices, (UINT)sizeof(ZEPixelToWorldPositionQuad));

	HRESULT Result = GetDevice()->GetRenderTargetData(ResultBuffer->ViewPort.FrameBuffer, ReadBackBuffer);
	if (FAILED(Result))
	{
		zeWarning("Reading render target data falied.");
		return;
	}

	D3DLOCKED_RECT Rect;
	Result = ReadBackBuffer->LockRect(&Rect, NULL, D3DLOCK_READONLY);
	if (FAILED(Result))
	{
		zeWarning("Reading render target data falied.");
		return;
	}
	
	float Depth = ((float*)Rect.pBits)[0];
	ReadBackBuffer->UnlockRect();

	ZERay Ray;
	Camera->GetScreenRay(Ray, (ZEUInt)(PixelCoordinate.x + 0.5f), (ZEUInt)(PixelCoordinate.y + 0.5f));

	WorldCoordinate = ZEVector4(Ray.v * Depth, 1.0f);

	NewInputSet = false;
	D3DPERF_EndEvent();
}

ZEPixelToWorldPositionQuad ZED3D9PixelWorldPositionProcessor::Vertices[4] =
{
	{{-1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}}
};

ZED3D9PixelWorldPositionProcessor::ZED3D9PixelWorldPositionProcessor()
{
	InputDepth = NULL;
	Renderer = NULL;
	
	PixelShader = NULL;
	VertexShader = NULL;
	ReadBackBuffer = NULL;
	ResultBuffer = NULL;

	VertexDeclaration = NULL;
	
	NewInputSet = false;
	PixelCoordinate = ZEVector2::One;
	WorldCoordinate = ZEVector4::One;
}

ZED3D9PixelWorldPositionProcessor::~ZED3D9PixelWorldPositionProcessor()
{
	Deinitialize();
}
