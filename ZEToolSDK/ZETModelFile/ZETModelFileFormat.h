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

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEDS/ZEArray.h"
#include "ZETDefinitions.h"
#include "ZETypes.h"

#define ZE_MDLF_MAX_NAME_SIZE						128
#define ZE_MDLF_MAX_FILENAME_SIZE					256

#define ZE_FILE_MAKEVERSION(Major, Minor)			((((ZEUInt32)(Major)) << 16) + (ZEUInt32)(Minor))
#define ZE_MDLF_VERSION								ZE_FILE_MAKEVERSION(0, 40)
#define ZE_MDLF_HEADER								((ZEUInt32)((ZEUInt32)'ZEMF' + (ZEUInt32)'MDL '))

#define	ZE_MDLF_MATERIAL_CHUNKID					((ZEUInt32)(ZE_MDLF_HEADER + (ZEUInt32)'MTRL'))
#define	ZE_MDLF_MESH_CHUNKID						((ZEUInt32)(ZE_MDLF_HEADER + (ZEUInt32)'MESH'))
#define	ZE_MDLF_BONE_CHUNKID						((ZEUInt32)(ZE_MDLF_HEADER + (ZEUInt32)'BONE'))
#define	ZE_MDLF_MESH_LOD_CHUNKID					((ZEUInt32)(ZE_MDLF_HEADER + (ZEUInt32)'LOD '))
#define ZE_MDLF_PHYSICAL_BODY_CHUNKID				((ZEUInt32)(ZE_MDLF_HEADER + (ZEUInt32)'PBDY'))
#define	ZE_MDLF_PHYSICAL_SHAPE_CHUNKID				((ZEUInt32)(ZE_MDLF_PHYSICAL_BODY_CHUNKID + (ZEUInt32)'PHSH'))
#define ZE_MDLF_PHYSICAL_SHAPE_VERTEX_CHUNKID		((ZEUInt32)(ZE_MDLF_PHYSICAL_SHAPE_CHUNKID + (ZEUInt32)'VRTX'))
#define ZE_MDLF_PHYSICAL_SHAPE_INDEX_CHUNKID		((ZEUInt32)(ZE_MDLF_PHYSICAL_SHAPE_CHUNKID + (ZEUInt32)'INDX'))
#define ZE_MDLF_PHYSICAL_JOINT_CHUNKID				((ZEUInt32)(ZE_MDLF_HEADER + (ZEUInt32)'PJNT'))
#define	ZE_MDLF_ANIMATION_CHUNKID					((ZEUInt32)(ZE_MDLF_HEADER + (ZEUInt32)'ANIM'))
#define	ZE_MDLF_ANIMATION_KEYFRAME_CHUNKID			((ZEUInt32)(ZE_MDLF_ANIMATION_CHUNKID + (ZEUInt32)'ANKF'))

typedef ZEUInt32 ZEModelFilePhysicalShapeType;
#define ZE_MFPST_BOX								0
#define ZE_MFPST_SPHERE								1
#define ZE_MFPST_CAPSULE							2
#define ZE_MFPST_CYLINDER							3
#define ZE_MFPST_CONVEX								4

struct ZEModelFileMaterialChunk
{
	ZEUInt32								ChunkId;
	char								Shader[ZE_MDLF_MAX_FILENAME_SIZE];
	ZEUInt32								ShaderComponents;

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
	ZEUInt32								HEADER;
	ZEUInt32								Version;

	ZEUInt32								MaterialCount;
	ZEUInt32								MeshCount;
	ZEUInt32								BoneCount;
	ZEUInt32								AnimationCount;
};

struct ZEModelFileAnimationChunk
{
	ZEUInt32								ChunkId;
	char								Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEUInt32								FrameCount;
};

struct ZEModelFileAnimationFrameChunk
{
	ZEUInt32								ChunkId;
	ZEUInt32								BoneKeyCount;
	ZEUInt32								MeshKeyCount;
};

struct ZEModelFilePhysicalPolygonChunk
{	
	int									VertexIndexes[3];
};

