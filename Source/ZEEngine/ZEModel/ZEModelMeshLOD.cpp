//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelMeshLOD.cpp
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

#include "ZEModelMeshLOD.h"
#include "ZEModel.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEMath/ZEAngle.h"

void ZEModelMeshLOD::ResetMaterial()
{
	RenderCommand.Material = Owner->GetModelResource()->GetMaterials()[(ZESize)LODResource->MaterialId];
}

void ZEModelMeshLOD::SetMaterial(const ZEMaterial* Material)
{
	RenderCommand.Material = Material;
}

const ZEMaterial* ZEModelMeshLOD::GetMaterial()
{
	return RenderCommand.Material;
}

const ZEModelResourceMeshLOD* ZEModelMeshLOD::GetLODResource()
{
	return LODResource;
}

ZEInt32 ZEModelMeshLOD::GetDrawStartDistance()
{
	return DrawStartDistance;
}

ZEInt32 ZEModelMeshLOD::GetDrawEndDistance()
{
	return DrawEndDistance;
}

bool ZEModelMeshLOD::IsSkinned()
{
	return Skinned;
}

void ZEModelMeshLOD::Draw(ZEDrawParameters* DrawParameters, float DrawOrder)
{
	if (VertexBuffer == NULL)
		return;

	if (Skinned)
	{
		RenderCommand.BoneTransforms.SetCount(LODResource->AffectingBoneIds.GetCount());
		for (ZESize I = 0; I < LODResource->AffectingBoneIds.GetCount(); I++)
		{
			ZEMatrix4x4::Multiply(RenderCommand.BoneTransforms[I], Owner->GetBones()[(ZESize)LODResource->AffectingBoneIds[I]].GetVertexTransform(), this->OwnerMesh->GetLocalTransform());
		}

		RenderCommand.WorldMatrix = Owner->GetWorldTransform();
	}
	else if(OwnerMesh->GetPhysicalCloth() != NULL)
	{
		RenderCommand.WorldMatrix = ZEMatrix4x4::Identity;
	}
	else
	{
		RenderCommand.WorldMatrix = OwnerMesh->GetWorldTransform();
	}

	RenderCommand.Order = DrawOrder;
	RenderCommand.ClippingPlanes = OwnerMesh->ClippingPlanes;

	DrawParameters->Renderer->AddToRenderList(&RenderCommand);
}

