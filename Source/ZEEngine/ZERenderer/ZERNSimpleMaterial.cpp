//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSimpleMaterial.cpp
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

#include "ZERNSimpleMaterial.h"

#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZERNStageManager.h"
#include "ZERNStage.h"
#include "ZEGraphics/ZEGRContext.h"

bool ZERNSimpleMaterial::InitializeSelf()
{
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	return true;
}

void ZERNSimpleMaterial::DeinitializeSelf()
{

}

ZERNSimpleMaterial::ZERNSimpleMaterial()
{
	Wireframe = false;
	TwoSided = false;
	VertexColorEnabled = true;
	TransparancyMode = ZE_MTM_NONE;
	Constants.TransparancyCullLimit = 0x80;
	Constants.MaterialColor = ZEVector4::One;
}

ZERNSimpleMaterial::~ZERNSimpleMaterial()
{
	
}

void ZERNSimpleMaterial::SetTwoSided(bool Enable)
{
	TwoSided = Enable;
}

bool ZERNSimpleMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZERNSimpleMaterial::SetWireframe(bool Enable)
{
	Wireframe = Enable;
}

bool ZERNSimpleMaterial::GetWireframe() const
{
	return Wireframe;
}

void ZERNSimpleMaterial::SetVertexColor(bool Enable)
{
	VertexColorEnabled = Enable;
}

bool ZERNSimpleMaterial::GetVertexColor()
{
	return VertexColorEnabled;
}

void ZERNSimpleMaterial::SetMaterialColor(const ZEVector4& Color)
{
	if (Constants.MaterialColor == Color)
		return;

	Constants.MaterialColor = Color;
}

const ZEVector4& ZERNSimpleMaterial::GetMaterialColor() const
{
	return Constants.MaterialColor;
}

void ZERNSimpleMaterial::SetTransparancyMode(ZEMaterialTransparancyMode Mode)
{
	if (TransparancyMode == Mode)
		return;

	TransparancyMode = Mode;
	DirtyRenderStates = true;
}

ZEMaterialTransparancyMode ZERNSimpleMaterial::GetTransparancyMode() const
{
	return TransparancyMode;
}

void ZERNSimpleMaterial::SetTransparancyCullLimit(ZEUInt Limit)
{
	if (Constants.TransparancyCullLimit == Limit)
		return;
	
	Constants.TransparancyCullLimit = Limit;
	DirtyConstants = true;
}

ZEUInt ZERNSimpleMaterial::GetTransparancyCullLimit() const
{
	return Constants.TransparancyCullLimit;
}

void ZERNSimpleMaterial::SetTexture(const ZERNSampler& Sampler)
{
	TextureSampler = Sampler;
	DirtyRenderStates;
}

const ZERNSampler& ZERNSimpleMaterial::GetTexture() const
{
	return TextureSampler;
}

bool ZERNSimpleMaterial::SetupMaterial(ZEGRContext* Context, ZERNStage* Stage)
{
	ZERNMaterial::SetupMaterial(Context, Stage);
	if (TextureSampler.GetTexture() != NULL)
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 2, TextureSampler.GetTexture());
		Context->SetSampler(ZEGR_ST_PIXEL, 2, TextureSampler.GetSamplerState());
	}
	else
	{
		Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	}
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, ZERN_CONSTANTS_MATERIAL, ConstantBuffer);

	return true;
}

void ZERNSimpleMaterial::Update()
{
	if (DirtyConstants)
		ConstantBuffer->SetData(&Constants);

	if (DirtyShaders || DirtyRenderStates)
	{
		ZEGRRenderState State = ZERNStageManager::GetInstance()->GetStage("GBuffer")->GetRenderState();

		ZEGRShaderCompileOptions Options;
		Options.SourceData = "#R:/ZEEngine/Shaders/D3D11/ZED11SimpleMaterial.hlsl";
		Options.Model = ZEGR_SM_4_0;

		Options.Type = ZEGR_ST_VERTEX;
		Options.EntryPoint = "VSMain";
		State.SetShader(Options.Type, ZEGRShader::Compile(Options));

		Options.Type = ZEGR_ST_PIXEL;
		Options.EntryPoint = "VSMain";
		State.SetShader(Options.Type, ZEGRShader::Compile(Options));
				
		RenderState = State.Compile();
		DirtyRenderStates = false;
	}
}

ZERNSimpleMaterial* ZERNSimpleMaterial::CreateInstance()
{
	return new ZERNSimpleMaterial();
}
