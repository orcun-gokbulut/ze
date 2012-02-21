//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - CloudMaterial.hlsl
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


/************************************************************************/
/*					        Cloud Shadow Pass		                    */
/************************************************************************/

//------------------------------------------------
// Samplers
//------------------------------------------------
sampler2D 	CloudTexturePass1		: register(s0);

//------------------------------------------------
// VS Constants
//------------------------------------------------
float4x4	WorldViewProjMatPass1	: register(vs, c0);

float2		HeightParametersPass1	: register(vs, c4);
float3		CameraPositionPass1		: register(vs, c5);
float4		UVParametersPass1		: register(vs, c6);
float4		XZParametersPass1		: register(vs, c7);

//------------------------------------------------
// PS Constants
//------------------------------------------------
float		CloudCoverPass1			: register(ps, c0);

//------------------------------------------------
// IO Structures
//------------------------------------------------
struct VS_INPUT_PASS1
{
	float4 Position : POSITION0;
};

struct VS_OUTPUT_PASS1 
{
	float4 Position		: POSITION0;
	float2 TextureCoord	: TEXCOORD0;
};

struct PS_INPUT_PASS1
{
	float2 TextureCoord	: TEXCOORD0;
};

struct PS_OUTPUT_PASS1
{
	float4 PixelColor : COLOR0;
};

//------------------------------------------------
// Vertex Shader
//------------------------------------------------
VS_OUTPUT_PASS1 vs_main_cloud_shadow(VS_INPUT_PASS1 Input)
{
	VS_OUTPUT_PASS1 Output = (VS_OUTPUT_PASS1)0.0f;
	
	// Calculate world position
	float4 WorldPosition = (float4)0.0f;
	WorldPosition.xz = Input.Position.xy * XZParametersPass1.xy + XZParametersPass1.zw;

	// Height is propotional to square distance in horizontal direction.
	float2 Direction = CameraPositionPass1.xz - WorldPosition.xz;
	float SquareDistance = dot(Direction, Direction);
	WorldPosition.y = SquareDistance * HeightParametersPass1.x + HeightParametersPass1.y;
	WorldPosition.w = 1.0f;
	
	// Transform
	Output.Position = mul(WorldViewProjMatPass1, WorldPosition);
	Output.TextureCoord = Input.Position.zw * UVParametersPass1.xy + UVParametersPass1.zw;
	return Output;
}

//------------------------------------------------
// Pixel Shader
//------------------------------------------------
PS_OUTPUT_PASS1 ps_main_cloud_shadow(PS_INPUT_PASS1 Input)
{
	PS_OUTPUT_PASS1 Output = (PS_OUTPUT_PASS1)0.0f;

	float4 CloudSample = tex2D(CloudTexturePass1, Input.TextureCoord);

	// Blend 4 channel of the sample according to cloud cover
	float4 CloudDensity = abs(CloudCoverPass1 - float4(0.25f, 0.50f, 0.75f, 1.00f)) / 0.25f;
	CloudDensity = saturate(1.0f - CloudDensity);
	float FinalDensity = dot(CloudSample, CloudDensity);

	// 0: Shadowed, 1: Lit
	Output.PixelColor = (float4)(1.0f - FinalDensity);

	return Output;
}


/************************************************************************/
/*					        Cloud Density Pass		                    */
/************************************************************************/

//------------------------------------------------
// Samplers
//------------------------------------------------
sampler2D 	CloudTexturePass2		: register(s0);

//------------------------------------------------
// VS Constants
//------------------------------------------------
float4x4	WorldViewProjMatPass2	: register(vs, c0);

float2		HeightParametersPass2	: register(vs, c4);
float3		CameraPositionPass2		: register(vs, c5);
float4		UVParametersPass2		: register(vs, c6);
float4		XZParametersPass2		: register(vs, c7);

//------------------------------------------------
// PS Constants
//------------------------------------------------
float		CloudCoverPass2			: register(ps, c0);

//------------------------------------------------
// IO Structures
//------------------------------------------------
struct VS_INPUT_PASS2
{
	float4 Position : POSITION0;
};

struct VS_OUTPUT_PASS2 
{
	float4 Position		: POSITION0;
	float2 TextureCoord	: TEXCOORD0;
};

struct PS_INPUT_PASS2
{
	float2 TextureCoord	: TEXCOORD0;
};

struct PS_OUTPUT_PASS2
{
	float4 PixelColor : COLOR0;
};

