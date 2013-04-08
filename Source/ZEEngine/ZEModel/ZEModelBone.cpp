//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelBone.cpp
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

#include "ZEModelBone.h"
#include "ZEModel.h"
#include "ZEGame/ZEScene.h"
#include <stdio.h>

void ZEModelBone::OnTransformChanged()
{
	if (PhysicalBody != NULL)
	{
		PhysicalBody->SetPosition(GetWorldPosition());
		PhysicalBody->SetRotation(GetWorldRotation());
	}
}

const char* ZEModelBone::GetName()
{
	return BoneResource->Name;
}

ZEModelBone* ZEModelBone::GetParentBone()
{
	return ParentBone;
}

const ZEArray<ZEModelBone*>& ZEModelBone::GetChildBones()
{
	return ChildBones;
}

ZEPhysicalRigidBody* ZEModelBone::GetPhysicalBody()
{
	return PhysicalBody;
}

bool ZEModelBone::IsRootBone()
{
	return ParentBone == NULL;
}

const ZEAABBox& ZEModelBone::GetBoundingBox() const
{
	return BoneResource->BoundingBox;
}

const ZEAABBox& ZEModelBone::GetModelBoundingBox() const
{
	ZEAABBox::Transform(ModelBoundingBox, BoneResource->BoundingBox, GetModelTransform());

	return ModelBoundingBox;
}

const ZEAABBox& ZEModelBone::GetWorldBoundingBox() const
{
	ZEAABBox::Transform(WorldBoundingBox, BoneResource->BoundingBox, GetWorldTransform());

	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEModelBone::GetTransform() const
{
	ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation);

	return LocalTransform;
}

const ZEMatrix4x4& ZEModelBone::GetModelTransform() const
{
	if (ParentBone == NULL)
		return GetTransform();
	else
	{
		ZEMatrix4x4::Multiply(ModelTransform, ParentBone->GetModelTransform(), GetTransform());
		return ModelTransform;
	}
}

const ZEMatrix4x4& ZEModelBone::GetWorldTransform() const
{
	if (ParentBone == NULL)
	{
		ZEMatrix4x4::Multiply(WorldTransform, Owner->GetWorldTransform(), GetTransform());
		return WorldTransform;
	}
	else
	{
		ZEMatrix4x4::Multiply(WorldTransform, ParentBone->GetWorldTransform(), GetTransform());
		return WorldTransform;
	}
}

const ZEVector3& ZEModelBone::GetInitialPosition() const
{
	return BoneResource->Position;
}

const ZEQuaternion& ZEModelBone::GetInitialRotation() const
{
	return BoneResource->Rotation;
}

const ZEMatrix4x4& ZEModelBone::GetInitialTransform() const
{
	return BoneResource->LocalTransform;
}

const ZEMatrix4x4& ZEModelBone::GetForwardTransform() const
{
	return BoneResource->ForwardTransform;
}

const ZEMatrix4x4& ZEModelBone::GetInverseTransform() const
{
	return BoneResource->InverseTransform;
}

const ZEMatrix4x4& ZEModelBone::GetVertexTransform() const
{
	ZEMatrix4x4::Multiply(VertexTransform, GetModelTransform(), BoneResource->InverseTransform);
	return VertexTransform;
}

void ZEModelBone::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEModelBone::GetPosition() const
{
	return Position;
}

void ZEModelBone::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
}

const ZEQuaternion& ZEModelBone::GetRotation() const
{
	return Rotation;
}

void ZEModelBone::SetModelPosition(const ZEVector3& ModelPosition)
{
	if (ParentBone == NULL)
		SetPosition(ModelPosition);
	else
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, ParentBone->GetModelTransform().Inverse(), ModelPosition);
		SetPosition(Result);
	}
}

const ZEVector3 ZEModelBone::GetModelPosition() const
{
	if (ParentBone == NULL)
		return Position;
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentBone->GetModelTransform(), Position);
		return Temp;
	}
}

void ZEModelBone::SetModelRotation(const ZEQuaternion& ModelRotation)
{
	if (ParentBone == NULL)
		SetRotation(ModelRotation);
	else
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, ParentBone->GetModelRotation());
		ZEQuaternion::Product(Result, Temp, ModelRotation);
		SetRotation(Result);
	}
}

const ZEQuaternion ZEModelBone::GetModelRotation() const
{
	if (ParentBone == NULL)
		return Rotation;
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, ParentBone->GetModelRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
}

void ZEModelBone::SetWorldPosition(const ZEVector3& WorldPosition)
{
	if (ParentBone == NULL)
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, Owner->GetWorldTransform().Inverse(), WorldPosition);
		SetPosition(Result);
	}
	else
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, ParentBone->GetWorldTransform(), WorldPosition);
		SetPosition(Result);
	}
}

