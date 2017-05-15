//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNDecodeNormals.hlsl
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

#ifndef __ZERN_DECODE_NORMALS_H__
#define __ZERN_DECODE_NORMALS_H__

float3 DecodeNormal(int2 NormalEncoded)
{
	float2 NormalNormalized = (float2)NormalEncoded  / 32767.0f;
	float NormalZSign = 2.0f * (NormalEncoded.x & 0x0001) - 1.0f;
	float NormalZ = sqrt(saturate(1.0f - dot(NormalNormalized, NormalNormalized))) * NormalZSign;
	return normalize(float3(NormalNormalized, NormalZ));
}

void DecodeTangentBinormal(in int2 TangentEncoded, in float3 Normal, out float3 Tangent, out float3 Binormal)
{
	float2 TangentNormalized = (float2)TangentEncoded  / 32767.0f;
	float TangentZSign = 2.0f * (TangentEncoded.x & 0x0001) - 1.0f;
	float TangentZ = sqrt(saturate(1.0f - dot(TangentNormalized, TangentNormalized))) * TangentZSign;
	Tangent = normalize(float3(TangentNormalized, TangentZ));

	float BinormalDirection = 2.0f * (TangentEncoded.y & 0x0001) - 1.0f;
	Binormal = normalize(cross(Tangent, Normal) * BinormalDirection); // Inverted UVs(Y axis) and Binormal (D3D)
}

#endif
