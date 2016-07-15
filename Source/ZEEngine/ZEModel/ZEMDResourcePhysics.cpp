//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourcePhysics.cpp
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

#include "ZEMDResourcePhysics.h"
#include "ZEML\ZEMLReader.h"


// ZEMDResourcePhysicalShape
/////////////////////////////////////////////////////////////////////////////

void ZEModelResourcePhysicalShape::SetType(ZEModelResourcePhysicalShapeType Type)
{
	this->Type = Type;
}

ZEModelResourcePhysicalShapeType ZEModelResourcePhysicalShape::GetType() const
{
	return Type;
}

void ZEModelResourcePhysicalShape::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEModelResourcePhysicalShape::GetPosition() const
{
	return Position;
}

void ZEModelResourcePhysicalShape::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
}

const ZEQuaternion& ZEModelResourcePhysicalShape::GetRotation() const
{
	return Rotation;
}

void ZEModelResourcePhysicalShape::SetRestitution(float Restitution)
{
	this->Restitution = Restitution;
}

float ZEModelResourcePhysicalShape::GetRestitution() const 
{
	return Restitution;
}

void ZEModelResourcePhysicalShape::SetStaticFriction(float StaticFriction)
{
	this->StaticFriction = StaticFriction;
}

float ZEModelResourcePhysicalShape::GetStaticFriction() const 
{
	return StaticFriction;
}

void ZEModelResourcePhysicalShape::SetDynamicFriction(float DynamicFriction)
{
	this->DynamicFriction = DynamicFriction;
}

float ZEModelResourcePhysicalShape::GetDynamicFriction() const 
{
	return DynamicFriction;
}

void ZEModelResourcePhysicalShape::SetWidth(float Width)
{
	this->Width = Width;
}

float ZEModelResourcePhysicalShape::GetWidth() const 
{
	return Width;
}

void ZEModelResourcePhysicalShape::SetHeight(float Height)
{
	this->Height = Height;
}

float ZEModelResourcePhysicalShape::GetHeight() const 
{
	return Height;
}

void ZEModelResourcePhysicalShape::SetLength(float Length)
{
	this->Length = Length;
}

float ZEModelResourcePhysicalShape::GetLength() const 
{
	return Length;
}

void ZEModelResourcePhysicalShape::SetRadius(float Radius)
{
	this->Radius = Radius;
}

float ZEModelResourcePhysicalShape::GetRadius() const 
{
	return Radius;
}

void ZEModelResourcePhysicalShape::SetVertices(const ZEArray<ZEVector3>& Vertices)
{
	this->Vertices = Vertices;
}

const ZEArray<ZEVector3> ZEModelResourcePhysicalShape::GetVertices()
{
	return Vertices;
}

void ZEModelResourcePhysicalShape::SetIndices(const ZEArray<ZEUInt16>& Indices)
{
	this->Indices = Indices;
}

const ZEArray<ZEUInt16> ZEModelResourcePhysicalShape::GetIndices()
{
	return Indices;
}

void ZEModelResourcePhysicalShape::SetUserDefinedProperties(ZEString UserDefinedProperties)
{
	this->UserDefinedProperties = UserDefinedProperties;
}

ZEString ZEModelResourcePhysicalShape::GetUserDefinedProperties() const 
{
	return UserDefinedProperties;
}

bool ZEModelResourcePhysicalShape::Load(const ZEMLReaderNode& ShapeNode)
{
	zeCheckError(!ShapeNode.IsValid(), false, "Invalid Shape node.");
	zeCheckError(ShapeNode.GetName() != "Shape", false, "Invalid Shape node name.");

	SetPosition(ShapeNode.ReadVector3("Position", ZEVector3::Zero));
	SetRotation(ShapeNode.ReadQuaternion("Rotation", ZEQuaternion::Identity));
	SetRestitution(ShapeNode.ReadFloat("Restitution", 0.0f));
	SetDynamicFriction(ShapeNode.ReadFloat("DynamicFriction", 0.0f));
	SetStaticFriction(ShapeNode.ReadFloat("StaticFriction"));
	SetType((ZEModelResourcePhysicalShapeType)ShapeNode.ReadInt32("Type", ZE_MRPST_BOX));
	SetUserDefinedProperties(ShapeNode.ReadString("UserDefinedProperties"));
	SetWidth(ShapeNode.ReadFloat("Width", 0.0f));
	SetHeight(ShapeNode.ReadFloat("Height", 0.0f));
	SetLength(ShapeNode.ReadFloat("Length", 0.0f));
	SetRadius(ShapeNode.ReadFloat("Radius", 0.0f));

	if (GetType() == ZE_MRPST_CONVEX)
	{
		if (!ShapeNode.IsPropertyExists("Vertices"))
		{
			zeError("Convex shape does not contains Vertices node.");
			return false;
		}

		if (!ShapeNode.IsPropertyExists("Indices"))
		{
			zeError("Convex shape does not contains Indices node.");
			return false;
		}

		Vertices.SetCount(ShapeNode.ReadDataSize("Vertices") / sizeof(ZEVector3));
		if (!ShapeNode.ReadDataItems("Vertices", Vertices.GetCArray(), sizeof(ZEVector3), Vertices.GetCount()))
		{
			zeError("Cannot read convex shape vertices.");
			return false;
		}

		Vertices.SetCount(ShapeNode.ReadDataSize("Indices") / sizeof(ZEVector3));
		if (!ShapeNode.ReadDataItems("Indices", Vertices.GetCArray(), sizeof(ZEVector3), Vertices.GetCount()))
		{
			zeError("Cannot read convex shape vertex indices.");
			return false;
		}
	}

	return true;
}