const ZEVector3 ZEModelBone::GetWorldPosition() const
{
	if (ParentBone == NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Owner->GetWorldTransform(), Position);
		return Temp;
	}
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentBone->GetWorldTransform(), Position);
		return Temp;
	}
}

void ZEModelBone::SetWorldRotation(const ZEQuaternion& WorldRotation)
{
	if (ParentBone == NULL)
	{	
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, Owner->GetWorldRotation());
		ZEQuaternion::Product(Result, Temp, WorldRotation);
		SetRotation(Result);
	}
	else
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, ParentBone->GetWorldRotation());
		ZEQuaternion::Product(Result, Temp, WorldRotation);
		SetRotation(Result);
	}
}

const ZEQuaternion ZEModelBone::GetWorldRotation() const
{
	if (ParentBone == NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Owner->GetWorldRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, ParentBone->GetWorldRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
}

void ZEModelBone::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this->AnimationType = AnimationType;
}

ZEModelAnimationType ZEModelBone::GetAnimationType() const
{
	return AnimationType;
}

void ZEModelBone::AddChild(ZEModelBone* Bone)
{
	Bone->ParentBone = this;
	ChildBones.Add(Bone);
}

void ZEModelBone::RemoveChild(ZEModelBone* Bone)
{
	Bone->ParentBone = NULL;
	ChildBones.DeleteValue(Bone);
}

void ZEModelBone::SetPhysicsEnabled(bool Enabled)
{
	PhysicsEnabled = Enabled;
}

bool ZEModelBone::GetPhysicsEnabled() const
{
	return PhysicsEnabled;
}

