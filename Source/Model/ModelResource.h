//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ModelResource.h
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

#include "Core/Resource.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/AABoundingBox.h"
#include "ZEDS/Array.h"
#include "Graphics/VertexTypes.h"

class ZEStaticVertexBuffer;
class ZETexture2DResource;
class ZEMaterial;

#define ZE_MDLF_MAX_NAME_SIZE					128
#define ZE_MDLF_MAX_FILENAME_SIZE				256

#include "Physics/PhysicalRigidBody.h"
#include "Physics/PhysicalShapes.h"

/*
enum ZEPhysicalShapeType
{
	ZE_PST_PLANE            = 0,
	ZE_PST_BOX				= 1,
	ZE_PST_SPHERE			= 2,
	ZE_PST_CYLINDER			= 3,
	ZE_PST_CAPSULE			= 4,
	ZE_PST_CONVEX			= 5,
	ZE_PST_TRIMESH          = 6
};

enum ZEPhysicalBodyType
{
	ZE_PBT_NONE				= 0,
	ZE_PBT_RIGID			= 1,
	ZE_PBT_DEFORMABLE		= 2,
	ZE_PBT_CLOTH			= 3
};
*/

enum ZEPhysicalJointType
{
	ZE_PJT_NONE				= 0,
	ZE_PJT_FIXED            = 1,
	ZE_PJT_SPHERICAL		= 2,
	ZE_PJT_REVOLUTE			= 3,
	ZE_PJT_PRISMATIC		= 4,
	ZE_PJT_CYLINDRICAL		= 5,
	ZE_PJT_DISTANCE			= 6,
	ZE_PJT_POINTONLINE		= 7,
	ZE_PJT_POINTINPLANE		= 8,
	ZE_PJT_PULLEY			= 9,
	ZE_PJT_FREE				= 10
};

struct ZEModelResourcePhysicalPolygon
{
	int											VertexIndexes[3];
};

struct ZEModelResourcePhysicalShape
{
	ZEPhysicalShapeType							Type;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	float										Restitution;
	float										StaticFriction;
	float										DynamicFriction;
	ZEDWORD										CollisionMask1;
	ZEDWORD										CollisionMask2;
	ZEDWORD										CollisionMask3;
	ZEDWORD										CollisionMask4;
	bool										Trigger;
	
	union
	{
		struct
		{
			float								Height;
			float								NormalX;
			float								NormalY;
			float								NormalZ;
		} Plane;

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
	};

	struct
	{
		ZEArray<ZEVector3>			Vertices;
	} Convex;

	struct
	{
		ZEArray<ZEVector3>						Vertices;
		ZEArray<ZEModelResourcePhysicalPolygon>	Indices;
	} TriMesh;
};

struct ZEModelResourcePhysicalBody
{
	ZEPhysicalBodyType							Type;
	bool										Enabled;
	float										Mass;
	bool										Kinematic;
	float										LinearDamping;
	float										AngularDamping;
	ZEVector3									Position;
	ZEQuaternion								Orientation;
	ZEVector3									MassCenter;
	ZEArray<ZEModelResourcePhysicalShape>		Shapes;
};

struct ZEModelResourcePhysicalJoint
{
	ZEDWORD 									JointType;
	bool										Enabled;
	ZEDWORD										Body1Id;
	ZEDWORD										Body2Id;
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

	union
	{
		struct
		{
			bool 								SwingLimit;
			float 								SwingLimitValue;
			float 								SwingLimitRestitution;
			bool 								TwistLimit;
			float 								TwistLimitLowValue;
			float 								TwistLimitHighValue;
			float 								TwistLimitRestitution;
		} Spherical;

		struct
		{
			bool 								HasLimit;
			float 								LimitLowValue;
			float 								LimitHighValue;
			float 								LimitRestitution;
			bool 								HasSpring;
			float 								SpringValue;
			float 								SpringDamper;
			float 								SpringTarget;
			bool 								HasMotor;
			float 								MotorForce;
			float 								MotorVelocity;
		} Revolute;

		struct
		{
			bool 								HasMinLimit;
			float 								MinDistance;
			bool 								HasMaxLimit;
			float 								MaxDistance;
			bool 								HasSpring;
			float 								SpringValue;
			float 								SpringDamper;
		} Distance;
	};

