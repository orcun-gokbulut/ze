//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Shadow.hlsl
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

#define ZE_SM_FILTER_PCF_POISSON_5_TAP

#if defined(ZE_SM_FILTER_PCF_BOX_4_TAP)

	#define ZE_SM_FILTER_TAP_COUNT	4
	static const float2 FilterTaps[ZE_SM_FILTER_TAP_COUNT] =
	{
		float2(0.0f, 0.0f), // Length = 1.0f
		float2(1.0f, 0.0f), // Length = 1.0f
		float2(0.0f, 1.0f), // Length = 1.0f
		float2(1.0f, 1.0f)  // Length = 1.0f
	};
	
#elif defined (ZE_SM_FILTER_PCF_POISSON_4_TAP)

	#define ZE_SM_FILTER_TAP_COUNT	4
	static const float2 FilterTaps[ZE_SM_FILTER_TAP_COUNT] =
	{
		float2(-0.2323524f, -0.04344069f),	// Length = 0.2363f
		float2( 0.3576663f, -0.03434586f),	// Length = 0.3593f
		float2( 0.4080991f,  0.47325150f),	// Length = 0.6249f
		float2(-0.8702538f,  0.24977860f)	// Length = 0.9053f
	};
	
#elif defined (ZE_SM_FILTER_PCF_POISSON_5_TAP)

	#define ZE_SM_FILTER_TAP_COUNT	5
	static const float2 FilterTaps[ZE_SM_FILTER_TAP_COUNT] =
	{
		float2(-0.47990960f,  0.2474051f),	// Length = 0.5399f
		float2( 0.58031870f,  0.0986226f),	// Length = 0.5886f
		float2( 0.02687145f, -0.6101344f),	// Length = 0.6107f
		float2(-0.75832520f, -0.4483475f),	// Length = 0.8809f
		float2(-0.01803716f,  0.9019188f)	// Length = 0.9020f
	};
	
#elif defined (ZE_SM_FILTER_PCF_POISSON_6_TAP)

	#define ZE_SM_FILTER_TAP_COUNT	6
	static const float2 FilterTaps[ZE_SM_FILTER_TAP_COUNT] =
	{
		float2(-0.07402445f, -0.01598004f),	// Length = 0.0757f
		float2(-0.78336110f,  0.06715248f),	// Length = 0.7862f
		float2(-0.03588826f, -0.82994880f),	// Length = 0.8307f
		float2( 0.49020810f,  0.75344700f),	// Length = 0.8988f
		float2( 0.84350380f, -0.41540620f),	// Length = 0.9402f
		float2(-0.46744003f,  0.82284230f)	// Length = 0.9463f
	};
	
#elif defined (ZE_SM_FILTER_PCF_POISSON_8_TAP)
	
	#define ZE_SM_FILTER_TAP_COUNT	8
	static const float2 FilterTaps[ZE_SM_FILTER_TAP_COUNT] =
	{
		float2( 0.12557910f,  0.07068630f),	// Length = 0.1446f
		float2(-0.58956410f, -0.08173649f),	// Length = 0.5952f
		float2( 0.53800000f,  0.50858990f),	// Length = 0.7403f
		float2(-0.43435290f, -0.71343960f),	// Length = 0.8352f
		float2( 0.33492010f, -0.83595340f),	// Length = 0.9005f
		float2( 0.93885600f, -0.03617476f),	// Length = 0.9396f
		float2(-0.06119142f,  0.94668170f),	// Length = 0.9487f
		float2(-0.84269830f,  0.47026770f)	// Length = 0.9650f
	};
	
#elif defined (ZE_SM_FILTER_PCF_POISSON_12_TAP)
	
	#define ZE_SM_FILTER_TAP_COUNT	12
	static const float2 FilterTaps[ZE_SM_FILTER_TAP_COUNT] =
	{
		float2( 0.1445972f,  0.04697967f),	// Length = 0.1520f
		float2(-0.1850145f, -0.18422770f),	// Length = 0.2610f
		float2( 0.2643003f, -0.34810840f),	// Length = 0.4370f
		float2(-0.4160157f,  0.36924030f),	// Length = 0.5562f
		float2( 0.5774809f,  0.18828500f),	// Length = 0.6074f
		float2( 0.3495790f,  0.58869550f),	// Length = 0.6846f
		float2(-0.4678557f, -0.59755570f),	// Length = 0.7589f
		float2(-0.2250305f,  0.73181550f),	// Length = 0.7656f
		float2(-0.8205884f, -0.26081130f),	// Length = 0.8610f
		float2( 0.2225451f, -0.88668820f),	// Length = 0.9141f
		float2(-0.8042874f,  0.49058400f),	// Length = 0.9420f
		float2( 0.6084549f, -0.77020600f)	// Length = 0.9815f
	};
	
#else
	#define ZE_SM_FILTER_TAP_COUNT	1
	static const float2 FilterTaps[ZE_SM_FILTER_TAP_COUNT] =
	{
		float2(0.0f, 0.0f)	// Length = 0.0f
	};
#endif

#define ZE_SM_RANDOM_ROTATION

float SampleShadowMap(sampler2D ShadowMap, float2 ShadowMapTexCoord, float2 ShadowMapTexSize, float ComparisonDepth, sampler2D RotationMap, float2 RotationMapTexCoord, float FilterScale)
{
	// Create rotation matrix
	#if defined(ZE_SM_RANDOM_ROTATION)
		float2 SinCos = tex2Dlod(RotationMap, float4(RotationMapTexCoord, 0.0f, 0.0f)).rg * 2.0f - 1.0f;
		float2x2 RotationMat = float2x2(float2(SinCos.y, -SinCos.x), float2(SinCos.x, SinCos.y));
	#endif
	
	float PcfAverage = 0.0f;
	[unroll]
	for (int SampleN = 0; SampleN < ZE_SM_FILTER_TAP_COUNT; SampleN++)
	{
		// Rotate if specified
		float2 FinalTap = float2(0.0f, 0.0f);
		#if defined(ZE_SM_RANDOM_ROTATION)
			float2 Temp = FilterTaps[SampleN];
			FinalTap = mul(RotationMat, Temp);
		#else
			FinalTap = FilterTaps[SampleN];
		#endif
		
		float2 SampleOffset = FinalTap * ShadowMapTexSize * FilterScale;
		
		// Sample depth
		float DepthSample = tex2Dlod(ShadowMap, float4(ShadowMapTexCoord + SampleOffset, 0.0f, 0.0f)).r;
		
		// Pcf
		PcfAverage += ComparisonDepth > DepthSample ? 1.0f : 0.0f;
	}
	return PcfAverage * (1.0f / ZE_SM_FILTER_TAP_COUNT);
}