void ZEModelBone::Initialize(ZEModel* Model, const ZEModelResourceBone* BoneResource)
{
	Owner = Model;
	this->BoneResource = BoneResource;
	Position = BoneResource->Position;
	Rotation = BoneResource->Rotation;

	ZEArray<ZEPhysicalShape*> ShapeList;

	if(BoneResource->PhysicalBody.Type != ZE_MRPBT_NONE)
	{
		PhysicalBody = ZEPhysicalRigidBody::CreateInstance();

		PhysicalBody->SetEnabled(BoneResource->PhysicalBody.Enabled);
		PhysicalBody->SetPhysicalBodyType(BoneResource->PhysicalBody.IsKinematic ? ZE_PBT_KINEMATIC : ZE_PBT_DYNAMIC);
		PhysicalBody->SetMass(BoneResource->PhysicalBody.Mass);
		PhysicalBody->SetLinearDamping(BoneResource->PhysicalBody.LinearDamping);
		PhysicalBody->SetAngularDamping(BoneResource->PhysicalBody.AngularDamping);
		PhysicalBody->SetPosition(this->GetWorldPosition());
		PhysicalBody->SetRotation(this->GetWorldRotation());
		PhysicalBody->SetMassCenterPosition(BoneResource->PhysicalBody.MassCenter);
		PhysicalBody->SetTransformChangeEvent(ZEDelegate<void (ZEPhysicalObject*, ZEVector3, ZEQuaternion)>::Create<ZEModel, &ZEModel::TransformChangeEvent>(this->Owner));

		for (ZESize I = 0; I < BoneResource->PhysicalBody.Shapes.GetCount(); I++)
		{
			const ZEModelResourcePhysicalShape* Shape = &BoneResource->PhysicalBody.Shapes[I];
			switch(Shape->Type)
			{
				case ZE_PST_BOX:
				{
					ZEPhysicalBoxShape* BoxShape = new ZEPhysicalBoxShape();
					BoxShape->SetWidth(Shape->Box.Width / 2.0f);
					BoxShape->SetHeight(Shape->Box.Height / 2.0f);
					BoxShape->SetLength(Shape->Box.Length / 2.0f);
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

	if (BoneResource->PhysicalJoint.JointType != ZE_PJT_NONE && BoneResource->ParentBone != -1)
	{
		if (PhysicalJoint == NULL)
			PhysicalJoint = ZEPhysicalJoint::CreateInstance();
		else
			PhysicalJoint->Deinitialize();

		PhysicalJoint->SetBodyA(PhysicalBody);
		PhysicalJoint->SetBodyB(ParentBone->PhysicalBody);

		PhysicalJoint->SetEnabled(BoneResource->PhysicalJoint.Enabled);

		PhysicalJoint->SetPosition(PhysicalBody->GetPosition());
		PhysicalJoint->SetRotation(PhysicalBody->GetRotation());

		if(BoneResource->PhysicalJoint.Breakable)
		{
			PhysicalJoint->SetBreakForce(BoneResource->PhysicalJoint.BreakForce);
			PhysicalJoint->SetBreakTorque(BoneResource->PhysicalJoint.BreakTorque);
		}

		float MassInertia = PhysicalBody->GetMass() * 0.13f; // Optimum mass inertia tensor is (%13 mass)
		PhysicalJoint->SetMassInertiaTensor(ZEVector3(MassInertia, MassInertia, MassInertia));
		PhysicalJoint->SetBodiesCollide(BoneResource->PhysicalJoint.CollideBodies);

		PhysicalJoint->SetXMotion(BoneResource->PhysicalJoint.XMotion);
		PhysicalJoint->SetYMotion(BoneResource->PhysicalJoint.YMotion);
		PhysicalJoint->SetZMotion(BoneResource->PhysicalJoint.ZMotion);

		if(BoneResource->PhysicalJoint.XMotion != ZE_PJMOTION_LOCKED || BoneResource->PhysicalJoint.YMotion != ZE_PJMOTION_LOCKED || BoneResource->PhysicalJoint.ZMotion != ZE_PJMOTION_LOCKED)
		{
			PhysicalJoint->SetLinearLimitValue(BoneResource->PhysicalJoint.LinearLimitValue);
			PhysicalJoint->SetLinearLimitRestitution(BoneResource->PhysicalJoint.LinearLimitRestitution);
			PhysicalJoint->SetLinearLimitSpring(BoneResource->PhysicalJoint.LinearLimitSpring);
			PhysicalJoint->SetLinearLimitDamping(BoneResource->PhysicalJoint.LinearLimitDamping);
		}

		PhysicalJoint->SetSwing1Motion(BoneResource->PhysicalJoint.Swing1Motion);

		if (BoneResource->PhysicalJoint.Swing1Motion != ZE_PJMOTION_LOCKED)
		{
			PhysicalJoint->SetSwing1LimitValue(BoneResource->PhysicalJoint.Swing1LimitValue);
			PhysicalJoint->SetSwing1LimitRestitution(BoneResource->PhysicalJoint.Swing1LimitRestitution);
			PhysicalJoint->SetSwing1LimitSpring(BoneResource->PhysicalJoint.Swing1LimitSpring);
			PhysicalJoint->SetSwing1LimitDamping(BoneResource->PhysicalJoint.Swing1LimitDamping);
		}

		PhysicalJoint->SetSwing2Motion(BoneResource->PhysicalJoint.Swing2Motion);

		if (BoneResource->PhysicalJoint.Swing2Motion != ZE_PJMOTION_LOCKED)
		{
			PhysicalJoint->SetSwing2LimitValue(BoneResource->PhysicalJoint.Swing2LimitValue);
			PhysicalJoint->SetSwing2LimitRestitution(BoneResource->PhysicalJoint.Swing2LimitRestitution);
			PhysicalJoint->SetSwing2LimitSpring(BoneResource->PhysicalJoint.Swing2LimitSpring);
			PhysicalJoint->SetSwing2LimitDamping(BoneResource->PhysicalJoint.Swing2LimitDamping);
		}

		PhysicalJoint->SetTwistMotion(BoneResource->PhysicalJoint.TwistMotion);

		if (BoneResource->PhysicalJoint.TwistMotion != ZE_PJMOTION_LOCKED)
		{
			PhysicalJoint->SetTwistLowLimitValue(BoneResource->PhysicalJoint.TwistLowLimitValue);
			PhysicalJoint->SetTwistLowLimitRestitution(BoneResource->PhysicalJoint.TwistLowLimitRestitution);
			PhysicalJoint->SetTwistLowLimitSpring(BoneResource->PhysicalJoint.TwistLowLimitSpring);
			PhysicalJoint->SetTwistLowLimitDamping(BoneResource->PhysicalJoint.TwistLowLimitDamping);

			PhysicalJoint->SetTwistHighLimitValue(BoneResource->PhysicalJoint.TwistHighLimitValue);
			PhysicalJoint->SetTwistHighLimitRestitution(BoneResource->PhysicalJoint.TwistHighLimitRestitution);
			PhysicalJoint->SetTwistHighLimitSpring(BoneResource->PhysicalJoint.TwistHighLimitSpring);
			PhysicalJoint->SetTwistHighLimitDamping(BoneResource->PhysicalJoint.TwistHighLimitDamping);
		}

		PhysicalJoint->SetMotorTargetPosition(BoneResource->PhysicalJoint.MotorTargetPosition);
		PhysicalJoint->SetMotorTargetOrientation(BoneResource->PhysicalJoint.MotorTargetOrientation);
		PhysicalJoint->SetMotorTargetVelocity(BoneResource->PhysicalJoint.MotorTargetVelocity);
		PhysicalJoint->SetMotorTargetAngularVelocity(BoneResource->PhysicalJoint.MotorTargetAngularVelocity);

		if(BoneResource->PhysicalJoint.LinearXMotor != ZE_PJMT_NONE)
		{
		PhysicalJoint->SetLinearXMotor(BoneResource->PhysicalJoint.LinearXMotor);
		PhysicalJoint->SetLinearXMotorForce(BoneResource->PhysicalJoint.LinearXMotorForce);
		PhysicalJoint->SetLinearXMotorSpring(BoneResource->PhysicalJoint.LinearXMotorSpring);
		PhysicalJoint->SetLinearXMotorDamper(BoneResource->PhysicalJoint.LinearXMotorDamper);
		}

		if(BoneResource->PhysicalJoint.LinearYMotor != ZE_PJMT_NONE)
		{
		PhysicalJoint->SetLinearYMotor(BoneResource->PhysicalJoint.LinearYMotor);
		PhysicalJoint->SetLinearYMotorForce(BoneResource->PhysicalJoint.LinearYMotorForce);
		PhysicalJoint->SetLinearYMotorSpring(BoneResource->PhysicalJoint.LinearYMotorSpring);
		PhysicalJoint->SetLinearYMotorDamper(BoneResource->PhysicalJoint.LinearYMotorDamper);
		}

		if(BoneResource->PhysicalJoint.LinearZMotor != ZE_PJMT_NONE)
		{
		PhysicalJoint->SetLinearZMotor(BoneResource->PhysicalJoint.LinearZMotor);
		PhysicalJoint->SetLinearZMotorForce(BoneResource->PhysicalJoint.LinearZMotorForce);
		PhysicalJoint->SetLinearZMotorSpring(BoneResource->PhysicalJoint.LinearZMotorSpring);
		PhysicalJoint->SetLinearZMotorDamper(BoneResource->PhysicalJoint.LinearZMotorDamper);
		}

		if(BoneResource->PhysicalJoint.AngularSwingMotor != ZE_PJMT_NONE)
		{
		PhysicalJoint->SetAngularSwingMotor(BoneResource->PhysicalJoint.AngularSwingMotor);
		PhysicalJoint->SetAngularSwingMotorForce(BoneResource->PhysicalJoint.AngularSwingMotorForce);
		PhysicalJoint->SetAngularSwingMotorSpring(BoneResource->PhysicalJoint.AngularSwingMotorSpring);
		PhysicalJoint->SetAngularSwingMotorDamper(BoneResource->PhysicalJoint.AngularSwingMotorDamper);
		}

		if(BoneResource->PhysicalJoint.AngularTwistMotor != ZE_PJMT_NONE)
		{
		PhysicalJoint->SetAngularTwistMotor(BoneResource->PhysicalJoint.AngularTwistMotor);
			PhysicalJoint->SetAngularTwistMotorForce(BoneResource->PhysicalJoint.AngularTwistMotorForce);
		PhysicalJoint->SetAngularTwistMotorSpring(BoneResource->PhysicalJoint.AngularTwistMotorSpring);
			PhysicalJoint->SetAngularTwistMotorDamper(BoneResource->PhysicalJoint.AngularTwistMotorDamper);
		}

		if(BoneResource->PhysicalJoint.AngularSlerpMotor != ZE_PJMT_NONE)
		{
			PhysicalJoint->SetAngularSlerpMotor(BoneResource->PhysicalJoint.AngularSlerpMotor);
			PhysicalJoint->SetAngularSlerpMotorForce(BoneResource->PhysicalJoint.AngularSlerpMotorForce);
			PhysicalJoint->SetAngularSlerpMotorSpring(BoneResource->PhysicalJoint.AngularSlerpMotorSpring);
			PhysicalJoint->SetAngularSlerpMotorDamper(BoneResource->PhysicalJoint.AngularSlerpMotorDamper);
		}

		PhysicalJoint->SetPhysicalWorld(zeScene->GetPhysicalWorld());
		PhysicalJoint->Initialize();
	}

// 	for (ZESize I = 0; I < ShapeList.GetCount(); I++)
// 	{
// 		delete ShapeList[I];
// 	}
// 
// 	ShapeList.Clear();
}

void ZEModelBone::Deinitialize()
{
	Owner = NULL;
	ParentBone = NULL;
	if (PhysicalBody != NULL)
	{
		PhysicalBody->Destroy();
		PhysicalBody = NULL;
	}

	if (PhysicalJoint != NULL)
	{
		PhysicalJoint->Destroy();
		PhysicalJoint = NULL;
	}

	ChildBones.Clear();
}

ZEModelBone::ZEModelBone()
{
	Owner = NULL;
	ParentBone = NULL;
	PhysicalJoint = NULL;
	PhysicalBody = NULL;
	BoneResource = NULL;
	PhysicsEnabled = false;
	AnimationType = ZE_MAT_NOANIMATION;
}

ZEModelBone::~ZEModelBone()
{
	Deinitialize();
}