bool ZEModelResourcePhysicalShape::Save(ZEMLWriterNode& ShapeNode) const
{
	return false;
}

ZEModelResourcePhysicalShape::ZEModelResourcePhysicalShape()
{
	Type = ZE_MRPST_BOX;
	Position = ZEVector3::One;
	Rotation = ZEQuaternion::Identity;
	Restitution = 0.0f;
	StaticFriction = 0.0f;
	DynamicFriction = 0.0f;

	Width = 0.0f;
	Height = 0.0f;
	Length = 0.0f;
	Radius = 0.0f;
}


// ZEMDResourcePhysicalBody
/////////////////////////////////////////////////////////////////////////////

void ZEModelResourcePhysicalBody::SetType(ZEModelResourcePhysicalBodyType Type)
{
	this->Type = Type;
}

ZEModelResourcePhysicalBodyType ZEModelResourcePhysicalBody::GetType() const 
{
	return Type;
}

void ZEModelResourcePhysicalBody::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEModelResourcePhysicalBody::GetPosition() const 
{
	return Position;
}

void ZEModelResourcePhysicalBody::SetOrientation(const ZEQuaternion& Orientation)
{
	this->Orientation = Orientation;
}

const ZEQuaternion& ZEModelResourcePhysicalBody::GetOrientation() const 
{
	return Orientation;
}

void ZEModelResourcePhysicalBody::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEModelResourcePhysicalBody::GetEnabled() const 
{
	return Enabled;
}

void ZEModelResourcePhysicalBody::SetKinematic(bool Kinematic)
{
	this->Kinematic = Kinematic;
}

bool ZEModelResourcePhysicalBody::GetKinematic() const 
{
	return Kinematic;
}

void ZEModelResourcePhysicalBody::SetMass(float Mass)
{
	this->Mass = Mass;
}

float ZEModelResourcePhysicalBody::GetMass() const 
{
	return Mass;
}

void ZEModelResourcePhysicalBody::SetCenterOfMass(const ZEVector3& CenterOfMass)
{
	this->CenterOfMass = CenterOfMass;
}

const ZEVector3& ZEModelResourcePhysicalBody::GetCenterOfMass() const 
{
	return CenterOfMass;
}

void ZEModelResourcePhysicalBody::SetLinearDamping(float LinearDamping)
{
	this->LinearDamping = LinearDamping;
}

float ZEModelResourcePhysicalBody::GetLinearDamping() const 
{
	return LinearDamping;
}

void ZEModelResourcePhysicalBody::SetAngularDamping(float AngularDamping)
{
	this->AngularDamping = AngularDamping;
}

float ZEModelResourcePhysicalBody::GetAngularDamping() const 
{
	return AngularDamping;
}

const ZEArray<ZEModelResourcePhysicalShape>& ZEModelResourcePhysicalBody::GetShapes() const
{
	return Shapes;
}

void ZEModelResourcePhysicalBody::AddShape(const ZEModelResourcePhysicalShape& Shape)
{
	Shapes.Add(Shape);
}

void ZEModelResourcePhysicalBody::RemoveShape(ZESize Index)
{
	Shapes.Remove(Index);
}

bool ZEModelResourcePhysicalBody::Load(const ZEMLReaderNode& BodyNode)
{
	zeCheckError(!BodyNode.IsValid(), false, "Invalid Body node.");
	zeCheckError(BodyNode.GetName() != "PhysicalBody", false, "Invalid Body node name.");

	SetEnabled(BodyNode.ReadBoolean("Enabled"));
	SetType((ZEModelResourcePhysicalBodyType)BodyNode.ReadInt32("Type"));
	SetKinematic(BodyNode.ReadBoolean("IsKinematic"));
	SetMass(BodyNode.ReadFloat("Mass"));
	SetCenterOfMass(BodyNode.ReadVector3("MassCenter"));
	SetLinearDamping(BodyNode.ReadFloat("LinearDamping"));
	SetAngularDamping(BodyNode.ReadFloat("AngularDamping"));

	ZEMLReaderNode ShapesNode = BodyNode.GetNode("PhysicalShapes");

	if (!ShapesNode.IsValid())
		return false;

	ZESize ShapeCount = ShapesNode.GetNodeCount("PhysicalShape");
	for (ZESize I = 0; I < ShapeCount; I++)
	{
		ZEMLReaderNode ShapesNode = ShapesNode.GetNode("PhysicalShape", I);
		ZEModelResourcePhysicalShape Shape;
		if (!Shape.Load(ShapesNode))
			return false;
		AddShape(Shape);
	}

	return true;
}

bool ZEModelResourcePhysicalBody::Save(ZEMLWriterNode& BodyNode) const
{
	return false;
}

ZEModelResourcePhysicalBody::ZEModelResourcePhysicalBody()
{
	Type = ZE_MRPBT_NONE;
	Enabled = true;
	Kinematic = false;
	Mass = 1.0f;
	LinearDamping = 0.0f;
	AngularDamping = 0.0f;
	Position = ZEVector3::Zero;
	Orientation = ZEQuaternion::Identity;
	CenterOfMass = ZEVector3::Zero;
}


// ZEMDResourcePhysicalBody
/////////////////////////////////////////////////////////////////////////////

void ZEModelResourcePhysicalJoint::SetJointType(ZEPhysicalJointType JointType)
{
	this->JointType = JointType;
}

ZEPhysicalJointType ZEModelResourcePhysicalJoint::GetJointType() const 
{
	return JointType;
}

void ZEModelResourcePhysicalJoint::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEModelResourcePhysicalJoint::GetEnabled() const 
{
	return Enabled;
}

