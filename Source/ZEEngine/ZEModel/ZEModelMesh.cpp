//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelMesh.cpp
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

#include "ZEModelMesh.h"
#include "ZEModel.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEPhysics/ZEPhysicalCloth.h"
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZEMath.h"

void ZEModelMesh::SetActiveLOD(ZEUInt LOD)
{
	AutoLOD = false;
	this->ActiveLOD = LOD;
}

ZEUInt ZEModelMesh::GetActiveLOD()
{
	return ActiveLOD;
}

void ZEModelMesh::SetAutoLOD(bool Enabled)
{
	AutoLOD = Enabled;
}

bool ZEModelMesh::GetAutoLOD()
{
	return AutoLOD;
}

ZEArray<ZEModelMeshLOD>& ZEModelMesh::GetLODs()
{
	return LODs;
}

void ZEModelMesh::SetVisible(bool Visible)
{
	this->Visible = Visible;
}

bool ZEModelMesh::GetVisible()
{
	return Visible;
}

ZEModelMesh* ZEModelMesh::GetParentMesh()
{
	return ParentMesh;
}

const ZEArray<ZEModelMesh*>& ZEModelMesh::GetChildMeshes()
{
	return ChildMeshes;
}

const char* ZEModelMesh::GetName()
{
	return MeshResource->Name;
}

ZEPhysicalRigidBody* ZEModelMesh::GetPhysicalBody()
{
	return PhysicalBody;
}

ZEPhysicalCloth* ZEModelMesh::GetPhysicalCloth()
{
	return PhysicalCloth;
}

const ZEAABBox& ZEModelMesh::GetLocalBoundingBox() const
{
	return LocalBoundingBox;
}

const ZEAABBox& ZEModelMesh::GetModelBoundingBox() const
{
	ZEAABBox::Transform(ModelBoundingBox, LocalBoundingBox, GetModelTransform());

	return ModelBoundingBox;
}

const ZEAABBox& ZEModelMesh::GetWorldBoundingBox() const
{
	ZEAABBox::Transform(WorldBoundingBox, LocalBoundingBox, GetWorldTransform());

	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEModelMesh::GetLocalTransform() const
{
	ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation, Scale);

	return LocalTransform;
}

const ZEMatrix4x4& ZEModelMesh::GetModelTransform() const
{
	if (ParentMesh == NULL)
		return GetLocalTransform();
	else
	{
		ZEMatrix4x4::Multiply(ModelTransform, ParentMesh->GetModelTransform(), GetLocalTransform());
		return ModelTransform;
	}
}

const ZEMatrix4x4& ZEModelMesh::GetWorldTransform() const
{
	if (ParentMesh == NULL)
	{
		ZEMatrix4x4::Multiply(WorldTransform, Owner->GetWorldTransform(), GetLocalTransform());
		return WorldTransform;
	}
	else
	{
		ZEMatrix4x4::Multiply(WorldTransform, ParentMesh->GetWorldTransform(), GetLocalTransform());
		return WorldTransform;
	}
}

const ZEMatrix4x4& ZEModelMesh::GetInvWorldTransform() const
{
	ZEMatrix4x4::Inverse(InvWorldTransform, GetWorldTransform());

	return InvWorldTransform;
}

void ZEModelMesh::SetLocalPosition(const ZEVector3& LocalPosition)
{
	Position = LocalPosition;
}

const ZEVector3& ZEModelMesh::GetLocalPosition() const
{
	return Position;
}

void ZEModelMesh::SetLocalRotation(const ZEQuaternion& LocalRotation)
{
	Rotation = LocalRotation;
}

const ZEQuaternion& ZEModelMesh::GetLocalRotation() const
{
	return Rotation;
}

void ZEModelMesh::SetLocalScale(const ZEVector3& LocalScale)
{
	Scale = LocalScale;
}

const ZEVector3& ZEModelMesh::GetLocalScale() const
{
	return Scale;
}

void ZEModelMesh::SetModelPosition(const ZEVector3& ModelPosition)
{
	if (ParentMesh == NULL)
		SetLocalPosition(ModelPosition);
	else
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, ParentMesh->GetModelTransform().Inverse(), ModelPosition);
		SetLocalPosition(Result);
	}
}

