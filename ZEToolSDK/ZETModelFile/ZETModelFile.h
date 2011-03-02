//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETModelFile.h
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
#ifndef __ZET_MODEL_FILE_H__
#define __ZET_MODEL_FILE_H__

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEAABoundingBox.h"
#include "ZEDS/ZEArray.h"
#include "ZETTypes.h"
#include "ZETDefinitions.h"

#define ZE_MDLF_MAX_NAME_SIZE					ZE_MAX_NAME_SIZE
#define ZE_MDLF_MAX_FILENAME_SIZE				ZE_MAX_FILE_NAME_SIZE

enum ZEPhysicalShapeType
{
	ZE_PBST_BOX				= 0,
	ZE_PBST_SPHERE			= 1,
	ZE_PBST_CAPSULE			= 2,
	ZE_PBST_CYLINDER		= 3,
	ZE_PBST_CONVEX			= 4
};

enum ZEPhysicalBodyType
{
	ZE_PBT_NONE				= 0,
	ZE_PBT_RIGID			= 1,
	ZE_PBT_DEFORMABLE		= 2,
	ZE_PBT_CLOTH			= 3
};

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


#define ZESHADER_SKINTRANSFORM					1
#define ZESHADER_DIFFUSEMAP						2
#define ZESHADER_NORMALMAP						4
#define ZESHADER_SPECULARMAP					8
#define ZESHADER_EMMISIVEMAP					16
#define ZESHADER_OCAPASITYMAP					32
#define ZESHADER_DETAILDIFFUSEMAP				64
#define ZESHADER_DETAILNORMALMAP				128
#define ZESHADER_REFLECTION						256
#define ZESHADER_REFRACTION						512
#define ZESHADER_LIGHTMAP						1024
#define ZESHADER_DISTORTIONMAP					2048

struct ZEModelFileMaterial
{
	char										Shader[ZE_MDLF_MAX_FILENAME_SIZE];
	ZEDWORD										ShaderComponents;
	
	bool										TwoSided;
	bool										LightningEnabled;
	bool										Wireframe;
	bool										Transparant;

	ZEVector3									AmbientColor;
	ZEVector3									DiffuseColor;
	ZEVector3									SpecularColor;
	ZEVector3									EmmisiveColor;
	float										EmmisiveFactor;
	float										SpecularFactor;
	float										Opasity;
	float										ReflectionFactor;
	float										RefractionFactor;
	ZEVector2									DetailMapTiling;
		
	char										DiffuseMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char										NormalMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char										SpecularMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char										EmmisiveMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char										OpasityMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char										DetailMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char										DetailNormalMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char										ReflectionMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char										RefractionMap[ZE_MDLF_MAX_FILENAME_SIZE];
	char										LightMap[ZE_MDLF_MAX_FILENAME_SIZE];
};

struct ZEModelFileSkinnedVertex
{
	ZEVector3									Position;
	ZEVector3									Normal;
	ZEVector3									Tangent;
	ZEVector3									Binormal;
	ZEVector2									Texcoord;
	ZEBYTE										BoneIndices[4];
	float										BoneWeights[4];
};

struct ZEModelFileVertex
{
	ZEVector3									Position;
	ZEVector3									Normal;
	ZEVector3									Tangent;
	ZEVector3									Binormal;
	ZEVector2									Texcoord;
};

struct ZEModelFileMeshLOD
{
	ZEINT32										LODLevel;
	ZEINT32										MaterialId;
	ZEArray<ZEModelFileVertex>					Vertices;
	ZEArray<ZEModelFileSkinnedVertex>			SkinnedVertices;
	ZEArray<ZEDWORD>							AffectingBoneIds;
};

struct ZEModelFilePhysicalPolygon
{
	ZEDWORD										Indices[3];
};

struct ZEModelFilePhysicalShape
{
	ZEPhysicalShapeType							Type;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	float										Restitution;
	float										StaticFriction;
	float										DynamicFriction;
	
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

struct ZEModelFilePhysicalBody
{
	ZEPhysicalBodyType							Type;
	bool										Enabled;
	float										Mass;
	float										LinearDamping;
	float										AngularDamping;
	ZEVector3									MassCenter;
	ZEArray<ZEModelFilePhysicalShape>			Shapes;	

