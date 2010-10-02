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
#include "ZECore\ZECore.h"

const char* ZEModelBone::GetName()
{
	return BoneResource->Name;
}

ZEModelBone* ZEModelBone::GetParentBone()
{
	return ParentBone;
}

const ZEArray<ZEModelBone*> ZEModelBone::GetChildBones()
{
	return ChildBones;
}

bool ZEModelBone::IsRootBone()
{
	return ParentBone == NULL;
}

const ZEAABoundingBox& ZEModelBone::GetLocalBoundingBox()
{
	return BoneResource->BoundingBox;
}

const ZEAABoundingBox& ZEModelBone::GetModelBoundingBox()
{
	if (UpdateModelBoundingBox)
	{
		ZEAABoundingBox::Transform(ModelBoundingBox, BoneResource->BoundingBox, GetModelTransform());
		UpdateModelBoundingBox = false;
	}
	return ModelBoundingBox;
}

const ZEAABoundingBox& ZEModelBone::GetWorldBoundingBox()
{
	if (UpdateWorldBoundingBox)
	{
		ZEAABoundingBox::Transform(WorldBoundingBox, BoneResource->BoundingBox, GetWorldTransform());
		UpdateWorldBoundingBox = false;
	}
	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEModelBone::GetInverseTransform()
{
	return BoneResource->InverseTransform;
}

const ZEMatrix4x4& ZEModelBone::GetLocalTransform()
{
	//if (UpdateLocalTransform)
	{
		if (ParentBone == NULL)
			LocalTransform = GetRelativeTransform();
		else
		{
			ZEMatrix4x4::Multiply(LocalTransform, GetRelativeTransform(), ParentBone->GetLocalTransform());
			return LocalTransform;
		}
		UpdateLocalTransform = false;
	}
	return LocalTransform;
}

const ZEMatrix4x4& ZEModelBone::GetWorldTransform()
{
//	if (UpdateWorldTransform)
	{
		ZEMatrix4x4::Multiply(WorldTransform, GetLocalTransform(), Owner->GetWorldTransform());
		UpdateWorldTransform = false;
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEModelBone::GetModelTransform()
{
	//if (UpdateWorldTransform)
	{
		ZEMatrix4x4::Multiply(WorldTransform, Owner->GetLocalTransform(), GetLocalTransform());
		UpdateWorldTransform = false;
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEModelBone::GetVertexTransform()
{
//	if (UpdateVertexTransform)
	{
		ZEMatrix4x4::Multiply(VertexTransform, BoneResource->InverseTransform, GetLocalTransform());
		UpdateVertexTransform = false;
	}

	return VertexTransform;
}

const ZEMatrix4x4& ZEModelBone::GetRelativeTransform()
{
//	if (UpdateRelativeTransform)
	{
		ZEMatrix4x4 Temp1, Temp2;
		ZEMatrix4x4::CreateTranslation(Temp1, RelativePosition);
		ZEMatrix4x4::CreateRotation(Temp2, RelativeRotation);
		ZEMatrix4x4::Multiply(RelativeTransform, Temp2, Temp1);

		UpdateRelativeTransform = false;
	}
	return RelativeTransform;
}

const ZEVector3& ZEModelBone::GetRelativePosition()
{
	return RelativePosition;
}

void ZEModelBone::SetRelativePosition(const ZEVector3& Position)
{
	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
	Owner->UpdateBoundingBox();

	RelativePosition = Position;
}

const ZEQuaternion& ZEModelBone::GetRelativeRotation()
{
	return RelativeRotation;
}

void ZEModelBone::SetRelativeRotation(const ZEQuaternion& Rotation)
{
	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
	Owner->UpdateBoundingBox();
	Owner->UpdateBoneTransforms();
	RelativeRotation = Rotation;
}

const ZEVector3 ZEModelBone::GetModelPosition()
{
	if (ParentBone == NULL)
		return RelativePosition;
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentBone->GetModelTransform(), RelativePosition);
		return Temp;
	}
}

const ZEQuaternion ZEModelBone::GetModelRotation()
{
	if (ParentBone == NULL)
		return RelativeRotation;
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, RelativeRotation, ParentBone->GetModelRotation());
		return Temp;
	}
}

const ZEVector3 ZEModelBone::GetWorldPosition()
{
	if (ParentBone == NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Owner->GetWorldTransform(), RelativePosition);
		return Temp;
	}
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentBone->GetWorldTransform(), RelativePosition);
		return Temp;
	}
}

