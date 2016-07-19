//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourcePhysics.h
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

#pragma once

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEPhysics/ZEPhysicalJoint.h"
#include "ZEMeta/ZEObject.h"

enum ZEMDResourcePhysicalShapeType
{
	ZE_MRPST_BOX			= 0,
	ZE_MRPST_SPHERE			= 1,
	ZE_MRPST_CAPSULE		= 2,
	ZE_MRPST_CYLINDER		= 3,
	ZE_MRPST_CONVEX			= 4,
};

enum ZEModelResourcePhysicalBodyType
{
	ZE_MRPBT_NONE			= 0,
	ZE_MRPBT_RIGID			= 1,
	ZE_MRPBT_DEFORMABLE		= 2,
	ZE_MRPBT_CLOTH			= 3
};

struct ZEModelResourcePhysicalPolygon
{
	ZEInt16 VertexIndexes[3];
};

class ZEMLReaderNode;
class ZEMLWriterNode;

class ZEMDResourcePhysicalShape : public ZEObject
{
	ZE_OBJECT
	private:
		ZEMDResourcePhysicalShapeType		Type;
		ZEVector3 Position;
		ZEQuaternion						Rotation;
		float								Restitution;
		float								StaticFriction;
		float								DynamicFriction;

		float								Width;
		float								Height;
		float								Length;
		float								Radius;
		ZEArray<ZEVector3>					Vertices;
		ZEArray<ZEUInt16>					Indices;

		ZEString							UserDefinedProperties;

	public:
		void								SetType(ZEMDResourcePhysicalShapeType Type);
		ZEMDResourcePhysicalShapeType		GetType() const;
		
		void								SetPosition(const ZEVector3& Position);
		const ZEVector3&					GetPosition() const;
		
		void								SetRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion&					GetRotation() const;
		
		void								SetRestitution(float Restitution);
		float								GetRestitution() const;

		void								SetStaticFriction(float Friction);
		float								GetStaticFriction() const;

		void								SetDynamicFriction(float DynamicFriction);
		float								GetDynamicFriction() const;

		void								SetWidth(float Value);
		float								GetWidth() const;

		void								SetHeight(float Value);
		float								GetHeight() const;

		void								SetLength(float Value);
		float								GetLength() const;

		void								SetRadius(float Value);
		float								GetRadius() const;

		void								SetVertices(const ZEArray<ZEVector3>& Vertices);
		const								ZEArray<ZEVector3> GetVertices();

		void								SetIndices(const ZEArray<ZEUInt16>& Indices);
		const								ZEArray<ZEUInt16> GetIndices();

		void								SetUserDefinedProperties(ZEString UserDefinedProperties);
		ZEString							GetUserDefinedProperties() const;

		bool								Load(const ZEMLReaderNode& ShapeNode);
		bool								Save(ZEMLWriterNode& ShapeNode) const;

											ZEMDResourcePhysicalShape();
};


class ZEMDResourcePhysicalBody : public ZEObject
{
	ZE_OBJECT
	private:
		ZEModelResourcePhysicalBodyType		Type;
		bool								Enabled;
		bool								Kinematic;
		float								Mass;
		float								LinearDamping;
		float								AngularDamping;
		ZEVector3							Position;
		ZEQuaternion						Orientation;
		ZEVector3							CenterOfMass;
		ZEArray<ZEMDResourcePhysicalShape>	Shapes;

	public:
		void								SetType(ZEModelResourcePhysicalBodyType Type);
		ZEModelResourcePhysicalBodyType		GetType() const;

		void								SetEnabled(bool Enabled);
		bool								GetEnabled() const;

		void								SetPosition(const ZEVector3& Position);
		const ZEVector3&					GetPosition() const;

		void								SetOrientation(const ZEQuaternion& Orientation);
		const ZEQuaternion&					GetOrientation() const;

		void								SetKinematic(bool Kinematic);
		bool								GetKinematic() const;

		void								SetMass(float Mass);
		float								GetMass() const;

		void								SetCenterOfMass(const ZEVector3& CenterOfMass);
		const ZEVector3&					GetCenterOfMass() const;

		void								SetLinearDamping(float LinearDamping);
		float								GetLinearDamping() const;

		void								SetAngularDamping(float AngularDamping);
		float								GetAngularDamping() const;