const ZEVector3 ZEModelMesh::GetModelPosition() const
{
	if (ParentMesh == NULL)
		return Position;
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentMesh->GetModelTransform(), Position);
		return Temp;
	}
}

void ZEModelMesh::SetModelRotation(const ZEQuaternion& ModelRotation)
{
	if (ParentMesh == NULL)
		SetLocalRotation(ModelRotation);
	else
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, ParentMesh->GetModelRotation());
		ZEQuaternion::Product(Result, Temp, ModelRotation);
		SetLocalRotation(Result);
	}
}

const ZEQuaternion ZEModelMesh::GetModelRotation() const
{
	if (ParentMesh == NULL)
		return Rotation;
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, ParentMesh->GetModelRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
}

void ZEModelMesh::SetModelScale(const ZEVector3& ModelScale)
{
	if (ParentMesh == NULL)
		SetLocalScale(ModelScale);
	else
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, ModelScale, ParentMesh->GetModelScale());
		SetLocalScale(Temp);
	}		
}

const ZEVector3 ZEModelMesh::GetModelScale() const
{
	if (ParentMesh == NULL)
		return Scale;
	else
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, ParentMesh->GetModelScale(), Scale);
		return Temp;
	}
}

void ZEModelMesh::SetWorldPosition(const ZEVector3& WorldPosition)
{
	if (ParentMesh == NULL)
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, Owner->GetWorldTransform().Inverse(), WorldPosition);
		SetLocalPosition(Result);
	}
	else
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, ParentMesh->GetWorldTransform().Inverse(), WorldPosition);
		SetLocalPosition(Result);
	}
}

const ZEVector3 ZEModelMesh::GetWorldPosition() const
{
	if (ParentMesh == NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Owner->GetWorldTransform(), Position);
		return Temp;
	}
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentMesh->GetWorldTransform(), Position);
		return Temp;
	}
}

void ZEModelMesh::SetWorldRotation(const ZEQuaternion& WorldRotation)
{
	if (ParentMesh == NULL)
	{	
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, Owner->GetWorldRotation());
		ZEQuaternion::Product(Result, Temp, WorldRotation);
		SetLocalRotation(Result);
	}
	else
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, ParentMesh->GetWorldRotation());
		ZEQuaternion::Product(Result, Temp, WorldRotation);
		SetLocalRotation(Result);
	}
}

const ZEQuaternion ZEModelMesh::GetWorldRotation() const
{
	if (ParentMesh == NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Owner->GetWorldRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, ParentMesh->GetWorldRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
}

void ZEModelMesh::SetWorldScale(const ZEVector3& WorldScale)
{
	if (ParentMesh == NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, WorldScale, Owner->GetWorldScale());
		SetLocalScale(Temp);
	}
	else
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, WorldScale, ParentMesh->GetWorldScale());
		SetLocalScale(Temp);
	}
}

const ZEVector3 ZEModelMesh::GetWorldScale() const
{
	if (ParentMesh == NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, Owner->GetWorldScale(), Scale);
		return Temp;
	}
	else
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, ParentMesh->GetWorldScale(), Scale);
		return Temp;
	}
}

void ZEModelMesh::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this-> AnimationType = AnimationType;
}

ZEModelAnimationType ZEModelMesh::GetAnimationType()
{
	return AnimationType;
}

void ZEModelMesh::AddChild(ZEModelMesh* Mesh)
{
	Mesh->ParentMesh = this;
	ChildMeshes.Add(Mesh);
}

void ZEModelMesh::RemoveChild(ZEModelMesh* Mesh)
{
	Mesh->ParentMesh = NULL;
	ChildMeshes.RemoveValue(Mesh);
}

void ZEModelMesh::SetPhysicsEnabled(bool Enabled)
{
	PhysicsEnabled = Enabled;
}

bool ZEModelMesh::GetPhysicsEnabled() const
{
	return PhysicsEnabled;
}

void ZEModelMesh::SetCustomDrawOrderEnabled(bool Enabled)
{
	DrawOrderIsUserDefined = Enabled;
}

void ZEModelMesh::SetCustomDrawOrder(ZEUInt8 DrawOrder)
{
	UserDefinedDrawOrder = DrawOrder;
}