	void										SetZero();
};

struct ZEModelFileMesh
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE]; 
	ZEAABoundingBox								BoundingBox;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	ZEVector3									Scale;
	bool										IsSkinned;
	ZEArray<ZEModelFileMeshLOD>					LODs;
	ZEModelFilePhysicalBody						PhysicalBody;
};

struct ZEModelFilePhysicalJoint
{
	ZEDWORD										ChunkId;

	ZEDWORD										JointType;

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

	ZEDWORD										XMotion;
	ZEDWORD										YMotion;
	ZEDWORD										ZMotion;

	float 										LinearLimitValue;
	float 										LinearLimitRestitution;
	float 										LinearLimitSpring;
	float 										LinearLimitDamping;

	ZEDWORD										TwistMotion;
	float 										TwistLowLimitValue;
	float 										TwistLowLimitRestitution;
	float 										TwistLowLimitSpring;
	float 										TwistLowLimitDamping;
	float 										TwistHighLimitValue;
	float 										TwistHighLimitRestitution;
	float 										TwistHighLimitSpring;
	float 										TwistHighLimitDamping;

	ZEDWORD										Swing1Motion;
	float 										Swing1LimitValue;
	float 										Swing1LimitRestitution;
	float 										Swing1LimitSpring;
	float 										Swing1LimitDamping;

	ZEDWORD										Swing2Motion;
	float 										Swing2LimitValue;
	float 										Swing2LimitRestitution;
	float 										Swing2LimitSpring;
	float 										Swing2LimitDamping;

	ZEVector3 									MotorTargetPosition;
	ZEQuaternion 								MotorTargetOrientation;
	ZEVector3 									MotorTargetVelocity;
	ZEVector3 									MotorTargetAngularVelocity;

	ZEDWORD 									LinearXMotor;
	float 										LinearXMotorForce;
	float 										LinearXMotorSpring;
	float 										LinearXMotorDamper;

	ZEDWORD 									LinearYMotor;
	float 										LinearYMotorForce;
	float 										LinearYMotorSpring;
	float 										LinearYMotorDamper;

	ZEDWORD 									LinearZMotor;
	float 										LinearZMotorForce;
	float 										LinearZMotorSpring;
	float 										LinearZMotorDamper;

	ZEDWORD										AngularSwingMotor;
	float										AngularSwingMotorForce;
	float										AngularSwingMotorSpring;
	float										AngularSwingMotorDamper;

	ZEDWORD										AngularTwistMotor;
	float										AngularTwistMotorForce;
	float										AngularTwistMotorSpring;
	float										AngularTwistMotorDamper;

	ZEDWORD										AngularSlerpMotor;
	float										AngularSlerpMotorForce;
	float										AngularSlerpMotorSpring;
	float										AngularSlerpMotorDamper;

	void										SetZero();
};

struct ZEModelFileBone
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEINT32										ParentBone;
	ZEVector3									RelativePosition;
	ZEQuaternion								RelativeRotation;
	ZEVector3									RelativeScale;
	ZEAABoundingBox								BoundingBox;
	ZEModelFilePhysicalBody						PhysicalBody;
	ZEModelFilePhysicalJoint					PhysicalJoint;
};

struct ZEModelFileAnimationKey
{
	ZEDWORD										ItemId;
	ZEVector3									Position;
	ZEQuaternion								Rotation;
	ZEVector3									Scale;
};

struct ZEModelFileAnimationFrame
{
	ZEArray<ZEModelFileAnimationKey>			BoneKeys;
	ZEArray<ZEModelFileAnimationKey>			MeshKeys;
};

struct ZEModelFileAnimation
{
	char										Name[ZE_MDLF_MAX_NAME_SIZE];
	ZEArray<ZEModelFileAnimationFrame>			Frames;
};


class ZEModelFile
{
	public:
		ZEAABoundingBox							BoundingBox;
		ZEArray<ZEModelFileMaterial>			Materials;
		ZEArray<ZEModelFileMesh>				Meshes;
		ZEArray<ZEModelFileBone>				Bones;
		ZEArray<ZEModelFileAnimation>			Animations;

		void									Clear();
		bool									WriteToFile(const char* FileName);
		bool									ReadFromFile(const char* FileName);
		bool									Validate();
};

#endif
