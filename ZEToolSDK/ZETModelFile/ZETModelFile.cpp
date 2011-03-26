//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETModelFile.cpp
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

#include "ZETModelFile.h"
#include "ZETModelFileFormat.h"
#include "ZETOutput.h"

#include <string.h>
#include <stdio.h>
#include <float.h>

#pragma warning(push)
#pragma warning(disable:4307 4267 4018 4996)

static void WriteMaterialsToFile(FILE* File, ZEModelFile* Model)
{	
	zesdkLog("Model File", "Writing materials. Material Count : %d.", Model->Materials.GetCount());

	for (size_t I = 0; I < Model->Materials.GetCount(); I++)
	{
		ZEModelFileMaterial* Material = &Model->Materials[I]; 
		ZEModelFileMaterialChunk MaterialChunk;

		MaterialChunk.ChunkId = ZE_MDLF_MATERIAL_CHUNKID;
		strncpy(MaterialChunk.Shader, Material->Shader, ZE_MDLF_MAX_FILENAME_SIZE);
		MaterialChunk.ShaderComponents = Material->ShaderComponents;

		MaterialChunk.TwoSided = Material->TwoSided;
		MaterialChunk.LightningEnabled = Material->LightningEnabled;
		MaterialChunk.Wireframe = Material->Wireframe;
		MaterialChunk.Transparant = Material->Transparant;

		MaterialChunk.AmbientColor = Material->AmbientColor;
		MaterialChunk.DiffuseColor = Material->DiffuseColor;
		MaterialChunk.SpecularColor = Material->SpecularColor;
		MaterialChunk.EmmisiveColor = Material->EmmisiveColor;
		MaterialChunk.EmmisiveFactor = Material->EmmisiveFactor;
		MaterialChunk.SpecularFactor = Material->SpecularFactor;
		MaterialChunk.Opasity = Material->Opasity;
		MaterialChunk.ReflectionFactor = Material->ReflectionFactor;
		MaterialChunk.RefractionFactor = Material->RefractionFactor;
		MaterialChunk.DetailMapTiling = Material->DetailMapTiling;

		strncpy(MaterialChunk.DiffuseMap, Material->DiffuseMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(MaterialChunk.NormalMap, Material->NormalMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(MaterialChunk.SpecularMap, Material->SpecularMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(MaterialChunk.EmmisiveMap, Material->EmmisiveMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(MaterialChunk.OpasityMap, Material->OpasityMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(MaterialChunk.DetailMap, Material->DetailMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(MaterialChunk.DetailNormalMap, Material->DetailNormalMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(MaterialChunk.ReflectionMap, Material->ReflectionMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(MaterialChunk.RefractionMap, Material->RefractionMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(MaterialChunk.LightMap, Material->LightMap, ZE_MDLF_MAX_FILENAME_SIZE);

		fwrite(&MaterialChunk, sizeof(ZEModelFileMaterialChunk), 1, File);
	}
}

static bool WritePhysicalBodyToFile(ZEModelFilePhysicalBody* Body, FILE* File)
{
	zesdkDebug("Model File", "Writing physical body.");

	ZEModelFilePhysicalBodyChunk BodyChunk;
	memset(&BodyChunk, 0, sizeof(ZEModelFilePhysicalBodyChunk));

	BodyChunk.ChunkId			= ZE_MDLF_PHYSICAL_BODY_CHUNKID;
	BodyChunk.Enabled			= Body->Enabled;
	BodyChunk.Type				= (ZEDWORD)Body->Type;
	BodyChunk.Mass				= Body->Mass;
	BodyChunk.AngularDamping	= Body->AngularDamping;
	BodyChunk.LinearDamping		= Body->LinearDamping;
	BodyChunk.MassCenter		= Body->MassCenter;
	BodyChunk.ShapeCount		= Body->Shapes.GetCount();

	fwrite(&BodyChunk, sizeof(ZEModelFilePhysicalBodyChunk), 1, File);

	for (size_t I = 0; I < Body->Shapes.GetCount(); I++)
	{
		ZEModelFilePhysicalShape* Shape = &Body->Shapes[I];

		ZEModelFilePhysicalShapeChunk ShapeChunk;
		memset(&ShapeChunk, 0, sizeof(ZEModelFilePhysicalShapeChunk));

		ShapeChunk.ChunkId 					= ZE_MDLF_PHYSICAL_SHAPE_CHUNKID;
		ShapeChunk.Type 					= (ZEDWORD)Shape->Type;
		ShapeChunk.Position 				= Shape->Position;
		ShapeChunk.Rotation 				= Shape->Rotation;
		ShapeChunk.StaticFriction 			= Shape->StaticFriction;
		ShapeChunk.DynamicFriction 			= Shape->DynamicFriction;
		ShapeChunk.Restitution 				= Shape->Restitution;

		switch (ShapeChunk.Type)
		{
			case ZE_PBST_BOX:
			{
				ShapeChunk.Box.Width		= Shape->Box.Width;
				ShapeChunk.Box.Height		= Shape->Box.Height;
				ShapeChunk.Box.Length		= Shape->Box.Length;
				break;
			}
			case ZE_PBST_SPHERE:
			{
				ShapeChunk.Sphere.Radius	= Shape->Sphere.Radius;
				break;
			}
			case ZE_PBST_CAPSULE:
			{
				ShapeChunk.Capsule.Height	= Shape->Capsule.Height;
				ShapeChunk.Capsule.Radius	= Shape->Capsule.Radius;
				break;
			}
			case ZE_PBST_CYLINDER:
			{
				ShapeChunk.Cylinder.Height	= Shape->Cylinder.Height;
				ShapeChunk.Cylinder.Radius	= Shape->Cylinder.Radius;
				break;
			}
			case ZE_PBST_CONVEX:
			{
				ShapeChunk.Convex.VertexCount = Shape->Convex.Vertices.GetCount();
				break;
			}
			default:
			{
				zesdkError("Model File", "Wrong physical shape type.");
				return false;
			}
		}

		fwrite(&ShapeChunk, sizeof(ZEModelFilePhysicalShapeChunk), 1, File);

		if (ShapeChunk.Type == ZE_PBST_CONVEX)
		{
			ZEDWORD ChunkId = ZE_MDLF_PHYSICAL_SHAPE_VERTEX_CHUNKID;
			fwrite(&ChunkId, sizeof(ZEDWORD), 1, File); 
			fwrite(Shape->Convex.Vertices.GetCArray(), sizeof(ZEVector3), ShapeChunk.Convex.VertexCount, File);
		}
	}

	return true;
}

bool WritePhysicalJointToFile(ZEModelFilePhysicalJoint* Joint, FILE* File)
{
	ZEModelFilePhysicalJointChunk JointChunk;
	memset(&JointChunk, 0, sizeof(ZEModelFilePhysicalJointChunk));

	JointChunk.ChunkId						= ZE_MDLF_PHYSICAL_JOINT_CHUNKID;

	JointChunk.Enabled						= Joint->Enabled;

	JointChunk.JointType 					= (ZEDWORD)Joint->JointType;

	JointChunk.Body1Id						= Joint->Body1Id;
	JointChunk.Body2Id						= Joint->Body2Id;

	JointChunk.UseGlobalAnchorAxis 			= Joint->UseGlobalAnchorAxis;
	JointChunk.GlobalAnchor 				= Joint->GlobalAnchor;
	JointChunk.GlobalAxis 					= Joint->GlobalAxis;

	JointChunk.LocalAnchor1 				= Joint->LocalAnchor1;
	JointChunk.LocalAnchor2 				= Joint->LocalAnchor2;

	JointChunk.LocalAxis1 					= Joint->LocalAxis1;
	JointChunk.LocalAxis2 					= Joint->LocalAxis2;

	JointChunk.Breakable 					= Joint->Breakable;
	JointChunk.BreakForce 					= Joint->BreakForce;
	JointChunk.BreakTorque 					= Joint->BreakTorque;

	JointChunk.CollideBodies				= Joint->CollideBodies;

	JointChunk.XMotion						= Joint->XMotion;
	JointChunk.YMotion						= Joint->YMotion;
	JointChunk.ZMotion						= Joint->ZMotion;

	JointChunk.LinearLimitDamping			= Joint->LinearLimitDamping;
	JointChunk.LinearLimitRestitution		= Joint->LinearLimitRestitution;
	JointChunk.LinearLimitSpring			= Joint->LinearLimitSpring;
	JointChunk.LinearLimitValue				= Joint->LinearLimitValue;

	JointChunk.Swing1Motion					= Joint->Swing1Motion;
	JointChunk.Swing1LimitDamping			= Joint->Swing1LimitDamping;
	JointChunk.Swing1LimitRestitution		= Joint->Swing1LimitRestitution;
	JointChunk.Swing1LimitSpring			= Joint->Swing1LimitSpring;
	JointChunk.Swing1LimitValue				= Joint->Swing1LimitValue;

	JointChunk.Swing2Motion					= Joint->Swing2Motion;
	JointChunk.Swing2LimitDamping			= Joint->Swing2LimitDamping;
	JointChunk.Swing2LimitRestitution		= Joint->Swing2LimitRestitution;
	JointChunk.Swing2LimitSpring			= Joint->Swing2LimitSpring;
	JointChunk.Swing2LimitValue				= Joint->Swing2LimitValue;

	JointChunk.TwistMotion					= Joint->TwistMotion;
	JointChunk.TwistLowLimitDamping			= Joint->TwistLowLimitDamping;
	JointChunk.TwistLowLimitValue			= Joint->TwistLowLimitValue;
	JointChunk.TwistLowLimitValue			= Joint->TwistLowLimitValue;
	JointChunk.TwistLowLimitRestitution		= Joint->TwistLowLimitRestitution;
	JointChunk.TwistLowLimitSpring			= Joint->TwistLowLimitSpring;
	JointChunk.TwistHighLimitDamping		= Joint->TwistHighLimitDamping;
	JointChunk.TwistHighLimitValue			= Joint->TwistHighLimitValue;
	JointChunk.TwistHighLimitValue			= Joint->TwistHighLimitValue;
	JointChunk.TwistHighLimitRestitution	= Joint->TwistHighLimitRestitution;
	JointChunk.TwistHighLimitSpring			= Joint->TwistHighLimitSpring;

	JointChunk.MotorTargetPosition			= Joint->MotorTargetPosition;
	JointChunk.MotorTargetOrientation		= Joint->MotorTargetOrientation;
	JointChunk.MotorTargetVelocity			= Joint->MotorTargetVelocity;
	JointChunk.MotorTargetAngularVelocity	= Joint->MotorTargetAngularVelocity;

	JointChunk.LinearXMotor					= Joint->LinearXMotor;
	JointChunk.LinearXMotorDamper			= Joint->LinearXMotorDamper;
	JointChunk.LinearXMotorForce			= Joint->LinearXMotorForce;
	JointChunk.LinearXMotorSpring			= Joint->LinearXMotorSpring;

	JointChunk.LinearYMotor					= Joint->LinearYMotor;
	JointChunk.LinearYMotorDamper			= Joint->LinearYMotorDamper;
	JointChunk.LinearYMotorForce			= Joint->LinearYMotorForce;
	JointChunk.LinearYMotorSpring			= Joint->LinearYMotorSpring;

	JointChunk.LinearZMotor					= Joint->LinearZMotor;
	JointChunk.LinearZMotorDamper			= Joint->LinearZMotorDamper;
	JointChunk.LinearZMotorForce			= Joint->LinearZMotorForce;
	JointChunk.LinearZMotorSpring			= Joint->LinearZMotorSpring;

	JointChunk.AngularSwingMotor			= Joint->AngularSwingMotor;
	JointChunk.AngularSwingMotorDamper		= Joint->AngularSwingMotorDamper;
	JointChunk.AngularSwingMotorForce		= Joint->AngularSwingMotorForce;
	JointChunk.AngularSwingMotorSpring		= Joint->AngularSwingMotorSpring;

	JointChunk.AngularTwistMotor			= Joint->AngularTwistMotor;
	JointChunk.AngularTwistMotorDamper		= Joint->AngularTwistMotorDamper;
	JointChunk.AngularTwistMotorForce		= Joint->AngularTwistMotorForce;
	JointChunk.AngularTwistMotorSpring		= Joint->AngularTwistMotorSpring;

	JointChunk.AngularSlerpMotor			= Joint->AngularSlerpMotor;
	JointChunk.AngularSlerpMotorDamper		= Joint->AngularSlerpMotorDamper;
	JointChunk.AngularSlerpMotorForce		= Joint->AngularSlerpMotorForce;
	JointChunk.AngularSlerpMotorSpring		= Joint->AngularSlerpMotorSpring;
	
	fwrite(&JointChunk, sizeof(ZEModelFilePhysicalJointChunk), 1, File);

	return true;
}

static void CalculateBoundingBox(ZEModelFileMesh* Mesh)
{
	Mesh->BoundingBox.Min = ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Mesh->BoundingBox.Max = ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (size_t I = 0; I < Mesh->LODs.GetCount(); I++)
	{
		ZEModelFileMeshLOD* CurrentLOD = &Mesh->LODs[I];
		if (Mesh->IsSkinned)
		{
			for (size_t N = 0; N < CurrentLOD->SkinnedVertices.GetCount(); N++)
			{
				ZEVector3& Position = CurrentLOD->SkinnedVertices[I].Position;

				if (Position.x < Mesh->BoundingBox.Min.x) Mesh->BoundingBox.Min.x = Position.x;
				if (Position.y < Mesh->BoundingBox.Min.y) Mesh->BoundingBox.Min.y = Position.y;
				if (Position.z < Mesh->BoundingBox.Min.z) Mesh->BoundingBox.Min.z = Position.z;
				if (Position.x > Mesh->BoundingBox.Max.x) Mesh->BoundingBox.Max.x = Position.x;
				if (Position.y > Mesh->BoundingBox.Max.y) Mesh->BoundingBox.Max.y = Position.y;
				if (Position.z > Mesh->BoundingBox.Max.z) Mesh->BoundingBox.Max.z = Position.z;
			}
		}
		else
		{
			for (size_t N = 0; N < CurrentLOD->Vertices.GetCount(); N++)
			{
				ZEVector3& Position = CurrentLOD->Vertices[I].Position;

				if (Position.x < Mesh->BoundingBox.Min.x) Mesh->BoundingBox.Min.x = Position.x;
				if (Position.y < Mesh->BoundingBox.Min.y) Mesh->BoundingBox.Min.y = Position.y;
				if (Position.z < Mesh->BoundingBox.Min.z) Mesh->BoundingBox.Min.z = Position.z;
				if (Position.x > Mesh->BoundingBox.Max.x) Mesh->BoundingBox.Max.x = Position.x;
				if (Position.y > Mesh->BoundingBox.Max.y) Mesh->BoundingBox.Max.y = Position.y;
				if (Position.z > Mesh->BoundingBox.Max.z) Mesh->BoundingBox.Max.z = Position.z;
			}
		}
	}
}

static void WriteMeshesToFile(FILE *File, ZEModelFile* Model)
{
	zesdkLog("Model File", "Writing meshes. Mesh Count : %d.", Model->Meshes.GetCount());


	for (size_t I = 0; I < Model->Meshes.GetCount(); I++)
	{
		ZEModelFileMesh* Mesh = &Model->Meshes[I];
		ZEModelFileMeshChunk MeshChunk;
		memset(&MeshChunk, 0, sizeof(ZEModelFileMeshChunk));

		MeshChunk.ChunkId						= ZE_MDLF_MESH_CHUNKID;
		strncpy(MeshChunk.Name, Mesh->Name, ZE_MDLF_MAX_NAME_SIZE);
		MeshChunk.Position						= Mesh->Position;
		MeshChunk.Rotation						= Mesh->Rotation;
		MeshChunk.Scale							= Mesh->Scale;
		MeshChunk.IsSkinned						= Mesh->IsSkinned;
		MeshChunk.BoundingBox					= Mesh->BoundingBox;
		MeshChunk.LODCount						= Mesh->LODs.GetCount();
		MeshChunk.HasPhysicalBody				= (Mesh->PhysicalBody.Type != ZE_PBT_NONE);
		fwrite(&MeshChunk, sizeof(ZEModelFileMeshChunk), 1, File);
		zesdkDebug("Model File", "Mesh %d is written. Name : %s, Position : <%f, %f, %f>, Rotation : <%f, %f, %f, %f>, Scale : <%f, %f, %f>, BoundingBox : [<%f, %f, %f>, <%f, %f, %f>], LOD count : %d.",
			I, MeshChunk.Name, 
			MeshChunk.Position.x, MeshChunk.Position.y, MeshChunk.Position.z, 
			MeshChunk.Rotation.x, MeshChunk.Rotation.y, MeshChunk.Rotation.z,  MeshChunk.Rotation.w, 
			MeshChunk.Scale.x,  MeshChunk.Scale.y,  MeshChunk.Scale.z, 
			MeshChunk.BoundingBox.Min.x,  MeshChunk.BoundingBox.Min.y, MeshChunk.BoundingBox.Min.z,
			MeshChunk.BoundingBox.Max.x,  MeshChunk.BoundingBox.Max.y, MeshChunk.BoundingBox.Max.z,
			MeshChunk.LODCount);

		zesdkDebug("Model File", "Writing LODs. LOD count : %d.", Mesh->LODs.GetCount());
		for (size_t I = 0; I < Mesh->LODs.GetCount(); I++)
		{
			ZEModelFileMeshLOD* MeshLOD = &Mesh->LODs[I];

			ZEModelFileMeshLODChunk MeshLODChunk;
			MeshLODChunk.ChunkId = ZE_MDLF_MESH_LOD_CHUNKID;
			MeshLODChunk.LODLevel = MeshLOD->LODLevel;
			MeshLODChunk.MaterialId = MeshLOD->MaterialId;
			MeshLODChunk.VertexCount = (MeshChunk.IsSkinned ? MeshLOD->SkinnedVertices.GetCount() : MeshLOD->Vertices.GetCount());
			MeshLODChunk.AffectingBoneCount = MeshLOD->AffectingBoneIds.GetCount();
			fwrite(&MeshLODChunk, sizeof(ZEModelFileMeshLODChunk), 1, File);

			if(MeshChunk.IsSkinned)
			{
				fwrite(MeshLOD->AffectingBoneIds.GetConstCArray(), sizeof(ZEDWORD), MeshLOD->AffectingBoneIds.GetCount(), File);

				for (int N = 0; N < MeshLODChunk.VertexCount; N++)
				{
					ZEVector3::Normalize(MeshLOD->SkinnedVertices[N].Normal, MeshLOD->SkinnedVertices[N].Normal);
					ZEVector3::Normalize(MeshLOD->SkinnedVertices[N].Binormal, MeshLOD->SkinnedVertices[N].Binormal);
					ZEVector3::Normalize(MeshLOD->SkinnedVertices[N].Tangent, MeshLOD->SkinnedVertices[N].Tangent);
				}
				fwrite(MeshLOD->SkinnedVertices.GetCArray(), sizeof(ZEModelFileSkinnedVertex),  MeshLOD->SkinnedVertices.GetCount(), File);
			}
			else
			{
				for (int N = 0; N < MeshLODChunk.VertexCount; N++)
				{
					ZEVector3::Normalize(MeshLOD->Vertices[N].Normal, MeshLOD->Vertices[N].Normal);
					ZEVector3::Normalize(MeshLOD->Vertices[N].Binormal, MeshLOD->Vertices[N].Binormal);
					ZEVector3::Normalize(MeshLOD->Vertices[N].Tangent, MeshLOD->Vertices[N].Tangent);
				}
				fwrite(MeshLOD->Vertices.GetCArray(), sizeof(ZEModelFileVertex),  MeshLOD->Vertices.GetCount(), File);
			}
			zesdkDebug("Model File", "Lod %d is written. LOD Level : %d, Material Id : %d, Vertex Count : %d.", I, MeshLODChunk.LODLevel, MeshLODChunk.MaterialId, MeshLODChunk.VertexCount);
		}

		if (MeshChunk.HasPhysicalBody)
			WritePhysicalBodyToFile(&Mesh->PhysicalBody, File);
	}
}

static void WriteBonesToFile(FILE *File, ZEModelFile* Model)
{	
	zesdkLog("Model File", "Writing Bones. Bone Count : %d.", Model->Bones.GetCount());

	for (size_t I = 0; I < Model->Bones.GetCount(); I++)
	{
		ZEModelFileBone* Bone = &Model->Bones[I];

		ZEModelFileBoneChunk BoneChunk;	
		memset(&BoneChunk, 0, sizeof(ZEModelFileBoneChunk));

		BoneChunk.ChunkId 					= ZE_MDLF_BONE_CHUNKID;
		strncpy(BoneChunk.Name, Bone->Name, ZE_MDLF_MAX_NAME_SIZE);
		BoneChunk.ParentBone 				= Bone->ParentBone;
		BoneChunk.RelativePosition 			= Bone->RelativePosition;
		BoneChunk.RelativeRotation	 		= Bone->RelativeRotation;
		BoneChunk.RelativeScale				= Bone->RelativeScale;
		BoneChunk.BoundingBox 				= Bone->BoundingBox;
		BoneChunk.HasPhysicalBody			= (Bone->PhysicalBody.Type != ZE_PBT_NONE);
		BoneChunk.HasPhysicalJoint			= (Bone->PhysicalJoint.JointType != ZE_PJT_NONE);

		fwrite(&BoneChunk, sizeof(ZEModelFileBoneChunk), 1, File);

		if (BoneChunk.HasPhysicalBody)
			WritePhysicalBodyToFile(&Bone->PhysicalBody, File);

		if (BoneChunk.HasPhysicalJoint)
		{
			ZEModelFilePhysicalJoint Joint		= Bone->PhysicalJoint;
			Joint.Body1Id 						= 0;
			Joint.Body2Id 						= 0;
			Joint.UseGlobalAnchorAxis 			= false;
			Joint.GlobalAnchor 					= Bone->RelativePosition;
			Joint.GlobalAxis 					= Bone->RelativeRotation;
			Joint.LocalAnchor1 					= ZEVector3(0.0f, 0.0f, 0.0f);
			Joint.LocalAnchor2 					= ZEVector3(0.0f, 0.0f, 0.0f);
			Joint.LocalAxis1 					= ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
			Joint.LocalAxis2 					= ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
			
			WritePhysicalJointToFile(&Joint, File);
		}
	}
}

static void WriteAnimationsToFile(FILE *File, ZEModelFile* Model)
{
	zesdkLog("Model File", "Writing Animations. Animation Count : %d.", Model->Animations.GetCount());

	for (size_t I = 0; I < Model->Animations.GetCount(); I++)
	{
		ZEModelFileAnimation* Animation = &Model->Animations[I];;
		
		ZEModelFileAnimationChunk AnimationChunk;
		AnimationChunk.ChunkId = ZE_MDLF_ANIMATION_CHUNKID;
		strncpy(AnimationChunk.Name, Animation->Name, ZE_MDLF_MAX_NAME_SIZE);
		AnimationChunk.FrameCount = Animation->Frames.GetCount();
		fwrite(&AnimationChunk, sizeof(ZEModelFileAnimationChunk), 1, File);

		zesdkDebug("Model File", "Animation %d is written. Name : \"%s\", Frame Count : %d. ", I, Animation->Name, Animation->Frames.GetCount());

		zesdkDebug("Model File", "Writing animation frames.");
		for (size_t I = 0; I < Animation->Frames.GetCount(); I++)
		{
			ZEModelFileAnimationFrame* AnimationFrame = &Animation->Frames[I];

			ZEModelFileAnimationFrameChunk AnimationFrameChunk;
			AnimationFrameChunk.ChunkId = ZE_MDLF_ANIMATION_KEYFRAME_CHUNKID;
			AnimationFrameChunk.BoneKeyCount = AnimationFrame->BoneKeys.GetCount();
			AnimationFrameChunk.MeshKeyCount = AnimationFrame->MeshKeys.GetCount();
			fwrite(&AnimationFrameChunk, sizeof(ZEModelFileAnimationFrameChunk), 1, File);
			zesdkDebug("Model File", "Animation frame %d is written. Bone keys count : %d, Mesh key count : %d.", I, AnimationFrameChunk.BoneKeyCount, AnimationFrameChunk.MeshKeyCount);

			fwrite(AnimationFrame->BoneKeys.GetCArray(), sizeof(ZEModelFileAnimationKey), AnimationFrame->BoneKeys.GetCount(), File);
			zesdkDebug("Model File", "Bone keys written.");

			fwrite(AnimationFrame->MeshKeys.GetCArray(), sizeof(ZEModelFileAnimationKey), AnimationFrame->MeshKeys.GetCount(), File);
			zesdkDebug("Model File", "Mesh keys written.");
		}
	}
}

bool ZEModelFile::WriteToFile(const char* FileName)
{
	zesdkLog("Model File", "Validating Model File.");
	if (!Validate())
	{
		zesdkError("Model File", "Model file is not validated. Please check validation errors and warnings.");
		return false;
	}
	
	zesdkLog("Model File",  "Writing ZEModel to file \"%s\".", FileName);
	FILE* File = fopen(FileName, "wb");
	if (File == NULL)
	{
		zesdkError("Model File", "Can not open ZEModel file for writing. (Filename : \"%s\")", FileName);
		return false;
	}

	ZEModelFileHeaderChunk HeaderChunk;
	HeaderChunk.HEADER = ZE_MDLF_HEADER;
	HeaderChunk.Version = ZE_MDLF_VERSION;
	HeaderChunk.MaterialCount = Materials.GetCount();
	HeaderChunk.MeshCount = Meshes.GetCount();
	HeaderChunk.BoneCount = Bones.GetCount();
	HeaderChunk.AnimationCount = Animations.GetCount();
	fwrite(&HeaderChunk, sizeof(ZEModelFileHeaderChunk), 1, File);

	WriteMaterialsToFile(File, this);
	WriteMeshesToFile(File, this);
	WriteBonesToFile(File, this);
	WriteAnimationsToFile(File, this);

	fclose(File);	
	zesdkLog("Model File", "ZEModel written to file \"%s\" succesfully.", FileName);
	return true;
}

//////////////////////////////////////////////////////////////// READ ////////////////////////////////////////////////////////////////


static bool ReadMaterialsFromFile(FILE* File, ZEModelFile* Model)
{
	zesdkLog("Model File", "Reading materials. Material Count : %d.", Model->Materials.GetCount());

	for (size_t I = 0; I < Model->Materials.GetCount(); I++)
	{
		ZEModelFileMaterial* Material = &Model->Materials[I]; 
		ZEModelFileMaterialChunk MaterialChunk;

		fread(&MaterialChunk, sizeof(ZEModelFileMaterialChunk), 1, File);
		if(MaterialChunk.ChunkId != ZE_MDLF_MATERIAL_CHUNKID)
		{
			zesdkError("Model File", "Corrupted ZEModel file. Materials chunk id does not matches.");
			return false;
		}

		strncpy(Material->Shader, MaterialChunk.Shader, ZE_MDLF_MAX_FILENAME_SIZE);

		Material->ShaderComponents = MaterialChunk.ShaderComponents;
		Material->TwoSided = MaterialChunk.TwoSided;
		Material->LightningEnabled = MaterialChunk.LightningEnabled;
		Material->Wireframe = MaterialChunk.Wireframe;
		Material->Transparant = MaterialChunk.Transparant;

		Material->AmbientColor = MaterialChunk.AmbientColor;
		Material->DiffuseColor = MaterialChunk.DiffuseColor;
		Material->SpecularColor = MaterialChunk.SpecularColor;
		Material->EmmisiveColor = MaterialChunk.EmmisiveColor;
		Material->EmmisiveFactor = MaterialChunk.EmmisiveFactor;
		Material->SpecularFactor = MaterialChunk.SpecularFactor;
		Material->Opasity = MaterialChunk.Opasity;
		Material->ReflectionFactor = MaterialChunk.ReflectionFactor;
		Material->RefractionFactor = MaterialChunk.RefractionFactor;
		Material->DetailMapTiling = MaterialChunk.DetailMapTiling;

		strncpy(Material->DiffuseMap, MaterialChunk.DiffuseMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(Material->NormalMap, MaterialChunk.NormalMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(Material->SpecularMap, MaterialChunk.SpecularMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(Material->EmmisiveMap, MaterialChunk.EmmisiveMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(Material->OpasityMap, MaterialChunk.OpasityMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(Material->DetailMap, MaterialChunk.DetailMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(Material->DetailNormalMap, MaterialChunk.DetailNormalMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(Material->ReflectionMap, MaterialChunk.ReflectionMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(Material->RefractionMap, MaterialChunk.RefractionMap, ZE_MDLF_MAX_FILENAME_SIZE);
		strncpy(Material->LightMap, MaterialChunk.LightMap, ZE_MDLF_MAX_FILENAME_SIZE);
	}
	return true;
}

static bool ReadPhysicalBodyFromFile(ZEModelFilePhysicalBody* Body, FILE* File)
{
	zesdkLog("Model File", "Reading physical body.");
	ZEModelFilePhysicalBodyChunk BodyChunk;
	fread(&BodyChunk, sizeof(ZEModelFilePhysicalBodyChunk), 1, File);
	
	if (BodyChunk.ChunkId != ZE_MDLF_PHYSICAL_BODY_CHUNKID)
	{
		zesdkError("Model Resource", "Corrupted ZEModel file. Physical body chunk id does not matches.");
		return false;
	}

	Body->Type				= (ZEPhysicalBodyType)BodyChunk.Type;
	Body->Enabled			= BodyChunk.Enabled;
	Body->Mass				= BodyChunk.Mass;
	Body->AngularDamping	= BodyChunk.AngularDamping;
	Body->LinearDamping		= BodyChunk.LinearDamping;
	Body->MassCenter		= BodyChunk.MassCenter;

	Body->Shapes.SetCount(BodyChunk.ShapeCount);
	for (size_t I = 0; I < Body->Shapes.GetCount(); I++)
	{
		ZEModelFilePhysicalShape* Shape = &Body->Shapes[I];

		ZEModelFilePhysicalShapeChunk ShapeChunk;
		fread(&ShapeChunk, sizeof(ZEModelFilePhysicalShapeChunk), 1, File);

		if(ShapeChunk.ChunkId != ZE_MDLF_PHYSICAL_SHAPE_CHUNKID)
		{
			zesdkError("Model Resource", "Corrupted ZEModel file. Physical shape chunk id does not matches.");
			return false;
		}

		Shape->Type						= (ZEPhysicalShapeType)ShapeChunk.Type;
		Shape->Position					= ShapeChunk.Position;
		Shape->Rotation					= ShapeChunk.Rotation;
		Shape->StaticFriction			= ShapeChunk.StaticFriction;
		Shape->DynamicFriction			= ShapeChunk.DynamicFriction;
		Shape->Restitution				= ShapeChunk.Restitution;

		
		switch (Shape->Type)
		{
			case ZE_PBST_BOX:
			{
				Shape->Box.Width		= ShapeChunk.Box.Width;
				Shape->Box.Height		= ShapeChunk.Box.Height;
				Shape->Box.Length		= ShapeChunk.Box.Length;
				break;
			}
			case ZE_PBST_SPHERE:
			{
				Shape->Sphere.Radius	= ShapeChunk.Sphere.Radius;
				break;
			}
			case ZE_PBST_CAPSULE:
			{
				Shape->Capsule.Height	= ShapeChunk.Capsule.Height;
				Shape->Capsule.Radius	= ShapeChunk.Capsule.Radius;
				break;
			}
			case ZE_PBST_CYLINDER:
			{
				Shape->Cylinder.Height	= ShapeChunk.Cylinder.Height;
				Shape->Cylinder.Radius	= ShapeChunk.Cylinder.Radius;
			}
			case ZE_PBST_CONVEX:
			{
				ZEDWORD ChunkId;
				fread(&ChunkId, sizeof(ZEDWORD),1, File);
				if (ChunkId != ZE_MDLF_PHYSICAL_SHAPE_VERTEX_CHUNKID)
				{
					zesdkError("Model Resource", "Corrupted ZEModel file. Physical body shape vertex chunk id does not matches.");
					return false;
				}
				
				Shape->Convex.Vertices.SetCount(ShapeChunk.Convex.VertexCount);
				fread(Shape->Convex.Vertices.GetCArray(), sizeof(ZEVector3), Shape->Convex.Vertices.GetCount(), File);
				break;
			}
			default:
			{
				zesdkError("Model Resource", "Wrong physical shape type.");
				return false;
			}
		}
	}
	return true;
}

bool ReadPhysicalJointFromFile(ZEModelFilePhysicalJoint* Joint, FILE* File)
{
	ZEModelFilePhysicalJointChunk JointChunk;
	
	if (Joint->JointType == ZE_PJT_NONE)
		return true;

	fread(&JointChunk, sizeof(ZEModelFilePhysicalJointChunk), 1, File);

	if (JointChunk.ChunkId != ZE_MDLF_PHYSICAL_JOINT_CHUNKID)
	{
		zesdkError("Model Resource", "Corrupted ZEModel file. Physical joint chunk id does not matches.");
		return false;
	}

	Joint->JointType					= (ZEPhysicalJointType)JointChunk.JointType;

	Joint->Enabled						= JointChunk.Enabled;

	Joint->Body1Id						= JointChunk.Body1Id;
	Joint->Body2Id						= JointChunk.Body2Id;

	Joint->Breakable					= JointChunk.Breakable;
	Joint->BreakForce					= JointChunk.BreakForce;
	Joint->BreakTorque					= JointChunk.BreakTorque;

	Joint->CollideBodies				= JointChunk.CollideBodies;

	Joint->UseGlobalAnchorAxis			= JointChunk.UseGlobalAnchorAxis;

	Joint->GlobalAnchor					= JointChunk.GlobalAnchor;
	Joint->GlobalAxis					= JointChunk.GlobalAxis;

	Joint->LocalAnchor1					= JointChunk.LocalAnchor1;
	Joint->LocalAnchor2					= JointChunk.LocalAnchor2;

	Joint->LocalAxis1					= JointChunk.LocalAxis1;
	Joint->LocalAxis2					= JointChunk.LocalAxis2;

	Joint->XMotion						= JointChunk.XMotion;
	Joint->YMotion						= JointChunk.YMotion;
	Joint->ZMotion						= JointChunk.ZMotion;

	Joint->LinearLimitDamping			= JointChunk.LinearLimitDamping;
	Joint->LinearLimitRestitution		= JointChunk.LinearLimitRestitution;
	Joint->LinearLimitSpring			= JointChunk.LinearLimitSpring;
	Joint->LinearLimitValue				= JointChunk.LinearLimitValue;

	Joint->Swing1Motion					= JointChunk.Swing1Motion;
	Joint->Swing1LimitDamping			= JointChunk.Swing1LimitDamping;
	Joint->Swing1LimitRestitution		= JointChunk.Swing1LimitRestitution;
	Joint->Swing1LimitSpring			= JointChunk.Swing1LimitSpring;
	Joint->Swing1LimitValue				= JointChunk.Swing1LimitValue;

	Joint->Swing2Motion					= JointChunk.Swing2Motion;
	Joint->Swing2LimitDamping			= JointChunk.Swing2LimitDamping;
	Joint->Swing2LimitRestitution		= JointChunk.Swing2LimitRestitution;
	Joint->Swing2LimitSpring			= JointChunk.Swing2LimitSpring;
	Joint->Swing2LimitValue				= JointChunk.Swing2LimitValue;

	Joint->TwistMotion					= JointChunk.TwistMotion;
	Joint->TwistLowLimitDamping			= JointChunk.TwistLowLimitDamping;
	Joint->TwistLowLimitValue			= JointChunk.TwistLowLimitValue;
	Joint->TwistLowLimitValue			= JointChunk.TwistLowLimitValue;
	Joint->TwistLowLimitRestitution		= JointChunk.TwistLowLimitRestitution;
	Joint->TwistLowLimitSpring			= JointChunk.TwistLowLimitSpring;
	Joint->TwistHighLimitDamping		= JointChunk.TwistHighLimitDamping;
	Joint->TwistHighLimitValue			= JointChunk.TwistHighLimitValue;
	Joint->TwistHighLimitValue			= JointChunk.TwistHighLimitValue;
	Joint->TwistHighLimitRestitution	= JointChunk.TwistHighLimitRestitution;
	Joint->TwistHighLimitSpring			= JointChunk.TwistHighLimitSpring;

	Joint->MotorTargetPosition			= JointChunk.MotorTargetPosition;
	Joint->MotorTargetOrientation		= JointChunk.MotorTargetOrientation;
	Joint->MotorTargetVelocity			= JointChunk.MotorTargetVelocity;
	Joint->MotorTargetAngularVelocity	= JointChunk.MotorTargetAngularVelocity;

	Joint->LinearXMotor					= JointChunk.LinearXMotor;
	Joint->LinearXMotorDamper			= JointChunk.LinearXMotorDamper;
	Joint->LinearXMotorForce			= JointChunk.LinearXMotorForce;
	Joint->LinearXMotorSpring			= JointChunk.LinearXMotorSpring;

	Joint->LinearYMotor					= JointChunk.LinearYMotor;
	Joint->LinearYMotorDamper			= JointChunk.LinearYMotorDamper;
	Joint->LinearYMotorForce			= JointChunk.LinearYMotorForce;
	Joint->LinearYMotorSpring			= JointChunk.LinearYMotorSpring;

	Joint->LinearZMotor					= JointChunk.LinearZMotor;
	Joint->LinearZMotorDamper			= JointChunk.LinearZMotorDamper;
	Joint->LinearZMotorForce			= JointChunk.LinearZMotorForce;
	Joint->LinearZMotorSpring			= JointChunk.LinearZMotorSpring;

	Joint->AngularSwingMotor			= JointChunk.AngularSwingMotor;
	Joint->AngularSwingMotorDamper		= JointChunk.AngularSwingMotorDamper;
	Joint->AngularSwingMotorForce		= JointChunk.AngularSwingMotorForce;
	Joint->AngularSwingMotorSpring		= JointChunk.AngularSwingMotorSpring;

	Joint->AngularTwistMotor			= JointChunk.AngularTwistMotor;
	Joint->AngularTwistMotorDamper		= JointChunk.AngularTwistMotorDamper;
	Joint->AngularTwistMotorForce		= JointChunk.AngularTwistMotorForce;
	Joint->AngularTwistMotorSpring		= JointChunk.AngularTwistMotorSpring;

	Joint->AngularSlerpMotor			= JointChunk.AngularSlerpMotor;
	Joint->AngularSlerpMotorDamper		= JointChunk.AngularSlerpMotorDamper;
	Joint->AngularSlerpMotorForce		= JointChunk.AngularSlerpMotorForce;
	Joint->AngularSlerpMotorSpring		= JointChunk.AngularSlerpMotorSpring;

	return true;
}

static bool ReadMeshesFromFile(FILE* File, ZEModelFile* Model)
{
	zesdkLog("Model File", "Reading meshes. Mesh count : %d.", Model->Meshes.GetCount());

	for (size_t I = 0; I < Model->Meshes.GetCount(); I++)
	{
		ZEModelFileMesh* Mesh = &Model->Meshes[I];

		ZEModelFileMeshChunk MeshChunk;
		fread(&MeshChunk, sizeof(ZEModelFileMeshChunk), 1, File);
		
		if(MeshChunk.ChunkId != ZE_MDLF_MESH_CHUNKID)
		{
			zesdkError("Model Resource",  "Corrupted ZEModel file. Mesh's chunk id does not matches.");
			return false;
		}

		strncpy(Mesh->Name, MeshChunk.Name, ZE_MDLF_MAX_NAME_SIZE);
		Mesh->IsSkinned						= MeshChunk.IsSkinned;
		Mesh->BoundingBox					= MeshChunk.BoundingBox;
		Mesh->LODs.SetCount(MeshChunk.LODCount);
		Mesh->Position						= MeshChunk.Position;
		Mesh->Rotation						= MeshChunk.Rotation;
		Mesh->Scale							= MeshChunk.Scale;

		for (size_t I = 0; I < Mesh->LODs.GetCount(); I++)
		{
			ZEModelFileMeshLOD* LOD	= &Mesh->LODs[I];
			ZEModelFileMeshLODChunk MeshLODChunk;
			fread(&MeshLODChunk, sizeof(ZEModelFileMeshLODChunk), 1, File);
			if (MeshLODChunk.ChunkId != ZE_MDLF_MESH_LOD_CHUNKID)
			{
				zesdkError("Model Resource",  "Corrupted ZEModel file. Mesh LOD's chunk id does not matches.");
				return false;			
			}

			LOD->MaterialId = MeshLODChunk.MaterialId;
			LOD->LODLevel = MeshLODChunk.LODLevel;
			if(MeshChunk.IsSkinned)
			{
				LOD->AffectingBoneIds.SetCount(MeshLODChunk.AffectingBoneCount);
				fread(LOD->AffectingBoneIds.GetCArray(), sizeof(ZEDWORD), LOD->AffectingBoneIds.GetCount(), File);

				LOD->SkinnedVertices.SetCount(MeshLODChunk.VertexCount);				
				fread(LOD->SkinnedVertices.GetCArray(), sizeof(ZEModelFileSkinnedVertex),  LOD->SkinnedVertices.GetCount(), File);
			}
			else
			{
				LOD->Vertices.SetCount(MeshLODChunk.VertexCount);				
				fread(LOD->Vertices.GetCArray(), sizeof(ZEModelFileVertex),  LOD->Vertices.GetCount(), File);
			}
			zesdkDebug("Model File", "Lod %d is read. LOD Level : %d, Material Id : %d, Vertex Count : %d.", I, LOD->LODLevel, LOD->MaterialId, (Mesh->IsSkinned ? LOD->SkinnedVertices.GetCount() : LOD->Vertices.GetCount()));
		}

		if (MeshChunk.HasPhysicalBody)
		{
			if (!ReadPhysicalBodyFromFile(&Mesh->PhysicalBody, File))
				return false;
		}
		else
			Mesh->PhysicalBody.SetZero();
	}

	return true;
}

static bool ReadBonesFromFile(FILE* File, ZEModelFile* Model)
{
	zesdkLog("Model File", "Reading bones. Bone count : %d.", Model->Bones.GetCount());

	for (size_t I = 0; I < Model->Bones.GetCount(); I++)
	{
		ZEModelFileBone* Bone = &Model->Bones[I];

		ZEModelFileBoneChunk BoneChunk;
		fread(&BoneChunk, sizeof(ZEModelFileBoneChunk), 1, File);

		if(BoneChunk.ChunkId != ZE_MDLF_BONE_CHUNKID)
		{
			zesdkError("Model Resource",  "Corrupted ZEModel file. Bone chunk id does not matches.");
			return false;
		}

		strncpy(Bone->Name, BoneChunk.Name, ZE_MDLF_MAX_NAME_SIZE);
		Bone->BoundingBox												= BoneChunk.BoundingBox;
		Bone->ParentBone												= BoneChunk.ParentBone;
		Bone->RelativeRotation											= BoneChunk.RelativeRotation;
		Bone->RelativePosition											= BoneChunk.RelativePosition;	
		Bone->RelativeScale												= BoneChunk.RelativeScale;
		if (BoneChunk.HasPhysicalBody)
		{
			if (!ReadPhysicalBodyFromFile(&Bone->PhysicalBody, File))
				return false;
		}
		else
			Bone->PhysicalBody.SetZero();

		if (BoneChunk.HasPhysicalJoint)
		{
			if (!ReadPhysicalJointFromFile(&Bone->PhysicalJoint, File))
				return false;
		}
		else
			Bone->PhysicalJoint.SetZero();

	}

	return true;
}

static bool ReadAnimationsFromFile(FILE* File, ZEModelFile* Model)
{
	zesdkLog("Model File", "Reading animations. Animation count : %d.", Model->Animations.GetCount());

	for (size_t I = 0; I < Model->Animations.GetCount(); I++)
	{
		ZEModelFileAnimation* Animation = &Model->Animations[I];

		ZEModelFileAnimationChunk AnimationChunk;
		fread(&AnimationChunk, sizeof(ZEModelFileAnimationChunk), 1, File);

		if(AnimationChunk.ChunkId != ZE_MDLF_ANIMATION_CHUNKID)
		{
			zesdkError("Model File", "Corrupted ZEModel file. Animation chunk id does not matches.");
			return false;
		}

		strncpy(Animation->Name, AnimationChunk.Name, ZE_MDLF_MAX_NAME_SIZE);
		zesdkDebug("Model File", "Animation %d is read. Name : \"%s\", Frames : %d.", Animation->Name, Animation->Frames.GetCount());

		zesdkDebug("Model File", "Reading animation frames.");
		Animation->Frames.SetCount(AnimationChunk.FrameCount);
		for (size_t I = 0; I < Animation->Frames.GetCount(); I++)
		{
			ZEModelFileAnimationFrame* AnimationFrame = &Animation->Frames[I];

			ZEModelFileAnimationFrameChunk AnimationFrameChunk;
			fread(&AnimationFrameChunk, sizeof(ZEModelFileAnimationFrameChunk), 1, File);

			if(AnimationFrameChunk.ChunkId != ZE_MDLF_ANIMATION_KEYFRAME_CHUNKID)
			{
				zesdkError("Model File", "Corrupted ZEModel file. Animation frame chunk id does not matches.");
				return false;
			}

			AnimationFrame->BoneKeys.SetCount(AnimationFrameChunk.BoneKeyCount);
			fread(AnimationFrame->BoneKeys.GetCArray(), sizeof(ZEModelFileAnimationKey), AnimationFrame->BoneKeys.GetCount(), File);

			AnimationFrame->MeshKeys.SetCount(AnimationFrameChunk.MeshKeyCount);		
			fread(AnimationFrame->MeshKeys.GetCArray(), sizeof(ZEModelFileAnimationKey), AnimationFrame->MeshKeys.GetCount(), File);
		}
	}
	return true;
}

bool ZEModelFile::ReadFromFile(const char *FileName)
{
	zesdkLog("Model File", "Reading \"%s\" ZEModel file.", FileName);
	FILE* File = fopen(FileName, "rb");
	if (File == NULL)
	{
		zesdkError("Model File", "Can not open file \"%s\".");
		return false;
	}

	Clear();

	zesdkDebug("Model File", "Reading header.");

	ZEModelFileHeaderChunk HeaderChunk;
	fread(&HeaderChunk, sizeof(ZEModelFileHeaderChunk), 1, File);

	if(HeaderChunk.HEADER != ZE_MDLF_HEADER)
	{
		zesdkError("Model File", "Unknown ZEModel file format.");
		return false;
	}

	if(HeaderChunk.Version != ZE_MDLF_VERSION)
	{
		zesdkError("Model File", "ZEModel file version mismatched.");
		return false;	
	}

	Materials.SetCount(HeaderChunk.MaterialCount);
	if (!ReadMaterialsFromFile(File, this))
	{
		zesdkError("Model File", "Can not read model file.");
		fclose(File);
		return false;
	}

	Meshes.SetCount(HeaderChunk.MeshCount);
	if(!ReadMeshesFromFile(File, this))
	{
		zesdkError("Model File", "Can not read model file.");
		fclose(File);
		return false;
	}

	Bones.SetCount(HeaderChunk.BoneCount);
	if(!ReadBonesFromFile(File, this))
	{
		zesdkError("Model File", "Can not read model file.");
		fclose(File);
		return false;
	}

	Animations.SetCount(HeaderChunk.AnimationCount);
	if(!ReadAnimationsFromFile(File, this))
	{
		zesdkError("Model File", "Can not read model file.");
		fclose(File);
		return false;
	}

	fclose(File);

	zesdkLog("Model File", "Validating Model File.");
	if (!Validate())
	{
		zesdkError("Model File", "Model file is not validated. Please be caution this file looks like corrupted. Please check validation errors and warnings.");
		return false;
	}
	return true;
}

bool ZEModelFile::Validate()
{
	bool Validated = true;

	for(size_t I = 0; I < Meshes.GetCount(); I++)
	{
		for (size_t N = 0; N < Meshes[I].LODs.GetCount(); N++)
		{
			ZEModelFileMeshLOD& MeshLOD = Meshes[I].LODs[N];
			if (MeshLOD.MaterialId < 0 || MeshLOD.MaterialId >= Materials.GetCount())
			{
				zesdkError("Model File Validation", "Mesh %d LOD %d does not have a valid material id.", I, N);
				Validated = false;
			}


			if (MeshLOD.SkinnedVertices.GetCount() != 0 && MeshLOD.Vertices.GetCount())
			{
				zesdkError("Model File Validation", "Mesh %d LOD %d both have skinned and non skinned vertices. Mesh file can contain only one type of vertices.", I, N);
				Validated = false;				
			}
			
			for (size_t M = 0; M < MeshLOD.AffectingBoneIds.GetCount(); M++)
			{
				if (MeshLOD.AffectingBoneIds[M] >= Bones.GetCount())
				{
					zesdkError("Model File Validation", "Mesh %d LOD %d Affecting Bone Index %d has ilvalid value. Value exceeds bone count.", I, N, M);
					Validated = false;						
				}
			}

			for (size_t M = 0; M < MeshLOD.SkinnedVertices.GetCount(); M++)
			{
				ZEModelFileSkinnedVertex& Vertex = MeshLOD.SkinnedVertices[M];
				for (int P = 0; P < 4; P++)
					if (Vertex.BoneIndices[P] >= MeshLOD.AffectingBoneIds.GetCount())
					{
						zesdkError("Model File Validation", "Mesh %d LOD %d Vertex %d Bone Index %d has ilvalid value. Value exceeds affecting bone count.", I, N, M, P);
						Validated = false;						
					}

					if (fabsf(Vertex.BoneWeights[0] + Vertex.BoneWeights[1] + Vertex.BoneWeights[2] + Vertex.BoneWeights[3] - 1.0f ) > 0.001)
						zesdkWarning("Model File Validation", "Mesh %d LOD %d Vertex %d sum of bone weights is not one.", I, N, M);					
			}
		}
	}

	/*for(size_t I = 0; I < Materials.GetCount(); I++)
	{
		bool Used = false;
		for(size_t N = 0; N < Meshes.GetCount(); N++)
			for(size_t M = 0; Meshes[N].LODs.GetCount(); M++)
				if(Meshes[N].LODs[M].MaterialId == I)
				{
					Used = true;
					break;
				}
		if(!Used)
			zesdkNotice("Model File Validation", "Material %d is never used.",I);
	}*/

	return Validated;
}

void ZEModelFilePhysicalBody::SetZero()
{
	Shapes.Clear();	
	memset(this, 0, sizeof(ZEModelFilePhysicalBody));
}

void ZEModelFilePhysicalJoint::SetZero()
{
	memset(this, 0, sizeof(ZEModelFilePhysicalJoint));
}

void ZEModelFile::Clear()
{
	Materials.Clear();
	Meshes.Clear();
	Bones.Clear();
	Animations.Clear();
}

#pragma warning(pop)
