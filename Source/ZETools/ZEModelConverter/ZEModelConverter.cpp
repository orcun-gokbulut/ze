//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelConverter.cpp
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

#include "ZEModelConverter.h"

#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"
#include "ZEFile/ZEPathManager.h"
#include "ZEFile/ZEFile.h"
#include "ZEModel/ZEMDVertex.h"
#include "ZEModel/ZEMDResourceAnimation.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEError.h"

struct ZEModelConverterMaterialDefinition
{
	ZEString Name;
	ZEString FilePath;
};

static ZEArray<ZEModelConverterMaterialDefinition> Materials;

static void ConvertPhysicalBody(ZEMLReaderNode* SourcePhysicalBodyNode, ZEMLWriterNode* DestinationPhysicalBodyNode)
{
	if (SourcePhysicalBodyNode == NULL || DestinationPhysicalBodyNode == NULL)
		return;

	if (!SourcePhysicalBodyNode->IsValid())
		return;

	DestinationPhysicalBodyNode->WriteBool("Enabled", SourcePhysicalBodyNode->ReadBoolean("Enabled"));
	DestinationPhysicalBodyNode->WriteInt32("Type", SourcePhysicalBodyNode->ReadInt32("Type"));
	DestinationPhysicalBodyNode->WriteBool("Kinematic", SourcePhysicalBodyNode->ReadBoolean("Kinematic", SourcePhysicalBodyNode->ReadBoolean("IsKinematic")));
	DestinationPhysicalBodyNode->WriteFloat("Mass", SourcePhysicalBodyNode->ReadFloat("Mass"));
	DestinationPhysicalBodyNode->WriteVector3("MassCenter", SourcePhysicalBodyNode->ReadVector3("MassCenter"));
	DestinationPhysicalBodyNode->WriteFloat("LinearDamping", SourcePhysicalBodyNode->ReadFloat("LinearDamping"));
	DestinationPhysicalBodyNode->WriteFloat("AngularDamping", SourcePhysicalBodyNode->ReadFloat("AngularDamping"));

	ZEMLReaderNode SourceShapesNode = SourcePhysicalBodyNode->GetNode("PhysicalShapes");

	if (!SourceShapesNode.IsValid())
		return;

	ZEMLWriterNode DestinationShapesNode;
	DestinationPhysicalBodyNode->OpenNode("PhysicalShapes", DestinationShapesNode);

	ZESize ShapeCount = SourceShapesNode.GetNodeCount("PhysicalShape");

	for (ZESize I = 0; I < ShapeCount; I++)
	{
		ZEMLWriterNode DestinationShapeNode;
		DestinationShapesNode.OpenNode("PhysicalShape", DestinationShapeNode);
		ZEMLReaderNode SourceShapeNode = SourceShapesNode.GetNode("PhysicalShape", I);

		DestinationShapeNode.WriteVector3("Position", SourceShapeNode.ReadVector3("Position"));
		DestinationShapeNode.WriteVector3("Rotation", SourceShapeNode.ReadVector3("Rotation"));
		DestinationShapeNode.WriteFloat("Restitution", SourceShapeNode.ReadFloat("Restitution"));
		DestinationShapeNode.WriteFloat("DynamicFriction", SourceShapeNode.ReadFloat("DynamicFriction"));
		DestinationShapeNode.WriteFloat("StaticFriction", SourceShapeNode.ReadFloat("StaticFriction"));
		DestinationShapeNode.WriteString("UserDefinedProperties", SourceShapeNode.ReadString("UserDefinedProperties"));
		ZEInt32 ShapeType = SourceShapeNode.ReadInt32("Type");
		DestinationShapeNode.WriteInt32("Type", ShapeType);
		
		if (SourceShapeNode.IsPropertyExists("Width"))
			DestinationShapeNode.WriteFloat("Width", SourceShapeNode.ReadFloat("Width"));
		
		if (SourceShapeNode.IsPropertyExists("Height"))
			DestinationShapeNode.WriteFloat("Height", SourceShapeNode.ReadFloat("Height"));

		if (SourceShapeNode.IsPropertyExists("Length"))
			DestinationShapeNode.WriteFloat("Length", SourceShapeNode.ReadFloat("Length"));

		if (SourceShapeNode.IsPropertyExists("Radius"))
			DestinationShapeNode.WriteFloat("Radius", SourceShapeNode.ReadFloat("Radius"));

		if (SourceShapeNode.IsPropertyExists("Vertices"))
		{
			ZEArray<ZEVector3> ShapeVertices;
			ShapeVertices.SetCount(SourceShapeNode.ReadDataSize("Vertices") / sizeof(ZEVector3));
			if (!SourceShapeNode.ReadDataItems("Vertices", ShapeVertices.GetCArray(), sizeof(ZEVector3), ShapeVertices.GetCount()))
				return;

			DestinationShapeNode.WriteData("Vertices", ShapeVertices.GetCArray(), ShapeVertices.GetCount() * sizeof(ZEVector3));
		}

		if (SourceShapeNode.IsPropertyExists("Indices"))
		{
			ZEArray<ZEVector3> ShapeIndices;
			ShapeIndices.SetCount(SourceShapeNode.ReadDataSize("Indices") / sizeof(ZEVector3));
			if (!SourceShapeNode.ReadDataItems("Indices", ShapeIndices.GetCArray(), sizeof(ZEVector3), ShapeIndices.GetCount()))
				return;

			DestinationShapeNode.WriteData("Indices", ShapeIndices.GetCArray(), ShapeIndices.GetCount() * sizeof(ZEVector3));
		}

		DestinationShapeNode.CloseNode();
		DestinationShapesNode.CloseNode();
	}
}

