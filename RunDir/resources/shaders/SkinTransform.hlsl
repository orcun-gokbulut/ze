//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SkinTransform.hlsl
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

void SkinTransform(inout VSInput Vertex)
{
	#ifdef ZE_SHADER_SKINTRANSFORM
		float4 Position = float4(mul(Vertex.Position, BoneMatrices[Vertex.BoneIndices[0]]).xyz * Vertex.BoneWeights[0], 1.0f);
		float3 Normal = mul(Vertex.Normal, BoneMatrices[Vertex.BoneIndices[0]]) * Vertex.BoneWeights[0];
		#ifdef ZE_SHADER_TANGENT_SPACE
			float3 Tangent = mul(Vertex.Tangent, BoneMatrices[Vertex.BoneIndices[0]]) * Vertex.BoneWeights[0];
			float3 Binormal = mul(Vertex.Binormal, BoneMatrices[Input.BoneIndices[0]]) * Vertex.BoneWeights[0];
		#endif
		
		for (int I = 1; I < 4; I++)
		{
			Position.xyz += mul(Vertex.Position, BoneMatrices[Vertex.BoneIndices[I]]).xyz * Vertex.BoneWeights[I];
			Normal += mul(Vertex.Normal, BoneMatrices[Vertex.BoneIndices[I]]) * Vertex.BoneWeights[I];
			#ifdef ZE_SHADER_TANGENT_SPACE
				Tangent += mul(Vertex.Tangent, BoneMatrices[Vertex.BoneIndices[I]]) * Vertex.BoneWeights[I];
				Binormal += mul(Vertex.Binormal, BoneMatrices[Input.BoneIndices[I]]) * Vertex.BoneWeights[I];
			#endif
		}
		
		Vertex.Position = Position;
		Vertex.Normal = Normal;
		#ifdef ZE_SHADER_TANGENT_SPACE 
			Vertex.Tangent = Tangent;
			Vertex.Binormal = Binormal;
		#endif
	#endif
}