	struct
	{
		ZEVector3 								Pulley1;
		ZEVector3 								Pulley2;
		float 									Distance;
		float 									Ratio;
		float 									Stiffness;
		bool 									IsRigid;
		bool									HasMotor;
		float 									MotorForce;
		float 									MotorVelocity;
	} Pulley;

	struct
	{
		ZEDWORD 								XMotion;
		ZEDWORD 								YMotion;
		ZEDWORD 								ZMotion;
		float 									LinearLimitValue;
		float 									LinearLimitRestitution;
		float 									LinearLimitSpring;
		float 									LinearLimitDamper;

		ZEDWORD 								TwistMotion;
		float 									TwistLimitHighValue;
		float 									TwistLimitLowValue;
		float 									TwistLimitRestitution;
		float 									TwistLimitSpring;
		float 									TwistLimitDamper;
 							
		ZEDWORD 								Swing1Motion;
		float 									Swing1LimitValue;
		float 									Swing1LimitRestitution;
		float 									Swing1LimitSpring;
		float 									Swing1LimitDamper;
		
		ZEDWORD 								Swing2Motion;
		float 									Swing2LimitValue;
		float 									Swing2LimitRestitution;
		float 									Swing2LimitSpring;
		float 									Swing2LimitDamper;

		ZEVector3 								LinearMotorPosition;
		ZEVector3 								LinearMotorVelocity;

		ZEDWORD 								LinearXMotor;
		float 									LinearXMotorForce;
		float 									LinearXMotorSpring;
		float 									LinearXMotorDamper;

		ZEDWORD 								LinearYMotor;
		float 									LinearYMotorForce;
		float 									LinearYMotorSpring;
		float 									LinearYMotorDamper;

		ZEDWORD 								LinearZMotor;
		float 									LinearZMotorForce;
		float 									LinearZMotorSpring;
		float 									LinearZMotorDamper;
 							
		ZEQuaternion 							AngularMotorOrientation;
		ZEVector3 								AngularMotorVelocity;

		ZEDWORD 								AngularMotor;
		float 									AngularMotorForce;
		float 									AngularMotorSpring;
		float 									AngularMotorDamper;
	} Free;
};


struct ZEModelResourceAnimationKey
{
	ZEDWORD										ItemId;
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
		ZEINT32									LODLevel;
		ZEINT32									MaterialId;
		ZEArray<ZEModelVertex>					Vertices;
		ZEArray<ZESkinnedModelVertex>			SkinnedVertices;
		
		ZEStaticVertexBuffer*					GetSharedVertexBuffer() const;
		ZEStaticVertexBuffer*					CreatePrivateVertexBuffer() const;

												ZEModelResourceMeshLOD();
												~ZEModelResourceMeshLOD();
};

struct ZEModelResourceMesh
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE]; 
	ZEAABoundingBox								BoundingBox;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	ZEVector3									Scale;
	bool										IsSkinned;
	ZEArray<ZEModelResourceMeshLOD>				LODs;
	ZEModelResourcePhysicalBody					PhysicalBody;
	ZEModelResourcePhysicalJoint				PhysicalJoint;
};

struct ZEModelResourceBone
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEAABoundingBox								BoundingBox;
	ZEINT32										ParentBone;
	ZEVector3									RelativePosition;
	ZEQuaternion								RelativeRotation;
	ZEVector3									RelativeScale;
	ZEMatrix4x4									RelativeTransform;
	ZEMatrix4x4									ForwardTransform;
	ZEMatrix4x4									InverseTransform;
	ZEModelResourcePhysicalJoint				PhysicalJoint;
};

class ZEModelResource : public ZEResource
{
	protected:
		virtual									~ZEModelResource();

	public:
		ZEArray<ZEModelResourceMesh>			Meshes;
		ZEArray<ZEModelResourceBone>			Bones;
		ZEArray<ZEModelResourceAnimation>		Animations;
		ZESmartArray<ZETexture2DResource*>		TextureResources;
		ZEArray<ZEMaterial*>					Materials;

		const char*								GetResourceType() const;
		
		static ZEModelResource*					LoadResource(const char* FileName);
		static const ZEModelResource*			LoadSharedResource(const char* FileName);
		static void								CacheResource(const char* FileName);
};

#endif