static void ConvertPhysicalJoint(ZEMLReaderNode* SourceJointNode, ZEMLWriterNode* DestinationJointNode)
{
	if (SourceJointNode == NULL || DestinationJointNode == NULL)
		return;

	if (!SourceJointNode->IsValid())
		return;

	DestinationJointNode->WriteBool("Enabled", SourceJointNode->ReadBoolean("Enabled"));
	DestinationJointNode->WriteInt32("Body1Id", SourceJointNode->ReadInt32("Body1Id", -1));
	DestinationJointNode->WriteInt32("Body2Id", SourceJointNode->ReadInt32("Body2Id", -1));

	bool IsUseGlobalAxis = SourceJointNode->ReadBoolean("UseGlobalAnchorAxis");
	DestinationJointNode->WriteBool("UseGlobalAnchorAxis", IsUseGlobalAxis);

	if (IsUseGlobalAxis)
	{
		DestinationJointNode->WriteVector3("GlobalAnchor", SourceJointNode->ReadVector3("GlobalAnchor", ZEVector3::Zero));
		DestinationJointNode->WriteQuaternion("GlobalAxis", SourceJointNode->ReadQuaternion("GlobalAxis", ZEQuaternion::Identity));
	}
	else
	{
		DestinationJointNode->WriteVector3("LocalAnchor1", SourceJointNode->ReadVector3("LocalAnchor1", ZEVector3::Zero));
		DestinationJointNode->WriteQuaternion("LocalAxis1", SourceJointNode->ReadQuaternion("LocalAxis1", ZEQuaternion::Identity));
		DestinationJointNode->WriteVector3("LocalAnchor2", SourceJointNode->ReadVector3("LocalAnchor2", ZEVector3::Zero));
		DestinationJointNode->WriteQuaternion("LocalAxis2", SourceJointNode->ReadQuaternion("LocalAxis2", ZEQuaternion::Identity));
	}

	DestinationJointNode->WriteInt32("JointType", SourceJointNode->ReadInt32("JointType"));
	DestinationJointNode->WriteBool("CollideBodies", SourceJointNode->ReadBoolean("CollideBodies", false));
	DestinationJointNode->WriteBool("Breakable", SourceJointNode->ReadBoolean("Breakable", false));
	DestinationJointNode->WriteFloat("BreakForce", SourceJointNode->ReadFloat("BreakForce", 0.0f));
	DestinationJointNode->WriteFloat("BreakTorque", SourceJointNode->ReadFloat("BreakTorque", 0.0f));

	DestinationJointNode->WriteInt32("XMotion", SourceJointNode->ReadInt32("XMotion"));
	DestinationJointNode->WriteInt32("YMotion", SourceJointNode->ReadInt32("YMotion"));
	DestinationJointNode->WriteInt32("ZMotion", SourceJointNode->ReadInt32("ZMotion"));
	DestinationJointNode->WriteFloat("LinearLimitValue", SourceJointNode->ReadFloat("LinearLimitValue", 0.0f));
	DestinationJointNode->WriteFloat("LinearLimitRestitution", SourceJointNode->ReadFloat("LinearLimitRestitution", 0.0f));
	DestinationJointNode->WriteFloat("LinearLimitSpring", SourceJointNode->ReadFloat("LinearLimitSpring", 0.0f));
	DestinationJointNode->WriteFloat("LinearLimitDamping", SourceJointNode->ReadFloat("LinearLimitDamping", 0.0f));

	DestinationJointNode->WriteInt32("TwistMotion", SourceJointNode->ReadInt32("TwistMotion"));
	DestinationJointNode->WriteFloat("TwistLowLimitValue", SourceJointNode->ReadFloat("TwistLowLimitValue", 0.0f));
	DestinationJointNode->WriteFloat("TwistLowLimitRestitution", SourceJointNode->ReadFloat("TwistLowLimitRestitution", 0.0f));
	DestinationJointNode->WriteFloat("TwistLowLimitSpring", SourceJointNode->ReadFloat("TwistLowLimitSpring", 0.0f));
	DestinationJointNode->WriteFloat("TwistLowLimitDamping", SourceJointNode->ReadFloat("TwistLowLimitDamping", 0.0f));
	DestinationJointNode->WriteFloat("TwistHighLimitValue", SourceJointNode->ReadFloat("TwistHighLimitValue", 0.0f));
	DestinationJointNode->WriteFloat("TwistHighLimitRestitution", SourceJointNode->ReadFloat("TwistHighLimitRestitution", 0.0f));
	DestinationJointNode->WriteFloat("TwistHighLimitSpring", SourceJointNode->ReadFloat("TwistHighLimitSpring", 0.0f));
	DestinationJointNode->WriteFloat("TwistHighLimitDamping", SourceJointNode->ReadFloat("TwistHighLimitDamping", 0.0f));

	DestinationJointNode->WriteInt32("Swing1Motion", SourceJointNode->ReadInt32("Swing1Motion"));
	DestinationJointNode->WriteFloat("Swing1LimitValue", SourceJointNode->ReadFloat("Swing1LimitValue", 0.0f));
	DestinationJointNode->WriteFloat("Swing1LimitRestitution", SourceJointNode->ReadFloat("Swing1LimitRestitution"));
	DestinationJointNode->WriteFloat("Swing1LimitSpring", SourceJointNode->ReadFloat("Swing1LimitSpring", 0.0f));
	DestinationJointNode->WriteFloat("Swing1LimitDamping", SourceJointNode->ReadFloat("Swing1LimitDamping", 0.0f));

	DestinationJointNode->WriteInt32("Swing2Motion", SourceJointNode->ReadInt32("Swing2Motion"));
	DestinationJointNode->WriteFloat("Swing2LimitValue", SourceJointNode->ReadFloat("Swing2LimitValue", 0.0f));
	DestinationJointNode->WriteFloat("Swing2LimitRestitution", SourceJointNode->ReadFloat("Swing2LimitRestitution", 0.0f));
	DestinationJointNode->WriteFloat("Swing2LimitSpring", SourceJointNode->ReadFloat("Swing2LimitSpring", 0.0f));
	DestinationJointNode->WriteFloat("Swing2LimitDamping", SourceJointNode->ReadFloat("Swing2LimitDamping", 0.0f));

	DestinationJointNode->WriteInt32("LinearXMotor", SourceJointNode->ReadInt32("LinearXMotor"));
	DestinationJointNode->WriteFloat("LinearXMotorForce", SourceJointNode->ReadFloat("LinearXMotorForce", 0.0f));
	DestinationJointNode->WriteFloat("LinearXMotorSpring", SourceJointNode->ReadFloat("LinearXMotorSpring", 0.0f));
	DestinationJointNode->WriteFloat("LinearXMotorDamper", SourceJointNode->ReadFloat("LinearXMotorDamper", 0.0f));

	DestinationJointNode->WriteInt32("LinearYMotor", SourceJointNode->ReadInt32("LinearYMotor"));
	DestinationJointNode->WriteFloat("LinearYMotorForce", SourceJointNode->ReadFloat("LinearYMotorForce", 0.0f));
	DestinationJointNode->WriteFloat("LinearYMotorSpring", SourceJointNode->ReadFloat("LinearYMotorSpring", 0.0f));
	DestinationJointNode->WriteFloat("LinearYMotorDamper", SourceJointNode->ReadFloat("LinearYMotorDamper", 0.0f));

	DestinationJointNode->WriteInt32("LinearZMotor", SourceJointNode->ReadInt32("LinearZMotor"));
	DestinationJointNode->WriteFloat("LinearZMotorForce", SourceJointNode->ReadFloat("LinearZMotorForce", 0.0f));
	DestinationJointNode->WriteFloat("LinearZMotorSpring", SourceJointNode->ReadFloat("LinearZMotorSpring", 0.0f));
	DestinationJointNode->WriteFloat("LinearZMotorDamper", SourceJointNode->ReadFloat("LinearZMotorDamper", 0.0f));

	DestinationJointNode->WriteInt32("AngularSwingMotor", SourceJointNode->ReadInt32("AngularSwingMotor"));
	DestinationJointNode->WriteFloat("AngularSwingMotorForce", SourceJointNode->ReadFloat("AngularSwingMotorForce", 0.0f));
	DestinationJointNode->WriteFloat("AngularSwingMotorSpring", SourceJointNode->ReadFloat("AngularSwingMotorSpring", 0.0f));
	DestinationJointNode->WriteFloat("AngularSwingMotorDamper", SourceJointNode->ReadFloat("AngularSwingMotorDamper", 0.0f));

	DestinationJointNode->WriteInt32("AngularTwistMotor", SourceJointNode->ReadInt32("AngularTwistMotor"));
	DestinationJointNode->WriteFloat("AngularTwistMotorForce", SourceJointNode->ReadFloat("AngularTwistMotorForce", 0.0f));
	DestinationJointNode->WriteFloat("AngularTwistMotorSpring", SourceJointNode->ReadFloat("AngularTwistMotorSpring", 0.0f));
	DestinationJointNode->WriteFloat("AngularTwistMotorDamper", SourceJointNode->ReadFloat("AngularTwistMotorDamper", 0.0f));

	DestinationJointNode->WriteInt32("AngularSlerpMotorForce", SourceJointNode->ReadInt32("AngularSlerpMotorForce"));
	DestinationJointNode->WriteFloat("AngularSlerpMotorForce", SourceJointNode->ReadFloat("AngularSlerpMotorForce", 0.0f));
	DestinationJointNode->WriteFloat("AngularSlerpMotorSpring", SourceJointNode->ReadFloat("AngularSlerpMotorSpring", 0.0f));
	DestinationJointNode->WriteFloat("AngularSlerpMotorDamper", SourceJointNode->ReadFloat("AngularSlerpMotorDamper", 0.0f));

	DestinationJointNode->WriteVector3("MotorTargetPosition", SourceJointNode->ReadVector3("MotorTargetPosition", ZEVector3::Zero));
	DestinationJointNode->WriteVector3("MotorTargetVelocity", SourceJointNode->ReadVector3("MotorTargetVelocity", ZEVector3::Zero));
	DestinationJointNode->WriteQuaternion("MotorTargetOrientation", SourceJointNode->ReadQuaternion("MotorTargetOrientation", ZEQuaternion::Identity));
	DestinationJointNode->WriteVector3("MotorTargetAngularVelocity", SourceJointNode->ReadVector3("MotorTargetAngularVelocity", ZEVector3::Zero));
}

