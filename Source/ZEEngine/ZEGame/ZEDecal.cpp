//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDecal.cpp
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

#include "ZEDecal.h"
#include "ZERenderer/ZEDecalMaterial.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEDrawParameters.h"
#include "ZERenderer/ZERenderer.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZERenderer/ZESimpleMaterial.h"

ZEDecal::ZEDecal()
{
	DrawHelperBox = true;
	Priority = 0;
	
	IsStatic = true;
	LoopingEnabled = false;
	FadeInEnabled = false;
	FadeInTime = 0.0f;
	FadeInLeftTime = 0.0f;
	InfiniteLifeEnabled = false;
	LifeTimeEnabled = false;
	LifeTime = 0.0f;
	LifeLeftTime = 0.0f;
	FadeOutEnabled = false;
	FadeOutTime = 0.0f;
	FadeOutLeftTime = 0.0f;
	FadeAmount = 0.0f;
	
	DiffuseMapResource = NULL;
	NormalMapResource = NULL;
	SpecularMapResource = NULL;
	OpacityMapResource = NULL;
	EmissiveMapResource = NULL;
	
	DecalMaterial = NULL;
	DecalRenderCommand.SetZero();
	DecalVertexBuffer = NULL;
	DecalVertexDeclaration = NULL;
	
	HelperBoxMaterial = NULL;
	HelperBoxRenderCommand.SetZero();
	HelperBoxVertexBuffer = NULL;
	HelperBoxVertexDeclaration = NULL;

	DecalProjectionMatrix = ZEMatrix4x4::Identity;

	// Create Material
	if (DecalMaterial == NULL)
	{
		DecalMaterial = ZEDecalMaterial::CreateInstance();
		DecalMaterial->UpdateMaterial();
	}

	// Create Material
	if (HelperBoxMaterial == NULL)
	{
		HelperBoxMaterial = ZESimpleMaterial::CreateInstance();
		HelperBoxMaterial->UpdateMaterial();
	}
}

ZEDecal::~ZEDecal()
{
	this->Deinitialize();
}