struct ZEModelFilePhysicalShapeChunk
{
	ZEUInt32								ChunkId;
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
			ZEUInt32						VertexCount;
		} Convex;
	};
};

struct ZEModelFilePhysicalBodyChunk
{
	ZEUInt32								ChunkId;
	ZEUInt32				                Type;
	bool								Enabled;
	float								Mass;
	float								LinearDamping;
	float								AngularDamping;
	ZEVector3							MassCenter;
	ZEUInt32								ShapeCount;
};

struct ZEModelFilePhysicalJointChunk
{
	ZEUInt32								ChunkId;

	ZEUInt32								JointType;

	bool								Enabled;

	ZEUInt32								Body1Id;
	ZEUInt32								Body2Id;

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

	ZEUInt32								XMotion;
	ZEUInt32								YMotion;
	ZEUInt32								ZMotion;

	float 								LinearLimitValue;
	float 								LinearLimitRestitution;
	float 								LinearLimitSpring;
	float 								LinearLimitDamping;

	ZEUInt32								TwistMotion;
	float 								TwistLowLimitValue;
	float 								TwistLowLimitRestitution;
	float 								TwistLowLimitSpring;
	float 								TwistLowLimitDamping;
	float 								TwistHighLimitValue;
	float 								TwistHighLimitRestitution;
	float 								TwistHighLimitSpring;
	float 								TwistHighLimitDamping;

	ZEUInt32								Swing1Motion;
	float 								Swing1LimitValue;
	float 								Swing1LimitRestitution;
	float 								Swing1LimitSpring;
	float 								Swing1LimitDamping;

	ZEUInt32								Swing2Motion;
	float 								Swing2LimitValue;
	float 								Swing2LimitRestitution;
	float 								Swing2LimitSpring;
	float 								Swing2LimitDamping;

	ZEVector3 							MotorTargetPosition;
	ZEQuaternion 						MotorTargetOrientation;
	ZEVector3 							MotorTargetVelocity;
	ZEVector3 							MotorTargetAngularVelocity;

	ZEUInt32 							LinearXMotor;
	float 								LinearXMotorForce;
	float 								LinearXMotorSpring;
	float 								LinearXMotorDamper;

	ZEUInt32 							LinearYMotor;
	float 								LinearYMotorForce;
	float 								LinearYMotorSpring;
	float 								LinearYMotorDamper;

	ZEUInt32 							LinearZMotor;
	float 								LinearZMotorForce;
	float 								LinearZMotorSpring;
	float 								LinearZMotorDamper;

	ZEUInt32								AngularSwingMotor;
	float								AngularSwingMotorForce;
	float								AngularSwingMotorSpring;
	float								AngularSwingMotorDamper;

	ZEUInt32								AngularTwistMotor;
	float								AngularTwistMotorForce;
	float								AngularTwistMotorSpring;
	float								AngularTwistMotorDamper;

	ZEUInt32								AngularSlerpMotor;
	float								AngularSlerpMotorForce;
	float								AngularSlerpMotorSpring;
	float								AngularSlerpMotorDamper;
};

struct ZEModelFileMeshLODChunk
{
	ZEUInt32								ChunkId;
	ZEInt32								LODLevel;
	ZEInt32								MaterialId;
	ZEUInt32								VertexCount;
	ZEUInt32								AffectingBoneCount;
};

struct ZEModelFileMeshChunk
{
	ZEUInt32								ChunkId;
	char								Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEAABBox						BoundingBox;
	ZEVector3							Position;
	ZEQuaternion						Rotation;
	ZEVector3							Scale;
	bool								IsSkinned;	
	ZEUInt32								LODCount;
	bool								HasPhysicalBody;
};

struct ZEModelFileBoneChunk
{
	ZEUInt32								ChunkId;
	char								Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEInt32								ParentBone;
	ZEVector3							RelativePosition;
	ZEQuaternion						RelativeRotation;
	ZEVector3							RelativeScale;
	ZEAABBox						BoundingBox;
	bool								HasPhysicalBody;
	bool								HasPhysicalJoint;
};

#endif