static void ConvertBone(ZEMLReaderNode* SourceBoneNode, ZEMLWriterNode* DestinationBoneNode)
{
	if (SourceBoneNode == NULL || DestinationBoneNode == NULL)
		return;

	if (!SourceBoneNode->IsValid())
		return;

	DestinationBoneNode->WriteString("Name", SourceBoneNode->ReadString("Name"));
	DestinationBoneNode->WriteInt32("ParentBone", SourceBoneNode->ReadInt32("ParentBone", -1));

	ZEMLReaderNode SourceBBoxNode = SourceBoneNode->GetNode("BoundingBox");

	if (!SourceBBoxNode.IsValid())
		return;

	ZEMLWriterNode DestinationBBoxNode;
	DestinationBoneNode->OpenNode("BoundingBox", DestinationBBoxNode);
	DestinationBBoxNode.WriteVector3("Min", SourceBBoxNode.ReadVector3("Min"));
	DestinationBBoxNode.WriteVector3("Max", SourceBBoxNode.ReadVector3("Max"));
	DestinationBBoxNode.CloseNode();

	DestinationBoneNode->WriteVector3("RelativePosition", SourceBoneNode->ReadVector3("RelativePosition"));
	DestinationBoneNode->WriteQuaternion("RelativeRotation", SourceBoneNode->ReadQuaternion("RelativeRotation"));
	DestinationBoneNode->WriteVector3("RelativeScale", SourceBoneNode->ReadVector3("RelativeScale"));

	if (SourceBoneNode->IsPropertyExists("UserDefinedProperties"))
		DestinationBoneNode->WriteString("UserDefinedProperties", SourceBoneNode->ReadString("UserDefinedProperties"));

	ZEMLReaderNode SourcePhysicalBodyNode = SourceBoneNode->GetNode("PhysicalBody");

	if (SourcePhysicalBodyNode.IsValid())
	{
		ZEMLWriterNode DestinationPhysicalBodyNode;
		DestinationBoneNode->OpenNode("PhysicalBody", DestinationPhysicalBodyNode);
		ConvertPhysicalBody(&SourcePhysicalBodyNode, &DestinationPhysicalBodyNode);
		DestinationPhysicalBodyNode.CloseNode();
	}

	ZEMLReaderNode SourcePhysicalJointNode = SourceBoneNode->GetNode("PhysicalJoint");

	if (SourcePhysicalJointNode.IsValid() && SourcePhysicalJointNode.ReadBoolean("Enabled"))
	{
		ZEMLWriterNode DestinationPhysicalJointNode;
		DestinationBoneNode->OpenNode("PhysicalJoint", DestinationPhysicalJointNode);
		ConvertPhysicalJoint(&SourcePhysicalJointNode, &DestinationPhysicalJointNode);
		DestinationPhysicalJointNode.CloseNode();
	}
}