void ZEModelResourcePhysicalJoint::SetBody1Id(ZEUInt32 Body1Id)
{
	this->Body1Id = Body1Id;
}

ZEUInt32 ZEModelResourcePhysicalJoint::GetBody1Id() const 
{
	return Body1Id;
}

void ZEModelResourcePhysicalJoint::SetBody2Id(ZEUInt32 Body2Id)
{
	this->Body2Id = Body2Id;
}

ZEUInt32 ZEModelResourcePhysicalJoint::GetBody2Id() const 
{
	return Body2Id;
}

void ZEModelResourcePhysicalJoint::SetCollideBodies(bool CollideBodies)
{
	this->CollideBodies = CollideBodies;
}

bool ZEModelResourcePhysicalJoint::GetCollideBodies() const 
{
	return CollideBodies;
}

void ZEModelResourcePhysicalJoint::SetUseGlobalAnchorAxis(bool UseGlobalAnchorAxis)
{
	this->UseGlobalAnchorAxis = UseGlobalAnchorAxis;
}

bool ZEModelResourcePhysicalJoint::GetUseGlobalAnchorAxis() const 
{
	return UseGlobalAnchorAxis;
}

void ZEModelResourcePhysicalJoint::SetGlobalAnchor(const ZEVector3& GlobalAnchor)
{
	this->GlobalAnchor = GlobalAnchor;
}

const ZEVector3& ZEModelResourcePhysicalJoint::GetGlobalAnchor() const 
{
	return GlobalAnchor;
}

void ZEModelResourcePhysicalJoint::SetGlobalAxis(const ZEQuaternion& GlobalAxis)
{
	this->GlobalAxis = GlobalAxis;
}

const ZEQuaternion& ZEModelResourcePhysicalJoint::GetGlobalAxis() const 
{
	return GlobalAxis;
}

void ZEModelResourcePhysicalJoint::SetLocalAnchor1(const ZEVector3& LocalAnchor1)
{
	this->LocalAnchor1 = LocalAnchor1;
}

const ZEVector3& ZEModelResourcePhysicalJoint::GetLocalAnchor1() const 
{
	return LocalAnchor1;
}

void ZEModelResourcePhysicalJoint::SetLocalAnchor2(const ZEVector3& LocalAnchor2)
{
	this->LocalAnchor2 = LocalAnchor2;
}

const ZEVector3& ZEModelResourcePhysicalJoint::GetLocalAnchor2() const 
{
	return LocalAnchor2;
}

void ZEModelResourcePhysicalJoint::SetLocalAxis1(const ZEQuaternion& LocalAxis1)
{
	this->LocalAxis1 = LocalAxis1;
}

const ZEQuaternion& ZEModelResourcePhysicalJoint::GetLocalAxis1() const 
{
	return LocalAxis1;
}

void ZEModelResourcePhysicalJoint::SetLocalAxis2(const ZEQuaternion& LocalAxis2)
{
	this->LocalAxis2 = LocalAxis2;
}

const ZEQuaternion& ZEModelResourcePhysicalJoint::GetLocalAxis2() const 
{
	return LocalAxis2;
}

void ZEModelResourcePhysicalJoint::SetBreakable(bool Breakable)
{
	this->Breakable = Breakable;
}

bool ZEModelResourcePhysicalJoint::GetBreakable() const 
{
	return Breakable;
}

void ZEModelResourcePhysicalJoint::SetBreakForce(float BreakForce)
{
	this->BreakForce = BreakForce;
}

float ZEModelResourcePhysicalJoint::GetBreakForce() const 
{
	return BreakForce;
}

void ZEModelResourcePhysicalJoint::SetBreakTorque(float BreakTorque)
{
	this->BreakTorque = BreakTorque;
}

float ZEModelResourcePhysicalJoint::GetBreakTorque() const 
{
	return BreakTorque;
}

void ZEModelResourcePhysicalJoint::SetXMotion(ZEPhysicalJointMotion XMotion)
{
	this->XMotion = XMotion;
}

ZEPhysicalJointMotion ZEModelResourcePhysicalJoint::GetXMotion() const 
{
	return XMotion;
}

void ZEModelResourcePhysicalJoint::SetYMotion(ZEPhysicalJointMotion YMotion)
{
	this->YMotion = YMotion;
}

ZEPhysicalJointMotion ZEModelResourcePhysicalJoint::GetYMotion() const 
{
	return YMotion;
}

void ZEModelResourcePhysicalJoint::SetZMotion(ZEPhysicalJointMotion ZMotion)
{
	this->ZMotion = ZMotion;
}

ZEPhysicalJointMotion ZEModelResourcePhysicalJoint::GetZMotion() const 
{
	return ZMotion;
}

void ZEModelResourcePhysicalJoint::SetLinearLimitValue(float LinearLimitValue)
{
	this->LinearLimitValue = LinearLimitValue;
}

float ZEModelResourcePhysicalJoint::GetLinearLimitValue() const 
{
	return LinearLimitValue;
}

void ZEModelResourcePhysicalJoint::SetLinearLimitRestitution(float LinearLimitRestitution)
{
	this->LinearLimitRestitution = LinearLimitRestitution;
}

float ZEModelResourcePhysicalJoint::GetLinearLimitRestitution() const 
{
	return LinearLimitRestitution;
}

void ZEModelResourcePhysicalJoint::SetLinearLimitSpring(float LinearLimitSpring)
{
	this->LinearLimitSpring = LinearLimitSpring;
}

float ZEModelResourcePhysicalJoint::GetLinearLimitSpring() const 
{
	return LinearLimitSpring;
}

