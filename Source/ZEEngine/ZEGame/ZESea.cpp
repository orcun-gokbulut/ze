//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESea.cpp
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

#include "ZESea.h"
//#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZEGame/ZEScene.h"
//#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZERenderer/ZERNRenderer.h"
//#include "ZEEntityProvider.h"
#include "ZETexture/ZETexture2DResource.h"
//#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEMath/ZEMath.h"

ZESea::ZESea()
{
	/*Material = ZESeaMaterial::CreateInstance();
	SetScale(ZEVector3::One * 200);
	
	ZETextureOptions Options;
	Options.CompressionType = ZE_TCT_NONE;
	Options.CompressionQuality = ZE_TCQ_HIGH;
	Options.DownSample = ZE_TDS_NONE;
	Options.FileCaching = ZE_TFC_ENABLED;
	Options.MipMapping = ZE_TMM_ENABLED;
	Options.MaximumMipmapLevel = 14;

	SetDiffuseTexture(ZETexture2DResource::LoadSharedResource("#R:/ZEEngine/Sea/SeaDiffuse.jpg", &Options)->GetTexture2D());
	SetNormalTexture(ZETexture2DResource::LoadSharedResource("#R:/ZEEngine/Sea/SeaNormal.jpg", &Options)->GetTexture2D());

	DiffuseVelocity = ZEVector2::One;
	NormalVelocity	= -ZEVector2::One;

	DiffuseOffset = ZEVector2::Zero;
	NormalOffset = ZEVector2::Zero;

	DiffuseTextureTile = ZEVector2::One / 10.0f;
	NormalTextureTile = ZEVector2::One / 10.0f;

	VertexDeclaration = ZEVertexDeclaration::CreateInstance();

	ZEGRVertexElement ElementArray[] = {
		{ZE_VES_POSITION, ZE_VET_FLOAT4, 0},
		{ZE_VES_TEXTCOORD, ZE_VET_FLOAT2, 0},
		{ZE_VES_NORMAL, ZE_VET_FLOAT3, 0},
		{ZE_VES_BINORMAL, ZE_VET_FLOAT3, 0},
		{ZE_VES_TANGENT, ZE_VET_FLOAT3, 0}
	};

	if (!VertexDeclaration->Create(ElementArray, 5))
	{
		VertexDeclaration->Destroy();
		VertexDeclaration = NULL;
	}

	struct SeaVertex
	{
		ZEVector4 Position;
		ZEVector2 TextureCoordinate;
		ZEVector3 Normal;
		ZEVector3 BiNormal;
		ZEVector3 Tangent;
	};

	VertexBuffer = ZEStaticVertexBuffer::CreateInstance();
	VertexBuffer->Create(sizeof(SeaVertex) * 6);

	SeaVertex* Vertices = (SeaVertex*)VertexBuffer->Lock();
	if (Vertices != NULL)
	{
		/*
		Vertices[0].Position = ZEVector4(-0.5f, 0.0f, 0.5f, 1.0f);
		Vertices[0].Normal = ZEVector3::UnitY;
		Vertices[0].BiNormal = ZEVector3::UnitX;
		Vertices[0].Tangent = ZEVector3::UnitZ;
		Vertices[0].TextureCoordinate = ZEVector2::Zero;

		Vertices[1].Position = ZEVector4(0.5f, 0.0f, 0.5f, 1.0f);
		Vertices[1].Normal = ZEVector3::UnitY;
		Vertices[1].BiNormal = ZEVector3::UnitX;
		Vertices[1].Tangent = ZEVector3::UnitZ;
		Vertices[1].TextureCoordinate = ZEVector2(1.0f, 0.0f);

		Vertices[2].Position = ZEVector4(0.5f, 0.0f, -0.5f, 1.0f);
		Vertices[2].Normal = ZEVector3::UnitY;
		Vertices[2].BiNormal = ZEVector3::UnitX;
		Vertices[2].Tangent = ZEVector3::UnitZ;
		Vertices[2].TextureCoordinate = ZEVector2(1.0f, 1.0f);

		Vertices[3].Position = ZEVector4(-0.5f, 0.0f, 0.5f, 1.0f);
		Vertices[3].Normal = ZEVector3::UnitY;
		Vertices[3].BiNormal = ZEVector3::UnitX;
		Vertices[3].Tangent = ZEVector3::UnitZ;
		Vertices[3].TextureCoordinate = ZEVector2::Zero;

		Vertices[4].Position = ZEVector4(0.5f, 0.0f, -0.5f, 1.0f);
		Vertices[4].Normal = ZEVector3::UnitY;
		Vertices[4].BiNormal = ZEVector3::UnitX;
		Vertices[4].Tangent = ZEVector3::UnitZ;
		Vertices[4].TextureCoordinate = ZEVector2::One;

		Vertices[5].Position = ZEVector4(-0.5f, 0.0f, -0.5f, 1.0f);
		Vertices[5].Normal = ZEVector3::UnitY;
		Vertices[5].BiNormal = ZEVector3::UnitX;
		Vertices[5].Tangent = ZEVector3::UnitZ;
		Vertices[5].TextureCoordinate = ZEVector2(0.0f, 1.0f);
		*/
		/*
		Vertices[0].Position = ZEVector4(-0.5f, 0.0f, 0.5f, 1.0f);
		Vertices[0].Normal = ZEVector3::UnitY;
		Vertices[0].BiNormal = ZEVector3::UnitX;
		Vertices[0].Tangent = -ZEVector3::UnitZ;
		Vertices[0].TextureCoordinate = ZEVector2::Zero;

		Vertices[1].Position = ZEVector4(0.5f, 0.0f, 0.5f, 1.0f);
		Vertices[1].Normal = ZEVector3::UnitY;
		Vertices[1].BiNormal = ZEVector3::UnitX;
		Vertices[1].Tangent = -ZEVector3::UnitZ;
		Vertices[1].TextureCoordinate = ZEVector2(1.0f, 0.0f);

		Vertices[2].Position = ZEVector4(0.5f, 0.0f, -0.5f, 1.0f);
		Vertices[2].Normal = ZEVector3::UnitY;
		Vertices[2].BiNormal = ZEVector3::UnitX;
		Vertices[2].Tangent = -ZEVector3::UnitZ;
		Vertices[2].TextureCoordinate = ZEVector2(1.0f, 1.0f);

		Vertices[3].Position = ZEVector4(-0.5f, 0.0f, 0.5f, 1.0f);
		Vertices[3].Normal = ZEVector3::UnitY;
		Vertices[3].BiNormal = ZEVector3::UnitX;
		Vertices[3].Tangent = -ZEVector3::UnitZ;
		Vertices[3].TextureCoordinate = ZEVector2::Zero;

		Vertices[4].Position = ZEVector4(0.5f, 0.0f, -0.5f, 1.0f);
		Vertices[4].Normal = ZEVector3::UnitY;
		Vertices[4].BiNormal = ZEVector3::UnitX;
		Vertices[4].Tangent = -ZEVector3::UnitZ;
		Vertices[4].TextureCoordinate = ZEVector2::One;

		Vertices[5].Position = ZEVector4(-0.5f, 0.0f, -0.5f, 1.0f);
		Vertices[5].Normal = ZEVector3::UnitY;
		Vertices[5].BiNormal = ZEVector3::UnitX;
		Vertices[5].Tangent = -ZEVector3::UnitZ;
		Vertices[5].TextureCoordinate = ZEVector2(0.0f, 1.0f);
	}

	VertexBuffer->Unlock();*/

	SetEntityFlags(ZE_EF_TICKABLE | ZE_EF_RENDERABLE);
}