const ZEQuaternion ZEModelBone::GetWorldRotation()
{
	if (ParentBone == NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, RelativeRotation, Owner->GetWorldRotation());
		return Temp;
	}
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, RelativeRotation, ParentBone->GetWorldRotation());
		return Temp;
	}
}

void ZEModelBone::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this->AnimationType = AnimationType;
}

ZEModelAnimationType ZEModelBone::GetAnimationType()
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

bool ZEModelBone::GetPhysicsEnabled()
{
	return PhysicsEnabled;
}

void ZEModelBone::Initialize(ZEModel* Model, const ZEModelResourceBone* BoneResource)
{
	Owner = Model;
	this->BoneResource = BoneResource;
	RelativePosition = BoneResource->RelativePosition;
	RelativeRotation = BoneResource->RelativeRotation;

	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;

	if(PhysicalBody == NULL)
	{
		PhysicalBody = ZEPhysicalRigidBody::CreateInstance();

		PhysicalBody->SetEnabled(BoneResource->PhysicalBody.Enabled);
		PhysicalBody->SetMass(BoneResource->PhysicalBody.Mass);
		PhysicalBody->SetLinearDamping(BoneResource->PhysicalBody.LinearDamping);
		PhysicalBody->SetAngularDamping(BoneResource->PhysicalBody.AngularDamping);
		PhysicalBody->SetPosition(this->GetWorldPosition());
		PhysicalBody->SetRotation(this->GetWorldRotation());
		PhysicalBody->SetMassCenterPosition(BoneResource->PhysicalBody.MassCenter);

		for (size_t I = 0; I < BoneResource->PhysicalBody.Shapes.GetCount(); I++)
		{
			const ZEModelResourcePhysicalShape* Shape = &BoneResource->PhysicalBody.Shapes[I];
			switch(Shape->Type)
			{
				case ZE_PST_BOX:
				{
					ZEPhysicalBoxShape BoxShape;
					BoxShape.SetWidth(Shape->Box.Width);
					BoxShape.SetHeight(Shape->Box.Height);
					BoxShape.SetLength(Shape->Box.Length);
					BoxShape.SetPosition(Shape->Position);
					BoxShape.SetRotation(Shape->Rotation);
					PhysicalBody->AddPhysicalShape(&BoxShape);
					break;
				}

				case ZE_PST_SPHERE:
				{
					ZEPhysicalSphereShape SphereShape;
					SphereShape.SetRadius(Shape->Sphere.Radius);
					SphereShape.SetPosition(Shape->Position);
					SphereShape.SetRotation(Shape->Rotation);
					PhysicalBody->AddPhysicalShape(&SphereShape);
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

		PhysicalJoint->SetPosition(GetWorldPosition());

		ZEQuaternion TempRotation;
		ZEQuaternion::Create(TempRotation, ZE_PI / 2, ZEVector3(0.0f, 0.0f, -1.0f));

		PhysicalJoint->SetRotation(TempRotation);

		//PhysicalJoint->SetBodyAConnectionPosition(GetWorldPosition());
		//PhysicalJoint->SetBodyAConnectionOrientation(GetWorldRotation());

		//PhysicalJoint->SetBodyBConnectionPosition(GetWorldPosition());
		//PhysicalJoint->SetBodyBConnectionOrientation(GetWorldRotation());

		if(BoneResource->PhysicalJoint.Breakable)
		{
			PhysicalJoint->SetBreakForce(BoneResource->PhysicalJoint.BreakForce);
			PhysicalJoint->SetBreakTorque(BoneResource->PhysicalJoint.BreakTorque);
		}

		PhysicalJoint->SetXMotion(BoneResource->PhysicalJoint.XMotion);
		PhysicalJoint->SetYMotion(BoneResource->PhysicalJoint.YMotion);
		PhysicalJoint->SetZMotion(BoneResource->PhysicalJoint.ZMotion);

		if(BoneResource->PhysicalJoint.XMotion != ZEPhysicalJointMotion::ZE_PJMOTION_LOCKED || BoneResource->PhysicalJoint.YMotion != ZEPhysicalJointMotion::ZE_PJMOTION_LOCKED || BoneResource->PhysicalJoint.ZMotion != ZEPhysicalJointMotion::ZE_PJMOTION_LOCKED)
		{
			PhysicalJoint->SetLinearLimitValue(BoneResource->PhysicalJoint.LinearLimitValue);
			PhysicalJoint->SetLinearLimitRestitution(BoneResource->PhysicalJoint.LinearLimitRestitution);
			PhysicalJoint->SetLinearLimitSpring(BoneResource->PhysicalJoint.LinearLimitSpring);
			PhysicalJoint->SetLinearLimitDamping(BoneResource->PhysicalJoint.LinearLimitDamping);
		}

		PhysicalJoint->SetSwing1Motion(BoneResource->PhysicalJoint.Swing1Motion);

		if (BoneResource->PhysicalJoint.Swing1Motion != ZEPhysicalJointMotion::ZE_PJMOTION_LOCKED)
		{
			PhysicalJoint->SetSwing1LimitValue(BoneResource->PhysicalJoint.Swing1LimitValue);
			PhysicalJoint->SetSwing1LimitRestitution(BoneResource->PhysicalJoint.Swing1LimitRestitution);
			PhysicalJoint->SetSwing1LimitSpring(BoneResource->PhysicalJoint.Swing1LimitSpring);
			PhysicalJoint->SetSwing1LimitDamping(BoneResource->PhysicalJoint.Swing1LimitDamping);
		}

		PhysicalJoint->SetSwing2Motion(BoneResource->PhysicalJoint.Swing2Motion);

		if (BoneResource->PhysicalJoint.Swing2Motion != ZEPhysicalJointMotion::ZE_PJMOTION_LOCKED)
		{
			PhysicalJoint->SetSwing2LimitValue(BoneResource->PhysicalJoint.Swing2LimitValue);
			PhysicalJoint->SetSwing2LimitRestitution(BoneResource->PhysicalJoint.Swing2LimitRestitution);
			PhysicalJoint->SetSwing2LimitSpring(BoneResource->PhysicalJoint.Swing2LimitSpring);
			PhysicalJoint->SetSwing2LimitDamping(BoneResource->PhysicalJoint.Swing2LimitDamping);
		}

		PhysicalJoint->SetTwistMotion(BoneResource->PhysicalJoint.TwistMotion);

		if (BoneResource->PhysicalJoint.TwistMotion != ZEPhysicalJointMotion::ZE_PJMOTION_LOCKED)
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

		if(BoneResource->PhysicalJoint.LinearXMotor != ZEPhysicalJointMotorType::ZE_PJMT_NONE)
		{
		PhysicalJoint->SetLinearXMotor(BoneResource->PhysicalJoint.LinearXMotor);
		PhysicalJoint->SetLinearXMotorForce(BoneResource->PhysicalJoint.LinearXMotorForce);
		PhysicalJoint->SetLinearXMotorSpring(BoneResource->PhysicalJoint.LinearXMotorSpring);
		PhysicalJoint->SetLinearXMotorDamper(BoneResource->PhysicalJoint.LinearXMotorDamper);
		}

		if(BoneResource->PhysicalJoint.LinearYMotor != ZEPhysicalJointMotorType::ZE_PJMT_NONE)
		{
		PhysicalJoint->SetLinearYMotor(BoneResource->PhysicalJoint.LinearYMotor);
		PhysicalJoint->SetLinearYMotorForce(BoneResource->PhysicalJoint.LinearYMotorForce);
		PhysicalJoint->SetLinearYMotorSpring(BoneResource->PhysicalJoint.LinearYMotorSpring);
		PhysicalJoint->SetLinearYMotorDamper(BoneResource->PhysicalJoint.LinearYMotorDamper);
		}

		if(BoneResource->PhysicalJoint.LinearZMotor != ZEPhysicalJointMotorType::ZE_PJMT_NONE)
		{
		PhysicalJoint->SetLinearZMotor(BoneResource->PhysicalJoint.LinearZMotor);
		PhysicalJoint->SetLinearZMotorForce(BoneResource->PhysicalJoint.LinearZMotorForce);
		PhysicalJoint->SetLinearZMotorSpring(BoneResource->PhysicalJoint.LinearZMotorSpring);
		PhysicalJoint->SetLinearZMotorDamper(BoneResource->PhysicalJoint.LinearZMotorDamper);
		}

		if(BoneResource->PhysicalJoint.AngularSwingMotor != ZEPhysicalJointMotorType::ZE_PJMT_NONE)
		{
		PhysicalJoint->SetAngularSwingMotor(BoneResource->PhysicalJoint.AngularSwingMotor);
		PhysicalJoint->SetAngularSwingMotorForce(BoneResource->PhysicalJoint.AngularSwingMotorForce);
		PhysicalJoint->SetAngularSwingMotorSpring(BoneResource->PhysicalJoint.AngularSwingMotorSpring);
		PhysicalJoint->SetAngularSwingMotorDamper(BoneResource->PhysicalJoint.AngularSwingMotorDamper);
		}

		if(BoneResource->PhysicalJoint.AngularTwistMotor != ZEPhysicalJointMotorType::ZE_PJMT_NONE)
		{
		PhysicalJoint->SetAngularTwistMotor(BoneResource->PhysicalJoint.AngularTwistMotor);
		PhysicalJoint->SetAngularTwistMotorForce(BoneResource->PhysicalJoint.AngularTwistMotorForce);
		PhysicalJoint->SetAngularTwistMotorSpring(BoneResource->PhysicalJoint.AngularTwistMotorSpring);
		PhysicalJoint->SetAngularTwistMotorDamper(BoneResource->PhysicalJoint.AngularTwistMotorDamper);
		}

		if(BoneResource->PhysicalJoint.AngularSlerpMotor != ZEPhysicalJointMotorType::ZE_PJMT_NONE)
		{
		PhysicalJoint->SetAngularSlerpMotor(BoneResource->PhysicalJoint.AngularSlerpMotor);
		PhysicalJoint->SetAngularSlerpMotorForce(BoneResource->PhysicalJoint.AngularSlerpMotorForce);
		PhysicalJoint->SetAngularSlerpMotorSpring(BoneResource->PhysicalJoint.AngularSlerpMotorSpring);
		PhysicalJoint->SetAngularSlerpMotorDamper(BoneResource->PhysicalJoint.AngularSlerpMotorDamper);
		}

		PhysicalJoint->SetPhysicalWorld(zeScene->GetPhysicalWorld());
		PhysicalJoint->Initialize();
	}
	Owner->UpdateBoneTransforms();
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

	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;

	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
}

void ZEModelBone::ModelWorldTransformChanged()
{
	UpdateVertexTransform = true;
	UpdateWorldTransform = true;
	UpdateWorldBoundingBox = true;
}

void ZEModelBone::ModelTransformChanged()
{
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
}

ZEModelBone::ZEModelBone()
{
	Owner = NULL;
	ParentBone = NULL;
	PhysicalJoint = NULL;
	PhysicalBody = NULL;

	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;

	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
}

ZEModelBone::~ZEModelBone()
{
	Deinitialize();
}