void ZEModelResourcePhysicalJoint::SetLinearLimitDamping(float LinearLimitDamping)
{
	this->LinearLimitDamping = LinearLimitDamping;
}

float ZEModelResourcePhysicalJoint::GetLinearLimitDamping() const 
{
	return LinearLimitDamping;
}

void ZEModelResourcePhysicalJoint::SetTwistMotion(ZEPhysicalJointMotion TwistMotion)
{
	this->TwistMotion = TwistMotion;
}

ZEPhysicalJointMotion ZEModelResourcePhysicalJoint::GetTwistMotion() const 
{
	return TwistMotion;
}

void ZEModelResourcePhysicalJoint::SetTwistLowLimitValue(float TwistLowLimitValue)
{
	this->TwistLowLimitValue = TwistLowLimitValue;
}

float ZEModelResourcePhysicalJoint::GetTwistLowLimitValue() const 
{
	return TwistLowLimitValue;
}

void ZEModelResourcePhysicalJoint::SetTwistLowLimitRestitution(float TwistLowLimitRestitution)
{
	this->TwistLowLimitRestitution = TwistLowLimitRestitution;
}

float ZEModelResourcePhysicalJoint::GetTwistLowLimitRestitution() const 
{
	return TwistLowLimitRestitution;
}

void ZEModelResourcePhysicalJoint::SetTwistLowLimitSpring(float TwistLowLimitSpring)
{
	this->TwistLowLimitSpring = TwistLowLimitSpring;
}

float ZEModelResourcePhysicalJoint::GetTwistLowLimitSpring() const 
{
	return TwistLowLimitSpring;
}

void ZEModelResourcePhysicalJoint::SetTwistLowLimitDamping(float TwistLowLimitDamping)
{
	this->TwistLowLimitDamping = TwistLowLimitDamping;
}

float ZEModelResourcePhysicalJoint::GetTwistLowLimitDamping() const 
{
	return TwistLowLimitDamping;
}

void ZEModelResourcePhysicalJoint::SetTwistHighLimitValue(float TwistHighLimitValue)
{
	this->TwistHighLimitValue = TwistHighLimitValue;
}

float ZEModelResourcePhysicalJoint::GetTwistHighLimitValue() const 
{
	return TwistHighLimitValue;
}

void ZEModelResourcePhysicalJoint::SetTwistHighLimitRestitution(float TwistHighLimitRestitution)
{
	this->TwistHighLimitRestitution = TwistHighLimitRestitution;
}

float ZEModelResourcePhysicalJoint::GetTwistHighLimitRestitution() const 
{
	return TwistHighLimitRestitution;
}

void ZEModelResourcePhysicalJoint::SetTwistHighLimitSpring(float TwistHighLimitSpring)
{
	this->TwistHighLimitSpring = TwistHighLimitSpring;
}

float ZEModelResourcePhysicalJoint::GetTwistHighLimitSpring() const 
{
	return TwistHighLimitSpring;
}

void ZEModelResourcePhysicalJoint::SetTwistHighLimitDamping(float TwistHighLimitDamping)
{
	this->TwistHighLimitDamping = TwistHighLimitDamping;
}

float ZEModelResourcePhysicalJoint::GetTwistHighLimitDamping() const 
{
	return TwistHighLimitDamping;
}

void ZEModelResourcePhysicalJoint::SetSwing1Motion(ZEPhysicalJointMotion Swing1Motion)
{
	this->Swing1Motion = Swing1Motion;
}

ZEPhysicalJointMotion ZEModelResourcePhysicalJoint::GetSwing1Motion() const 
{
	return Swing1Motion;
}

void ZEModelResourcePhysicalJoint::SetSwing1LimitValue(float Swing1LimitValue)
{
	this->Swing1LimitValue = Swing1LimitValue;
}

float ZEModelResourcePhysicalJoint::GetSwing1LimitValue() const 
{
	return Swing1LimitValue;
}

void ZEModelResourcePhysicalJoint::SetSwing1LimitRestitution(float Swing1LimitRestitution)
{
	this->Swing1LimitRestitution = Swing1LimitRestitution;
}

float ZEModelResourcePhysicalJoint::GetSwing1LimitRestitution() const 
{
	return Swing1LimitRestitution;
}

void ZEModelResourcePhysicalJoint::SetSwing1LimitSpring(float Swing1LimitSpring)
{
	this->Swing1LimitSpring = Swing1LimitSpring;
}

float ZEModelResourcePhysicalJoint::GetSwing1LimitSpring() const 
{
	return Swing1LimitSpring;
}

void ZEModelResourcePhysicalJoint::SetSwing1LimitDamping(float Swing1LimitDamping)
{
	this->Swing1LimitDamping = Swing1LimitDamping;
}

float ZEModelResourcePhysicalJoint::GetSwing1LimitDamping() const 
{
	return Swing1LimitDamping;
}

void ZEModelResourcePhysicalJoint::SetSwing2Motion(ZEPhysicalJointMotion Swing2Motion)
{
	this->Swing2Motion = Swing2Motion;
}

ZEPhysicalJointMotion ZEModelResourcePhysicalJoint::GetSwing2Motion() const 
{
	return Swing2Motion;
}

void ZEModelResourcePhysicalJoint::SetSwing2LimitValue(float Swing2LimitValue)
{
	this->Swing2LimitValue = Swing2LimitValue;
}

float ZEModelResourcePhysicalJoint::GetSwing2LimitValue() const 
{
	return Swing2LimitValue;
}

void ZEModelResourcePhysicalJoint::SetSwing2LimitRestitution(float Swing2LimitRestitution)
{
	this->Swing2LimitRestitution = Swing2LimitRestitution;
}

