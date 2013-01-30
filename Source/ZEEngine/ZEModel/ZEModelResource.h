//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelResource.h
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
#ifndef	__ZE_MODEL_RESOURCE_H__
#define __ZE_MODEL_RESOURCE_H__

#include "ZETypes.h"
#include "ZECore/ZEResource.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEDS/ZEArray.h"
#include "ZEGraphics/ZEVertexTypes.h"


class ZEStaticVertexBuffer;
class ZETexture2D;
class ZETexture2DResource;
class ZEMaterial;
class ZEPhysicalJoint;
class ZEMLSerialReader;

#define ZE_MDLF_MAX_NAME_SIZE					128
#define ZE_MDLF_MAX_FILENAME_SIZE				256

#include "ZEPhysics/ZEPhysicalRigidBody.h"
#include "ZEPhysics/ZEPhysicalJoint.h"
#include "ZEPhysics/ZEPhysicalShapes.h"


enum ZEModelResourcePhysicalShapeType
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

enum ZEModelResourceHelperOwnerType
{
	ZE_MRHOT_MODEL			= 0,
	ZE_MRHOT_MESH			= 1,
	ZE_MRHOT_BONE			= 2
};


struct ZEModelResourcePhysicalPolygon
{
	ZEInt										VertexIndexes[3];
};

struct ZEModelResourcePhysicalShape
{

	ZEModelResourcePhysicalShapeType			Type;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	float										Restitution;
	float										StaticFriction;
	float										DynamicFriction;
	ZEString									UserDefinedProperties;

	union
	{
		struct
		{
			float								Width;
			float								Height;
			float								Length;
		} Box;	

		struct
		{
			float								Radius;
		} Sphere;

		struct
		{
			float								Radius;
			float								Height;
		} Capsule;

		struct
		{
			float								Radius;
			float								Height;
		} Cylinder;
	};

	struct
	{
		ZEArray<ZEVector3>						Vertices;
	} Convex;
};

struct ZEModelResourcePhysicalBody
{
	ZEModelResourcePhysicalBodyType				Type;
	bool										Enabled;
	bool										IsKinematic;
	float										Mass;
	float										LinearDamping;
	float										AngularDamping;
	ZEVector3									Position;
	ZEQuaternion								Orientation;
	ZEVector3									MassCenter;
	ZEArray<ZEModelResourcePhysicalShape>		Shapes;
};

struct ZEModelResourcePhysicalJoint
{
	ZEPhysicalJointType							JointType;

	bool										Enabled;

	ZEUInt32									Body1Id;
	ZEUInt32									Body2Id;

	bool										CollideBodies;

	bool										UseGlobalAnchorAxis;

	ZEVector3									GlobalAnchor;
	ZEQuaternion								GlobalAxis;

	ZEVector3									LocalAnchor1;
	ZEVector3									LocalAnchor2;

	ZEQuaternion								LocalAxis1;
	ZEQuaternion								LocalAxis2;

	bool										Breakable;
	float										BreakForce;
	float										BreakTorque;

	ZEPhysicalJointMotion						XMotion;
	ZEPhysicalJointMotion 						YMotion;
	ZEPhysicalJointMotion 						ZMotion;

	float 										LinearLimitValue;
	float 										LinearLimitRestitution;
	float 										LinearLimitSpring;
	float 										LinearLimitDamping;

	ZEPhysicalJointMotion						TwistMotion;
	float 										TwistLowLimitValue;
	float 										TwistLowLimitRestitution;
	float 										TwistLowLimitSpring;
	float 										TwistLowLimitDamping;
	float 										TwistHighLimitValue;
	float 										TwistHighLimitRestitution;
	float 										TwistHighLimitSpring;
	float 										TwistHighLimitDamping;
						
	ZEPhysicalJointMotion						Swing1Motion;
	float 										Swing1LimitValue;
	float 										Swing1LimitRestitution;
	float 										Swing1LimitSpring;
	float 										Swing1LimitDamping;
	
	ZEPhysicalJointMotion						Swing2Motion;
	float 										Swing2LimitValue;
	float 										Swing2LimitRestitution;
	float 										Swing2LimitSpring;
	float 										Swing2LimitDamping;

	ZEVector3 									MotorTargetPosition;
	ZEQuaternion 								MotorTargetOrientation;
	ZEVector3 									MotorTargetVelocity;
	ZEVector3 									MotorTargetAngularVelocity;

	ZEPhysicalJointMotorType					LinearXMotor;
	float 										LinearXMotorForce;
	float 										LinearXMotorSpring;
	float 										LinearXMotorDamper;

	ZEPhysicalJointMotorType					LinearYMotor;
	float 										LinearYMotorForce;
	float 										LinearYMotorSpring;
	float 										LinearYMotorDamper;

	ZEPhysicalJointMotorType					LinearZMotor;
	float 										LinearZMotorForce;
	float 										LinearZMotorSpring;
	float 										LinearZMotorDamper;