static void ConvertMeshBoundingBox(ZEMLReaderNode* SourceMeshNode, ZEMLWriterNode* DestinationBoundingBoxNode)
{
	if (SourceMeshNode == NULL || DestinationBoundingBoxNode == NULL)
		return;

	if (!SourceMeshNode->IsValid())
		return;

	ZEMLReaderNode LODsNode = SourceMeshNode->GetNode("LODs");
	if (!LODsNode.IsValid())
		return;

	ZEMLReaderNode MainLODNode = LODsNode.GetNode("LOD", 0);
	if (!MainLODNode.IsValid())
		return;

	ZEAABBox BoundingBox;
	bool Skinned = SourceMeshNode->ReadBoolean("IsSkinned");

	if (Skinned)
	{
		ZEArray<ZEMDVertexSkin>	VerticesSkin;
		VerticesSkin.SetCount(MainLODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertexSkin));
		if (!MainLODNode.ReadDataItems("Vertices", VerticesSkin.GetCArray(), sizeof(ZEMDVertexSkin), VerticesSkin.GetCount()))
			return;

		if (VerticesSkin.GetCount() == 0)
		{
			BoundingBox.Min = BoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);
		}
		else
		{
			BoundingBox.Min = BoundingBox.Max = VerticesSkin[0].Position;

			for (ZESize I = 0; I < VerticesSkin.GetCount(); I++)
			{
				ZEVector3 Point = VerticesSkin[I].Position;
				if (Point.x < BoundingBox.Min.x) BoundingBox.Min.x = Point.x;
				if (Point.y < BoundingBox.Min.y) BoundingBox.Min.y = Point.y;
				if (Point.z < BoundingBox.Min.z) BoundingBox.Min.z = Point.z;
				if (Point.x > BoundingBox.Max.x) BoundingBox.Max.x = Point.x;
				if (Point.y > BoundingBox.Max.y) BoundingBox.Max.y = Point.y;
				if (Point.z > BoundingBox.Max.z) BoundingBox.Max.z = Point.z;
			}
		}
	}
	else
	{
		ZEArray<ZEMDVertex>	Vertices;
		Vertices.SetCount(MainLODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertex));
		if (!MainLODNode.ReadDataItems("Vertices", Vertices.GetCArray(), sizeof(ZEMDVertex), Vertices.GetCount()))
			return;

		if (Vertices.GetCount() == 0)
		{
			BoundingBox.Min = BoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);
		}
		else
		{
			BoundingBox.Min = BoundingBox.Max = Vertices[0].Position;

			for (ZESize I = 0; I < Vertices.GetCount(); I++)
			{
				ZEVector3 Point = Vertices[I].Position;
				if (Point.x < BoundingBox.Min.x) BoundingBox.Min.x = Point.x;
				if (Point.y < BoundingBox.Min.y) BoundingBox.Min.y = Point.y;
				if (Point.z < BoundingBox.Min.z) BoundingBox.Min.z = Point.z;
				if (Point.x > BoundingBox.Max.x) BoundingBox.Max.x = Point.x;
				if (Point.y > BoundingBox.Max.y) BoundingBox.Max.y = Point.y;
				if (Point.z > BoundingBox.Max.z) BoundingBox.Max.z = Point.z;
			}
		}
	}

	DestinationBoundingBoxNode->WriteVector3("Min", BoundingBox.Min);
	DestinationBoundingBoxNode->WriteVector3("Max", BoundingBox.Max);
}