float ZEModelResourcePhysicalJoint::GetSwing2LimitRestitution() const 
{
	return Swing2LimitRestitution;
}

void ZEModelResourcePhysicalJoint::SetSwing2LimitSpring(float Swing2LimitSpring)
{
	this->Swing2LimitSpring = Swing2LimitSpring;
}

float ZEModelResourcePhysicalJoint::GetSwing2LimitSpring() const 
{
	return Swing2LimitSpring;
}

void ZEModelResourcePhysicalJoint::SetSwing2LimitDamping(float Swing2LimitDamping)
{
	this->Swing2LimitDamping = Swing2LimitDamping;
}

float ZEModelResourcePhysicalJoint::GetSwing2LimitDamping() const 
{
	return Swing2LimitDamping;
}

void ZEModelResourcePhysicalJoint::SetMotorTargetPosition(const ZEVector3& MotorTargetPosition)
{
	this->MotorTargetPosition = MotorTargetPosition;
}

const ZEVector3& ZEModelResourcePhysicalJoint::GetMotorTargetPosition() const 
{
	return MotorTargetPosition;
}

void ZEModelResourcePhysicalJoint::SetMotorTargetOrientation(const ZEQuaternion& MotorTargetOrientation)
{
	this->MotorTargetOrientation = MotorTargetOrientation;
}

const ZEQuaternion& ZEModelResourcePhysicalJoint::GetMotorTargetOrientation() const 
{
	return MotorTargetOrientation;
}

void ZEModelResourcePhysicalJoint::SetMotorTargetVelocity(const ZEVector3& MotorTargetVelocity)
{
	this->MotorTargetVelocity = MotorTargetVelocity;
}

const ZEVector3& ZEModelResourcePhysicalJoint::GetMotorTargetVelocity() const 
{
	return MotorTargetVelocity;
}

void ZEModelResourcePhysicalJoint::SetMotorTargetAngularVelocity(const ZEVector3& MotorTargetAngularVelocity)
{
	this->MotorTargetAngularVelocity = MotorTargetAngularVelocity;
}

const ZEVector3& ZEModelResourcePhysicalJoint::GetMotorTargetAngularVelocity() const 
{
	return MotorTargetAngularVelocity;
}

void ZEModelResourcePhysicalJoint::SetLinearXMotor(ZEPhysicalJointMotorType LinearXMotor)
{
	this->LinearXMotor = LinearXMotor;
}

ZEPhysicalJointMotorType ZEModelResourcePhysicalJoint::GetLinearXMotor() const 
{
	return LinearXMotor;
}

void ZEModelResourcePhysicalJoint::SetLinearXMotorForce(float LinearXMotorForce)
{
	this->LinearXMotorForce = LinearXMotorForce;
}

float ZEModelResourcePhysicalJoint::GetLinearXMotorForce() const 
{
	return LinearXMotorForce;
}

void ZEModelResourcePhysicalJoint::SetLinearXMotorSpring(float LinearXMotorSpring)
{
	this->LinearXMotorSpring = LinearXMotorSpring;
}

float ZEModelResourcePhysicalJoint::GetLinearXMotorSpring() const 
{
	return LinearXMotorSpring;
}

void ZEModelResourcePhysicalJoint::SetLinearXMotorDamper(float LinearXMotorDamper)
{
	this->LinearXMotorDamper = LinearXMotorDamper;
}

float ZEModelResourcePhysicalJoint::GetLinearXMotorDamper() const 
{
	return LinearXMotorDamper;
}

void ZEModelResourcePhysicalJoint::SetLinearYMotor(ZEPhysicalJointMotorType LinearYMotor)
{
	this->LinearYMotor = LinearYMotor;
}

ZEPhysicalJointMotorType ZEModelResourcePhysicalJoint::GetLinearYMotor() const 
{
	return LinearYMotor;
}

void ZEModelResourcePhysicalJoint::SetLinearYMotorForce(float LinearYMotorForce)
{
	this->LinearYMotorForce = LinearYMotorForce;
}

float ZEModelResourcePhysicalJoint::GetLinearYMotorForce() const 
{
	return LinearYMotorForce;
}

void ZEModelResourcePhysicalJoint::SetLinearYMotorSpring(float LinearYMotorSpring)
{
	this->LinearYMotorSpring = LinearYMotorSpring;
}

float ZEModelResourcePhysicalJoint::GetLinearYMotorSpring() const 
{
	return LinearYMotorSpring;
}

void ZEModelResourcePhysicalJoint::SetLinearYMotorDamper(float LinearYMotorDamper)
{
	this->LinearYMotorDamper = LinearYMotorDamper;
}

float ZEModelResourcePhysicalJoint::GetLinearYMotorDamper() const 
{
	return LinearYMotorDamper;
}

void ZEModelResourcePhysicalJoint::SetLinearZMotor(ZEPhysicalJointMotorType LinearZMotor)
{
	this->LinearZMotor = LinearZMotor;
}

ZEPhysicalJointMotorType ZEModelResourcePhysicalJoint::GetLinearZMotor() const 
{
	return LinearZMotor;
}

void ZEModelResourcePhysicalJoint::SetLinearZMotorForce(float LinearZMotorForce)
{
	this->LinearZMotorForce = LinearZMotorForce;
}

float ZEModelResourcePhysicalJoint::GetLinearZMotorForce() const 
{
	return LinearZMotorForce;
}

void ZEModelResourcePhysicalJoint::SetLinearZMotorSpring(float LinearZMotorSpring)
{
	this->LinearZMotorSpring = LinearZMotorSpring;
}