void ZEDecal::CreateBuffers()
{

	/************************************************************************/
	/*						    Create Helper Cube							*/
	/************************************************************************/
	struct CubeVertex
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;

		} Pos;	// Position

		struct 
		{
			float u;
			float v;

		} Tex; // Texture Coordinates

		struct 
		{
			float r;
			float g;
			float b;
			float a;

		} Col; // Color
		
	};

	if (HelperBoxVertexDeclaration == NULL)
	{
		HelperBoxVertexDeclaration = ZEVertexLayout::CreateInstance();
		const ZEVertexElement Declaration[] = 
		{
			//	Semantic			Type		Index	Offset
			{ZE_VES_POSITION,	ZE_VET_FLOAT4,	0},
			{ZE_VES_TEXTCOORD,	ZE_VET_FLOAT2,	0},
			{ZE_VES_TEXTCOORD,	ZE_VET_FLOAT4,	1}
		};

		/*if (!HelperBoxVertexDeclaration->Create(Declaration, HelperBoxMaterial))
		{
			HelperBoxVertexDeclaration->Destroy();
			HelperBoxVertexDeclaration = NULL;
		}*/
	}

	if (HelperBoxVertexBuffer == NULL)
	{
		ZESize CubeVertexCount = 12 * 2; // 12 lines * 2 vertices
		HelperBoxVertexBuffer = ZEStaticVertexBuffer::CreateInstance();
		if (!HelperBoxVertexBuffer->Create(CubeVertexCount, sizeof(CubeVertex)))
		{
			HelperBoxVertexBuffer->Destroy();
			HelperBoxVertexBuffer = NULL;
		}

		CubeVertex* Data = NULL;
		Data = (CubeVertex*)HelperBoxVertexBuffer->Lock();

		// No need to set texture coordinates
		memset((void*)Data, 0, HelperBoxVertexBuffer->GetBufferSize());

		// Lines
		// Front
		Data[0].Pos.x=-0.5f; Data[0].Pos.y=+0.5f; Data[0].Pos.z=+0.5f; Data[0].Pos.w=1.0f;
		Data[1].Pos.x=-0.5f; Data[1].Pos.y=-0.5f; Data[1].Pos.z=+0.5f; Data[1].Pos.w=1.0f;
		Data[2].Pos.x=-0.5f; Data[2].Pos.y=-0.5f; Data[2].Pos.z=+0.5f; Data[2].Pos.w=1.0f;
		Data[3].Pos.x=+0.5f; Data[3].Pos.y=-0.5f; Data[3].Pos.z=+0.5f; Data[3].Pos.w=1.0f;
		Data[4].Pos.x=+0.5f; Data[4].Pos.y=-0.5f; Data[4].Pos.z=+0.5f; Data[4].Pos.w=1.0f;
		Data[5].Pos.x=+0.5f; Data[5].Pos.y=+0.5f; Data[5].Pos.z=+0.5f; Data[5].Pos.w=1.0f;
		Data[6].Pos.x=+0.5f; Data[6].Pos.y=+0.5f; Data[6].Pos.z=+0.5f; Data[6].Pos.w=1.0f;
		Data[7].Pos.x=-0.5f; Data[7].Pos.y=+0.5f; Data[7].Pos.z=+0.5f; Data[7].Pos.w=1.0f;

		// Back
		Data[8].Pos.x=-0.5f; Data[8].Pos.y=+0.5f; Data[8].Pos.z=-0.5f; Data[8].Pos.w=1.0f;
		Data[9].Pos.x=-0.5f; Data[9].Pos.y=-0.5f; Data[9].Pos.z=-0.5f; Data[9].Pos.w=1.0f;
		Data[10].Pos.x=-0.5f; Data[10].Pos.y=-0.5f; Data[10].Pos.z=-0.5f; Data[10].Pos.w=1.0f;
		Data[11].Pos.x=+0.5f; Data[11].Pos.y=-0.5f; Data[11].Pos.z=-0.5f; Data[11].Pos.w=1.0f;
		Data[12].Pos.x=+0.5f; Data[12].Pos.y=-0.5f; Data[12].Pos.z=-0.5f; Data[12].Pos.w=1.0f;
		Data[13].Pos.x=+0.5f; Data[13].Pos.y=+0.5f; Data[13].Pos.z=-0.5f; Data[13].Pos.w=1.0f;
		Data[14].Pos.x=+0.5f; Data[14].Pos.y=+0.5f; Data[14].Pos.z=-0.5f; Data[14].Pos.w=1.0f;
		Data[15].Pos.x=-0.5f; Data[15].Pos.y=+0.5f; Data[15].Pos.z=-0.5f; Data[15].Pos.w=1.0f;

		// Sides
		Data[16].Pos.x=-0.5f; Data[16].Pos.y=+0.5f; Data[16].Pos.z=-0.5f; Data[16].Pos.w=1.0f;
		Data[17].Pos.x=-0.5f; Data[17].Pos.y=+0.5f; Data[17].Pos.z=+0.5f; Data[17].Pos.w=1.0f;
		Data[18].Pos.x=+0.5f; Data[18].Pos.y=+0.5f; Data[18].Pos.z=-0.5f; Data[18].Pos.w=1.0f;
		Data[19].Pos.x=+0.5f; Data[19].Pos.y=+0.5f; Data[19].Pos.z=+0.5f; Data[19].Pos.w=1.0f;
		Data[20].Pos.x=+0.5f; Data[20].Pos.y=-0.5f; Data[20].Pos.z=-0.5f; Data[20].Pos.w=1.0f;
		Data[21].Pos.x=+0.5f; Data[21].Pos.y=-0.5f; Data[21].Pos.z=+0.5f; Data[21].Pos.w=1.0f;
		Data[22].Pos.x=-0.5f; Data[22].Pos.y=-0.5f; Data[22].Pos.z=-0.5f; Data[22].Pos.w=1.0f;
		Data[23].Pos.x=-0.5f; Data[23].Pos.y=-0.5f; Data[23].Pos.z=+0.5f; Data[23].Pos.w=1.0f;

		// Texture Coordinates
		Data[0].Tex.u=0.0f; Data[0].Tex.v=0.0f;
		Data[1].Tex.u=0.0f; Data[1].Tex.v=0.0f;
		Data[2].Tex.u=0.0f; Data[2].Tex.v=0.0f;
		Data[3].Tex.u=0.0f; Data[3].Tex.v=0.0f;
		Data[4].Tex.u=0.0f; Data[4].Tex.v=0.0f;
		Data[5].Tex.u=0.0f; Data[5].Tex.v=0.0f;
		Data[6].Tex.u=0.0f; Data[6].Tex.v=0.0f;
		Data[7].Tex.u=0.0f; Data[7].Tex.v=0.0f;
		Data[8].Tex.u=0.0f; Data[8].Tex.v=0.0f;
		Data[9].Tex.u=0.0f; Data[9].Tex.v=0.0f;
		Data[10].Tex.u=0.0f; Data[10].Tex.v=0.0f;
		Data[11].Tex.u=0.0f; Data[11].Tex.v=0.0f;
		Data[12].Tex.u=0.0f; Data[12].Tex.v=0.0f;
		Data[13].Tex.u=0.0f; Data[13].Tex.v=0.0f;
		Data[14].Tex.u=0.0f; Data[14].Tex.v=0.0f;
		Data[15].Tex.u=0.0f; Data[15].Tex.v=0.0f;
		Data[16].Tex.u=0.0f; Data[16].Tex.v=0.0f;
		Data[17].Tex.u=0.0f; Data[17].Tex.v=0.0f;
		Data[18].Tex.u=0.0f; Data[18].Tex.v=0.0f;
		Data[19].Tex.u=0.0f; Data[19].Tex.v=0.0f;
		Data[20].Tex.u=0.0f; Data[20].Tex.v=0.0f;
		Data[21].Tex.u=0.0f; Data[21].Tex.v=0.0f;
		Data[22].Tex.u=0.0f; Data[22].Tex.v=0.0f;
		Data[23].Tex.u=0.0f; Data[23].Tex.v=0.0f;

		// Color
		Data[0].Col.r=1.0f; Data[0].Col.g=0.0f; Data[0].Col.b=0.0f;
		Data[1].Col.r=1.0f; Data[1].Col.g=0.0f; Data[1].Col.b=0.0f;
		Data[2].Col.r=1.0f; Data[2].Col.g=0.0f; Data[2].Col.b=0.0f;
		Data[3].Col.r=1.0f; Data[3].Col.g=0.0f; Data[3].Col.b=0.0f;
		Data[4].Col.r=1.0f; Data[4].Col.g=0.0f; Data[4].Col.b=0.0f;
		Data[5].Col.r=1.0f; Data[5].Col.g=0.0f; Data[5].Col.b=0.0f;
		Data[6].Col.r=1.0f; Data[6].Col.g=0.0f; Data[6].Col.b=0.0f;
		Data[7].Col.r=1.0f; Data[7].Col.g=0.0f; Data[7].Col.b=0.0f;
		Data[8].Col.r=0.0f; Data[8].Col.g=1.0f; Data[8].Col.b=0.0f;
		Data[9].Col.r=0.0f; Data[9].Col.g=1.0f; Data[9].Col.b=0.0f;
		Data[10].Col.r=0.0f; Data[10].Col.g=1.0f; Data[10].Col.b=0.0f;
		Data[11].Col.r=0.0f; Data[11].Col.g=1.0f; Data[11].Col.b=0.0f;
		Data[12].Col.r=0.0f; Data[12].Col.g=1.0f; Data[12].Col.b=0.0f;
		Data[13].Col.r=0.0f; Data[13].Col.g=1.0f; Data[13].Col.b=0.0f;
		Data[14].Col.r=0.0f; Data[14].Col.g=1.0f; Data[14].Col.b=0.0f;
		Data[15].Col.r=0.0f; Data[15].Col.g=1.0f; Data[15].Col.b=0.0f;
		Data[16].Col.r=0.0f; Data[16].Col.g=1.0f; Data[16].Col.b=0.0f;
		Data[17].Col.r=1.0f; Data[17].Col.g=0.0f; Data[17].Col.b=0.0f;
		Data[18].Col.r=0.0f; Data[18].Col.g=1.0f; Data[18].Col.b=0.0f;
		Data[19].Col.r=1.0f; Data[19].Col.g=0.0f; Data[19].Col.b=0.0f;
		Data[20].Col.r=0.0f; Data[20].Col.g=1.0f; Data[20].Col.b=0.0f;
		Data[21].Col.r=1.0f; Data[21].Col.g=0.0f; Data[21].Col.b=0.0f;
		Data[22].Col.r=0.0f; Data[22].Col.g=1.0f; Data[22].Col.b=0.0f;
		Data[23].Col.r=1.0f; Data[23].Col.g=0.0f; Data[23].Col.b=0.0f;

		HelperBoxVertexBuffer->Unlock();
	}


	/************************************************************************/
	/*                      Create Projection Cube                          */
	/************************************************************************/

	struct CubeVertexProjection
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;

		} Pos;	// Position

		struct  
		{
			float x;
			float y;
			float z;

		}Norm;	// Normal

		struct  
		{
			float x;
			float y;
			float z;

		}Binorm;	// Binormal

		struct
		{
			float x;
			float y;
			float z;

		}Tan;	// Tangent
	};

	if (DecalVertexDeclaration == NULL)
	{
		DecalVertexDeclaration = ZEVertexLayout::CreateInstance();
		const ZEVertexElement Declaration[] = 
		{
			//	Semantic			Type		Index	Offset
			{ZE_VES_POSITION,	ZE_VET_FLOAT4,	0},
			{ZE_VES_NORMAL,		ZE_VET_FLOAT3,	0},
			{ZE_VES_BINORMAL,	ZE_VET_FLOAT3,	0},
			{ZE_VES_TANGENT,	ZE_VET_FLOAT3,	0}

		};
		/*
		if (!DecalVertexDeclaration->Create(Declaration, 4))
		{
			DecalVertexDeclaration->Destroy();
			DecalVertexDeclaration = NULL;
		}*/
	}

	if (DecalVertexBuffer == NULL)
	{
		ZESize CubeVertexCount = 6 * 2 * 3;
		DecalVertexBuffer = ZEStaticVertexBuffer::CreateInstance();
		if (!DecalVertexBuffer->Create(CubeVertexCount, sizeof(CubeVertexProjection)))
		{
			DecalVertexBuffer->Destroy();
			DecalVertexBuffer = NULL;
		}

		CubeVertexProjection* Data = NULL;
		Data = (CubeVertexProjection*)DecalVertexBuffer->Lock();


		// NOTE:	Faces looks inside
		// NOTE:	All faces have the same data with the face that is looking to -z (front face)
		//			This allows us to render the decal normals as rendering on a flat surface


		// Position
		// Left Face
		Data[0].Pos.x=-0.5f; Data[0].Pos.y=-0.5f; Data[0].Pos.z=+0.5f; Data[0].Pos.w=1.0f;
		Data[1].Pos.x=-0.5f; Data[1].Pos.y=-0.5f; Data[1].Pos.z=-0.5f; Data[1].Pos.w=1.0f;
		Data[2].Pos.x=-0.5f; Data[2].Pos.y=+0.5f; Data[2].Pos.z=-0.5f; Data[2].Pos.w=1.0f;
		Data[3].Pos.x=-0.5f; Data[3].Pos.y=+0.5f; Data[3].Pos.z=-0.5f; Data[3].Pos.w=1.0f;
		Data[4].Pos.x=-0.5f; Data[4].Pos.y=+0.5f; Data[4].Pos.z=+0.5f; Data[4].Pos.w=1.0f;
		Data[5].Pos.x=-0.5f; Data[5].Pos.y=-0.5f; Data[5].Pos.z=+0.5f; Data[5].Pos.w=1.0f;
		// Front Face
		Data[6].Pos.x=+0.5f; Data[6].Pos.y=-0.5f; Data[6].Pos.z=+0.5f; Data[6].Pos.w=1.0f;
		Data[7].Pos.x=-0.5f; Data[7].Pos.y=-0.5f; Data[7].Pos.z=+0.5f; Data[7].Pos.w=1.0f;
		Data[8].Pos.x=-0.5f; Data[8].Pos.y=+0.5f; Data[8].Pos.z=+0.5f; Data[8].Pos.w=1.0f;
		Data[9].Pos.x=-0.5f; Data[9].Pos.y=+0.5f; Data[9].Pos.z=+0.5f; Data[9].Pos.w=1.0f;
		Data[10].Pos.x=+0.5f; Data[10].Pos.y=+0.5f; Data[10].Pos.z=+0.5f; Data[10].Pos.w=1.0f;
		Data[11].Pos.x=+0.5f; Data[11].Pos.y=-0.5f; Data[11].Pos.z=+0.5f; Data[11].Pos.w=1.0f;
		// Right Face
		Data[12].Pos.x=+0.5f; Data[12].Pos.y=-0.5f; Data[12].Pos.z=-0.5f; Data[12].Pos.w=1.0f;
		Data[13].Pos.x=+0.5f; Data[13].Pos.y=-0.5f; Data[13].Pos.z=+0.5f; Data[13].Pos.w=1.0f;
		Data[14].Pos.x=+0.5f; Data[14].Pos.y=+0.5f; Data[14].Pos.z=+0.5f; Data[14].Pos.w=1.0f;
		Data[15].Pos.x=+0.5f; Data[15].Pos.y=+0.5f; Data[15].Pos.z=+0.5f; Data[15].Pos.w=1.0f;
		Data[16].Pos.x=+0.5f; Data[16].Pos.y=+0.5f; Data[16].Pos.z=-0.5f; Data[16].Pos.w=1.0f;
		Data[17].Pos.x=+0.5f; Data[17].Pos.y=-0.5f; Data[17].Pos.z=-0.5f; Data[17].Pos.w=1.0f;
		// Back Face
		Data[18].Pos.x=-0.5f; Data[18].Pos.y=-0.5f; Data[18].Pos.z=-0.5f; Data[18].Pos.w=1.0f;
		Data[19].Pos.x=+0.5f; Data[19].Pos.y=-0.5f; Data[19].Pos.z=-0.5f; Data[19].Pos.w=1.0f;
		Data[20].Pos.x=+0.5f; Data[20].Pos.y=+0.5f; Data[20].Pos.z=-0.5f; Data[20].Pos.w=1.0f;
		Data[21].Pos.x=+0.5f; Data[21].Pos.y=+0.5f; Data[21].Pos.z=-0.5f; Data[21].Pos.w=1.0f;
		Data[22].Pos.x=-0.5f; Data[22].Pos.y=+0.5f; Data[22].Pos.z=-0.5f; Data[22].Pos.w=1.0f;
		Data[23].Pos.x=-0.5f; Data[23].Pos.y=-0.5f; Data[23].Pos.z=-0.5f; Data[23].Pos.w=1.0f;
		// Upper Face
		Data[24].Pos.x=-0.5f; Data[24].Pos.y=+0.5f; Data[24].Pos.z=-0.5f; Data[24].Pos.w=1.0f;
		Data[25].Pos.x=+0.5f; Data[25].Pos.y=+0.5f; Data[25].Pos.z=-0.5f; Data[25].Pos.w=1.0f;
		Data[26].Pos.x=+0.5f; Data[26].Pos.y=+0.5f; Data[26].Pos.z=+0.5f; Data[26].Pos.w=1.0f;
		Data[27].Pos.x=+0.5f; Data[27].Pos.y=+0.5f; Data[27].Pos.z=+0.5f; Data[27].Pos.w=1.0f;
		Data[28].Pos.x=-0.5f; Data[28].Pos.y=+0.5f; Data[28].Pos.z=+0.5f; Data[28].Pos.w=1.0f;
		Data[29].Pos.x=-0.5f; Data[29].Pos.y=+0.5f; Data[29].Pos.z=-0.5f; Data[29].Pos.w=1.0f;
		// Lower Face
		Data[30].Pos.x=-0.5f; Data[30].Pos.y=-0.5f; Data[30].Pos.z=-0.5f; Data[30].Pos.w=1.0f;
		Data[31].Pos.x=-0.5f; Data[31].Pos.y=-0.5f; Data[31].Pos.z=+0.5f; Data[31].Pos.w=1.0f;
		Data[32].Pos.x=+0.5f; Data[32].Pos.y=-0.5f; Data[32].Pos.z=+0.5f; Data[32].Pos.w=1.0f;
		Data[33].Pos.x=+0.5f; Data[33].Pos.y=-0.5f; Data[33].Pos.z=+0.5f; Data[33].Pos.w=1.0f;
		Data[34].Pos.x=+0.5f; Data[34].Pos.y=-0.5f; Data[34].Pos.z=-0.5f; Data[34].Pos.w=1.0f;
		Data[35].Pos.x=-0.5f; Data[35].Pos.y=-0.5f; Data[35].Pos.z=-0.5f; Data[35].Pos.w=1.0f;
		
		// Normal test
		// Left Face
		Data[0].Norm.x=+0.0f; Data[0].Norm.y=+0.0f; Data[0].Norm.z=-1.0f;
		Data[1].Norm.x=+0.0f; Data[1].Norm.y=+0.0f; Data[1].Norm.z=-1.0f;
		Data[2].Norm.x=+0.0f; Data[2].Norm.y=+0.0f; Data[2].Norm.z=-1.0f;
		Data[3].Norm.x=+0.0f; Data[3].Norm.y=+0.0f; Data[3].Norm.z=-1.0f;
		Data[4].Norm.x=+0.0f; Data[4].Norm.y=+0.0f; Data[4].Norm.z=-1.0f;
		Data[5].Norm.x=+0.0f; Data[5].Norm.y=+0.0f; Data[5].Norm.z=-1.0f;
		// Front Face
		Data[6].Norm.x=+0.0f; Data[6].Norm.y=+0.0f; Data[6].Norm.z=-1.0f;
		Data[7].Norm.x=+0.0f; Data[7].Norm.y=+0.0f; Data[7].Norm.z=-1.0f;
		Data[8].Norm.x=+0.0f; Data[8].Norm.y=+0.0f; Data[8].Norm.z=-1.0f;
		Data[9].Norm.x=+0.0f; Data[9].Norm.y=+0.0f; Data[9].Norm.z=-1.0f;
		Data[10].Norm.x=+0.0f; Data[10].Norm.y=+0.0f; Data[10].Norm.z=-1.0f;
		Data[11].Norm.x=+0.0f; Data[11].Norm.y=+0.0f; Data[11].Norm.z=-1.0f;
		// Right Face
		Data[12].Norm.x=+0.0f; Data[12].Norm.y=+0.0f; Data[12].Norm.z=-1.0f;
		Data[13].Norm.x=+0.0f; Data[13].Norm.y=+0.0f; Data[13].Norm.z=-1.0f;
		Data[14].Norm.x=+0.0f; Data[14].Norm.y=+0.0f; Data[14].Norm.z=-1.0f;
		Data[15].Norm.x=+0.0f; Data[15].Norm.y=+0.0f; Data[15].Norm.z=-1.0f;
		Data[16].Norm.x=+0.0f; Data[16].Norm.y=+0.0f; Data[16].Norm.z=-1.0f;
		Data[17].Norm.x=+0.0f; Data[17].Norm.y=+0.0f; Data[17].Norm.z=-1.0f;
		// Back Face
		Data[18].Norm.x=+0.0f; Data[18].Norm.y=+0.0f; Data[18].Norm.z=-1.0f;
		Data[19].Norm.x=+0.0f; Data[19].Norm.y=+0.0f; Data[19].Norm.z=-1.0f;
		Data[20].Norm.x=+0.0f; Data[20].Norm.y=+0.0f; Data[20].Norm.z=-1.0f;
		Data[21].Norm.x=+0.0f; Data[21].Norm.y=+0.0f; Data[21].Norm.z=-1.0f;
		Data[22].Norm.x=+0.0f; Data[22].Norm.y=+0.0f; Data[22].Norm.z=-1.0f;
		Data[23].Norm.x=+0.0f; Data[23].Norm.y=+0.0f; Data[23].Norm.z=-1.0f;
		// Upper Face
		Data[24].Norm.x=+0.0f; Data[24].Norm.y=+0.0f; Data[24].Norm.z=-1.0f;
		Data[25].Norm.x=+0.0f; Data[25].Norm.y=+0.0f; Data[25].Norm.z=-1.0f;
		Data[26].Norm.x=+0.0f; Data[26].Norm.y=+0.0f; Data[26].Norm.z=-1.0f;
		Data[27].Norm.x=+0.0f; Data[27].Norm.y=+0.0f; Data[27].Norm.z=-1.0f;
		Data[28].Norm.x=+0.0f; Data[28].Norm.y=+0.0f; Data[28].Norm.z=-1.0f;
		Data[29].Norm.x=+0.0f; Data[29].Norm.y=+0.0f; Data[29].Norm.z=-1.0f;
		// Lower Face
		Data[30].Norm.x=+0.0f; Data[30].Norm.y=+0.0f; Data[30].Norm.z=-1.0f;
		Data[31].Norm.x=+0.0f; Data[31].Norm.y=+0.0f; Data[31].Norm.z=-1.0f;
		Data[32].Norm.x=+0.0f; Data[32].Norm.y=+0.0f; Data[32].Norm.z=-1.0f;
		Data[33].Norm.x=+0.0f; Data[33].Norm.y=+0.0f; Data[33].Norm.z=-1.0f;
		Data[34].Norm.x=+0.0f; Data[34].Norm.y=+0.0f; Data[34].Norm.z=-1.0f;
		Data[35].Norm.x=+0.0f; Data[35].Norm.y=+0.0f; Data[35].Norm.z=-1.0f;

		// Bi normal
		// Left Face
		Data[0].Binorm.x=+1.0f; Data[0].Binorm.y=+0.0f; Data[0].Binorm.z=+0.0f;
		Data[1].Binorm.x=+1.0f; Data[1].Binorm.y=+0.0f; Data[1].Binorm.z=+0.0f;
		Data[2].Binorm.x=+1.0f; Data[2].Binorm.y=+0.0f; Data[2].Binorm.z=+0.0f;
		Data[3].Binorm.x=+1.0f; Data[3].Binorm.y=+0.0f; Data[3].Binorm.z=+0.0f;
		Data[4].Binorm.x=+1.0f; Data[4].Binorm.y=+0.0f; Data[4].Binorm.z=+0.0f;
		Data[5].Binorm.x=+1.0f; Data[5].Binorm.y=+0.0f; Data[5].Binorm.z=+0.0f;
		// Front Face
		Data[6].Binorm.x=+1.0f; Data[6].Binorm.y=+0.0f; Data[6].Binorm.z=+0.0f;
		Data[7].Binorm.x=+1.0f; Data[7].Binorm.y=+0.0f; Data[7].Binorm.z=+0.0f;
		Data[8].Binorm.x=+1.0f; Data[8].Binorm.y=+0.0f; Data[8].Binorm.z=+0.0f;
		Data[9].Binorm.x=+1.0f; Data[9].Binorm.y=+0.0f; Data[9].Binorm.z=+0.0f;
		Data[10].Binorm.x=+1.0f; Data[10].Binorm.y=+0.0f; Data[10].Binorm.z=+0.0f;
		Data[11].Binorm.x=+1.0f; Data[11].Binorm.y=+0.0f; Data[11].Binorm.z=+0.0f;
		// Right Face
		Data[12].Binorm.x=+1.0f; Data[12].Binorm.y=+0.0f; Data[12].Binorm.z=+0.0f;
		Data[13].Binorm.x=+1.0f; Data[13].Binorm.y=+0.0f; Data[13].Binorm.z=+0.0f;
		Data[14].Binorm.x=+1.0f; Data[14].Binorm.y=+0.0f; Data[14].Binorm.z=+0.0f;
		Data[15].Binorm.x=+1.0f; Data[15].Binorm.y=+0.0f; Data[15].Binorm.z=+0.0f;
		Data[16].Binorm.x=+1.0f; Data[16].Binorm.y=+0.0f; Data[16].Binorm.z=+0.0f;
		Data[17].Binorm.x=+1.0f; Data[17].Binorm.y=+0.0f; Data[17].Binorm.z=+0.0f;
		// Back Face
		Data[18].Binorm.x=+1.0f; Data[18].Binorm.y=+0.0f; Data[18].Binorm.z=+0.0f;
		Data[19].Binorm.x=+1.0f; Data[19].Binorm.y=+0.0f; Data[19].Binorm.z=+0.0f;
		Data[20].Binorm.x=+1.0f; Data[20].Binorm.y=+0.0f; Data[20].Binorm.z=+0.0f;
		Data[21].Binorm.x=+1.0f; Data[21].Binorm.y=+0.0f; Data[21].Binorm.z=+0.0f;
		Data[22].Binorm.x=+1.0f; Data[22].Binorm.y=+0.0f; Data[22].Binorm.z=+0.0f;
		Data[23].Binorm.x=+1.0f; Data[23].Binorm.y=+0.0f; Data[23].Binorm.z=+0.0f;
		// Upper Face
		Data[24].Binorm.x=+1.0f; Data[24].Binorm.y=+0.0f; Data[24].Binorm.z=+0.0f;
		Data[25].Binorm.x=+1.0f; Data[25].Binorm.y=+0.0f; Data[25].Binorm.z=+0.0f;
		Data[26].Binorm.x=+1.0f; Data[26].Binorm.y=+0.0f; Data[26].Binorm.z=+0.0f;
		Data[27].Binorm.x=+1.0f; Data[27].Binorm.y=+0.0f; Data[27].Binorm.z=+0.0f;
		Data[28].Binorm.x=+1.0f; Data[28].Binorm.y=+0.0f; Data[28].Binorm.z=+0.0f;
		Data[29].Binorm.x=+1.0f; Data[29].Binorm.y=+0.0f; Data[29].Binorm.z=+0.0f;
		// Lower Face
		Data[30].Binorm.x=+1.0f; Data[30].Binorm.y=+0.0f; Data[30].Binorm.z=+0.0f;
		Data[31].Binorm.x=+1.0f; Data[31].Binorm.y=+0.0f; Data[31].Binorm.z=+0.0f;
		Data[32].Binorm.x=+1.0f; Data[32].Binorm.y=+0.0f; Data[32].Binorm.z=+0.0f;
		Data[33].Binorm.x=+1.0f; Data[33].Binorm.y=+0.0f; Data[33].Binorm.z=+0.0f;
		Data[34].Binorm.x=+1.0f; Data[34].Binorm.y=+0.0f; Data[34].Binorm.z=+0.0f;
		Data[35].Binorm.x=+1.0f; Data[35].Binorm.y=+0.0f; Data[35].Binorm.z=+0.0f;

		// Tangent test
		// Left Face
		Data[0].Tan.x=+0.0f; Data[0].Tan.y=-1.0f; Data[0].Tan.z=+0.0f;
		Data[1].Tan.x=+0.0f; Data[1].Tan.y=-1.0f; Data[1].Tan.z=+0.0f;
		Data[2].Tan.x=+0.0f; Data[2].Tan.y=-1.0f; Data[2].Tan.z=+0.0f;
		Data[3].Tan.x=+0.0f; Data[3].Tan.y=-1.0f; Data[3].Tan.z=+0.0f;
		Data[4].Tan.x=+0.0f; Data[4].Tan.y=-1.0f; Data[4].Tan.z=+0.0f;
		Data[5].Tan.x=+0.0f; Data[5].Tan.y=-1.0f; Data[5].Tan.z=+0.0f;
		// Front Face
		Data[6].Tan.x=+0.0f; Data[6].Tan.y=-1.0f; Data[6].Tan.z=+0.0f;
		Data[7].Tan.x=+0.0f; Data[7].Tan.y=-1.0f; Data[7].Tan.z=+0.0f;
		Data[8].Tan.x=+0.0f; Data[8].Tan.y=-1.0f; Data[8].Tan.z=+0.0f;
		Data[9].Tan.x=+0.0f; Data[9].Tan.y=-1.0f; Data[9].Tan.z=+0.0f;
		Data[10].Tan.x=+0.0f; Data[10].Tan.y=-1.0f; Data[10].Tan.z=+0.0f;
		Data[11].Tan.x=+0.0f; Data[11].Tan.y=-1.0f; Data[11].Tan.z=+0.0f;
		// Right Face
		Data[12].Tan.x=+0.0f; Data[12].Tan.y=-1.0f; Data[12].Tan.z=+0.0f;
		Data[13].Tan.x=+0.0f; Data[13].Tan.y=-1.0f; Data[13].Tan.z=+0.0f;
		Data[14].Tan.x=+0.0f; Data[14].Tan.y=-1.0f; Data[14].Tan.z=+0.0f;
		Data[15].Tan.x=+0.0f; Data[15].Tan.y=-1.0f; Data[15].Tan.z=+0.0f;
		Data[16].Tan.x=+0.0f; Data[16].Tan.y=-1.0f; Data[16].Tan.z=+0.0f;
		Data[17].Tan.x=+0.0f; Data[17].Tan.y=-1.0f; Data[17].Tan.z=+0.0f;
		// Back Face
		Data[18].Tan.x=+0.0f; Data[18].Tan.y=-1.0f; Data[18].Tan.z=+0.0f;
		Data[19].Tan.x=+0.0f; Data[19].Tan.y=-1.0f; Data[19].Tan.z=+0.0f;
		Data[20].Tan.x=+0.0f; Data[20].Tan.y=-1.0f; Data[20].Tan.z=+0.0f;
		Data[21].Tan.x=+0.0f; Data[21].Tan.y=-1.0f; Data[21].Tan.z=+0.0f;
		Data[22].Tan.x=+0.0f; Data[22].Tan.y=-1.0f; Data[22].Tan.z=+0.0f;
		Data[23].Tan.x=+0.0f; Data[23].Tan.y=-1.0f; Data[23].Tan.z=+0.0f;
		// Upper Face
		Data[24].Tan.x=+0.0f; Data[24].Tan.y=-1.0f; Data[24].Tan.z=+0.0f;
		Data[25].Tan.x=+0.0f; Data[25].Tan.y=-1.0f; Data[25].Tan.z=+0.0f;
		Data[26].Tan.x=+0.0f; Data[26].Tan.y=-1.0f; Data[26].Tan.z=+0.0f;
		Data[27].Tan.x=+0.0f; Data[27].Tan.y=-1.0f; Data[27].Tan.z=+0.0f;
		Data[28].Tan.x=+0.0f; Data[28].Tan.y=-1.0f; Data[28].Tan.z=+0.0f;
		Data[29].Tan.x=+0.0f; Data[29].Tan.y=-1.0f; Data[29].Tan.z=+0.0f;
		// Lower Face
		Data[30].Tan.x=+0.0f; Data[30].Tan.y=-1.0f; Data[30].Tan.z=+0.0f;
		Data[31].Tan.x=+0.0f; Data[31].Tan.y=-1.0f; Data[31].Tan.z=+0.0f;
		Data[32].Tan.x=+0.0f; Data[32].Tan.y=-1.0f; Data[32].Tan.z=+0.0f;
		Data[33].Tan.x=+0.0f; Data[33].Tan.y=-1.0f; Data[33].Tan.z=+0.0f;
		Data[34].Tan.x=+0.0f; Data[34].Tan.y=-1.0f; Data[34].Tan.z=+0.0f;
		Data[35].Tan.x=+0.0f; Data[35].Tan.y=-1.0f; Data[35].Tan.z=+0.0f;

		DecalVertexBuffer->Unlock();
	}
}

