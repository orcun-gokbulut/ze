//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SeaMaterial.hlsl
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

#include "GBuffer.hlsl"
#include "LBuffer.hlsl"

sampler2D Texture				: register(s0);

float4x4 WorldViewPorjMatrix	: register(vs, c0);
float4x4 WorldViewMatrix 		: register(vs, c4);

float4 TextureOffset 			: register(ps, c0);
float4 MaterialDiffuseColor		: register(ps, c1);
float4 MaterialSpecularColor	: register(ps, c2);
float4 Factors					: register(ps, c3);
float  SpecularShineness		: register(ps, c1);

float4 TileScales				: register(ps, c8);
	
float4 PixelProperties			: register(ps, c4);

#define PixelSize				PixelProperties.xy
#define HalfPixelSize			PixelProperties.zw
#define DiffuseFactor			Factors.x
#define SpecularFactor			Factors.y
#define DiffuseSampleOffset		TextureOffset.xy
#define NormalSampleOffset		TextureOffset.zw

#define EntityScale				TileScales.xy
#define NormalTileScale			TileScales.zw
#define DiffuseTileScale		TileScales.zw

struct VSInput
{
	float4 Position			: POSITION0;
	float2 Textcoord		: TEXCOORD0;
	float3 Normal			: NORMAL0;
	float3 BiNormal			: BINORMAL0;
	float3 Tangent			: TANGENT0;
};

struct VSOutputGPass
{
	float4 Position			: POSITION0;
	float2 Texcoord			: TEXCOORD0;
	float3 Normal			: TEXCOORD1;
	float3 BiNormal			: TEXCOORD2;
	float3 Tangent			: TEXCOORD3;
	float3 ViewPosition		: TEXCOORD4;
};

VSOutputGPass VSMainGPass(VSInput Input)
{
	VSOutputGPass Output;

	Output.Texcoord = Input.Textcoord * EntityScale;
	Output.Position = mul(WorldViewPorjMatrix, Input.Position);
	Output.Normal 	= mul((float3x3)WorldViewMatrix, Input.Normal);
	Output.BiNormal = mul((float3x3)WorldViewMatrix, Input.BiNormal);
	Output.Tangent 	= mul((float3x3)WorldViewMatrix, Input.Tangent);
	Output.ViewPosition = mul(WorldViewMatrix, Input.Position).xyz;
	
	return Output;
}

struct PSInputGPass
{
	float2 Texcoord		: TEXCOORD0;
	float3 Normal		: TEXCOORD1;
	float3 BiNormal		: TEXCOORD2;
	float3 Tangent		: TEXCOORD3;
	float3 ViewPosition	: TEXCOORD4;
	float  Side			: VFACE;
};

ZEGBuffer PSMainGPass(PSInputGPass Input)
{
	ZEGBuffer GBuffer = (ZEGBuffer)0;

	float3 Normal = tex2D(Texture, Input.Texcoord * NormalTileScale + NormalSampleOffset).xyz * 2.0f - 1.0f;
	Normal = normalize(Normal.x * Input.Tangent + Normal.y * Input.BiNormal + Normal.z * Input.Normal);

	ZEGBuffer_SetViewNormal(GBuffer, Normal * Input.Side);	
	ZEGBuffer_SetViewPosition(GBuffer, Input.ViewPosition);
	ZEGBuffer_SetSpecularGlossiness(GBuffer, SpecularShineness);
	ZEGBuffer_SetSubSurfaceScatteringFactor(GBuffer, 0.0f);
	ZEGBuffer_SetScreenVelocity(GBuffer, float2(0.0f, 0.0f));
	
	return GBuffer;
}


///////////////////////////////////////////////////////////////////////////////////

struct VSOutputForwardPass 
{
	float4 Position			: POSITION0;
	float2 Texcoord			: TEXCOORD0;
		
};

VSOutputForwardPass VSMainForwardPass(VSInput Input)
{
	VSOutputForwardPass Output = (VSOutputForwardPass)0;
	
	Output.Texcoord = Input.Textcoord * EntityScale;
	Output.Position = mul(WorldViewPorjMatrix, Input.Position);
	
	return Output;
}

struct PSInputForwardPass
{
	float2 Texcoord				: TEXCOORD0;
	float3 ScreenPosition		: VPOS;
};

float4 PSMainForwardPass(PSInputForwardPass Input) : COLOR0
{
	float3 OutputColor = (float3)0.0f;
	
	float2 ScreenPosition = Input.ScreenPosition.xy * PixelSize + HalfPixelSize;
	
	float3 DiffuseColor = MaterialDiffuseColor.rgb * DiffuseFactor;
	DiffuseColor *= ZELBuffer_GetDiffuse(ScreenPosition);
	OutputColor += DiffuseColor;
		
	float3 SpecularColor = MaterialSpecularColor.rgb * SpecularFactor;
	SpecularColor *= ZELBuffer_GetSpecular(ScreenPosition);
	OutputColor += SpecularColor;

	OutputColor *= tex2D(Texture, Input.Texcoord * DiffuseTileScale + DiffuseSampleOffset).rgb;
	
	return float4(OutputColor, 1.0f);
}