float ZEModelResourcePhysicalJoint::GetLinearZMotorSpring() const 
{
	return LinearZMotorSpring;
}

void ZEModelResourcePhysicalJoint::SetLinearZMotorDamper(float LinearZMotorDamper)
{
	this->LinearZMotorDamper = LinearZMotorDamper;
}

float ZEModelResourcePhysicalJoint::GetLinearZMotorDamper() const 
{
	return LinearZMotorDamper;
}

void ZEModelResourcePhysicalJoint::SetAngularSwingMotor(ZEPhysicalJointMotorType AngularSwingMotor)
{
	this->AngularSwingMotor = AngularSwingMotor;
}

ZEPhysicalJointMotorType ZEModelResourcePhysicalJoint::GetAngularSwingMotor() const 
{
	return AngularSwingMotor;
}

void ZEModelResourcePhysicalJoint::SetAngularSwingMotorForce(float AngularSwingMotorForce)
{
	this->AngularSwingMotorForce = AngularSwingMotorForce;
}

float ZEModelResourcePhysicalJoint::GetAngularSwingMotorForce() const 
{
	return AngularSwingMotorForce;
}

void ZEModelResourcePhysicalJoint::SetAngularSwingMotorSpring(float AngularSwingMotorSpring)
{
	this->AngularSwingMotorSpring = AngularSwingMotorSpring;
}

float ZEModelResourcePhysicalJoint::GetAngularSwingMotorSpring() const 
{
	return AngularSwingMotorSpring;
}

void ZEModelResourcePhysicalJoint::SetAngularSwingMotorDamper(float AngularSwingMotorDamper)
{
	this->AngularSwingMotorDamper = AngularSwingMotorDamper;
}

float ZEModelResourcePhysicalJoint::GetAngularSwingMotorDamper() const 
{
	return AngularSwingMotorDamper;
}

void ZEModelResourcePhysicalJoint::SetAngularTwistMotor(ZEPhysicalJointMotorType AngularTwistMotor)
{
	this->AngularTwistMotor = AngularTwistMotor;
}

ZEPhysicalJointMotorType ZEModelResourcePhysicalJoint::GetAngularTwistMotor() const 
{
	return AngularTwistMotor;
}

void ZEModelResourcePhysicalJoint::SetAngularTwistMotorForce(float AngularTwistMotorForce)
{
	this->AngularTwistMotorForce = AngularTwistMotorForce;
}

float ZEModelResourcePhysicalJoint::GetAngularTwistMotorForce() const 
{
	return AngularTwistMotorForce;
}

void ZEModelResourcePhysicalJoint::SetAngularTwistMotorSpring(float AngularTwistMotorSpring)
{
	this->AngularTwistMotorSpring = AngularTwistMotorSpring;
}

float ZEModelResourcePhysicalJoint::GetAngularTwistMotorSpring() const 
{
	return AngularTwistMotorSpring;
}

void ZEModelResourcePhysicalJoint::SetAngularTwistMotorDamper(float AngularTwistMotorDamper)
{
	this->AngularTwistMotorDamper = AngularTwistMotorDamper;
}

float ZEModelResourcePhysicalJoint::GetAngularTwistMotorDamper() const 
{
	return AngularTwistMotorDamper;
}

void ZEModelResourcePhysicalJoint::SetAngularSlerpMotor(ZEPhysicalJointMotorType AngularSlerpMotor)
{
	this->AngularSlerpMotor = AngularSlerpMotor;
}

ZEPhysicalJointMotorType ZEModelResourcePhysicalJoint::GetAngularSlerpMotor() const 
{
	return AngularSlerpMotor;
}

void ZEModelResourcePhysicalJoint::SetAngularSlerpMotorForce(float AngularSlerpMotorForce)
{
	this->AngularSlerpMotorForce = AngularSlerpMotorForce;
}

float ZEModelResourcePhysicalJoint::GetAngularSlerpMotorForce() const 
{
	return AngularSlerpMotorForce;
}

void ZEModelResourcePhysicalJoint::SetAngularSlerpMotorSpring(float AngularSlerpMotorSpring)
{
	this->AngularSlerpMotorSpring = AngularSlerpMotorSpring;
}

float ZEModelResourcePhysicalJoint::GetAngularSlerpMotorSpring() const 
{
	return AngularSlerpMotorSpring;
}

void ZEModelResourcePhysicalJoint::SetAngularSlerpMotorDamper(float AngularSlerpMotorDamper)
{
	this->AngularSlerpMotorDamper = AngularSlerpMotorDamper;
}

float ZEModelResourcePhysicalJoint::GetAngularSlerpMotorDamper() const 
{
	return AngularSlerpMotorDamper;
}