void ZEDecal::DestroyBuffers()
{
	if (HelperBoxVertexDeclaration != NULL)
	{
		HelperBoxVertexDeclaration->Destroy();
		HelperBoxVertexDeclaration = NULL;
	}
	
	if (HelperBoxVertexBuffer != NULL)
	{
		HelperBoxVertexBuffer->Destroy();
		HelperBoxVertexBuffer = NULL;
	}

	if (DecalVertexBuffer != NULL)
	{
		DecalVertexBuffer->Destroy();
		DecalVertexBuffer = NULL;
	}
	
	if (DecalVertexDeclaration != NULL)
	{
		DecalVertexDeclaration->Destroy();
		DecalVertexDeclaration = NULL;
	}
}

void ZEDecal::CalculateProjectionParameters()
{

}

void ZEDecal::SetDrawHelperBox(bool Value)
{
	DrawHelperBox = Value;
}

bool ZEDecal::GetDrawHelperBox() const
{
	return DrawHelperBox;
}

void ZEDecal::SetPriority(ZEInt Value)
{
	Priority = Value;
}

ZEInt ZEDecal::GetPriority() const
{
	return Priority;
}

void ZEDecal::SetIsStatic(bool Value)
{
	IsStatic = Value;
}

bool ZEDecal::GetIsStatic() const
{
	return IsStatic;
}

