//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - HBAOProcessor.hlsl
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

#ifndef __ZE_HBAO_PROCESSOR_HLSL__
#define __ZE_HBAO_PROCESSOR_HLSL__

#include	"GBuffer.hlsl"

sampler2D	RandomTexture		: register(s3);
sampler2D	SSAOTexture			: register(s4);

float2		PixelSize			: register(c0);
float		Radius				: register(c1);
float		RadiusInv			: register(c2);
float		RadiusSquare		: register(c3);
float		AngleBias			: register(c4);
float		AngleBiasTan		: register(c5);
float		Contrast			: register(c6);
float2		FocalLenght			: register(c7);
float2		FocalLenghtInv		: register(c8);
float2		Resolution			: register(c9);
float		Attenuation			: register(c10);
float2		RandomResolution	: register(c11);

float		BlurRadius			: register(c12);
float		BlurFalloff			: register(c13);
float		BlurSharpness		: register(c14);
float		BlurEdgeThreshold	: register(c15);


#define		M_PI	3.14159f

struct VSInput
{
	float3 Position		: POSITION0;
	float2 TexCoord		: TEXCOORD0;
};

struct VSOutput
{
	float4 Position		: POSITION0;
	float2 TexCoord		: TEXCOORD0;
	float2 TexCoordRand : TEXCOORD1;
};

struct PSInput
{
	float2 TexCoord		: TEXCOORD0;
	float2 TexCoordRand : TEXCOORD1;
	float4 ScreenPosition 	: VPOS;
};

struct PSOutput
{
	float4 Color		: COLOR0;
};

//--------------------------------------------------------------------------------
VSOutput VSMainCommon(VSInput Input)
{
	VSOutput Output = (VSOutput)0.0f;

	Output.Position = float4(sign(Input.Position).xyz, 1.0f);
	Output.TexCoord = Input.TexCoord + 0.5f * PixelSize;
	Output.TexCoordRand = Output.TexCoord * (Resolution / RandomResolution);

	return Output;
}

//----------------------------------------------------------------------------------
float tangent(float3 P, float3 S)
{
    return (P.z - S.z) / length(S.xy - P.xy);
}

//----------------------------------------------------------------------------------
float3 uv_to_eye(float2 uv, float eye_z)
{
    uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));
    return float3(uv * FocalLenghtInv * eye_z, eye_z);
}

//----------------------------------------------------------------------------------
float3 fetch_eye_pos(float2 uv)
{
    float z = ZEGBuffer_GetDepth(uv);
	if (z < 0.001f) discard;
	
    return uv_to_eye(uv, z);
}

//----------------------------------------------------------------------------------
float3 tangent_eye_pos(float2 uv, float4 tangentPlane)
{
    // view vector going through the surface point at uv
    float3 V = fetch_eye_pos(uv);
    float NdotV = dot(tangentPlane.xyz, V);

    // intersect with tangent plane except for silhouette edges
    if (NdotV < 0.0) V *= (tangentPlane.w / NdotV);
    return V;
}

float length2(float3 v) { return dot(v, v); }

//----------------------------------------------------------------------------------
float3 min_diff(float3 P, float3 Pr, float3 Pl)
{
    float3 V1 = Pr - P;
    float3 V2 = P - Pl;
    return (length2(V1) < length2(V2)) ? V1 : V2;
}

//----------------------------------------------------------------------------------
float falloff(float r)
{
    return 1.0f - Attenuation * r * r;
}

//----------------------------------------------------------------------------------
float2 snap_uv_offset(float2 uv)
{
    return round(uv * Resolution) * PixelSize;
}

float2 snap_uv_coord(float2 uv)
{
    //return (floor(uv * Resolution) + 0.5f) * PixelSize;
    return uv - (frac(uv * Resolution) - 0.5f) * PixelSize;
}

//----------------------------------------------------------------------------------
float tan_to_sin(float x)
{
    return x / sqrt(1.0f + x*x);
}

//----------------------------------------------------------------------------------
float3 tangent_vector(float2 deltaUV, float3 dPdu, float3 dPdv)
{
    return deltaUV.x * dPdu + deltaUV.y * dPdv;
}

//----------------------------------------------------------------------------------
float tangent(float3 T)
{
    return -T.z / length(T.xy);
}

//----------------------------------------------------------------------------------
float biased_tangent(float3 T)
{
    float phi = atan(tangent(T)) + AngleBias;
    return tan(min(phi, M_PI * 0.5));
}

#define		g_NumSteps	5
#define		g_NumDir	8

//----------------------------------------------------------------------------------
float AccumulatedHorizonOcclusion(float2 deltaUV, float2 uv0, float3 P, float numSteps, float randstep, float3 dPdu, float3 dPdv)
{
    // Randomize starting point within the first sample distance
    float2 uv = uv0 + snap_uv_offset( randstep * deltaUV );
    
    // Snap increments to pixels to avoid disparities between xy 
    // and z sample locations and sample along a line
    deltaUV = snap_uv_offset( deltaUV );

    // Compute tangent vector using the tangent plane
    float3 T = deltaUV.x * dPdu + deltaUV.y * dPdv;

    float tanH = biased_tangent(T);
    float sinH = tanH / sqrt(1.0f + tanH*tanH);

    float ao = 0;
	[unroll]
    for(int j = 1; j <= g_NumSteps; ++j)
	{
        uv += deltaUV;
        float3 S = fetch_eye_pos(uv);
        
        // Ignore any samples outside the radius of influence
        float d2  = length2(S - P);
        if (d2 < RadiusSquare)
		{
            float tanS = tangent(P, S);

            [branch]
            if(tanS > tanH)
			{
                // Accumulate AO between the horizon and the sample
                float sinS = tanS / sqrt(1.0f + tanS*tanS);
                float r = sqrt(d2) * RadiusInv;
                ao += falloff(r) * (sinS - sinH);
                
                // Update the current horizon angle
                tanH = tanS;
                sinH = sinS;
            }
        } 
    }

    return ao;
}