//------------------------------------------------
// Vertex Shader
//------------------------------------------------
VS_OUTPUT_PASS2 vs_main_cloud_density(VS_INPUT_PASS2 Input)
{
	VS_OUTPUT_PASS2 Output = (VS_OUTPUT_PASS2)0.0f;

	// Calculate world position
	float4 WorldPosition = (float4)0.0f;
	WorldPosition.xz = Input.Position.xy * XZParametersPass2.xy + XZParametersPass2.zw;

	// Height is propotional to square distance in horizontal direction.
	float2 Direction = CameraPositionPass2.xz - WorldPosition.xz;
	float SquareDistance = dot(Direction, Direction);
	WorldPosition.y = SquareDistance * HeightParametersPass2.x + HeightParametersPass2.y;
	WorldPosition.w = 1.0f;
	
	// Transform
	Output.Position = mul(WorldViewProjMatPass2, WorldPosition);
	Output.TextureCoord = Input.Position.zw * UVParametersPass2.xy + UVParametersPass2.zw;
	return Output;
}

//------------------------------------------------
// Pixel Shader
//------------------------------------------------
PS_OUTPUT_PASS2 ps_main_cloud_density(PS_INPUT_PASS2 Input)
{
	PS_OUTPUT_PASS2 Output = (PS_OUTPUT_PASS2)0.0f;

	float4 CloudSample = tex2D(CloudTexturePass2, Input.TextureCoord);

	// Blend 4 channel of the sample according to cloud cover
	float4 CloudDensity = abs(CloudCoverPass2 - float4(0.25f, 0.50f, 0.75f, 1.00f)) / 0.25f;
	CloudDensity = saturate(1.0f - CloudDensity);

	Output.PixelColor = (float4)dot(CloudSample, CloudDensity);	
	return Output;
}

/************************************************************************/
/*					        Density Blur Pass		                    */
/************************************************************************/

//------------------------------------------------
// Samplers
//------------------------------------------------
sampler2D 	DensityTexturePass3		: register(s0);

//------------------------------------------------
// VS Constants
//------------------------------------------------
float4x4	InvWorldViewProjMatPass3: register(vs, c0);

float2		PixelSizePass3			: register(vs, c4);
float4		OffsetPass3				: register(vs, c5);

//------------------------------------------------
// PS Constants
//------------------------------------------------
float2		InvMaxLenghtPass3		: register(ps, c0);
float3		CameraPositionPass3		: register(ps, c1);
float3		ParametersPass3			: register(ps, c2);
float4		FallOffPass3			: register(ps, c3);

#define		FILTER_WIDTH			16

//------------------------------------------------
// IO Structures
//------------------------------------------------
struct VS_INPUT_PASS3
{
	float4 Position			: POSITION0;
	float2 TextureCoord		: TEXCOORD0;
};

struct VS_OUTPUT_PASS3 
{
	float4 Position			: POSITION0;
	float4 TextureCoord		: TEXCOORD0;
	float4 WorldPos			: TEXCOORD1;
};

struct PS_INPUT_PASS3
{
	float4 TextureCoord		: TEXCOORD0;
	float4 WorldPos			: TEXCOORD1;
	float2 ScreenPos		: VPOS;
};

struct PS_OUTPUT_PASS3
{
	float4 PixelColor : COLOR0;
};

//------------------------------------------------
// Vertex Shader
//------------------------------------------------
VS_OUTPUT_PASS3 vs_main_density_blur(VS_INPUT_PASS3 Input)
{
	VS_OUTPUT_PASS3 Output = (VS_OUTPUT_PASS3)0.0f;

	// Projection point
	Output.Position = float4(Input.Position.xy, 0.5f, 1.0f);

	// Transform clip space position back to world space
	Output.WorldPos = mul(InvWorldViewProjMatPass3, Input.Position);

	// TexCoord
	Output.TextureCoord.xy = Input.TextureCoord + PixelSizePass3;

	// Compute blur direction
	Output.TextureCoord.zw = Input.TextureCoord * OffsetPass3.xy + OffsetPass3.zw;

	return Output;
}

//------------------------------------------------
// Pixel Shader
//------------------------------------------------
PS_OUTPUT_PASS3 ps_main_density_blur(PS_INPUT_PASS3 Input)
{
	PS_OUTPUT_PASS3 Output = (PS_OUTPUT_PASS3)0.0f;

	// Compute ray direction
	float3 WorldPos = Input.WorldPos.xyz / Input.WorldPos.w;
	float3 Ray = normalize(WorldPos - CameraPositionPass3);

	// Compute the distance that light travels in atmosphere.
	float Sin = Ray.y;
	float RSin = ParametersPass3.x * Sin;
	float Distance = sqrt(RSin * RSin + ParametersPass3.y) - RSin;

	// Compute UV offset
	float2 UVOffset = Input.TextureCoord.zw / FILTER_WIDTH * (ParametersPass3.z / Distance);

	// Limit blur factor
	float2 Lenght = abs(UVOffset * InvMaxLenghtPass3);
	float Over = max(Lenght.x, Lenght.y);
	float Scale = (Over > 1.0f) ? 1.0f / Over : 1.0f;
	UVOffset.xy *= Scale;

	// Scale parameter of exponential weight.
	float4 Dist = dot(UVOffset.xy, UVOffset.xy);
	Dist *= FallOffPass3;

	// Blur
	float2 UV = Input.TextureCoord.xy;
	float4 Color = tex2D(DensityTexturePass3, UV);
	float4 WeightSum = 1.0f;
	for (int I = 1; I < FILTER_WIDTH; ++I)
	{
		float4 Weight = exp(Dist * I);
		WeightSum += Weight;

		float2 Move = UVOffset * I;
		float4 CloudDensity = tex2D(DensityTexturePass3, UV + Move);
		Color += Weight * CloudDensity;
	}

	Output.PixelColor = Color / WeightSum;
	return Output;
}