bool ZEModelMeshLOD::UpdateVertexBuffer(ZEArray<ZEVector3> Vertices, ZEArray<ZEUInt32> Indices)
{
	ZEStaticVertexBuffer* Buffer = LODResource->GetSharedVertexBuffer();
	
	if(Buffer == NULL)
		return false;

	ZESize VertexCount = Vertices.GetCount();
	ZEArray<ZEVector3> Normals;
	Normals.SetCount(VertexCount);

	ZEModelVertex* VertexData = (ZEModelVertex*)Buffer->Lock();
	if (VertexData == NULL)
	{
		zeError("Can not lock static vertex buffer.");
		return false;
	}
	
	ZEVector4 TempVector;
	for (ZESize I = 0; I < VertexCount; I += 3)
	{
		ZEVector3 Vertex0 = Vertices[Indices[I + 0]];
		ZEVector3 Vertex1 = Vertices[Indices[I + 1]];
		ZEVector3 Vertex2 = Vertices[Indices[I + 2]];

		VertexData[I + 0].Position = Vertex0;
		VertexData[I + 1].Position = Vertex1;
		VertexData[I + 2].Position = Vertex2;
	
		//First Vertex Normal
		Normals[I] = ZEVector3::Zero;

		ZEVector3 EdgeA = Vertex1 - Vertex0;
		ZEVector3 EdgeB = Vertex2 - Vertex0;
		ZEVector3 TriangleNormal;

		ZEVector3::CrossProduct(TriangleNormal, EdgeA, EdgeB);
		float Weight = ZEVector3::DotProduct(EdgeA.Normalize(), EdgeB.Normalize()) * -0.5f + 0.5f;
		Normals[Indices[I]] += TriangleNormal * Weight;

		//Second Vertex Normal
		Normals[I + 1] = ZEVector3::Zero;

		EdgeA = Vertex2 - Vertex1;
		EdgeB = Vertex0 - Vertex1;

		ZEVector3::CrossProduct(TriangleNormal, EdgeA, EdgeB);
		Weight = ZEVector3::DotProduct(EdgeA.Normalize(), EdgeB.Normalize()) * -0.5f + 0.5f;
		Normals[Indices[I + 1]] += TriangleNormal * Weight;

		//Third Vertex Normal
		Normals[I + 2] = ZEVector3::Zero;

		EdgeA = Vertex0 - Vertex2;
		EdgeB = Vertex1 - Vertex2;

		ZEVector3::CrossProduct(TriangleNormal, EdgeA, EdgeB);
		Weight = ZEVector3::DotProduct(EdgeA.Normalize(), EdgeB.Normalize()) * -0.5f + 0.5f;
		Normals[Indices[I + 2]] += TriangleNormal * Weight;
	}

	for (ZESize I = 0; I < VertexCount; I += 3)
	{
		VertexData[I + 0].Normal = Normals[Indices[I + 0]].Normalize();
		VertexData[I + 1].Normal = Normals[Indices[I + 1]].Normalize();
		VertexData[I + 2].Normal = Normals[Indices[I + 2]].Normalize();

		ZEVector3 VectorP = VertexData[I + 2].Position - VertexData[I + 0].Position;
		ZEVector3 VectorQ = VertexData[I + 1].Position - VertexData[I + 0].Position;

		ZEVector2 TextureCoord0 = this->GetLODResource()->Vertices[I + 0].Texcoord;
		ZEVector2 TextureCoord1 = this->GetLODResource()->Vertices[I + 1].Texcoord;
		ZEVector2 TextureCoord2 = this->GetLODResource()->Vertices[I + 2].Texcoord;

		float S1 = TextureCoord2.x - TextureCoord0.x;
		float S2 = TextureCoord1.x - TextureCoord0.x;
		float T1 = TextureCoord2.y - TextureCoord0.y;
		float T2 = TextureCoord1.y - TextureCoord0.y;

		float Handedness = 0.0f;

		if(fabs(S1 * T2 - S2 * T1))
			Handedness = 1.0f;
		else
			Handedness = 1.0f / (S1 * T2 - S2 * T1);

		ZEVector3 Tangent;
		Tangent.x = (T2 * VectorP.x - T1 * VectorQ.x);
		Tangent.y = (T2 * VectorP.y - T1 * VectorQ.y);
		Tangent.z = (T2 * VectorP.z - T1 * VectorQ.z);
		Tangent *= Handedness;

		ZEVector3 Binormal;
		Binormal.x = (S1 * VectorQ.x - S2 * VectorP.x);
		Binormal.y = (S1 * VectorQ.y - S2 * VectorP.y);
		Binormal.z = (S1 * VectorQ.z - S2 * VectorP.z);
		Binormal *= Handedness;

		VertexData[I + 0].Tangent = Tangent.Normalize();
		VertexData[I + 1].Tangent = VertexData[I + 0].Tangent;
		VertexData[I + 2].Tangent = VertexData[I + 0].Tangent;

		VertexData[I + 0].Binormal = Binormal.Normalize();
		VertexData[I + 1].Binormal = VertexData[I + 0].Binormal;
		VertexData[I + 2].Binormal = VertexData[I + 0].Binormal;
	}
	Buffer->Unlock();

	return true;
}

void ZEModelMeshLOD::Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource)
{
	Owner = Model;
	OwnerMesh = Mesh;
	this->LODResource = LODResource;
	DrawStartDistance = LODResource->LODStartDistance;
	DrawEndDistance = LODResource->LODEndDistance;

	Skinned = LODResource->Vertices.GetCount() == 0 ? true : false;

	RenderCommand.SetZero();
	RenderCommand.Priority = 3;
	RenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING | (Skinned ? ZE_ROF_SKINNED : 0);
	RenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderCommand.VertexBuffer = VertexBuffer = LODResource->GetSharedVertexBuffer();
	RenderCommand.PrimitiveCount = Skinned ? LODResource->SkinnedVertices.GetCount() / 3: LODResource->Vertices.GetCount() / 3;
	RenderCommand.VertexDeclaration = Skinned ? ZESkinnedModelVertex::GetVertexDeclaration() : ZEModelVertex::GetVertexDeclaration();
	RenderCommand.Material = Owner->GetModelResource()->GetMaterials()[(ZESize)LODResource->MaterialId];
}

void ZEModelMeshLOD::Deinitialize()
{
	Owner = NULL;
	OwnerMesh = NULL;
	DrawStartDistance = 0;
	DrawEndDistance = 0;
	RenderCommand.SetZero();
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
	Skinned = false;
}

ZEModelMeshLOD::ZEModelMeshLOD()
{
	Skinned = false;
	Owner = NULL;
	OwnerMesh = NULL;
	DrawStartDistance = 0;
	DrawEndDistance = 0;
	RenderCommand.SetZero();
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{
	Deinitialize();
}
