//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SSAAProcessor.hlsl
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

// Includes
#include "GBuffer.hlsl"

// Application Passed Parameters
float2 		PixelSize  : register(c0);

sampler2D 	ColorMap : register (s4);

// Vertex Shader
struct VS_INPUT
{
   float4 Position  : POSITION;
};

struct VS_OUTPUT 
{
   float4 Position    : POSITION;
   float2 Center      : TEXCOORD0;
   float2 Top         : TEXCOORD1;
   float2 Bottom      : TEXCOORD2;
   float2 Left        : TEXCOORD3;
   float2 Right       : TEXCOORD4;
   float2 TopLeft     : TEXCOORD5;
   float2 TopRight    : TEXCOORD6;
   float2 BottomLeft  : TEXCOORD7;
   float2 BottomRight : TEXCOORD8;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;
  
   Output.Position = float4(sign(Input.Position).xy, 0.0f, 1.0f);
   
   Output.Center.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
   Output.Center.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);
   
   Output.Top.x = Output.Center.x;
   Output.Top.y = Output.Center.y + PixelSize.y;
   
   Output.Bottom.x = Output.Center.x;
   Output.Bottom.y = Output.Center.y - PixelSize.y;
   
   Output.Left.x = Output.Center.x - PixelSize.x;
   Output.Left.y = Output.Center.y;
   
   Output.Right.x = Output.Center.x + PixelSize.x;
   Output.Right.y = Output.Center.y;
   
   Output.TopLeft.x = Output.Center.x - PixelSize.x;
   Output.TopLeft.y = Output.Center.y + PixelSize.y;
   
   Output.TopRight.x = Output.Center.x + PixelSize.x;
   Output.TopRight.y = Output.Center.y + PixelSize.y;
   
   Output.BottomLeft.x = Output.Center.x - PixelSize.x;
   Output.BottomLeft.y = Output.Center.y - PixelSize.y;
   
   Output.BottomRight.x = Output.Center.x + PixelSize.x;
   Output.BottomRight.y = Output.Center.y - PixelSize.y;
   
   return Output;
}

// Pixel Shader 
struct PS_INPUT
{
   float2 TexCoord    : TEXCOORD0;
   float2 Top         : TEXCOORD1;
   float2 Bottom      : TEXCOORD2;
   float2 Left        : TEXCOORD3;
   float2 Right       : TEXCOORD4;
   float2 TopLeft     : TEXCOORD5;
   float2 TopRight    : TEXCOORD6;
   float2 BottomLeft  : TEXCOORD7;
   float2 BottomRight : TEXCOORD8;
};

struct PS_OUTPUT
{
   float4 PixelColor  : COLOR0;
};

PS_OUTPUT ps_main( PS_INPUT Input )
{
   PS_OUTPUT Output;
   
   float3 NormVec = ZEGBuffer_GetViewNormal(Input.TexCoord);
   float4 NormDiscont;
   NormDiscont.x = dot(NormVec, ZEGBuffer_GetViewNormal(Input.Top));
   NormDiscont.y = dot(NormVec, ZEGBuffer_GetViewNormal(Input.Bottom));
   NormDiscont.z = dot(NormVec, ZEGBuffer_GetViewNormal(Input.Right));
   NormDiscont.w = dot(NormVec, ZEGBuffer_GetViewNormal(Input.Left));
   NormDiscont -= 0.6f;
   NormDiscont = (NormDiscont <= 0.0f) ? 1.0f : 0.0f;
   float NormalAmount = saturate(dot(NormDiscont, 0.4f));
 
   float Depth = ZEGBuffer_GetDepth(Input.TexCoord);
   float4 DepthDiscont;
   DepthDiscont.x = ZEGBuffer_GetDepth(Input.TopLeft);
   DepthDiscont.y = ZEGBuffer_GetDepth(Input.TopRight);
   DepthDiscont.z = ZEGBuffer_GetDepth(Input.Left);
   DepthDiscont.w = ZEGBuffer_GetDepth(Input.Top);
   float4 DepthDiscontDest;
   DepthDiscontDest.x = ZEGBuffer_GetDepth(Input.BottomRight);
   DepthDiscontDest.y = ZEGBuffer_GetDepth(Input.BottomLeft);
   DepthDiscontDest.z = ZEGBuffer_GetDepth(Input.Right);
   DepthDiscontDest.w = ZEGBuffer_GetDepth(Input.Bottom);
   DepthDiscont += DepthDiscontDest;
   DepthDiscont = abs(DepthDiscont / 2.0f - Depth) - 0.3f;
   DepthDiscont = DepthDiscont >= 0.0f ? 1.0f : 0.0f;
   float DepthAmount = saturate(dot(DepthDiscont, 0.1f));
   
   float Weight = max(NormalAmount, DepthAmount) * 0.8f;
   
   float2 Offset = Input.TexCoord * (1 - Weight);
   float4 Sample1 = tex2D(ColorMap, Offset + Input.TopRight * Weight);
   float4 Sample2 = tex2D(ColorMap, Offset + Input.BottomLeft * Weight);
   float4 Sample3 = tex2D(ColorMap, Offset + Input.BottomRight * Weight);
   float4 Sample4 = tex2D(ColorMap, Offset + Input.TopLeft * Weight);
   
   Output.PixelColor = (Sample1 + Sample2 + Sample3 + Sample4) * 0.25f;
   
   return Output;
}