ZEUInt8 ZEModelMesh::GetCustomDrawOrder()
{
	return UserDefinedDrawOrder;
}


void ZEModelMesh::SetClippingPlaneCount(ZESize Count)
{
	ClippingPlanes.SetCount(Count);
}

ZESize ZEModelMesh::GetClippingPlaneCount()
{
	return ClippingPlanes.GetCount();
}

void ZEModelMesh::SetClippingPlane(ZESize Index, const ZEPlane& Plane)
{
	ClippingPlanes[Index] = Plane;
}

const ZEPlane& ZEModelMesh::GetClippingPlane(ZESize Index)
{
	return ClippingPlanes[Index];
}

void ZEModelMesh::Initialize(ZEModel* Model,  const ZEModelResourceMesh* MeshResource)
{
	Owner = Model;
	this->MeshResource = MeshResource;
	ActiveLOD = 0;
	AutoLOD = true;
	AnimationType = ZE_MAT_PREDEFINED;
	Position = MeshResource->Position;
	Rotation = MeshResource->Rotation;
	Scale = MeshResource->Scale;
	Visible = MeshResource->IsVisible;
	LocalBoundingBox = MeshResource->BoundingBox;
	PhysicsEnabled = false;

	ZEArray<ZEPhysicalShape*> ShapeList;

	if(PhysicalBody == NULL)
	{
		if (MeshResource->PhysicalBody.Type == ZE_MRPBT_RIGID)
		{
			PhysicalBody = ZEPhysicalRigidBody::CreateInstance();

			PhysicalBody->SetEnabled(MeshResource->PhysicalBody.Enabled);
			PhysicalBody->SetMass(MeshResource->PhysicalBody.Mass);
			PhysicalBody->SetLinearDamping(MeshResource->PhysicalBody.LinearDamping);
			PhysicalBody->SetAngularDamping(MeshResource->PhysicalBody.AngularDamping);
			PhysicalBody->SetPosition(Owner->GetWorldPosition());
			PhysicalBody->SetRotation(Owner->GetWorldRotation());
			PhysicalBody->SetMassCenterPosition(MeshResource->PhysicalBody.MassCenter);
			PhysicalBody->SetTransformChangeEvent(ZEDelegate<void (ZEPhysicalObject*, ZEVector3, ZEQuaternion)>::Create<ZEModel, &ZEModel::TransformChangeEvent>(this->Owner));

			for (ZESize I = 0; I < MeshResource->PhysicalBody.Shapes.GetCount(); I++)
			{
				const ZEModelResourcePhysicalShape* Shape = &MeshResource->PhysicalBody.Shapes[I];
				switch(Shape->Type)
				{
					case ZE_PST_BOX:
					{
						ZEPhysicalBoxShape* BoxShape = new ZEPhysicalBoxShape();
						BoxShape->SetWidth(Shape->Box.Width);
						BoxShape->SetHeight(Shape->Box.Height);
						BoxShape->SetLength(Shape->Box.Length);
						BoxShape->SetPosition(Shape->Position);
						BoxShape->SetRotation(Shape->Rotation);
						ShapeList.Add(BoxShape);
						PhysicalBody->AddPhysicalShape(BoxShape);
						break;
					}

					case ZE_PST_SPHERE:
					{
						ZEPhysicalSphereShape* SphereShape = new ZEPhysicalSphereShape();
						SphereShape->SetRadius(Shape->Sphere.Radius);
						SphereShape->SetPosition(Shape->Position);
						SphereShape->SetRotation(Shape->Rotation);
						ShapeList.Add(SphereShape);
						PhysicalBody->AddPhysicalShape(SphereShape);
						break;
					}
					case ZE_PST_CYLINDER:
					{
						// Problematic
						break;
					}

					case ZE_PST_CAPSULE:
					{
						ZEPhysicalCapsuleShape* CapsuleShape = new ZEPhysicalCapsuleShape();
						CapsuleShape->SetRadius(Shape->Capsule.Radius);
						CapsuleShape->SetHeight(Shape->Capsule.Height);
						CapsuleShape->SetPosition(Shape->Position);
						CapsuleShape->SetRotation(Shape->Rotation);
						ShapeList.Add(CapsuleShape);
						PhysicalBody->AddPhysicalShape(CapsuleShape);
						break;
					}

					case ZE_PST_CONVEX:
						// Problematic
						break;
				}
			}

			PhysicalBody->SetPhysicalWorld(zeScene->GetPhysicalWorld());
			PhysicalBody->Initialize();
		}
		else if(MeshResource->PhysicalBody.Type == ZE_MRPBT_CLOTH)
		{
			PhysicalCloth = ZEPhysicalCloth::CreateInstance();

			ZESize VertexCount = MeshResource->LODs[0].Vertices.GetCount();
			ZEArray<ZEVector3>& ClothVertices = PhysicalCloth->GetVertices();
			ClothVertices.SetCount(VertexCount);

			for(ZESize I = 0; I < VertexCount; I++)
				ClothVertices[I] = MeshResource->LODs[0].Vertices[I].Position;

			PhysicalCloth->SetPosition(Owner->GetWorldTransform() * Position);
			ZEQuaternion TempRotation;
			ZEQuaternion::CreateFromMatrix(TempRotation, Owner->GetWorldTransform() * GetLocalTransform());
			PhysicalCloth->SetRotation(TempRotation);

			PhysicalCloth->SetEnabled(true);
			PhysicalCloth->SetThickness(0.5f);
			PhysicalCloth->SetBendingMode(true);
			PhysicalCloth->SetBendingStiffness(1.0f);
			PhysicalCloth->SetStretchingStiffness(1.0f);
			PhysicalCloth->SetPhysicalWorld(zeScene->GetPhysicalWorld());
			PhysicalCloth->Initialize();
		}
	}

	LODs.SetCount(MeshResource->LODs.GetCount());
	for (ZESize I = 0; I < MeshResource->LODs.GetCount(); I++)
		LODs[I].Initialize(Owner, this, &MeshResource->LODs[I]);

	for (ZESize I = 0; I < ShapeList.GetCount(); I++)
		delete ShapeList[I];
	ShapeList.Clear();


}