void ZEDecal::SetLoopingEnabled(bool Value)
{
	LoopingEnabled = Value;
}

bool ZEDecal::GetLoopingEnabled()
{
	return LoopingEnabled;
}

void ZEDecal::SetFadeInEnabled(bool Value)
{
	FadeInEnabled = Value;
}

bool ZEDecal::GetFadeInEnabled() const
{
	return FadeInEnabled;
}

void ZEDecal::SetFadeInTime(float Value)
{
	FadeInTime = Value;
	FadeInLeftTime = Value;
}

float ZEDecal::GetFadeInTime() const
{
	return FadeInTime;
}

void ZEDecal::SetFadeOutEnabled(bool Value)
{
	FadeOutEnabled = Value;
}

bool ZEDecal::GetFadeOutEnabled() const
{
	return FadeOutEnabled;
}

void ZEDecal::SetFadeOutTime(float Value)
{
	FadeOutTime = Value;
	FadeOutLeftTime = Value;
}

float ZEDecal::GetFadeOutTime() const
{
	return FadeOutTime;
}

void ZEDecal::SetLifeTimeEnabled(bool Value)
{
	LifeTimeEnabled = Value;
}

bool ZEDecal::GetLifeTimeEnabled() const
{
	return LifeTimeEnabled;
}