static void ConvertMeshLOD(ZEMLReaderNode* SourceLODNode, ZEMLWriterNode* DestinationLODNode)
{
	if (SourceLODNode == NULL || DestinationLODNode == NULL)
		return;

	if (!SourceLODNode->IsValid())
		return;

	DestinationLODNode->WriteInt32("Level", SourceLODNode->ReadInt32("LODLevel", 0));
	DestinationLODNode->WriteFloat("StartDistance", SourceLODNode->ReadFloat("LODStartDistance", 0.0f));
	DestinationLODNode->WriteFloat("EndDistance", SourceLODNode->ReadInt32("LODEndDistance", 100000.0f));

	bool IsSourceLODSkinned = SourceLODNode->IsPropertyExists("AffectingBoneIds");
	DestinationLODNode->WriteInt32("VertexType", SourceLODNode->ReadInt32("VertexType", (IsSourceLODSkinned?ZEMD_VT_SKINNED:ZEMD_VT_NORMAL)));

	//CHANGE INDEX TYPE
	DestinationLODNode->WriteInt32("IndexType", SourceLODNode->ReadInt32("IndexType", ZEMD_VIT_NONE)); 


	ZESize VertexCount = 0;
	if (IsSourceLODSkinned)
	{
		ZEArray<ZEMDVertexSkin>	VerticesSkin;
		VerticesSkin.SetCount(SourceLODNode->ReadDataSize("Vertices") / sizeof(ZEMDVertexSkin));
		if (!SourceLODNode->ReadDataItems("Vertices", VerticesSkin.GetCArray(), sizeof(ZEMDVertexSkin), VerticesSkin.GetCount()))
			return;

		DestinationLODNode->WriteData("Vertices", VerticesSkin.GetCArray(), VerticesSkin.GetCount() * sizeof(ZEMDVertexSkin));
		VertexCount = VerticesSkin.GetCount();
	}
	else
	{
		ZEArray<ZEMDVertex>	Vertices;
		Vertices.SetCount(SourceLODNode->ReadDataSize("Vertices") / sizeof(ZEMDVertex));
		if (!SourceLODNode->ReadDataItems("Vertices", Vertices.GetCArray(), sizeof(ZEMDVertex), Vertices.GetCount()))
			return;

		DestinationLODNode->WriteData("Vertices", Vertices.GetCArray(), Vertices.GetCount() * sizeof(ZEMDVertex));
		VertexCount = Vertices.GetCount();
	}

	//INDICES ENTRIES HERE

	if (SourceLODNode->IsSubNodeExists("Draws"))
		return;

	ZEMLWriterNode DrawsNode, DrawNode;
	DestinationLODNode->OpenNode("Draws", DrawsNode);
	DrawsNode.OpenNode("Draw", DrawNode);

	DrawNode.WriteUInt32("Offset", 0);
	DrawNode.WriteUInt32("Count", VertexCount);

	ZEInt32 MaterialID = SourceLODNode->ReadInt32("MaterialId");

	if (MaterialID < Materials.GetCount())
	{
		DrawNode.WriteString("MaterialName", Materials[MaterialID].Name);
		DrawNode.WriteString("MaterialFilePath", Materials[MaterialID].FilePath);
	}

	DrawNode.CloseNode();
	DrawsNode.CloseNode();
}