void ZEModelMesh::Deinitialize()
{
	Owner = NULL;
	ParentMesh = NULL;

	if (PhysicalBody != NULL)
	{
		PhysicalBody->Destroy();
		PhysicalBody = NULL;
	}

	ChildMeshes.Clear();
	LODs.Clear();
}

void ZEModelMesh::OnTransformChanged()
{
	if (PhysicalBody != NULL)
	{
		PhysicalBody->SetPosition(Owner->GetWorldPosition());
		PhysicalBody->SetRotation(Owner->GetWorldRotation());
	}
}

void ZEModelMesh::Draw(ZEDrawParameters* DrawParameters)
{
	if (!Visible)
		return;

	float DrawOrder = 0.0f;
	ZEInt32 CurrentLOD = 0;
	float LODDistanceSquare = 0.0f;

	ZEVector3 WorldPosition;
	ZEMatrix4x4::Transform(WorldPosition, GetWorldTransform(), ZEVector3::Zero);
	float EntityDistanceSquare = ZEVector3::DistanceSquare(DrawParameters->View->Camera->GetWorldPosition(), WorldPosition);

	if (!DrawOrderIsUserDefined)
		DrawOrder = EntityDistanceSquare;
	else
		DrawOrder = EntityDistanceSquare * (UserDefinedDrawOrder + 1);
 	
	float CurrentDistanceSquare = 0.0f;

	for (ZESize I = 0; I < LODs.GetCount(); I++)
	{
		LODDistanceSquare = LODs[I].GetDrawStartDistance() * LODs[I].GetDrawStartDistance();

		if (LODDistanceSquare < EntityDistanceSquare)
		{
			if (CurrentDistanceSquare <= LODDistanceSquare)
			{
				CurrentDistanceSquare = LODDistanceSquare;
				CurrentLOD = I;
			}
		}
	}

	if (EntityDistanceSquare < (LODs[CurrentLOD].GetDrawEndDistance() * LODs[CurrentLOD].GetDrawEndDistance()))
		LODs[(ZESize)CurrentLOD].Draw(DrawParameters, DrawOrder);
}

