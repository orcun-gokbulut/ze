//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SkyDomeMaterial.hlsl
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


// Parameter Definitions
// 
// MieConstSunInt		= MieConstant * SunIntensity.
// RayConstSunInt		= RayleighConstant * SunIntensity.
// InvWaveLenghtPow4	= 1 / pow(WaveLenght, 4).
// CameraHeight			= Leght of camera position vector.
// Mie4PI				= MieConstant * 4 * PI.
// Ray4PI				= RayleighConstant * 4 * PI.
// Scale				= 1 / (OuterRadius - InnerRadius).
// ScaleDepth			= Rayleight scale depth.
// ScaleOverScaleDepth	= Scale / ScaleDepth.
// G					= Paramter to modify symmetry of scattering.
// GPow2				= G * G.

// Application parameters
// Vertex shader parameters
float4x4	WorldViewProjMatrix		: register(vs, c0);		// Projections matrix.
float4x4	ScaleMatrix				: register(vs, c4);		// Scaling matrix
float3		InvWaveLenghtPow4		: register(vs, c8);		// 1 / pow(WaveLenght, 4).
float3		SunDirection			: register(vs, c9)		// Sun position.
									: register(ps, c3);
float4		Parameters3				: register(vs, c10);	// xyz: CameraPositionOffset, w: ScaleOverScaleDepth
float4		Parameters4				: register(vs, c11);	// xyz: Camera position. w: CameraHeight

float4		Parameters0				: register(vs, c12);	// x: OuterRadius, y: InnerRadius, z: MieConstSunInt, w: RayConstSunInt
float4		Parameters1				: register(vs, c13);	// x: Mie4PI, y: Ray4PI, z: Scale, w: ScaleDepth

// Pixel shader parameters
float4		Parameters2				: register(ps, c2);		// x: G, y: GPow2, z: AmbienFactor

#define		CameraPositionOffset	Parameters3.xyz
#define		ScaleOverScaleDepth		Parameters3.w

#define		OuterRadius				Parameters0.x
#define		InnerRadius				Parameters0.y
#define		MieConstSunInt			Parameters0.z
#define		RayConstSunInt			Parameters0.w

#define		Mie4PI					Parameters1.x
#define		Ray4PI					Parameters1.y
#define		Scale					Parameters1.z
#define		ScaleDepth				Parameters1.w

#define		G						Parameters2.x
#define		GPow2					Parameters2.y

#define		CameraPosition			Parameters4.xyz
#define		CameraHeight			Parameters4.w

#define		SunHeight				-SunDirection.y

#define		SAMPLE_COUNT_INT		5
#define		SAMPLE_COUNT_FLOAT		5.0f


// Vertex Shader Input
struct VS_INPUT
{
	float4 Position			: POSITION0;	// Position
	float3 Normal			: NORMAL0;		// Normal
	float2 TexCoord0		: TEXCOORD0;	// Texture Coordinates
	float4 TexCoord1		: TEXCOORD1;	// Color
};

// Vertex Shader Output
struct VS_OUTPUT
{
	float4 Position			: POSITION0;
	float4 VertColorMie		: TEXCOORD0;
	float4 VertColorRay		: TEXCOORD1;
	float3 ScatteringDir	: TEXCOORD2;
	
};

// Pixel Shader Input
struct PS_INPUT
{
	float4 VertColorMie		: TEXCOORD0;
	float4 VertColorRay		: TEXCOORD1;
	float3 ScatteringDir	: TEXCOORD2;
};

// Pixel Shader Output
struct PS_OUTPUT
{
	float4 PixelColor		: COLOR0;
};

// Scale equation
float ScaleAngle(float Cosine)
{
   float X = 1.0f - Cosine;
   return ScaleDepth * exp(-0.00287f + X * (0.459f + X * (3.83f + X * (-6.80f + X * 5.25f))));
}

VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0.0f;
	float3 VertPosition = mul(ScaleMatrix, Input.Position).xyz;
	
	float3 NewCameraPosition = CameraPosition + CameraPositionOffset;
	
	// Calculate the ray from camera to vertex and its lenght
	float3 Ray = VertPosition - NewCameraPosition;
	float Far = length(Ray);
	Ray /= Far;

	// Calculate rays starting position and then scattering offset
	float3 RayStart = NewCameraPosition;
	float Height = length(RayStart);
	float Depth = exp(ScaleOverScaleDepth * (InnerRadius - CameraHeight));
	float StartAngle = dot(Ray, RayStart) / Height;
	const float StartOffset = Depth * ScaleAngle(StartAngle);

	// Initialize the scattering loop variables
	float SampleLength = Far / SAMPLE_COUNT_FLOAT;
	float ScaledLength = SampleLength * Scale;
	float3 SampleRay = Ray * SampleLength;
	float3 SamplePoint = RayStart + SampleRay * 0.5f;

	float3 Constant = (InvWaveLenghtPow4 * Ray4PI + Mie4PI);
	
	// Now loop through the sample rays
	float3 FrontColor = (float3)0.0f;
	
	int I = 0;
	[unroll(SAMPLE_COUNT_INT)]
	while(I < SAMPLE_COUNT_INT)
	{
		float SamplePointHeight = length(SamplePoint);
		float SamplePointDepth = exp(ScaleOverScaleDepth * (InnerRadius - SamplePointHeight));
		
		// Limit Smaple point Depth not to get extreme color values where SamplePointDepth < 0!!
		SamplePointDepth = saturate(SamplePointDepth);
		
		float SamplePointLightAngle = dot(-SunDirection, SamplePoint) / SamplePointHeight;
		float SamplePointCameraAngle = dot(Ray, SamplePoint) / SamplePointHeight;
	
		float ScaledLightAngle = ScaleAngle(SamplePointLightAngle);
		float ScaledCameraAngle = ScaleAngle(SamplePointCameraAngle);

		float Scatter = (StartOffset + SamplePointDepth * (ScaledLightAngle- ScaledCameraAngle));
		float3 Attenuate = exp(-Scatter.xxxx * Constant);
		FrontColor += Attenuate * (SamplePointDepth * ScaledLength);
		SamplePoint += SampleRay;
		I++;
	}

	Output.Position	= mul(WorldViewProjMatrix, (VertPosition - CameraPositionOffset));
	Output.Position.z = Output.Position.w;
	
	Output.ScatteringDir = NewCameraPosition - VertPosition;
	Output.VertColorRay.xyz	= FrontColor * (InvWaveLenghtPow4 * RayConstSunInt);
	Output.VertColorMie.xyz	= FrontColor * MieConstSunInt;

	return Output;
}

// Mie phase function
float GetMiePhase(float Cosine, float CosinePow2, float FormalG, float FormalGPow2)
{
   return 1.5f * ((1.0f - FormalGPow2) / (2.0f + FormalGPow2)) * (1.0f + CosinePow2) / pow(1.0f + FormalGPow2 - 2.0f * FormalG * Cosine, 1.5f);
}

// Rayleight phase function
float GetRayleighPhase(float CosinePow2)
{
   return 0.75f + 0.75f * CosinePow2;
}

PS_OUTPUT ps_main(PS_INPUT Input)
{
	PS_OUTPUT Output;
	
	float Cosine = dot(-SunDirection, Input.ScatteringDir) / length(Input.ScatteringDir);
	float CosinePow2 = Cosine * Cosine;

	Output.PixelColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Output.PixelColor.rgb += GetRayleighPhase(CosinePow2) * Input.VertColorRay.rgb;
	Output.PixelColor.rgb += GetMiePhase(Cosine, CosinePow2, G, GPow2) * Input.VertColorMie.rgb;

	// try 11x/(11x / 1.1 +1) for transparency calculation
	Output.PixelColor.a = saturate((log10(Output.PixelColor.b * 2.0f + 0.025f) + 1.6f));

	return Output;
}