//----------------------------------------------------------------------------------
float2 rotate_direction(float2 Dir, float2 CosSin)
{
    return float2(Dir.x * CosSin.x - Dir.y * CosSin.y, Dir.x * CosSin.y + Dir.y * CosSin.x);
}

//----------------------------------------------------------------------------------
PSOutput PSMainAmbientOcclusion( PSInput Input )
{
	PSOutput Output = (PSOutput)0.0f;

	float2 Coord = Input.ScreenPosition.xy * PixelSize + PixelSize * 0.5f;

	float3 P = fetch_eye_pos(Coord);

	// Project the radius of influence Radius from eye space to texture space.
	// The scaling by 0.5 is to go from [-1,1] to [0,1].
	float2 step_size = 0.5f * Radius  * FocalLenght / P.z;
	step_size = step_size / (g_NumSteps + 1.0f);
	
	float3 N = ZEGBuffer_GetViewNormal(Coord);

	float4 tangentPlane = float4(N, dot(P, N));
	float3 Pr = tangent_eye_pos(Input.TexCoord + float2(PixelSize.x, 0.0f), tangentPlane);
	float3 Pl = tangent_eye_pos(Input.TexCoord + float2(-PixelSize.x, 0.0f), tangentPlane);
	float3 Pt = tangent_eye_pos(Input.TexCoord + float2(0.0f, PixelSize.y), tangentPlane);
	float3 Pb = tangent_eye_pos(Input.TexCoord + float2(0.0f, -PixelSize.y), tangentPlane);

	// Screen-aligned basis for the tangent plane
	float3 dPdu = min_diff(P, Pr, Pl);
	float3 dPdv = min_diff(P, Pt, Pb) * (Resolution.y * PixelSize.x);

	// (cos(alpha),sin(alpha),jitter)
	float3 rand = tex2D(RandomTexture, Input.TexCoordRand).xyz;
	
	//float d = 0.0f;
	float ao = 0.0f;
	float alpha = 2.0f * M_PI / g_NumDir;

	[unroll]
	for (int d = 0; d < g_NumDir; d++)
	{
		float angle = alpha * (float)d;
		float2 dir = float2(cos(angle), sin(angle));
		float2 deltaUV = rotate_direction(dir, rand.xy) * step_size.xy;
		ao += AccumulatedHorizonOcclusion(deltaUV, Coord, P, 0.0f, rand.z, dPdu, dPdv);
	}

	float Occlusion = saturate(ao / g_NumDir * Contrast);
	Output.Color = (1.0 - Occlusion).xxxx;

	return Output;
}

//-------------------------------------------------------------------------
float fetch_eye_z(float2 uv)
{
    float z = ZEGBuffer_GetDepth(uv);
    return z;
}

//-------------------------------------------------------------------------
float BlurFunction(float2 uv, float r, float center_c, float center_d, inout float w_total)
{
    float d = fetch_eye_z(uv);
	float c = tex2D(SSAOTexture, uv).r;

    float ddiff = d - center_d;
    float w = exp(-r * r * BlurFalloff - ddiff * ddiff * BlurSharpness);
    w_total += w;

    return w*c;
}

#define		BlurRadius		5.0f

//-------------------------------------------------------------------------
float4 PSMainBlurHorizontal( PSInput Input ): SV_TARGET
{
    float b = 0.0f;
    float w_total = 0.0f;
    float center_c = tex2D(SSAOTexture, Input.TexCoord).r;
    float center_d = fetch_eye_z(Input.TexCoord);
    
    for (float r = -BlurRadius; r <= BlurRadius; ++r)
    {
        float2 uv = Input.TexCoord + float2(r * PixelSize.x , 0.0f);
        b += BlurFunction(uv, r, center_c, center_d, w_total);
    }

    return max(0.0f, (b / w_total)).xxxx;
}

//-------------------------------------------------------------------------
float4 PSMainBlurVertical( PSInput Input ): SV_TARGET
{
    float b = 0.0f;
    float w_total = 0.0f;
    float center_c = tex2D(SSAOTexture, Input.TexCoord).r;
    float center_d = fetch_eye_z(Input.TexCoord);
    
    for (float r = -BlurRadius; r <= BlurRadius; ++r)
    {
        float2 uv = Input.TexCoord + float2(0.0f, r * PixelSize.y);
        b += BlurFunction(uv, r, center_c, center_d, w_total);
    }

    return max(0.0f, (b / w_total)).xxxx;
}

//-------------------------------------------------------------------------
float4 PSMainDownSampleDepth( PSInput Input ): SV_TARGET
{
	float Depth = ZEGBuffer_GetDepth(Input.TexCoord);
	float DepthBR = ZEGBuffer_GetDepth(Input.TexCoord + PixelSize);
	float DepthR = ZEGBuffer_GetDepth(Input.TexCoord + float2(PixelSize.x, 0.0f));
	float DepthB = ZEGBuffer_GetDepth(Input.TexCoord + float2(0.0f, PixelSize.y));
	
	return max(max(max(Depth, DepthBR), DepthR), DepthB).xxxx;
}

//-------------------------------------------------------------------------
float4 PSMainDownSampleNormal( PSInput Input ): SV_TARGET
{
	float3 Normal = ZEGBuffer_GetViewNormal(Input.TexCoord) * 0.5f + 0.5f;
	return float4(Normal, 1.0f);
}

#endif