bool ZEModelResourcePhysicalJoint::Load(const ZEMLReaderNode& JointNode)
{
	zeCheckError(!JointNode.IsValid(), false, "Invalid Joint node.");
	zeCheckError(JointNode.GetName() != "PhysicalJoint", false, "Invalid Joint node name.");

	SetEnabled(JointNode.ReadBoolean("Enabled", false));
	SetBody1Id(JointNode.ReadInt32("Body1Id", -1));
	SetBody2Id(JointNode.ReadInt32("Body2Id", -1));

	SetUseGlobalAnchorAxis(JointNode.ReadBoolean("UseGlobalAnchorAxis"));

	if (GetUseGlobalAnchorAxis())
	{
		SetGlobalAnchor(JointNode.ReadVector3("GlobalAnchor", ZEVector3::Zero));
		SetGlobalAxis(JointNode.ReadQuaternion("GlobalAxis", ZEQuaternion::Identity));
	}
	else
	{
		SetLocalAnchor1(JointNode.ReadVector3("LocalAnchor1", ZEVector3::Zero));
		SetLocalAxis1(JointNode.ReadQuaternion("LocalAxis1", ZEQuaternion::Identity));
		SetLocalAnchor2(JointNode.ReadVector3("LocalAnchor2", ZEVector3::Zero));
		SetLocalAxis2(JointNode.ReadQuaternion("LocalAxis2", ZEQuaternion::Identity));
	}

	SetJointType((ZEPhysicalJointType)JointNode.ReadInt32("JointType", ZE_PJT_NONE));
	SetCollideBodies(JointNode.ReadBoolean("CollideBodies", false));
	SetBreakable(JointNode.ReadBoolean("Breakable", false));
	SetBreakForce(JointNode.ReadFloat("BreakForce", 0.0f));
	SetBreakTorque(JointNode.ReadFloat("BreakTorque", 0.0f));

	SetXMotion((ZEPhysicalJointMotion)JointNode.ReadInt32("XMotion", ZE_PJMOTION_LOCKED));
	SetYMotion((ZEPhysicalJointMotion)JointNode.ReadInt32("YMotion", ZE_PJMOTION_LOCKED));
	SetZMotion((ZEPhysicalJointMotion)JointNode.ReadInt32("ZMotion", ZE_PJMOTION_LOCKED));
	SetLinearLimitValue(JointNode.ReadFloat("LinearLimitValue", 0.0f));
	SetLinearLimitRestitution(JointNode.ReadFloat("LinearLimitRestitution", 0.0f));
	SetLinearLimitSpring(JointNode.ReadFloat("LinearLimitSpring", 0.0f));
	SetLinearLimitDamping(JointNode.ReadFloat("LinearLimitDamping", 0.0f));

	SetTwistMotion((ZEPhysicalJointMotion)JointNode.ReadInt32("TwistMotion", ZE_PJMOTION_LOCKED));
	SetTwistLowLimitValue(JointNode.ReadFloat("TwistLowLimitValue", 0.0f));
	SetTwistLowLimitRestitution(JointNode.ReadFloat("TwistLowLimitRestitution", 0.0f));
	SetTwistLowLimitSpring(JointNode.ReadFloat("TwistLowLimitSpring", 0.0f));
	SetTwistLowLimitDamping(JointNode.ReadFloat("TwistLowLimitDamping", 0.0f));
	SetTwistHighLimitValue(JointNode.ReadFloat("TwistHighLimitValue", 0.0f));
	SetTwistHighLimitRestitution(JointNode.ReadFloat("TwistHighLimitRestitution", 0.0f));
	SetTwistHighLimitSpring(JointNode.ReadFloat("TwistHighLimitSpring", 0.0f));
	SetTwistHighLimitDamping(JointNode.ReadFloat("TwistHighLimitDamping", 0.0f));

	SetSwing1Motion((ZEPhysicalJointMotion)JointNode.ReadInt32("Swing1Motion", ZE_PJMOTION_LOCKED));
	SetSwing1LimitValue(JointNode.ReadFloat("Swing1LimitValue", 0.0f));
	SetSwing1LimitRestitution(JointNode.ReadFloat("Swing1LimitRestitution"));
	SetSwing1LimitSpring(JointNode.ReadFloat("Swing1LimitSpring", 0.0f));
	SetSwing1LimitDamping(JointNode.ReadFloat("Swing1LimitDamping", 0.0f));

	SetSwing2Motion((ZEPhysicalJointMotion)JointNode.ReadInt32("Swing2Motion", ZE_PJMOTION_LOCKED));
	SetSwing2LimitValue(JointNode.ReadFloat("Swing2LimitValue", 0.0f));
	SetSwing2LimitRestitution(JointNode.ReadFloat("Swing2LimitRestitution", 0.0f));
	SetSwing2LimitSpring(JointNode.ReadFloat("Swing2LimitSpring", 0.0f));
	SetSwing2LimitDamping(JointNode.ReadFloat("Swing2LimitDamping", 0.0f));

	SetLinearXMotor((ZEPhysicalJointMotorType)JointNode.ReadInt32("LinearXMotor", ZE_PJMOTION_LOCKED));
	SetLinearXMotorForce(JointNode.ReadFloat("LinearXMotorForce", 0.0f));
	SetLinearXMotorSpring(JointNode.ReadFloat("LinearXMotorSpring", 0.0f));
	SetLinearXMotorDamper(JointNode.ReadFloat("LinearXMotorDamper", 0.0f));

	SetLinearYMotor((ZEPhysicalJointMotorType)JointNode.ReadInt32("LinearYMotor", ZE_PJMT_NONE));
	SetLinearYMotorForce(JointNode.ReadFloat("LinearYMotorForce", 0.0f));
	SetLinearYMotorSpring(JointNode.ReadFloat("LinearYMotorSpring", 0.0f));
	SetLinearYMotorDamper(JointNode.ReadFloat("LinearYMotorDamper", 0.0f));

	SetLinearZMotor((ZEPhysicalJointMotorType)JointNode.ReadInt32("LinearZMotor", ZE_PJMT_NONE));
	SetLinearZMotorForce(JointNode.ReadFloat("LinearZMotorForce", 0.0f));
	SetLinearZMotorSpring(JointNode.ReadFloat("LinearZMotorSpring", 0.0f));
	SetLinearZMotorDamper(JointNode.ReadFloat("LinearZMotorDamper", 0.0f));

	SetAngularSwingMotor((ZEPhysicalJointMotorType)JointNode.ReadInt32("AngularSwingMotor", ZE_PJMT_NONE));
	SetAngularSwingMotorForce(JointNode.ReadFloat("AngularSwingMotorForce", 0.0f));
	SetAngularSwingMotorSpring(JointNode.ReadFloat("AngularSwingMotorSpring", 0.0f));
	SetAngularSwingMotorDamper(JointNode.ReadFloat("AngularSwingMotorDamper", 0.0f));

	SetAngularTwistMotor((ZEPhysicalJointMotorType)JointNode.ReadInt32("AngularTwistMotor", ZE_PJMT_NONE));
	SetAngularTwistMotorForce(JointNode.ReadFloat("AngularTwistMotorForce", 0.0f));
	SetAngularTwistMotorSpring(JointNode.ReadFloat("AngularTwistMotorSpring", 0.0f));
	SetAngularTwistMotorDamper(JointNode.ReadFloat("AngularTwistMotorDamper", 0.0f));

	SetAngularSlerpMotor((ZEPhysicalJointMotorType)JointNode.ReadInt32("AngularSlerpMotor", ZE_PJMT_NONE));
	SetAngularSlerpMotorForce(JointNode.ReadFloat("AngularSlerpMotorForce", 0.0f));
	SetAngularSlerpMotorSpring(JointNode.ReadFloat("AngularSlerpMotorSpring", 0.0f));
	SetAngularSlerpMotorDamper(JointNode.ReadFloat("AngularSlerpMotorDamper", 0.0f));
	
	SetMotorTargetPosition(JointNode.ReadVector3("MotorTargetPosition", ZEVector3::Zero));
	SetMotorTargetVelocity(JointNode.ReadVector3("MotorTargetVelocity", ZEVector3::Zero));
	SetMotorTargetOrientation(JointNode.ReadQuaternion("MotorTargetOrientation", ZEQuaternion::Identity));
	SetMotorTargetAngularVelocity(JointNode.ReadVector3("MotorTargetAngularVelocity", ZEVector3::Zero));

	return true;
}