static void ConvertMesh(ZEMLReaderNode* SourceMeshNode, ZEMLWriterNode* DestinationMeshNode)
{
	if (SourceMeshNode == NULL || DestinationMeshNode == NULL)
		return;

	if (!SourceMeshNode->IsValid())
		return;

	DestinationMeshNode->WriteString("Name", SourceMeshNode->ReadString("Name"));
	DestinationMeshNode->WriteInt32("ParentMesh", SourceMeshNode->ReadInt32("ParentMesh", -1));

	ZEMLWriterNode BoundingBoxNode;
	DestinationMeshNode->OpenNode("BoundingBox", BoundingBoxNode);
	ConvertMeshBoundingBox(SourceMeshNode, &BoundingBoxNode);
	BoundingBoxNode.CloseNode();

	DestinationMeshNode->WriteBool("Visible", SourceMeshNode->ReadBoolean("Visible", SourceMeshNode->ReadBoolean("IsVisible")));
	DestinationMeshNode->WriteBool("Skinned", SourceMeshNode->ReadBoolean("Skinned", SourceMeshNode->ReadBoolean("IsSkinned")));

	DestinationMeshNode->WriteVector3("Position", SourceMeshNode->ReadVector3("Position"));
	DestinationMeshNode->WriteQuaternion("Rotation", SourceMeshNode->ReadQuaternion("Rotation"));
	DestinationMeshNode->WriteVector3("Scale", SourceMeshNode->ReadVector3("Scale"));

	if (SourceMeshNode->IsPropertyExists("UserDefinedProperties"))
		DestinationMeshNode->WriteString("UserDefinedProperties", SourceMeshNode->ReadString("UserDefinedProperties"));

	ZEMLReaderNode SourcePhysicalBodyNode = SourceMeshNode->GetNode("PhysicalBody");

	if (SourcePhysicalBodyNode.IsValid())
	{
		ZEMLWriterNode DestinationPhysicalBodyNode;
		DestinationMeshNode->OpenNode("PhysicalBody", DestinationPhysicalBodyNode);
		ConvertPhysicalBody(&SourcePhysicalBodyNode, &DestinationPhysicalBodyNode);
		DestinationPhysicalBodyNode.CloseNode();
	}

	ZEMLReaderNode SourceLODsNode = SourceMeshNode->GetNode("LODs");

	if (!SourceLODsNode.IsValid())
		return;

	ZESize LODCount = SourceLODsNode.GetNodeCount("LOD");

	ZEMLWriterNode DestinationLODsNode;
	DestinationMeshNode->OpenNode("LODs", DestinationLODsNode);
	
	for (ZESize I = 0; I < LODCount; I++)
	{
		ZEMLWriterNode DestinationLODNode;
		DestinationLODsNode.OpenNode("LOD", DestinationLODNode);
		ConvertMeshLOD(&SourceLODsNode.GetNode("LOD", I), &DestinationLODNode);
		DestinationLODNode.CloseNode();
	}

	DestinationLODsNode.CloseNode();
}