ZESea::~ZESea()
{

}

void ZESea::Tick(float ElapsedTime)
{
	DiffuseOffset += DiffuseVelocity * ElapsedTime;
	NormalOffset += NormalVelocity * ElapsedTime;

	ZEEntity::Tick(ElapsedTime);
}

ZESea* ZESea::CreateInstance()
{
	return new ZESea();
}

void ZESea::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	float Height = GetWorldPosition().y;
	ZEPlane Plane(ZEVector3::UnitY, ZEVector3(0.0f, Height, 0.0f));
	
	float MinT;
	if (!ZEPlane::IntersectionTest(Plane, Parameters.Ray, MinT))
		return;

	if (!Report.CheckDistance(MinT))
		return;

	ZERayCastCollision Collision;
	Collision.Object = this;
	Collision.Position = Parameters.Ray.GetPointOn(MinT);
	Collision.Distance = MinT;

	if (Parameters.Components.GetFlags(ZE_RCRE_BOUNDING_BOX_ENTER))
	{
		Collision.Type = ZE_RCCT_BOUNDING_BOX_ENTER;
		Report.AddCollision(Collision);
	}

	if (Parameters.Components.GetFlags(ZE_RCRE_BOUNDING_BOX_EXIT) && !Report.CheckDone())
	{
		Collision.Type = ZE_RCCT_BOUNDING_BOX_EXIT;
		Report.AddCollision(Collision);
	}

	if (Parameters.Components.GetFlags(ZE_RCRE_POLYGONS) && !Report.CheckDone())
	{
		ZEVector3 Binormal, Tangent;
		ZEVector3::CrossProduct(Binormal, Plane.n, ZEVector3::UnitX);
		ZEVector3::CrossProduct(Tangent, Plane.n, Binormal);
			
		Collision.Type = ZE_RCCT_POLGON;
		Collision.Polygon.V0 = Plane.p;
		Collision.Polygon.V1 = Plane.p + Tangent;
		Collision.Polygon.V2 = Plane.p + Binormal;	
		Report.AddCollision(Collision);
	}
}