bool ZEModelResourcePhysicalJoint::Save(ZEMLWriterNode& JointNode) const
{
	return false;
}

ZEModelResourcePhysicalJoint::ZEModelResourcePhysicalJoint()
{
	JointType = ZE_PJT_NONE;

	Enabled = false;

	Body1Id = -1;
	Body2Id = -1;

	CollideBodies = false;

	UseGlobalAnchorAxis = false;
	GlobalAnchor = ZEVector3::Zero;
	GlobalAxis = ZEQuaternion::Identity;

	LocalAnchor1 = ZEVector3::Zero;
	LocalAnchor2 = ZEVector3::Zero;

	LocalAxis1 = ZEQuaternion::Identity;
	LocalAxis2 = ZEQuaternion::Identity;

	Breakable = false;
	BreakForce = 0.0f;
	BreakTorque = 0.0f;

	XMotion = ZE_PJMOTION_LOCKED;
	YMotion = ZE_PJMOTION_LOCKED;
	ZMotion = ZE_PJMOTION_LOCKED;

	LinearLimitValue = 0.0f;
	LinearLimitRestitution = 0.0f;
	LinearLimitSpring = 0.0f;
	LinearLimitDamping = 0.0f;

	TwistMotion = ZE_PJMOTION_LOCKED;
	TwistLowLimitValue = 0.0f;
	TwistLowLimitRestitution = 0.0f;
	TwistLowLimitSpring = 0.0f;
	TwistLowLimitDamping = 0.0f;
	TwistHighLimitValue = 0.0f;
	TwistHighLimitRestitution = 0.0f;
	TwistHighLimitSpring = 0.0f;
	TwistHighLimitDamping = 0.0f;

	Swing1Motion = ZE_PJMOTION_LOCKED;
	Swing1LimitValue = 0.0f;
	Swing1LimitRestitution = 0.0f;
	Swing1LimitSpring = 0.0f;
	Swing1LimitDamping = 0.0f;

	Swing2Motion = ZE_PJMOTION_LOCKED;
	Swing2LimitValue = 0.0f;
	Swing2LimitRestitution = 0.0f;
	Swing2LimitSpring = 0.0f;
	Swing2LimitDamping = 0.0f;

	MotorTargetPosition = ZEVector3::Zero;
	MotorTargetOrientation = ZEQuaternion::Identity;
	MotorTargetVelocity = ZEVector3::Zero;
	MotorTargetAngularVelocity = ZEVector3::Zero;

	LinearXMotor = ZE_PJMT_NONE;
	LinearXMotorForce = 0.0f;
	LinearXMotorSpring = 0.0f;
	LinearXMotorDamper = 0.0f;

	LinearYMotor = ZE_PJMT_NONE;
	LinearYMotorForce = 0.0f;
	LinearYMotorSpring = 0.0f;
	LinearYMotorDamper = 0.0f;

	LinearZMotor = ZE_PJMT_NONE;
	LinearZMotorForce = 0.0f;
	LinearZMotorSpring = 0.0f;
	LinearZMotorDamper = 0.0f;

	AngularSwingMotor = ZE_PJMT_NONE;
	AngularSwingMotorForce = 0.0f;
	AngularSwingMotorSpring;
	AngularSwingMotorDamper;

	AngularTwistMotor = ZE_PJMT_NONE;
	AngularTwistMotorForce = 0.0f;
	AngularTwistMotorSpring = 0.0f;
	AngularTwistMotorDamper = 0.0f;

	AngularSlerpMotor = ZE_PJMT_NONE;
	AngularSlerpMotorForce = 0.0f;
	AngularSlerpMotorSpring = 0.0f;
	AngularSlerpMotorDamper = 0.0f;
}
