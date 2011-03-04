//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETModelFileFormat.h
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
#ifndef __ZET_MODEL_FILE_FORMAT_H__
#define __ZET_MODEL_FILE_FORMAT_H__

#include "ZEMath\ZEVector.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEMath\ZEAABoundingBox.h"
#include "ZEDS\ZEArray.h"
#include "ZETDefinitions.h"
#include "ZETTypes.h"

#define ZE_MDLF_MAX_NAME_SIZE						128
#define ZE_MDLF_MAX_FILENAME_SIZE					256

#define ZE_FILE_MAKEVERSION(Major, Minor)			((((ZEDWORD)(Major)) << 16) + (ZEDWORD)(Minor))
#define ZE_MDLF_VERSION								ZE_FILE_MAKEVERSION(0, 40)
#define ZE_MDLF_HEADER								((ZEDWORD)((ZEDWORD)'ZEMF' + (ZEDWORD)'MDL '))

#define	ZE_MDLF_MATERIAL_CHUNKID					((ZEDWORD)(ZE_MDLF_HEADER + (ZEDWORD)'MTRL'))
#define	ZE_MDLF_MESH_CHUNKID						((ZEDWORD)(ZE_MDLF_HEADER + (ZEDWORD)'MESH'))
#define	ZE_MDLF_BONE_CHUNKID						((ZEDWORD)(ZE_MDLF_HEADER + (ZEDWORD)'BONE'))
#define	ZE_MDLF_MESH_LOD_CHUNKID					((ZEDWORD)(ZE_MDLF_HEADER + (ZEDWORD)'LOD '))
#define ZE_MDLF_PHYSICAL_BODY_CHUNKID				((ZEDWORD)(ZE_MDLF_HEADER + (ZEDWORD)'PBDY'))
#define	ZE_MDLF_PHYSICAL_SHAPE_CHUNKID				((ZEDWORD)(ZE_MDLF_PHYSICAL_BODY_CHUNKID + (ZEDWORD)'PHSH'))
#define ZE_MDLF_PHYSICAL_SHAPE_VERTEX_CHUNKID		((ZEDWORD)(ZE_MDLF_PHYSICAL_SHAPE_CHUNKID + (ZEDWORD)'VRTX'))
#define ZE_MDLF_PHYSICAL_SHAPE_INDEX_CHUNKID		((ZEDWORD)(ZE_MDLF_PHYSICAL_SHAPE_CHUNKID + (ZEDWORD)'INDX'))
#define ZE_MDLF_PHYSICAL_JOINT_CHUNKID				((ZEDWORD)(ZE_MDLF_HEADER + (ZEDWORD)'PJNT'))
#define	ZE_MDLF_ANIMATION_CHUNKID					((ZEDWORD)(ZE_MDLF_HEADER + (ZEDWORD)'ANIM'))
#define	ZE_MDLF_ANIMATION_KEYFRAME_CHUNKID			((ZEDWORD)(ZE_MDLF_ANIMATION_CHUNKID + (ZEDWORD)'ANKF'))

typedef ZEDWORD ZEModelFilePhysicalShapeType;
#define ZE_MFPST_BOX								0
#define ZE_MFPST_SPHERE								1
#define ZE_MFPST_CAPSULE							2
#define ZE_MFPST_CYLINDER							3
#define ZE_MFPST_CONVEX								4

struct ZEModelFileMaterialChunk
{
	ZEDWORD								ChunkId;
	char								Shader[ZE_MDLF_MAX_FILENAME_SIZE];
	ZEDWORD								ShaderComponents;

	bool								TwoSided;
	bool								LightningEnabled;
	bool								Wireframe;
	bool								Transparant;

	ZEVector3							AmbientColor;
	ZEVector3							DiffuseColor;
	ZEVector3							SpecularColor;
	ZEVector3							EmmisiveColor;
	float								EmmisiveFactor;
	float								SpecularFactor;
	float								Opasity;
	float								ReflectionFactor;
	float								RefractionFactor;
	ZEVector2							DetailMapTiling;

	char								DiffuseMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char								NormalMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char								SpecularMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char								EmmisiveMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char								OpasityMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char								DetailMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char								DetailNormalMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char								ReflectionMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char								RefractionMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char								LightMap[ZE_MDLF_MAX_FILENAME_SIZE];
};

struct ZEModelFileHeaderChunk
{
	ZEDWORD								HEADER;
	ZEDWORD								Version;

	ZEDWORD								MaterialCount;
	ZEDWORD								MeshCount;
	ZEDWORD								BoneCount;
	ZEDWORD								AnimationCount;
};

struct ZEModelFileAnimationChunk
{
	ZEDWORD								ChunkId;
	char								Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEDWORD								FrameCount;
};