void ZEDecal::SetInfiniteLifeEnabled(bool Value)
{
	InfiniteLifeEnabled = Value;
}

bool ZEDecal::GetInfiniteLifeEnabled() const
{
	return InfiniteLifeEnabled;
}

void ZEDecal::SetLifeTime(float Value)
{
	LifeTime = Value;
	LifeLeftTime = Value;
}

float ZEDecal::GetLifeTime() const
{
	return LifeTime;
}

float ZEDecal::GetFadeAmount() const
{
	return FadeAmount;
}

void ZEDecal::SetDiffuseMapFile(const ZEString& FileName)
{
	// Load the texture
	if (DiffuseMapResource != NULL)
	{
		DiffuseMapResource->Release();
		DiffuseMapResource = NULL;
	}

	DiffuseMapResource = ZETexture2DResource::LoadResource(FileName, NULL);

	if (DiffuseMapResource != NULL)
		DecalMaterial->SetDiffuseMapEnabled(true);
	
}

const ZEString ZEDecal::GetDiffuseMapFile() const
{
	return (DiffuseMapResource == NULL) ? "" : DiffuseMapResource->GetFileName();
}

void ZEDecal::SetNormalMapFile(const ZEString& FileName)
{
	// Load the texture
	if (NormalMapResource != NULL)
	{
		NormalMapResource->Release();
		NormalMapResource = NULL;
	}

	NormalMapResource = ZETexture2DResource::LoadResource(FileName, NULL);

	if (NormalMapResource != NULL)
		DecalMaterial->SetNormalMapEnabled(true);
}