	ZEPhysicalJointMotorType					AngularSwingMotor;
	float										AngularSwingMotorForce;
	float										AngularSwingMotorSpring;
	float										AngularSwingMotorDamper;

	ZEPhysicalJointMotorType					AngularTwistMotor;
	float										AngularTwistMotorForce;
	float										AngularTwistMotorSpring;
	float										AngularTwistMotorDamper;

	ZEPhysicalJointMotorType					AngularSlerpMotor;
	float										AngularSlerpMotorForce;
	float										AngularSlerpMotorSpring;
	float										AngularSlerpMotorDamper;
};

struct ZEModelResourceAnimationKey
{
	ZEUInt32									ItemId;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	ZEVector3									Scale;
};

struct ZEModelResourceAnimationFrame
{
	ZEArray<ZEModelResourceAnimationKey>		BoneKeys;
	ZEArray<ZEModelResourceAnimationKey>		MeshKeys;
};

struct ZEModelResourceAnimation
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEArray<ZEModelResourceAnimationFrame>		Frames;
};


class ZEModelResourceMeshLOD
{
	private:
		ZEStaticVertexBuffer*					SharedVertexBuffer;

	public:
		ZEInt32									LODLevel;
		ZEInt32									MaterialId;
		ZEArray<ZEModelVertex>					Vertices;
		ZEArray<ZESkinnedModelVertex>			SkinnedVertices;
		ZEArray<ZEUInt32>						AffectingBoneIds;

		ZEStaticVertexBuffer*					GetSharedVertexBuffer() const;
		ZEStaticVertexBuffer*					CreatePrivateVertexBuffer() const;

												ZEModelResourceMeshLOD();
												~ZEModelResourceMeshLOD();
};

struct ZEModelResourceMesh
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE]; 
	ZEAABBox									BoundingBox;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	ZEVector3									Scale;
	bool										IsVisible;
	bool										IsSkinned;
	ZEArray<ZEModelResourceMeshLOD>				LODs;
	ZEModelResourcePhysicalBody					PhysicalBody;
	ZEString									UserDefinedProperties;
};

struct ZEModelResourceBone
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEAABBox									BoundingBox;
	ZEInt32										ParentBone;
	ZEVector3									RelativePosition;
	ZEQuaternion								RelativeRotation;
	ZEVector3									RelativeScale;
	ZEMatrix4x4									RelativeTransform;
	ZEMatrix4x4									ForwardTransform;
	ZEMatrix4x4									InverseTransform;
	ZEModelResourcePhysicalBody					PhysicalBody;
	ZEModelResourcePhysicalJoint				PhysicalJoint;
	ZEString									UserDefinedProperties;
};

struct ZEModelResourceHelper
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEModelResourceHelperOwnerType				OwnerType;
	ZEInt32										OwnerId;
	ZEModelResourceMesh*						OwnerMesh;
	ZEModelResourceBone*						OwnerBone;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	ZEVector3									Scale;
	ZEString									UserDefinedProperties;
};


class ZEModelResource : public ZEResource
{
	private:
		ZEArray<ZEModelResourceMesh>				Meshes;
		ZEArray<ZEModelResourceBone>				Bones;
		ZEArray<ZEModelResourceHelper>				Helpers;
		ZEArray<ZEModelResourceAnimation>			Animations;
		ZESmartArray<ZETexture2DResource*>			TextureResources;
		ZEArray<ZEMaterial*>						Materials;

		bool										ReadBones(ZEMLSerialReader* NodeReader);
		void										ProcessBones(ZEModelResourceBone* Bone, ZEInt BoneId);
		bool										ReadMeshes(ZEMLSerialReader* NodeReader);
		bool										ReadHelpers(ZEMLSerialReader* NodeReader);
		bool										ReadAnimations(ZEMLSerialReader* NodeReader);
		bool										ReadMaterials(ZEMLSerialReader* NodeReader);
		bool										ReadPhysicalBody(ZEModelResourcePhysicalBody* Body, ZEMLSerialReader* NodeReader);
		bool										ReadPhysicalJoint(ZEModelResourcePhysicalJoint* Joint, ZEMLSerialReader* NodeReader);

		const ZETexture2D*							ManageModelMaterialTextures(const ZEString& FileName);
		bool  										ReadModelFromFile(ZEFile* ResourceFile);

		virtual										~ZEModelResource();

	public:
		const char*									GetResourceType() const;
		
		const ZESmartArray<ZETexture2DResource*>&	GetTextures() const;
		const ZEArray<ZEMaterial*>&					GetMaterials() const;
		const ZEArray<ZEModelResourceBone>&			GetBones() const;
		const ZEArray<ZEModelResourceMesh>&			GetMeshes() const;
		const ZEArray<ZEModelResourceHelper>&		GetHelpers() const;
		const ZEArray<ZEModelResourceAnimation>&	GetAnimations() const;

		static ZEModelResource*						LoadResource(const ZEString& FileName);
		static ZEModelResource*						LoadSharedResource(const ZEString& FileName);
		static void									CacheResource(const ZEString& FileName);
};

#endif