bool ZEModelMesh::RayCastPoligons(const ZERay& Ray, float& MinT, ZESize& PoligonIndex)
{
	if (MeshResource->LODs.GetCount() == 0)
		return false;

	bool HaveIntersection = false;
	const ZEArray<ZEModelVertex>& Vertices = MeshResource->LODs[0].Vertices;

	for (ZESize I = 0; I < Vertices.GetCount(); I += 3)
	{
		ZETriangle Triangle(Vertices[I].Position, Vertices[I + 1].Position, Vertices[I + 2].Position);

		float RayT;
		if (ZETriangle::IntersectionTest(Triangle, Ray, RayT))
		{
			if (RayT < MinT)
			{
				MinT = RayT;
				PoligonIndex = I / 3;
				HaveIntersection = true;
			}
		}
	}

	return HaveIntersection;
}

bool ZEModelMesh::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	if (MeshResource == NULL || MeshResource->IsSkinned == true)
		return false;

	ZERay LocalRay;
	ZEMatrix4x4::Transform(LocalRay.p, GetInvWorldTransform(), Parameters.Ray.p);
	ZEMatrix4x4::Transform3x3(LocalRay.v, GetInvWorldTransform(), Parameters.Ray.v);
	LocalRay.v.NormalizeSelf();

	float RayT;
	if (!ZEAABBox::IntersectionTest(GetLocalBoundingBox(), LocalRay, RayT))
		return false;

	float MinT = ZE_FLOAT_MAX;
	ZESize PoligonIndex;
	if (RayCastPoligons(LocalRay, MinT, PoligonIndex))
	{
		ZEVector3 WorldPosition;
		ZEMatrix4x4::Transform(WorldPosition, GetWorldTransform(), LocalRay.GetPointOn(MinT));

		float DistanceSquare = ZEVector3::DistanceSquare(Parameters.Ray.p, WorldPosition);
		if (Report.Distance * Report.Distance > DistanceSquare && DistanceSquare < Parameters.MaximumDistance * Parameters.MaximumDistance)
		{
			Report.Distance = ZEMath::Sqrt(DistanceSquare);
			Report.Position = WorldPosition;
			Report.SubComponent = this;
			Report.PoligonIndex = PoligonIndex;

			if (Parameters.Extras.GetFlags(ZE_RCRE_NORMAL) || Parameters.Extras.GetFlags(ZE_RCRE_BINORMAL))
			{
				ZEVector3 V0 = MeshResource->LODs[0].Vertices[3 * Report.PoligonIndex].Position;
				ZEVector3 V1 = MeshResource->LODs[0].Vertices[3 * Report.PoligonIndex + 1].Position;
				ZEVector3 V2 = MeshResource->LODs[0].Vertices[3 * Report.PoligonIndex + 2].Position;

				ZEVector3 Binormal = ZEVector3(V0, V1);
				ZEVector3 Tangent = ZEVector3(V0, V2);
				ZEVector3 Normal;
				ZEVector3::CrossProduct(Normal, Binormal, Tangent);

				if (Parameters.Extras.GetFlags(ZE_RCRE_NORMAL))
				{
					ZEMatrix4x4::Transform3x3(Report.Normal, GetWorldTransform(), Normal);
					Report.Normal.NormalizeSelf();
				}

				if (Parameters.Extras.GetFlags(ZE_RCRE_BINORMAL))
				{
					ZEMatrix4x4::Transform3x3(Report.Binormal, GetWorldTransform(), Binormal);
					Report.Binormal.NormalizeSelf();
				}
			}

			return true;
		}
	}

	return false;
}


ZEModelMesh::ZEModelMesh()
{
	Owner = NULL;
	ParentMesh = NULL;
	MeshResource = NULL;
	PhysicalBody = NULL;
	PhysicalCloth = NULL;
	Visible = true;
	PhysicsEnabled = false;
	AutoLOD = false;
	ActiveLOD = 0;
	AnimationType = ZE_MAT_NOANIMATION;
	DrawOrderIsUserDefined = false;
	UserDefinedDrawOrder = 0;
}

ZEModelMesh::~ZEModelMesh()
{
	Deinitialize();
}