static void ConvertHelper(ZEMLReaderNode* SourceHelperNode, ZEMLWriterNode* DestinationHelperNode)
{
	if (SourceHelperNode == NULL || DestinationHelperNode == NULL)
		return;

	if (!SourceHelperNode->IsValid())
		return;

	DestinationHelperNode->WriteString("Name", SourceHelperNode->ReadString("Name"));
	DestinationHelperNode->WriteInt32("OwnerType", SourceHelperNode->ReadInt32("OwnerType"));
	DestinationHelperNode->WriteInt32("OwnerId", SourceHelperNode->ReadInt32("OwnerId", -1));
	DestinationHelperNode->WriteVector3("Position", SourceHelperNode->ReadVector3("Position"));
	DestinationHelperNode->WriteQuaternion("Rotation", SourceHelperNode->ReadQuaternion("Rotation"));
	DestinationHelperNode->WriteVector3("Scale", SourceHelperNode->ReadVector3("Scale"));

	if (SourceHelperNode->IsPropertyExists("UserDefinedProperties"))
		DestinationHelperNode->WriteString("UserDefinedProperties", SourceHelperNode->ReadString("UserDefinedProperties"));
}

static void ConvertAnimation(ZEMLReaderNode* SourceAnimationNode, ZEMLWriterNode* DestinationAnimationNode)
{
	if (SourceAnimationNode == NULL || DestinationAnimationNode == NULL)
		return;

	if (!SourceAnimationNode->IsValid())
		return;

	DestinationAnimationNode->WriteString("Name", SourceAnimationNode->ReadString("Name"));
	DestinationAnimationNode->WriteFloat("FramesPerSecond", SourceAnimationNode->ReadFloat("FramesPerSecond", 30.0f));
	DestinationAnimationNode->WriteUInt32("BoneKeyCount", SourceAnimationNode->ReadUInt32("BoneKeyCount"));
	DestinationAnimationNode->WriteUInt32("MeshKeyCount", SourceAnimationNode->ReadUInt32("MeshKeyCount"));

	ZEArray<ZEMDResourceAnimationKey> FrameKeys;
	FrameKeys.SetCount(SourceAnimationNode->ReadDataSize("Frames") / sizeof(ZEMDResourceAnimationKey));
	if (!SourceAnimationNode->ReadDataItems("Frames", FrameKeys.GetCArray(), sizeof(ZEMDResourceAnimationKey), FrameKeys.GetCount()))
		return;

	DestinationAnimationNode->WriteData("Frames", FrameKeys.GetCArray(), FrameKeys.GetCount() * sizeof(ZEMDResourceAnimationKey));
}