		const ZEArray<ZEMDResourcePhysicalShape>& GetShapes() const;
		void								AddShape(const ZEMDResourcePhysicalShape& Shape);
		void								RemoveShape(ZESize Index);

		bool								Load(const ZEMLReaderNode& BodyNode);
		bool								Save(ZEMLWriterNode& BodyNode) const;

											ZEMDResourcePhysicalBody();
};


struct ZEMDResourcePhysicalJoint : public ZEObject
{
	ZE_OBJECT
	private:
		ZEPhysicalJointType					JointType;

		bool								Enabled;

		ZEUInt32							Body1Id; // To Pointer
		ZEUInt32							Body2Id; // To Pointer

		bool								CollideBodies;

		bool								UseGlobalAnchorAxis;
		ZEVector3							GlobalAnchor;
		ZEQuaternion						GlobalAxis;

		ZEVector3							LocalAnchor1;
		ZEVector3							LocalAnchor2;

		ZEQuaternion						LocalAxis1;
		ZEQuaternion						LocalAxis2;

		bool								Breakable;
		float								BreakForce;
		float								BreakTorque;

		ZEPhysicalJointMotion				XMotion;
		ZEPhysicalJointMotion				YMotion;
		ZEPhysicalJointMotion				ZMotion;

		float								LinearLimitValue;
		float								LinearLimitRestitution;
		float								LinearLimitSpring;
		float								LinearLimitDamping;

		ZEPhysicalJointMotion				TwistMotion;
		float								TwistLowLimitValue;
		float								TwistLowLimitRestitution;
		float								TwistLowLimitSpring;
		float								TwistLowLimitDamping;
		float								TwistHighLimitValue;
		float								TwistHighLimitRestitution;
		float								TwistHighLimitSpring;
		float								TwistHighLimitDamping;
						
		ZEPhysicalJointMotion				Swing1Motion;
		float								Swing1LimitValue;
		float								Swing1LimitRestitution;
		float								Swing1LimitSpring;
		float								Swing1LimitDamping;
	
		ZEPhysicalJointMotion				Swing2Motion;
		float								Swing2LimitValue;
		float								Swing2LimitRestitution;
		float								Swing2LimitSpring;
		float								Swing2LimitDamping;

		ZEVector3							MotorTargetPosition;
		ZEQuaternion						MotorTargetOrientation;
		ZEVector3							MotorTargetVelocity;
		ZEVector3							MotorTargetAngularVelocity;

		ZEPhysicalJointMotorType			LinearXMotor;
		float								LinearXMotorForce;
		float								LinearXMotorSpring;
		float								LinearXMotorDamper;

		ZEPhysicalJointMotorType			LinearYMotor;
		float								LinearYMotorForce;
		float								LinearYMotorSpring;
		float								LinearYMotorDamper;

		ZEPhysicalJointMotorType			LinearZMotor;
		float								LinearZMotorForce;
		float								LinearZMotorSpring;
		float								LinearZMotorDamper;

		ZEPhysicalJointMotorType			AngularSwingMotor;
		float								AngularSwingMotorForce;
		float								AngularSwingMotorSpring;
		float								AngularSwingMotorDamper;

		ZEPhysicalJointMotorType			AngularTwistMotor;
		float								AngularTwistMotorForce;
		float								AngularTwistMotorSpring;
		float								AngularTwistMotorDamper;

		ZEPhysicalJointMotorType			AngularSlerpMotor;
		float								AngularSlerpMotorForce;
		float								AngularSlerpMotorSpring;
		float								AngularSlerpMotorDamper;

	public:
		void								SetJointType(ZEPhysicalJointType JointType);
		ZEPhysicalJointType					GetJointType() const;

		void								SetEnabled(bool Enabled);
		bool								GetEnabled() const;

		void								SetBody1Id(ZEUInt32 Body1Id);
		ZEUInt32							GetBody1Id() const;

		void								SetBody2Id(ZEUInt32 Body2Id);
		ZEUInt32							GetBody2Id() const;

		void								SetCollideBodies(bool CollideBodies);
		bool								GetCollideBodies() const;

		void								SetUseGlobalAnchorAxis(bool UseGlobalAnchorAxis);
		bool								GetUseGlobalAnchorAxis() const;

