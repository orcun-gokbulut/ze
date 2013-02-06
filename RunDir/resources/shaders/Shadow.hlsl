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
/*
#define ShadowHardwareFiltering		1
#define ZE_SHF_NONE					0
#define ZE_SHF_NVIDIA				1
#define ZE_SHF_ATI					2


#define ShadowFiltering				1
#define ZE_SF_NONE					0
#define ZE_SF_PCF_2X2				1
#define ZE_SF_PCF_4X4				2
#define ZE_SF_POISSION_PCF			3
#define ZE_SF_RR_POISSION_PCF		4

#define ShadowMapping				1
#define ZE_SM_DISABLED				0
#define ZE_SM_ENABLED				1

float4 ShadowKernel2x2[2];
float4 ShadowKernel4x4[2];

float SampleShadowMap1x1(sampler2D ShadowMap, float4 Texcoord)
{
	#if ShadowHardwareFiltering == ZE_SHF_NVIDIA
		return tex2Dproj(ShadowMap, Texcoord).x; // nVidia - Hardware PCF
	#else //ShadowHardwareFiltering == ZE_SHF_NONE
		return tex2Dproj(ShadowMap, Texcoord).x > (Texcoord.z / Texcoord.w); // Standard
	#endif
}

float SampleShadowMap2x2(sampler2D ShadowMap, float4 Texcoord)
{
	#if ShadowHardwareFiltering == ZE_SHF_NVIDIA
		return tex2Dproj(ShadowMap, Texcoord).x;
	#elif ShadowHardwareFiltering == ZE_SHF_ATI
		float4 Comparasion = tex2Dproj(ShadowMap, Texcoord).rgba > (Texcoord.z / Texcoord.w).xxxx ? 1.0f : 0.0f;
		return dot(Comparasion, float4(0.25f, 0.25f, 0.25f, 0.25f));
	#else //ShadowHardwareFiltering == ZE_SHF_NONE
		float4 Values = float4(
			tex2Dproj(ShadowMap, float4(Texcoord.xy + ShadowKernel2x2[0].xy, Texcoord.zw)).x,
			tex2Dproj(ShadowMap, float4(Texcoord.xy + ShadowKernel2x2[0].wz, Texcoord.zw)).x,
			tex2Dproj(ShadowMap, float4(Texcoord.xy + ShadowKernel2x2[1].xy, Texcoord.zw)).x,
			tex2Dproj(ShadowMap, float4(Texcoord.xy + ShadowKernel2x2[1].wz, Texcoord.zw)).x);
		float Comparasion = Values > (Texcoord.z / Texcoord.w) ? 1.0f : 0.0f;
		return dot(Comparasion, float4(0.25f, 0.25f, 0.25f, 0.25f));
	#endif
}

float SampleShadowMap(sampler2D ShadowMap, float4 Texcoord)
{
	#if ShadowMapping == ZE_SM_ENABLED
		#if ShadowFiltering == ZE_SF_PCF_2X2
			return SampleShadowMap2x2(ShadowMap, Texcoord);
		#elif ShadowFiltering == ZE_SF_PCF_4X4
			float4 Values = float4(
				SampleShadowMap2x2(ShadowMap, float4(Texcoord.xy + ShadowKernel4x4[0].xy, Texcoord.zw)),
				SampleShadowMap2x2(ShadowMap, float4(Texcoord.xy + ShadowKernel4x4[0].zw, Texcoord.zw)),
				SampleShadowMap2x2(ShadowMap, float4(Texcoord.xy + ShadowKernel4x4[1].xy, Texcoord.zw)),
				SampleShadowMap2x2(ShadowMap, float4(Texcoord.xy + ShadowKernel4x4[1].zw, Texcoord.zw)));
			return dot(Values, float4(0.25, 0.25, 0.25, 0.25));
		#elif ShadowFiltering == ZE_SF_POISSION_PCF
			// Shadow Quality 4: 12 tap Possion Filtering	
		#elif ShadowFiltering == ZE_SF_RR_POISSION_PCF
			// Shadow Quality 5: 12 tap Randomly Rotated Possion Filtering
		#else //ShadowFiltering == ZE_SF_NONE
			return SampleShadowMap1x1(ShadowMap, Texcoord);
		#endif
	#else
		return 1.0f;
	#endif
}

float SampleParaboloidShadowMap(sampler2D FrontShadowMap, sampler2D BackShadowMap, float3 Position)
{
}

*/