const ZEString ZEDecal::GetNormalMapFile() const
{
	return (NormalMapResource == NULL) ? "" : NormalMapResource->GetFileName();
}

void ZEDecal::SetSpecularMapFile(const ZEString& FileName)
{
	// Load the texture
	if (SpecularMapResource != NULL)
	{
		SpecularMapResource->Release();
		SpecularMapResource = NULL;
	}

	SpecularMapResource = ZETexture2DResource::LoadResource(FileName, NULL);

	if (SpecularMapResource != NULL)
		DecalMaterial->SetSpecularMapEnabled(true);
}

const ZEString ZEDecal::GetSpecularMapFile() const
{
	return (SpecularMapResource == NULL) ? "" : SpecularMapResource->GetFileName();
}

void ZEDecal::SetOpacityMapFile(const ZEString& FileName)
{
	// Load the texture
	if (OpacityMapResource != NULL)
	{
		OpacityMapResource->Release();
		OpacityMapResource = NULL;
	}

	OpacityMapResource = ZETexture2DResource::LoadResource(FileName, NULL);

	if (OpacityMapResource != NULL)
		DecalMaterial->SetOpacityMapEnabled(true);

}

const ZEString ZEDecal::GetOpacityMapFile() const
{
	return (OpacityMapResource == NULL) ? "" : OpacityMapResource->GetFileName();
}