		void								SetGlobalAnchor(const ZEVector3& GlobalAnchor);
		const ZEVector3&					GetGlobalAnchor() const;

		void								SetGlobalAxis(const ZEQuaternion& GlobalAxis);
		const ZEQuaternion&					GetGlobalAxis() const;

		void								SetLocalAnchor1(const ZEVector3& LocalAnchor1);
		const ZEVector3&					GetLocalAnchor1() const;

		void								SetLocalAnchor2(const ZEVector3& LocalAnchor2);
		const ZEVector3&					GetLocalAnchor2() const;

		void								SetLocalAxis1(const ZEQuaternion& LocalAxis1);
		const ZEQuaternion&					GetLocalAxis1() const;

		void								SetLocalAxis2(const ZEQuaternion& LocalAxis2);
		const ZEQuaternion&					GetLocalAxis2() const;

		void								SetBreakable(bool Breakable);
		bool								GetBreakable() const;

		void								SetBreakForce(float BreakForce);
		float								GetBreakForce() const;

		void								SetBreakTorque(float BreakTorque);
		float								GetBreakTorque() const;

		void								SetXMotion(ZEPhysicalJointMotion XMotion);
		ZEPhysicalJointMotion				GetXMotion() const;

		void								SetYMotion(ZEPhysicalJointMotion YMotion);
		ZEPhysicalJointMotion				GetYMotion() const;

		void								SetZMotion(ZEPhysicalJointMotion ZMotion);
		ZEPhysicalJointMotion				GetZMotion() const;

		void								SetLinearLimitValue(float LinearLimitValue);
		float								GetLinearLimitValue() const;

		void								SetLinearLimitRestitution(float LinearLimitRestitution);
		float								GetLinearLimitRestitution() const;

		void								SetLinearLimitSpring(float LinearLimitSpring);
		float								GetLinearLimitSpring() const;

		void								SetLinearLimitDamping(float LinearLimitDamping);
		float								GetLinearLimitDamping() const;

		void								SetTwistMotion(ZEPhysicalJointMotion TwistMotion);
		ZEPhysicalJointMotion				GetTwistMotion() const;

		void								SetTwistLowLimitValue(float TwistLowLimitValue);
		float								GetTwistLowLimitValue() const;

		void								SetTwistLowLimitRestitution(float TwistLowLimitRestitution);
		float								GetTwistLowLimitRestitution() const;

		void								SetTwistLowLimitSpring(float TwistLowLimitSpring);
		float								GetTwistLowLimitSpring() const;

		void								SetTwistLowLimitDamping(float TwistLowLimitDamping);
		float								GetTwistLowLimitDamping() const;

		void								SetTwistHighLimitValue(float TwistHighLimitValue);
		float								GetTwistHighLimitValue() const;

		void								SetTwistHighLimitRestitution(float TwistHighLimitRestitution);
		float								GetTwistHighLimitRestitution() const;

		void								SetTwistHighLimitSpring(float TwistHighLimitSpring);
		float								GetTwistHighLimitSpring() const;

		void								SetTwistHighLimitDamping(float TwistHighLimitDamping);
		float								GetTwistHighLimitDamping() const;

		void								SetSwing1Motion(ZEPhysicalJointMotion Swing1Motion);
		ZEPhysicalJointMotion				GetSwing1Motion() const;

		void								SetSwing1LimitValue(float Swing1LimitValue);
		float								GetSwing1LimitValue() const;

		void								SetSwing1LimitRestitution(float Swing1LimitRestitution);
		float								GetSwing1LimitRestitution() const;

		void								SetSwing1LimitSpring(float Swing1LimitSpring);
		float								GetSwing1LimitSpring() const;

		void								SetSwing1LimitDamping(float Swing1LimitDamping);
		float								GetSwing1LimitDamping() const;

		void								SetSwing2Motion(ZEPhysicalJointMotion Swing2Motion);
		ZEPhysicalJointMotion				GetSwing2Motion() const;

		void								SetSwing2LimitValue(float Swing2LimitValue);
		float								GetSwing2LimitValue() const;

		void								SetSwing2LimitRestitution(float Swing2LimitRestitution);
		float								GetSwing2LimitRestitution() const;

		void								SetSwing2LimitSpring(float Swing2LimitSpring);
		float								GetSwing2LimitSpring() const;