/************************************************************************/
/*					        Render Cloud Pass		                    */
/************************************************************************/

//------------------------------------------------
// Samplers
//------------------------------------------------
sampler2D 	DensityTexturePass4			: register(s0);
sampler2D 	DensityBlurTexturePass4		: register(s1);

//------------------------------------------------
// VS Constants
//------------------------------------------------
float4x4	InvWorldViewProjMatPass4: register(vs, c0);

float3		CameraPositionPass4		: register(vs, c4)
									: register(ps, c0);
float3		SunLightDirectionPass4	: register(vs, c5)
									: register(ps, c2);

//------------------------------------------------
// PS Constants
//------------------------------------------------
//float3	CameraPositionPass4		: register(ps, c0);
float2		CloudDistancePass4		: register(ps, c1);
//float3	SunLightDirectionPass4	: register(ps, c2);
float3		SunLightColorPass4		: register(ps, c3);
float3		AmbientColorPass4		: register(ps, c4);

// Scattering parameters
float4		Rayleigh				: register(ps, c5);
float4		Mie						: register(ps, c6);
float4		ESun					: register(ps, c7);
float4		RayMieSum				: register(ps, c8);
float4		Ambient					: register(ps, c9);


//------------------------------------------------
// IO Structures
//------------------------------------------------
struct VS_INPUT_PASS4
{
	float4 Position			: POSITION0;
	float2 TextureCoord		: TEXCOORD0;
};

struct VS_OUTPUT_PASS4 
{
	float4 Position			: POSITION0;
	float2 TextureCoord		: TEXCOORD0;
	float4 WorldPos			: TEXCOORD1;
};

struct PS_INPUT_PASS4
{
	float2 TextureCoord		: TEXCOORD0;
	float4 WorldPos			: TEXCOORD1;
	float2 ScreenPos		: VPOS;
};

struct PS_OUTPUT_PASS4
{
	float4 PixelColor : COLOR0;
};

//------------------------------------------------
// Vertex Shader
//------------------------------------------------
VS_OUTPUT_PASS4 vs_main_draw_cloud(VS_INPUT_PASS4 Input)
{
	VS_OUTPUT_PASS4 Output = (VS_OUTPUT_PASS4)0.0f;

	Output.Position = Input.Position;
	Output.WorldPos = mul(InvWorldViewProjMatPass4, Input.Position);
	Output.TextureCoord = Input.TextureCoord;

	return Output;
}

//------------------------------------------------
// Pixel Shader
//------------------------------------------------
float3 ApplyScattering(float3 Input, float3 Ray)
{
	// In scattering
	float VL = dot(SunLightDirectionPass4, -Ray);
	float G = Mie.w + Rayleigh.w * VL;
	G = rsqrt(G);
	G = G * G * G;
	float3 FinalMie = Mie.rgb * G;
	float3 FinalRayleigh = Rayleigh.rgb * (1.0f + VL * VL);
	float3 Inscattering = ESun.rgb * (FinalMie + FinalRayleigh) + Ambient.rgb;

	// Distance to cloud
	float Sin = Ray.y;
	float RSin = CloudDistancePass4.x * Sin;
	float Distance = sqrt(RSin * RSin + CloudDistancePass4.y) - RSin;

	float3 Ratio = exp(-RayMieSum.rgb * Distance);
	return lerp(Inscattering, Input, Ratio);
}


PS_OUTPUT_PASS4 ps_main_draw_cloud(PS_INPUT_PASS4 Input)
{
	PS_OUTPUT_PASS4 Output = (PS_OUTPUT_PASS4)0.0f;

	float4 PixelDensity = tex2D(DensityTexturePass4, Input.TextureCoord.xy);
	float4 PixelDensityBlur = 1.0f - tex2D(DensityBlurTexturePass4, Input.TextureCoord.xy);

	// Light cloud
	float3 CloudLight = AmbientColorPass4 + SunLightColorPass4 * PixelDensityBlur.r;

	//Ray direction
	float3 WorldPos = Input.WorldPos.xyz / Input.WorldPos.w;
	float3 Ray = WorldPos - CameraPositionPass4;
	float SquareDistance = dot(Ray, Ray);
	Ray = normalize(Ray);

	//Scatter
	Output.PixelColor.rgb = ApplyScattering(CloudLight, Ray);
	Output.PixelColor.rgb = saturate(Output.PixelColor.rgb);
	Output.PixelColor.a = PixelDensity.a;

	return Output;
}