struct ZEModelFileAnimationFrameChunk
{
	ZEDWORD								ChunkId;
	ZEDWORD								BoneKeyCount;
	ZEDWORD								MeshKeyCount;
};

struct ZEModelFilePhysicalPolygonChunk
{	
	int									VertexIndexes[3];
};

struct ZEModelFilePhysicalShapeChunk
{
	ZEDWORD								ChunkId;
	ZEModelFilePhysicalShapeType		Type;
	ZEVector3							Position;
	ZEQuaternion						Rotation;
	float								Restitution;
	float								StaticFriction;
	float								DynamicFriction;

	union
	{
		struct
		{
			float						Width;
			float						Height;
			float						Length;
		} Box;

		struct
		{
			float						Radius;
		} Sphere;

		struct
		{
			float						Radius;
			float						Height;
		} Capsule;

		struct  
		{
			float						Radius;
			float						Height;
		} Cylinder;

		struct
		{
			ZEDWORD						VertexCount;
		} Convex;
	};
};

struct ZEModelFilePhysicalBodyChunk
{
	ZEDWORD								ChunkId;
	ZEDWORD				                Type;
	bool								Enabled;
	float								Mass;
	float								LinearDamping;
	float								AngularDamping;
	ZEVector3							MassCenter;
	ZEDWORD								ShapeCount;
};

struct ZEModelFilePhysicalJointChunk
{
	ZEDWORD								ChunkId;

	ZEDWORD								JointType;

	bool								Enabled;

	ZEDWORD								Body1Id;
	ZEDWORD								Body2Id;

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

	ZEDWORD								XMotion;
	ZEDWORD								YMotion;
	ZEDWORD								ZMotion;

	float 								LinearLimitValue;
	float 								LinearLimitRestitution;
	float 								LinearLimitSpring;
	float 								LinearLimitDamping;

	ZEDWORD								TwistMotion;
	float 								TwistLowLimitValue;
	float 								TwistLowLimitRestitution;
	float 								TwistLowLimitSpring;
	float 								TwistLowLimitDamping;
	float 								TwistHighLimitValue;
	float 								TwistHighLimitRestitution;
	float 								TwistHighLimitSpring;
	float 								TwistHighLimitDamping;

	ZEDWORD								Swing1Motion;
	float 								Swing1LimitValue;
	float 								Swing1LimitRestitution;
	float 								Swing1LimitSpring;
	float 								Swing1LimitDamping;

	ZEDWORD								Swing2Motion;
	float 								Swing2LimitValue;
	float 								Swing2LimitRestitution;
	float 								Swing2LimitSpring;
	float 								Swing2LimitDamping;

	ZEVector3 							MotorTargetPosition;
	ZEQuaternion 						MotorTargetOrientation;
	ZEVector3 							MotorTargetVelocity;
	ZEVector3 							MotorTargetAngularVelocity;

	ZEDWORD 							LinearXMotor;
	float 								LinearXMotorForce;
	float 								LinearXMotorSpring;
	float 								LinearXMotorDamper;

	ZEDWORD 							LinearYMotor;
	float 								LinearYMotorForce;
	float 								LinearYMotorSpring;
	float 								LinearYMotorDamper;

	ZEDWORD 							LinearZMotor;
	float 								LinearZMotorForce;
	float 								LinearZMotorSpring;
	float 								LinearZMotorDamper;

	ZEDWORD								AngularSwingMotor;
	float								AngularSwingMotorForce;
	float								AngularSwingMotorSpring;
	float								AngularSwingMotorDamper;

	ZEDWORD								AngularTwistMotor;
	float								AngularTwistMotorForce;
	float								AngularTwistMotorSpring;
	float								AngularTwistMotorDamper;

	ZEDWORD								AngularSlerpMotor;
	float								AngularSlerpMotorForce;
	float								AngularSlerpMotorSpring;
	float								AngularSlerpMotorDamper;
};

struct ZEModelFileMeshLODChunk
{
	ZEDWORD								ChunkId;
	ZEINT32								LODLevel;
	ZEINT32								MaterialId;
	ZEDWORD								VertexCount;
	ZEDWORD								AffectingBoneCount;
};

struct ZEModelFileMeshChunk
{
	ZEDWORD								ChunkId;
	char								Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEAABoundingBox						BoundingBox;
	ZEVector3							Position;
	ZEQuaternion						Rotation;
	ZEVector3							Scale;
	bool								IsSkinned;	
	ZEDWORD								LODCount;
	bool								HasPhysicalBody;
};

struct ZEModelFileBoneChunk
{
	ZEDWORD								ChunkId;
	char								Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEINT32								ParentBone;
	ZEVector3							RelativePosition;
	ZEQuaternion						RelativeRotation;
	ZEVector3							RelativeScale;
	ZEAABoundingBox						BoundingBox;
	bool								HasPhysicalBody;
	bool								HasPhysicalJoint;
};

#endif