		void								SetSwing2LimitDamping(float Swing2LimitDamping);
		float								GetSwing2LimitDamping() const;

		void								SetMotorTargetPosition(const ZEVector3& MotorTargetPosition);
		const								ZEVector3& GetMotorTargetPosition() const;

		void								SetMotorTargetOrientation(const ZEQuaternion& MotorTargetOrientation);
		const								ZEQuaternion& GetMotorTargetOrientation() const;

		void								SetMotorTargetVelocity(const ZEVector3& MotorTargetVelocity);
		const								ZEVector3& GetMotorTargetVelocity() const;

		void								SetMotorTargetAngularVelocity(const ZEVector3& MotorTargetAngularVelocity);
		const								ZEVector3& GetMotorTargetAngularVelocity() const;

		void								SetLinearXMotor(ZEPhysicalJointMotorType LinearXMotor);
		ZEPhysicalJointMotorType			GetLinearXMotor() const;

		void								SetLinearXMotorForce(float LinearXMotorForce);
		float								GetLinearXMotorForce() const;

		void								SetLinearXMotorSpring(float LinearXMotorSpring);
		float								GetLinearXMotorSpring() const;

		void								SetLinearXMotorDamper(float LinearXMotorDamper);
		float								GetLinearXMotorDamper() const;

		void								SetLinearYMotor(ZEPhysicalJointMotorType LinearYMotor);
		ZEPhysicalJointMotorType			GetLinearYMotor() const;

		void								SetLinearYMotorForce(float LinearYMotorForce);
		float								GetLinearYMotorForce() const;

		void								SetLinearYMotorSpring(float LinearYMotorSpring);
		float								GetLinearYMotorSpring() const;

		void								SetLinearYMotorDamper(float LinearYMotorDamper);
		float								GetLinearYMotorDamper() const;

		void								SetLinearZMotor(ZEPhysicalJointMotorType LinearZMotor);
		ZEPhysicalJointMotorType			GetLinearZMotor() const;

		void								SetLinearZMotorForce(float LinearZMotorForce);
		float								GetLinearZMotorForce() const;

		void								SetLinearZMotorSpring(float LinearZMotorSpring);
		float								GetLinearZMotorSpring() const;

		void								SetLinearZMotorDamper(float LinearZMotorDamper);
		float								GetLinearZMotorDamper() const;

		void								SetAngularSwingMotor(ZEPhysicalJointMotorType AngularSwingMotor);
		ZEPhysicalJointMotorType			GetAngularSwingMotor() const;

		void								SetAngularSwingMotorForce(float AngularSwingMotorForce);
		float								GetAngularSwingMotorForce() const;

		void								SetAngularSwingMotorSpring(float AngularSwingMotorSpring);
		float								GetAngularSwingMotorSpring() const;

		void								SetAngularSwingMotorDamper(float AngularSwingMotorDamper);
		float								GetAngularSwingMotorDamper() const;

		void								SetAngularTwistMotor(ZEPhysicalJointMotorType AngularTwistMotor);
		ZEPhysicalJointMotorType			GetAngularTwistMotor() const;

		void								SetAngularTwistMotorForce(float AngularTwistMotorForce);
		float								GetAngularTwistMotorForce() const;

		void								SetAngularTwistMotorSpring(float AngularTwistMotorSpring);
		float								GetAngularTwistMotorSpring() const;

		void								SetAngularTwistMotorDamper(float AngularTwistMotorDamper);
		float								GetAngularTwistMotorDamper() const;

		void								SetAngularSlerpMotor(ZEPhysicalJointMotorType AngularSlerpMotor);
		ZEPhysicalJointMotorType			GetAngularSlerpMotor() const;

		void								SetAngularSlerpMotorForce(float AngularSlerpMotorForce);
		float								GetAngularSlerpMotorForce() const;

		void								SetAngularSlerpMotorSpring(float AngularSlerpMotorSpring);
		float								GetAngularSlerpMotorSpring() const;

		void								SetAngularSlerpMotorDamper(float AngularSlerpMotorDamper);
		float								GetAngularSlerpMotorDamper() const;

		bool								Load(const ZEMLReaderNode& JointNode);
		bool								Save(ZEMLWriterNode& JointNode) const;

											ZEMDResourcePhysicalJoint();
};
