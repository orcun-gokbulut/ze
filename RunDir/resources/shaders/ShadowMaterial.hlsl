//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ShadowMaterial.hlsl
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

// Vertex Transformation
float4x4 LightWorldViewProj : register(vs, c0);

bool EnableSkin 			: register(vs, b0);
float4x4 BoneMatrices[58] 	: register(vs, c32);

struct VertexShaderInput
{
	float4 Position 			: POSITION0;

	//#if defined(ZE_SHADER_SKIN_TRANSFORM)
 		int4 BoneIndices        : BLENDINDICES0;
		float4 BoneWeights      : BLENDWEIGHT0;
	//#endif
};

struct VertexShaderOutput
{
	float4 Position 	: POSITION0;
	float4 Depth 		: TEXCOORD0;
};

struct PixelShaderInput
{
	float4 Depth 		: TEXCOORD0;
};

struct PixelShaderOutput
{
	float4 Depth		: COLOR0;
};

// Skinning for shadow map generation
// ----------------------------------------------------------------------
void ShadowSkinTransform(inout VertexShaderInput Vertex)
{
	float4x4 Matrix =  (float4x4)0.0f;
	
	for (int I = 0; I < 4; I++)
		Matrix += BoneMatrices[Vertex.BoneIndices[I]] * Vertex.BoneWeights[I];
	
	Vertex.Position = float4(mul(Matrix, Vertex.Position).xyz, 1.0f);
}

// Projective Light Shadow Map Generation
// ----------------------------------------------------------------------
VertexShaderOutput ProjectiveLightShadowVS(VertexShaderInput Input)
{
	VertexShaderOutput Output;
	
	Output.Position = mul(LightWorldViewProj, Input.Position);
	Output.Position.z *= Output.Position.z;
	Output.Depth = Output.Position.z;
	
	return Output;
}

PixelShaderOutput ProjectiveLightShadowPS(PixelShaderInput Input)
{
	PixelShaderOutput Output;

	Output.Depth = Input.Depth;
	return Output;
}

// Omni Light Shadow Map Generation
// ----------------------------------------------------------------------
VertexShaderOutput OmniProjectiveLightShadowVS(VertexShaderInput Input) 
{
	VertexShaderOutput Output;
	
	Output.Position = mul(LightWorldViewProj, Input.Position);
	Output.Position = Output.Position / Output.Position.w;
	
	float Distance = length(Output.Position);
	
	Output.Position /= Distance;
	
	Output.Position.xy /= (Output.Position.z + 1.0f);
	Output.Position.z = Distance ;/* / Range;*/
	Output.Position.w = 1.0f;
	
	Output.Depth = Output.Position.z;
	return Output;
}

PixelShaderOutput OmniProjectiveLightShadowPS(PixelShaderInput Input)
{
	PixelShaderOutput Output;

	Output.Depth = Input.Depth;
	return Output;
}

// Directional Light Shadow Map Generation
// ----------------------------------------------------------------------

VertexShaderOutput DirectionalLightShadowVS(VertexShaderInput Input)
{
	VertexShaderOutput Output;

	if (EnableSkin)
			ShadowSkinTransform(Input);
	
	Output.Position = mul(LightWorldViewProj, Input.Position);
	
	Output.Depth = Output.Position / Output.Position.w;

	return Output;
}

static const float DepthBias = 0.001f;
static const float SlopeScaledDepthBias = 0.5f;

PixelShaderOutput DirectionalLightShadowPS(PixelShaderInput Input)
{
	PixelShaderOutput Output;

	float DDX = clamp(ddx(Input.Depth), 0.0f, 0.01f);
	float DDY = clamp(ddy(Input.Depth), 0.0f, 0.01f);
	float SlopeFactor = max(DDX, DDY);
	
	float FinalBias = SlopeFactor * SlopeScaledDepthBias + DepthBias;
	Output.Depth = (Input.Depth.z /*+ FinalBias*/).xxxx;
	
	return Output;
}
