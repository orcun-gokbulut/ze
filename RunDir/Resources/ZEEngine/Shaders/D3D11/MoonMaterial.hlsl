//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - MoonMaterial.hlsl
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
/*					        Moon Render Pass		                    */
/************************************************************************/

//------------------------------------------------
// Samplers
//------------------------------------------------
sampler3D 	MoonTexture			: register(s0);

//------------------------------------------------
// VS Constants
//------------------------------------------------
float4x4	WorldViewProjMat	: register(vs, c0);

//------------------------------------------------
// PS Constants
//------------------------------------------------
float4		MoonParameters		: register(ps, c0);

#define		MoonAmbientColor		MoonParameters.rgb
#define		MoonLightIntensity		MoonParameters.a

//------------------------------------------------
// IO Structures
//------------------------------------------------
struct VS_INPUT
{
	float4 Position			: POSITION0;
	float3 TextureCoord		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position			: POSITION0;
	float3 TextureCoord		: TEXCOORD0;
};

struct PS_INPUT
{
	float3 TextureCoord		: TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 PixelColor		: COLOR0;
};


//------------------------------------------------
// Vertex Shader
//------------------------------------------------
VS_OUTPUT vs_main_render_moon(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0.0f;
	
	Output.TextureCoord = Input.TextureCoord;

	Output.Position		= mul(WorldViewProjMat, Input.Position.xyz);
	Output.Position.z	= Output.Position.w;
	
	return Output;
}

//------------------------------------------------
// Pixel Shader
//------------------------------------------------
PS_OUTPUT ps_main_render_moon(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	Output.PixelColor = MoonLightIntensity * float4(MoonAmbientColor.xyz, 1.0f) * tex3D(MoonTexture, Input.TextureCoord);
	// Output.PixelColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return Output;
}