static void ConvertModel(ZEMLReaderNode* Unserializer, ZEMLWriterNode* Serializer)
{
	if (!Unserializer->IsValid())
		return;

	Serializer->WriteUInt8("VersionMajor", 2);
	Serializer->WriteUInt8("VersionMinor", 0);

	Materials.Clear();
	ZEMLReaderNode SourceMaterialsNode = Unserializer->GetNode("Materials");
	
	if (SourceMaterialsNode.IsValid())
	{
		Materials.SetCount(SourceMaterialsNode.GetNodeCount("Material"));

		for (ZESize I = 0; I < Materials.GetCount(); I++)
		{
			ZEMLReaderNode MaterialNode = SourceMaterialsNode.GetNode("Material", I);
			Materials[I].Name = MaterialNode.ReadString("Name");
			Materials[I].FilePath = MaterialNode.ReadString("FilePath");
		}
	}

	ZEMLReaderNode SourceBonesNode = Unserializer->GetNode("Bones");

	if (SourceBonesNode.IsValid())
	{
		ZEMLWriterNode DestinationBonesNode;
		Serializer->OpenNode("Bones", DestinationBonesNode);
		ZESize BoneCount = SourceBonesNode.GetNodeCount("Bone");

		for (ZESize I = 0; I < BoneCount; I++)
		{
			ZEMLWriterNode DestinationBoneNode;
			DestinationBonesNode.OpenNode("Bone", DestinationBoneNode);
			ConvertBone(&SourceBonesNode.GetNode("Bone", I), &DestinationBoneNode);
			DestinationBoneNode.CloseNode();
		}

		DestinationBonesNode.CloseNode();
	}

	ZEMLReaderNode SourceMeshesNode = Unserializer->GetNode("Meshes");

	if (SourceMeshesNode.IsValid())
	{
		ZEMLWriterNode DestinationMeshesNode;
		Serializer->OpenNode("Meshes", DestinationMeshesNode);
		ZESize MeshCount = SourceMeshesNode.GetNodeCount("Mesh");

		for (ZESize I = 0; I < MeshCount; I++)
		{
			ZEMLWriterNode DestinationMeshNode;
			DestinationMeshesNode.OpenNode("Mesh", DestinationMeshNode);
			ConvertMesh(&SourceMeshesNode.GetNode("Mesh", I), &DestinationMeshNode);
			DestinationMeshNode.CloseNode();
		}
		
		DestinationMeshesNode.CloseNode();
	}

	ZEMLReaderNode SourceAnimationsNode = Unserializer->GetNode("Animations");

	if (SourceAnimationsNode.IsValid())
	{
		ZEMLWriterNode DestinationAnimationsNode;
		Serializer->OpenNode("Animations", DestinationAnimationsNode);
		ZESize AnimationCount = SourceAnimationsNode.GetNodeCount("Animation");

		for (ZESize I = 0; I < AnimationCount; I++)
		{
			ZEMLWriterNode DestinationAnimationNode;
			DestinationAnimationsNode.OpenNode("Animation", DestinationAnimationNode);
			ConvertAnimation(&SourceAnimationsNode.GetNode("Animation", I), &DestinationAnimationNode);
			DestinationAnimationNode.CloseNode();
		}

		DestinationAnimationsNode.CloseNode();
	}

}

bool ZEModelConverter::Convert(const char* Source, const char* Destination)
{
	ZEFile SourceFile;
	if (!SourceFile.Open(Source, ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot open source model file. File Name: \"%s\".", Source);
		return false;
	}

	ZEMLReader Unserializer;
	Unserializer.Open(&SourceFile);
	ZEMLReaderNode SourceModelNode = Unserializer.GetRootNode();

	if (SourceModelNode.GetName() != "ZEModel")
	{
		zeError("Cannot load model. Corrupted model file. File Name: \"%s\".", Source);
		SourceFile.Close();
		return false;
	}

	printf(" Checking source version.\n");

	ZEUInt8 SourceFileMajorVersion = SourceModelNode.ReadUInt8("MajorVersion");
	ZEUInt8 SourceFileMinorVersion = SourceModelNode.ReadUInt8("MinorVersion");

	if (SourceFileMajorVersion != 1 || SourceFileMinorVersion != 0)
	{
		if (SourceFileMajorVersion == 2 && SourceFileMinorVersion == 0)
		{
			printf("Material file is already converted.\n");
			return true;
		}

		zeError("Cannot load model. Different model file version detected. File Name: \"%s\".", Source);
		SourceFile.Close();
		return false;
	}

	ZEFile DestinationFile;
	if (!DestinationFile.Open(Destination, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Cannot open destination ZEModel file. File Name: \"%s\".", Destination);
		return false;
	}

	ZEMLWriter Serializer;
	Serializer.Open(Destination);
	ZEMLWriterNode DestinationModelNode;
	Serializer.OpenRootNode("ZEModel", DestinationModelNode);

	ConvertModel(&SourceModelNode, &DestinationModelNode);
	
	DestinationModelNode.CloseNode();
	Serializer.Close();

	Unserializer.Close();

	return true;
}