void ZEDecal::SetEmissiveMapFile(const ZEString& FileName)
{
	// Load the texture
	if (EmissiveMapResource != NULL)
	{
		EmissiveMapResource->Release();
		EmissiveMapResource = NULL;
	}

	EmissiveMapResource = ZETexture2DResource::LoadResource(FileName, NULL);

	if (EmissiveMapResource != NULL)
		DecalMaterial->SetEmissiveMapEnabled(true);
}

const ZEString ZEDecal::GetEmissiveMapFile() const
{
	return (EmissiveMapResource == NULL) ? "" : EmissiveMapResource->GetFileName();
}

ZEDecalMaterial* ZEDecal::GetDecalMaterial() const
{
	return DecalMaterial;
}

ZEDrawFlags ZEDecal::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

bool ZEDecal::Initialize()
{	
	CreateBuffers();

	ZEEntity::Initialize();

	return true;
}

void ZEDecal::Deinitialize()
{
	if (DecalMaterial != NULL)
	{
		DecalMaterial->Destroy();
		DecalMaterial = NULL;
	}

	if (HelperBoxMaterial != NULL)
	{
		HelperBoxMaterial->Destroy();
		HelperBoxMaterial = NULL;
	}

	if (DiffuseMapResource != NULL)
	{
		DiffuseMapResource->Release();
		DiffuseMapResource = NULL;
	}

	if (NormalMapResource != NULL)
	{
		NormalMapResource->Release();
		NormalMapResource = NULL;
	}

	if (SpecularMapResource != NULL)
	{
		SpecularMapResource->Release();
		SpecularMapResource = NULL;
	}

	if (OpacityMapResource != NULL)
	{
		OpacityMapResource->Release();
		OpacityMapResource = NULL;
	}

	if (EmissiveMapResource != NULL)
	{
		EmissiveMapResource->Release();
		EmissiveMapResource = NULL;
	}

	DestroyBuffers();

	DecalRenderCommand.SetZero();
	HelperBoxRenderCommand.SetZero();

	ZEEntity::Deinitialize();
}

