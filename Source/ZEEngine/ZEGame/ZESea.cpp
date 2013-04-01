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
#include "ZEGraphics/ZEVertexDeclaration.h"
#include "ZEGraphics/ZEVertexBuffer.h"

#include "ZEGraphics/ZESeaMaterial.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEDrawParameters.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEEntityProvider.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEGraphics/ZETexture2D.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZESea);

ZEEntityRunAt ZESeaDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}

ZESea::ZESea()
{
	Material = ZESeaMaterial::CreateInstance();
	SetScale(ZEVector3::One * 200);

	SetDiffuseTexture(ZETexture2DResource::LoadSharedResource("ZEEngine\\Sea\\SeaDiffuse.jpg")->GetTexture());
	SetNormalTexture(ZETexture2DResource::LoadSharedResource("ZEEngine\\Sea\\SeaNormal.jpg")->GetTexture());

	DiffuseVelocity = ZEVector2::One;
	NormalVelocity	= -ZEVector2::One;

	DiffuseOffset = ZEVector2::Zero;
	NormalOffset = ZEVector2::Zero;

	DiffuseTextureTile = ZEVector2::One;
	NormalTextureTile = ZEVector2::One;

	VertexDeclaration = ZEVertexDeclaration::CreateInstance();

	ZEVertexElement ElementArray[] = {
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
	}

	VertexBuffer->Unlock();
}

ZESea::~ZESea()
{
	Material->Destroy();
}

void ZESea::SetDiffuseVelocity(const ZEVector2& Velocity)
{
	DiffuseVelocity = Velocity;
}

ZEVector2 ZESea::GetDiffuseVelocity() const
{
	return DiffuseVelocity;
}

void ZESea::SetNormalVelocity(const ZEVector2& Velocity)
{
	NormalVelocity = Velocity;
}

ZEVector2 ZESea::GetNormalVelocity() const
{
	return NormalVelocity;
}

void ZESea::SetDiffuseTexture(const ZETexture2D* Texture)
{
	Material->SetDiffuseTexture(Texture);
}

void  ZESea::SetNormalTexture(const ZETexture2D* Texture)
{
	Material->SetNormalTexture(Texture);
}

void ZESea::Tick(float ElapsedTime)
{
	DiffuseOffset += DiffuseVelocity * ElapsedTime;
	NormalOffset += NormalVelocity * ElapsedTime;

	Material->DiffuseTextureOffset = DiffuseOffset;
	Material->NormalTextureOffset = NormalOffset;
	Material->EntityXZScale.x = GetScale().x;
	Material->EntityXZScale.y = GetScale().z;
	Material->NormalTextureTile = NormalTextureTile;
	Material->DiffuseTextureTile = DiffuseTextureTile;

	ZEEntity::Tick(ElapsedTime);
}

void ZESea::Draw(ZEDrawParameters* DrawParameters)
{
	
	Command.SetZero();

	Command.Material = Material;
	Command.PrimitiveType = ZE_ROPT_TRIANGLE;
	Command.PrimitiveCount = 2;
	Command.WorldMatrix = GetWorldTransform();
	Command.VertexBuffer = VertexBuffer;
	Command.VertexBufferOffset = 0;
	Command.VertexDeclaration = VertexDeclaration;
	Command.Order = 100.0f;
	Command.Priority = 100;
	Command.IndexBuffer = NULL;
	Command.Pipeline = ZE_RORP_3D;
	Command.PrimitiveParameters = 0;
	Command.Flags = 0;

	DrawParameters->Renderer->AddToRenderList(&Command);
}

ZEDrawFlags ZESea::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

ZESea* ZESea::CreateInstance()
{
	return new ZESea();
}

void ZESea::SetDiffuseTile(const ZEVector2& Tile)
{
	DiffuseTextureTile = Tile;
}

ZEVector2 ZESea::GetDiffuseTile() const
{
	return DiffuseTextureTile;
}

void ZESea::SetDiffuseColor(const ZEVector3& Color)
{
	Material->SetDiffuseColor(ZEVector4(Color, 1.0f));
}

ZEVector3 ZESea::GetDiffuseColor() const
{
	return Material->GetDiffuseColor().ToVector3();
}

void ZESea::SetDiffuseFactor(float Factor)
{
	Material->SetDiffuseFactor(Factor);
}

float ZESea::GetDiffuseFactor() const
{
	return Material->GetDiffuseFactor();
}

void ZESea::SetSpecularFactor(float Factor)
{
	Material->SetSpecularFactor(Factor);
}

float ZESea::GetSpecularFactor() const
{
	return Material->GetSpecularFactor();
}

void ZESea::SetSpecularShineness(float Shineness)
{
	Material->SetSpecularShineness(Shineness);
}

float ZESea::GetSpecularShineness() const
{
	return Material->GetSpecularShineness();
}

void ZESea::SetSpecularColor(const ZEVector3& Color)
{
	Material->SetSpecularColor(ZEVector4(Color, 1.0f));
}

ZEVector3 ZESea::GetSpecularColor() const
{
	return Material->GetSpecularColor().ToVector3();
}

void ZESea::SetNormalTile(const ZEVector2& Tile)
{
	NormalTextureTile = Tile;
}

ZEVector2 ZESea::GetNormalTile() const
{
	return NormalTextureTile;
}