void ZEDecal::Draw(ZEDrawParameters* DrawParameters)
{
	ZEVector3 Scale = GetScale();
	ZEMatrix4x4 WorldTransform = GetWorldTransform();
	
	ZEVector3 ProjPoint;
	ZEVector4 ProjectionPoint;
	ZEMatrix4x4::Transform(ProjectionPoint, WorldTransform, ZEVector4(0.0f, 0.0f, -0.5f, 1.0f));
	ProjPoint = ZEVector3(ProjectionPoint.x, ProjectionPoint.y, ProjectionPoint.z);

	// Projection parameters
	DecalMaterial->FadeAmount = FadeAmount;
	DecalMaterial->ProjectionUp = GetUp();
	DecalMaterial->ProjectionDirection = GetFront();
	DecalMaterial->ProjectionPoint = ProjPoint;
	DecalMaterial->ProjectionWidth = Scale.x;
	DecalMaterial->ProjectionHeight = Scale.y;
	DecalMaterial->ProjectionDepth = Scale.z;
	DecalMaterial->DecalProjectionMatrix = DecalProjectionMatrix;
	
	// Projection maps
	DecalMaterial->DiffuseMap = DiffuseMapResource == NULL ? NULL : DiffuseMapResource->GetTexture();
	DecalMaterial->NormalMap = NormalMapResource == NULL ? NULL : NormalMapResource->GetTexture();
	DecalMaterial->SpecularMap = SpecularMapResource == NULL ? NULL : SpecularMapResource->GetTexture();
	DecalMaterial->OpacityMap = OpacityMapResource == NULL ? NULL : OpacityMapResource->GetTexture();
	DecalMaterial->EmissiveMap = EmissiveMapResource == NULL ? NULL : EmissiveMapResource->GetTexture();

	// TODO: Adjust Order, Priority
	// Fill decal render command
	DecalRenderCommand.Order = 1.3f;
	DecalRenderCommand.Priority = 2;
	DecalRenderCommand.Material = DecalMaterial;
	DecalRenderCommand.Pipeline = ZE_RORP_3D;
	DecalRenderCommand.PrimitiveCount = 12;
	DecalRenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE_LIST;
	DecalRenderCommand.Flags = 0;
	DecalRenderCommand.StartVertex = 0;
	DecalRenderCommand.VertexBuffer = DecalVertexBuffer;
	DecalRenderCommand.VertexDeclaration = DecalVertexDeclaration;
	DecalRenderCommand.IndexBuffer = NULL;
	DecalRenderCommand.WorldMatrix = WorldTransform;
	DrawParameters->Renderer->AddToRenderList(&DecalRenderCommand);

	if (DrawHelperBox)
	{
		// Not important since we do not use fixed material
		ZERenderCommandFlags Flags =	ZE_ROF_ENABLE_NO_Z_WRITE| 
										ZE_ROF_ENABLE_Z_CULLING | 
										ZE_ROF_ENABLE_WORLD_TRANSFORM | 
										ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM;

		// TODO: Adjust Order, Priority
		HelperBoxRenderCommand.Order = 1.4f;
		HelperBoxRenderCommand.Priority = 2;
		HelperBoxRenderCommand.Material = HelperBoxMaterial;
		HelperBoxRenderCommand.Pipeline = ZE_RORP_3D;
		HelperBoxRenderCommand.PrimitiveCount = 12;
		HelperBoxRenderCommand.PrimitiveType = ZE_ROPT_LINE_LIST;
		HelperBoxRenderCommand.Flags = Flags;
		HelperBoxRenderCommand.StartVertex = 0;
		HelperBoxRenderCommand.VertexBuffer = HelperBoxVertexBuffer;
		HelperBoxRenderCommand.VertexDeclaration = HelperBoxVertexDeclaration;
		HelperBoxRenderCommand.IndexBuffer = NULL;
		HelperBoxRenderCommand.WorldMatrix = WorldTransform;
		DrawParameters->Renderer->AddToRenderList(&HelperBoxRenderCommand);
	}
}

void ZEDecal::Tick(float Time)
{
	ZEVector3 Scale = GetScale();
	ZEMatrix4x4 WorldTransform = GetWorldTransform();

	ZEVector3 ProjPoint;
	ZEVector4 ProjectionPoint;
	ZEMatrix4x4::Transform(ProjectionPoint, WorldTransform, ZEVector4(0.0f, 0.0f, -0.5f, 1.0f));
	ProjPoint = ZEVector3(ProjectionPoint.x, ProjectionPoint.y, ProjectionPoint.z);

	// Calculate projection matrix
	ZEQuaternion Orientation;
	ZEQuaternion::CreateFromDirection(Orientation, GetFront(), GetUp());

	ZEMatrix4x4 DecalView;
	ZEMatrix4x4::CreateViewTransform(DecalView, ProjPoint, Orientation);

	ZEMatrix4x4 DecalProj;
	ZEMatrix4x4::CreateOrthographicProjection(DecalProj, Scale.x, Scale.y, 0.001f, Scale.z);

	ZEMatrix4x4 DecalTex(	0.5f,  0.0f, 0.0f, 0.5f,
							0.0f, -0.5f, 0.0f, 0.5f,
							0.0f,  0.0f, 1.0f, 0.0f,
							0.0f,  0.0f, 0.0f, 1.0f );

	// For transforming pixel world position to texture coordinate: world->view->clip->texture
	DecalProjectionMatrix = DecalTex * DecalProj * DecalView;


	// No fade in-out
	if (IsStatic)
	{
		FadeAmount = 0.0f;
		return;
	}

	if (FadeInEnabled && FadeInLeftTime > 0.0f)
	{
		FadeInLeftTime -= Time;
		FadeAmount = FadeInLeftTime / FadeInTime;

		FadeAmount = ZEMath::Saturate(FadeAmount);
	}
	else if (LifeTimeEnabled && LifeLeftTime > 0.0f)
	{	
		if (InfiniteLifeEnabled)
		{
			FadeAmount = 0.0f;
		}
		else
		{
			LifeLeftTime -= Time;
			FadeAmount = 0.0f;
		}
	}
	else if (FadeOutEnabled && FadeOutLeftTime > 0.0f)
	{
		FadeOutLeftTime -= Time;
		FadeAmount = 1.0f - (FadeOutLeftTime / FadeOutTime);

		FadeAmount = ZEMath::Saturate(FadeAmount);
	}
	else	// Loop is completed
	{
		// Reset timers
		if (LoopingEnabled)
		{
			LifeLeftTime = LifeTime;
			FadeInLeftTime = FadeInTime;
			FadeOutLeftTime = FadeOutTime;
		}
	}

}

ZEDecal* ZEDecal::CreateInstance()
{
	return new ZEDecal();
}
