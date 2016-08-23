//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECVModelConverterV2.cpp
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

#include "ZECVModelConverterV2.h"

#include "ZEError.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEFile/ZEPathManager.h"
#include "ZEFile/ZEFile.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"

#include "ZEModel/ZEMDResourceAnimation.h"
#include "ZEMath/ZETriangle.h"

#include "tootlelib.h"

enum ZEMDVertexType
{
	ZEMD_VT_NORMAL		= 0,
	ZEMD_VT_SKINNED		= 1
};

enum ZEMDVertexIndexType
{
	ZEMD_VIT_NONE		= 0,
	ZEMD_VIT_16BIT		= 16,
	ZEMD_VIT_32BIT		= 32
};

struct ZEModelConverterMaterialDefinition
{
	ZEString Name;
	ZEString FilePath;
};

static ZEArray<ZEModelConverterMaterialDefinition> Materials;

bool ZECVModelConverterV2::ConvertPhysicalBody(ZEMLReaderNode* SourcePhysicalBodyNode, ZEMLWriterNode* DestinationPhysicalBodyNode)
{
	zeDebugCheck(SourcePhysicalBodyNode == NULL || DestinationPhysicalBodyNode == NULL || !SourcePhysicalBodyNode->IsValid(), "Invalid arguments.");

	DestinationPhysicalBodyNode->WriteBool("Enabled", SourcePhysicalBodyNode->ReadBoolean("Enabled"));
	DestinationPhysicalBodyNode->WriteInt32("Type", SourcePhysicalBodyNode->ReadInt32("Type"));
	DestinationPhysicalBodyNode->WriteBool("Kinematic", SourcePhysicalBodyNode->ReadBoolean("Kinematic", SourcePhysicalBodyNode->ReadBoolean("IsKinematic")));
	DestinationPhysicalBodyNode->WriteFloat("Mass", SourcePhysicalBodyNode->ReadFloat("Mass"));
	DestinationPhysicalBodyNode->WriteVector3("MassCenter", SourcePhysicalBodyNode->ReadVector3("MassCenter"));
	DestinationPhysicalBodyNode->WriteFloat("LinearDamping", SourcePhysicalBodyNode->ReadFloat("LinearDamping"));
	DestinationPhysicalBodyNode->WriteFloat("AngularDamping", SourcePhysicalBodyNode->ReadFloat("AngularDamping"));

	ZEMLReaderNode SourceShapesNode = SourcePhysicalBodyNode->GetNode("PhysicalShapes");
	if (!SourceShapesNode.IsValid())
		return true;

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
			{
				zeError("Cannot read physical shape vertices from source file.");
				return false;
			}

			DestinationShapeNode.WriteData("Vertices", ShapeVertices.GetCArray(), ShapeVertices.GetCount() * sizeof(ZEVector3));
		}

		if (SourceShapeNode.IsPropertyExists("Indices"))
		{
			ZEArray<ZEVector3> ShapeIndices;
			ShapeIndices.SetCount(SourceShapeNode.ReadDataSize("Indices") / sizeof(ZEVector3));
			if (!SourceShapeNode.ReadDataItems("Indices", ShapeIndices.GetCArray(), sizeof(ZEVector3), ShapeIndices.GetCount()))
			{
				zeError("Cannot read physical shape indices from source file.");
				return false;
			}


			DestinationShapeNode.WriteData("Indices", ShapeIndices.GetCArray(), ShapeIndices.GetCount() * sizeof(ZEVector3));
		}

		DestinationShapeNode.CloseNode();
		DestinationShapesNode.CloseNode();
	}

	return true;
}

bool ZECVModelConverterV2::ConvertPhysicalJoint(ZEMLReaderNode* SourceJointNode, ZEMLWriterNode* DestinationJointNode)
{
	zeDebugCheck(SourceJointNode == NULL || DestinationJointNode == NULL || !SourceJointNode->IsValid(), "Invalid parameters.");

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

	return true;
}

bool ZECVModelConverterV2::ConvertBone(ZEMLReaderNode* SourceBoneNode, ZEMLWriterNode* DestinationBoneNode)
{
	zeDebugCheck(SourceBoneNode == NULL || DestinationBoneNode == NULL || !SourceBoneNode->IsValid(), "Invalid parameters.");

	DestinationBoneNode->WriteString("Name", SourceBoneNode->ReadString("Name"));
	DestinationBoneNode->WriteInt32("ParentBone", SourceBoneNode->ReadInt32("ParentBone", -1));

	ZEMLReaderNode SourceBBoxNode = SourceBoneNode->GetNode("BoundingBox");
	if (!SourceBBoxNode.IsValid())
		return false;

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
		if (!ConvertPhysicalBody(&SourcePhysicalBodyNode, &DestinationPhysicalBodyNode))
		{
			zeError("Cannot conver physical body.");
			return false;
		}
		DestinationPhysicalBodyNode.CloseNode();
	}

	ZEMLReaderNode SourcePhysicalJointNode = SourceBoneNode->GetNode("PhysicalJoint");

	if (SourcePhysicalJointNode.IsValid() && SourcePhysicalJointNode.ReadBoolean("Enabled"))
	{
		ZEMLWriterNode DestinationPhysicalJointNode;
		DestinationBoneNode->OpenNode("PhysicalJoint", DestinationPhysicalJointNode);
		if (!ConvertPhysicalJoint(&SourcePhysicalJointNode, &DestinationPhysicalJointNode))
		{
			zeError("Cannot conver physical joint.");
			return false;
		}
		DestinationPhysicalJointNode.CloseNode();
	}

	return true;
}

bool ZECVModelConverterV2::ConvertMeshBoundingBox(ZEMLReaderNode* SourceMeshNode, ZEMLWriterNode* DestinationBoundingBoxNode)
{
	zeDebugCheck(SourceMeshNode == NULL || DestinationBoundingBoxNode == NULL || !SourceMeshNode->IsValid(), "Invalid parameters.");

	ZEMLReaderNode LODsNode = SourceMeshNode->GetNode("LODs");
	if (!LODsNode.IsValid())
		return false;

	ZEMLReaderNode MainLODNode = LODsNode.GetNode("LOD", 0);
	if (!MainLODNode.IsValid())
		return false;

	ZEAABBox BoundingBox;
	bool Skinned = SourceMeshNode->ReadBoolean("IsSkinned");

	if (Skinned)
	{
		ZEArray<ZEMDVertexSkinV2>	VerticesSkin;
		VerticesSkin.SetCount(MainLODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertexSkinV2));
		if (!MainLODNode.ReadDataItems("Vertices", VerticesSkin.GetCArray(), sizeof(ZEMDVertexSkinV2), VerticesSkin.GetCount()))
			return false;

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
		ZEArray<ZEMDVertexV2>	Vertices;
		Vertices.SetCount(MainLODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertexV2));
		if (!MainLODNode.ReadDataItems("Vertices", Vertices.GetCArray(), sizeof(ZEMDVertexV2), Vertices.GetCount()))
		{
			zeError("Cannot read LOD vertex data.");
			return false;
		}

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

	return true;
}

void ZECVModelConverterV2::GenerateIndices(const ZEArray<ZEMDVertexV2>& Vertices, ZEArray<ZEUInt32>& Indices, ZEArray<ZEMDVertexV2>& IndexedVertices)
{
	// GENERATING INDICES
	ZEArray<ZEInt> VertexDuplicateIndices;
	VertexDuplicateIndices.SetCount(Vertices.GetCount());
	VertexDuplicateIndices.Fill(-1);

	Indices.SetCount(Vertices.GetCount());

	ZEUInt IndexedVertexCount = 0;
	ZEArray<ZEInt> VertexMapping;
	VertexMapping.SetCount(Vertices.GetCount());

	for (ZEUInt I = 0; I < Vertices.GetCount(); I++)
	{
		if (VertexDuplicateIndices[I] != -1)
		{
			Indices[I] = VertexDuplicateIndices[I];
			continue;
		}
		else
		{
			Indices[I] = IndexedVertexCount;
			VertexMapping[IndexedVertexCount] = I;
		}

		for (ZEUInt N = I + 1; N < Vertices.GetCount(); N++)
		{
			const ZEMDVertexV2& VertexA = Vertices[I];
			const ZEMDVertexV2& VertexB = Vertices[N];

			if (VertexDuplicateIndices[N] != -1)
				continue;

			if (VertexA.Position != VertexB.Position || 
				VertexA.Normal[0] != VertexB.Normal[0] ||
				VertexA.Normal[1] != VertexB.Normal[1] ||
				VertexA.Tangent[0] != VertexB.Tangent[0] ||
				VertexA.Tangent[1] != VertexB.Tangent[1] ||
				VertexA.Texcoords != VertexB.Texcoords)
			{
				continue;
			}

			VertexDuplicateIndices[N] = IndexedVertexCount;
		}

		IndexedVertexCount++;
	}

	// GENERATE SKINNED VERTEX BUFFER
	IndexedVertices.SetCount(IndexedVertexCount);
	for (ZESize I = 0; I < IndexedVertexCount; I++)
	{
		IndexedVertices[I] = Vertices[VertexMapping[I]];
	}
}

void ZECVModelConverterV2::GenerateIndices(const ZEArray<ZEMDVertexSkinV2>& Vertices, ZEArray<ZEUInt32>& Indices, ZEArray<ZEMDVertexSkinV2>& IndexedVertices)
{
	// GENERATING INDICES
	ZEArray<ZEInt> VertexDuplicateIndices;
	VertexDuplicateIndices.SetCount(Vertices.GetCount());
	VertexDuplicateIndices.Fill(-1);

	Indices.SetCount(Vertices.GetCount());

	ZEUInt IndexedVertexCount = 0;
	ZEArray<ZEInt> VertexMapping;
	VertexMapping.SetCount(Vertices.GetCount());

	for (ZEUInt I = 0; I < Vertices.GetCount(); I++)
	{
		if (VertexDuplicateIndices[I] != -1)
		{
			Indices[I] = VertexDuplicateIndices[I];
			continue;
		}
		else
		{
			Indices[I] = IndexedVertexCount;
			VertexMapping[IndexedVertexCount] = I;
		}

		for (ZEUInt N = I + 1; N < Vertices.GetCount(); N++)
		{
			const ZEMDVertexSkinV2& VertexA = Vertices[I];
			const ZEMDVertexSkinV2& VertexB = Vertices[N];

			if (VertexDuplicateIndices[N] != -1)
				continue;

			if (VertexA.Position != VertexB.Position || 
				VertexA.Normal[0] != VertexB.Normal[0] ||
				VertexA.Normal[1] != VertexB.Normal[1] ||
				VertexA.Tangent[0] != VertexB.Tangent[0] ||
				VertexA.Tangent[1] != VertexB.Tangent[1] ||
				VertexA.Texcoords != VertexB.Texcoords)
			{
				continue;
			}


			bool SkinFound[4] = {false};
			for (int J = 0; J < 4; J++)
			{
				for (ZESize K = 0; K < 4; K++)
				{
					if (SkinFound[K])
						continue;

					if (VertexA.Indices[J] == VertexB.Indices[K] &&
						VertexA.Weights[J] == VertexB.Weights[K])
					{
						SkinFound[K] = true;
						break;
					}		
				}
			}

			if (!SkinFound[0] || !SkinFound[1] || !SkinFound[2] || !SkinFound[3])
				continue;

			VertexDuplicateIndices[N] = IndexedVertexCount;
		}

		IndexedVertexCount++;
	}

	// GENERATE SKINNED VERTEX BUFFER
	IndexedVertices.SetCount(IndexedVertexCount);
	for (ZESize I = 0; I < IndexedVertexCount; I++)
	{
		IndexedVertices[I] = Vertices[VertexMapping[I]];
	}
}

template<typename ZEVertexTypeV1, typename ZEVertexTypeV2>
void ConvertVertexBase(ZESize Index, const ZEVertexTypeV1& InputVertex, ZEVertexTypeV2& OutputVertex)
{
	OutputVertex.Position = InputVertex.Position;
	OutputVertex.Texcoords = InputVertex.Texcoords;

	ZEVector3 Normal = InputVertex.Normal.Normalize();
	OutputVertex.Normal[0] = (ZEInt16)(Normal.x * 32766.0f);
	OutputVertex.Normal[1] = (ZEInt16)(Normal.y * 32766.0f);
	if (InputVertex.Normal.z < 0.0f)
		OutputVertex.Normal[0] &= 0xFFFE;
	else
		OutputVertex.Normal[0] |= 0x0001;

	ZEVector3 Tangent = InputVertex.Tangent.Normalize();
	OutputVertex.Tangent[0] = (ZEInt16)(Tangent.x * 32766.0f);
	OutputVertex.Tangent[1] = (ZEInt16)(Tangent.y * 32766.0f);

	if (InputVertex.Tangent.z < 0.0f)
		OutputVertex.Tangent[0] &= 0xFFFE;
	else
		OutputVertex.Tangent[0] |= 0x0001;

	// Binormals of the 3ds Max Export is inverted (OpenGL to D3D Conversion)
	// Because of that InvertedBinormal determination mus be done on inverted space. B = T x N.
	// However after conversation shader must calculate its binormal as B = N x T.
	ZEVector3 Binormal = -InputVertex.Binormal.Normalize();
	ZEVector3 CalculatedBinormal;
	ZEVector3::CrossProduct(CalculatedBinormal, Tangent, Normal);
	CalculatedBinormal.NormalizeSelf();

	if (ZEVector3::DotProduct(CalculatedBinormal, Binormal) < 0.0f) // Mirrored
	{
		CalculatedBinormal *= -1.0f;
		OutputVertex.Tangent[1] &= 0xFFFE;
	}
	else
	{
		OutputVertex.Tangent[1] |= 0x0001;
	}

	// VERIFICATION
	float Error = (Binormal - CalculatedBinormal).Abs().Max();
	if (Error >= 0.2f)
		zeError("Calculated Binormal is not equals to original binormal. Weird binormal. Index: %u. Error: %f", Index, Error);

	ZEVector3 RegeneratedNormal;
	RegeneratedNormal.x = (float)OutputVertex.Normal[0] / 32767.0f;
	RegeneratedNormal.y = (float)OutputVertex.Normal[1] / 32767.0f;
	float Sign = (2.0f * (OutputVertex.Normal[0] & 0x0001) - 1.0f);
	RegeneratedNormal.z = ZEMath::Sqrt(ZEMath::Saturate(1.0f - RegeneratedNormal.x * RegeneratedNormal.x - RegeneratedNormal.y * RegeneratedNormal.y)) * Sign;
	RegeneratedNormal.NormalizeSelf();	
	if (!RegeneratedNormal.IsValid())
		zeError("Validation check failed. Regenerated normal is not valid. Index: %u.", Index);

	Error = (RegeneratedNormal - Normal).Abs().Max();
	if (Error >= 0.1f)
		zeError("Validation check failed. Regenerated normal is not equals to original normal. Index: %u. Error: %f", Index, Error);
	else if (Error >= 0.02f)
		zeWarning("Validation check failed. Regenerated normal is not equals to original normal. Index: %u. Error: %f", Index, Error);


	ZEVector3 RegeneratedTangent;
	RegeneratedTangent.x = (float)OutputVertex.Tangent[0] / 32767.0f;
	RegeneratedTangent.y = (float)OutputVertex.Tangent[1] / 32767.0f;
	Sign = (2.0f * (OutputVertex.Tangent[0] & 0x0001) - 1.0f);
	RegeneratedTangent.z = ZEMath::Sqrt(ZEMath::Saturate(1.0f - RegeneratedTangent.x * RegeneratedTangent.x - RegeneratedTangent.y * RegeneratedTangent.y)) * Sign;
	RegeneratedTangent.NormalizeSelf();
	if (!RegeneratedTangent.IsValid())
		zeError("Validation check failed. Regenerated tangent is not valid. Index: %u.", Index);

	Error = (RegeneratedTangent - Tangent).Abs().Max();
	if (Error >= 0.1f)
		zeError("Validation check failed. Regenerated tangent is not equals to original tangent. Index: %u. Error: %f", Index, Error);
	else if (Error >= 0.02f)
		zeError("Validation check failed. Regenerated tangent is not equals to original tangent. Index: %u. Error: %f", Index, Error);


	ZEVector3 RegeneratedBinormal;
	ZEVector3::CrossProduct(RegeneratedBinormal, RegeneratedTangent, RegeneratedNormal);
	Sign = (2.0f * (OutputVertex.Tangent[1] & 0x0001) - 1.0f);
	RegeneratedBinormal = RegeneratedBinormal.Normalize() * Sign;
	if (!RegeneratedBinormal.IsValid())
		zeError("Validation check failed. Regenerated binormal is not valid. Index: %u.", Index);

	Error = (RegeneratedBinormal - CalculatedBinormal).Abs().Max();
	if (Error >= 0.1f)
		zeError("Validation check failed. Regenerated binormal is not equals to calculated binormal. Index: %u. Error: %f", Index, Error);
	else if (Error >= 0.02f)
		zeWarning("Validation check failed. Regenerated binormal is not equals to calculated binormal. Index: %u. Error: %f", Index, Error);
}

void ZECVModelConverterV2::ConvertVertexData(ZEArray<ZEMDVertexV2>& Output, const ZEArray<ZEMDVertexV1>& Input, bool& DegenerateFaceDetected)
{
	DegenerateFaceDetected = false;
	Output.SetCount(Input.GetCount());
	for (ZESize I = 0 ; I < Input.GetCount(); I++)
	{
		if (I % 3 == 0)
		{ 
			ZETriangle Triangle(ZETriangle(Input[I].Position, Input[I + 1].Position, Input[I + 2].Position));
			float Area = ZETriangle::GetArea(Triangle);
			if (Area <= 0.0001)
			{
				zeError("Degenerate face detected. Face Index: %u. Vertex Index: %u.", I / 3, I);
				DegenerateFaceDetected = true;
			}
		}

		Output[I].Reserved0 = 1.0f;
		ConvertVertexBase(I, Input[I], Output[I]);
	}
}

void ZECVModelConverterV2::ConvertVertexData(ZEArray<ZEMDVertexSkinV2>& Output, const ZEArray<ZEMDVertexSkinV1>& Input, bool& DegenerateFaceDetected)
{
	DegenerateFaceDetected = false;
	Output.SetCount(Input.GetCount());
	for (ZESize I = 0 ; I < Input.GetCount(); I++)
	{
		if (I % 3 == 0)
		{ 
			ZETriangle Triangle(ZETriangle(Input[I].Position, Input[I + 1].Position, Input[I + 2].Position));
			float Area = ZETriangle::GetArea(Triangle);
			if (Area <= 0.0001)
			{
				zeError("Degenerate face detected. Face Index: %u. Vertex Index: %u.", I / 3, I);
				DegenerateFaceDetected = true;
			}
		}

		Output[I].Indices[0] = Input[I].Indices[0];
		Output[I].Indices[1] = Input[I].Indices[1];
		Output[I].Indices[2] = Input[I].Indices[2];
		Output[I].Indices[3] = Input[I].Indices[3];
		Output[I].Weights = Input[I].Weights;

		ConvertVertexBase(I, Input[I], Output[I]);
	}
}

bool ZECVModelConverterV2::ConvertMeshLOD(ZEMLReaderNode* SourceLODNode, ZEMLWriterNode* DestinationLODNode)
{
	zeDebugCheck(SourceLODNode == NULL || DestinationLODNode == NULL || !SourceLODNode->IsValid(), "Invalid parameters");
	zeLog("Processing LOD: %d.", SourceLODNode->ReadInt32("LODLevel", 0));

	ZEError::GetInstance()->SetBreakOnErrorEnabled(false);

	DestinationLODNode->WriteInt32("Level", SourceLODNode->ReadInt32("LODLevel", 0));
	DestinationLODNode->WriteFloat("StartDistance", (float)SourceLODNode->ReadInt32("LODStartDistance", 0));
	DestinationLODNode->WriteFloat("EndDistance", (float)SourceLODNode->ReadInt32("LODEndDistance", 100000));
	bool IsSourceLODSkinned = SourceLODNode->IsPropertyExists("AffectingBoneIds");

	ZESize VertexCount;
	ZESize VertexBufferSize;
	ZEArray<ZEMDVertexV2> Vertices;
	ZEArray<ZEMDVertexSkinV2> VerticesSkin;
	ZESize OriginalVertexBufferSize;
	bool DegenerateFaceDetected = false;
	if (!IsSourceLODSkinned)
	{
		ZEArray<ZEMDVertexV1> VerticesV1;
		VerticesV1.SetCount(SourceLODNode->ReadDataSize("Vertices") / sizeof(ZEMDVertexV1));
		if (!SourceLODNode->ReadDataItems("Vertices", VerticesV1.GetCArray(), sizeof(ZEMDVertexV1), VerticesV1.GetCount()))
		{
			zeError("Cannot read vertices.");
			return false;
		}

		zeLog("Converting to new vertex format. vertex Count: %d.", VerticesV1.GetCount());
		ConvertVertexData(Vertices, VerticesV1, DegenerateFaceDetected);

		VertexCount = Vertices.GetCount();
		VertexBufferSize = Vertices.GetCount() * sizeof(ZEMDVertexV2);
		OriginalVertexBufferSize = Vertices.GetCount() * sizeof(ZEMDVertexV1);
	}
	else
	{
		ZEArray<ZEMDVertexSkinV1> VerticesSkinV1;
		VerticesSkinV1.SetCount(SourceLODNode->ReadDataSize("Vertices") / sizeof(ZEMDVertexSkinV1));
		if (!SourceLODNode->ReadDataItems("Vertices", VerticesSkinV1.GetCArray(), sizeof(ZEMDVertexSkinV1), VerticesSkinV1.GetCount()))
		{
			zeError("Cannot read LOD vertex data.");
			return false;
		}

		zeLog("Converting to new skinned vertex format. vertex Count: %d.", VerticesSkinV1.GetCount());
		ConvertVertexData(VerticesSkin, VerticesSkinV1, DegenerateFaceDetected);

		VertexCount = VerticesSkin.GetCount();
		VertexBufferSize = VerticesSkin.GetCount() * sizeof(ZEMDVertexSkinV2);
		OriginalVertexBufferSize = VerticesSkin.GetCount() * sizeof(ZEMDVertexV1);
	}

	bool IsIndexed = false;
	ZEArray<ZEUInt32> Indexes;
	ZEArray<ZEMDVertexV2> IndexedVertices;
	ZEArray<ZEMDVertexSkinV2> IndexedVerticesSkin;
	ZESize IndexedVertexCount;
	ZESize IndexedVertexBufferSize;
	if (VertexCount > 600)
	{
		zeLog("Generating Indexes.");
		if (!IsSourceLODSkinned)
		{
			GenerateIndices(Vertices, Indexes, IndexedVertices);
			IndexedVertexCount = IndexedVertices.GetCount();
			IndexedVertexBufferSize = Vertices.GetCount() * (IndexedVertexCount > 65536 ? sizeof(ZEUInt32) : sizeof(ZEUInt16)) + IndexedVertexCount *  sizeof(ZEMDVertexV2);
		}
		else
		{
			GenerateIndices(VerticesSkin, Indexes, IndexedVerticesSkin);
			IndexedVertexCount = IndexedVerticesSkin.GetCount();
			IndexedVertexBufferSize = VerticesSkin.GetCount() * (IndexedVertexCount > 65536 ? sizeof(ZEUInt32) : sizeof(ZEUInt16)) + IndexedVertexCount *  sizeof(ZEMDVertexSkinV2);
		}

		float GainRatio = (float)IndexedVertexBufferSize / (float)VertexBufferSize;
		if (GainRatio <= 0.85f)
		{
			zeLog("Indexing Accepted. Gain ratio: %f (Limit: 0.85), Vertex Count: %d, Indexed Vertex Count: %d.", GainRatio, VertexCount, IndexedVertexCount);
			IsIndexed = true;
		}
		else
		{		
			zeLog("Indexing Rejected. Gain ratio: %f (Limit: 0.85), Vertex Count: %d, Indexed Vertex Count: %d.", GainRatio, VertexCount, IndexedVertexCount);
			IsIndexed = false;
		}

		if (!DegenerateFaceDetected)
		{
			if (!IsSourceLODSkinned)
			{
				zeLog("Reordering indexes for vertex cache optimization.");
				TootleOptimize(IndexedVertices.GetConstCArray(), Indexes.GetConstCArray(), IndexedVertices.GetCount(), Indexes.GetCount() / 3, 
					sizeof(ZEMDVertexV2), TOOTLE_DEFAULT_VCACHE_SIZE, NULL, 0, TOOTLE_CW, Indexes.GetCArray(), NULL);
				
				zeLog("Reordering vertices for vertex precache optimization.");
				TootleOptimizeVertexMemory(IndexedVertices.GetConstCArray(), Indexes.GetConstCArray(), IndexedVertices.GetCount(), Indexes.GetCount() / 3, 
					sizeof(ZEMDVertexV2), IndexedVertices.GetCArray(), Indexes.GetCArray(), NULL);
			}
			else
			{
				zeLog("Reordering indexes for vertex cache optimization.");
				TootleOptimize(IndexedVerticesSkin.GetConstCArray(), Indexes.GetConstCArray(), IndexedVerticesSkin.GetCount(), Indexes.GetCount() / 3, 
					sizeof(ZEMDVertexSkinV2), TOOTLE_DEFAULT_VCACHE_SIZE, NULL, 0, TOOTLE_CW, Indexes.GetCArray(), NULL);

				zeLog("Reordering vertices for vertex precache optimization.");
				TootleOptimizeVertexMemory(IndexedVerticesSkin.GetConstCArray(), Indexes.GetConstCArray(), IndexedVerticesSkin.GetCount(), Indexes.GetCount() / 3, 
					sizeof(ZEMDVertexSkinV2), IndexedVerticesSkin.GetCArray(), Indexes.GetCArray(), NULL);
			}
		}
		else
		{
			zeError("Vertex cache optimization disabled. Degenerate faces detected.");
		}
	}
	else
	{
		zeLog("Indexing Rejected. Vertex count is low.");
	}
	
	if (IsIndexed)
	{
		ZESize IndexBufferSize;
		if (IndexedVertexCount > 65536)
		{
			// 32 Bit Indices
			IndexBufferSize = Indexes.GetCount() * sizeof(ZEUInt32);
			DestinationLODNode->WriteUInt8("IndexType", ZEMD_VIT_32BIT);
			DestinationLODNode->WriteData("Indices", Indexes.GetConstCArray(), IndexBufferSize);
			
		}
		else
		{
			// 16 Bit Indices
			ZEArray<ZEUInt16> IndexList16;
			IndexList16.SetCount(Indexes.GetCount());
			for (ZESize I = 0; I < Indexes.GetCount(); I++)
				IndexList16[I] = (ZEUInt16)Indexes[I];

			IndexBufferSize = IndexList16.GetCount() * sizeof(ZEUInt16);
			DestinationLODNode->WriteUInt8("IndexType", ZEMD_VIT_16BIT);
			DestinationLODNode->WriteData("Indices", IndexList16.GetConstCArray(), IndexBufferSize);
		}

		if (!IsSourceLODSkinned)
		{
			DestinationLODNode->WriteUInt8("VertexType", ZEMD_VT_NORMAL);
			DestinationLODNode->WriteData("Vertices", IndexedVertices.GetCArray(), IndexedVertices.GetCount() * sizeof(ZEMDVertexV2));
		}
		else
		{
			DestinationLODNode->WriteUInt8("VertexType", ZEMD_VT_SKINNED);
			DestinationLODNode->WriteData("Vertices", IndexedVerticesSkin.GetCArray(), IndexedVerticesSkin.GetCount() * sizeof(ZEMDVertexSkinV2));
		}

		float Gain = (float)(IndexedVertexBufferSize + IndexBufferSize) / (float)OriginalVertexBufferSize;
		zeLog("Vertex Data (Includes Indexes) Size Optimization Gain: %f. Original Size: %Iu, New Size: %Iu.", Gain, OriginalVertexBufferSize, IndexedVertexBufferSize + IndexBufferSize);
	}
	else
	{
		DestinationLODNode->WriteUInt8("IndexType", ZEMD_VIT_NONE);

		if (!IsSourceLODSkinned)
		{
			DestinationLODNode->WriteUInt8("VertexType", ZEMD_VT_NORMAL);
			DestinationLODNode->WriteData("Vertices", Vertices.GetCArray(), Vertices.GetCount() * sizeof(ZEMDVertexV2));
		}
		else
		{
			DestinationLODNode->WriteUInt8("VertexType", ZEMD_VT_SKINNED);
			DestinationLODNode->WriteData("Vertices", VerticesSkin.GetCArray(), VerticesSkin.GetCount() * sizeof(ZEMDVertexSkinV2));
		}

		float Gain = (float)VertexBufferSize / (float)OriginalVertexBufferSize;
		zeLog("Vertex Data (Includes Indexes) Size Optimization Gain: %f. Original Size: %Iu, New Size: %Iu.", Gain, OriginalVertexBufferSize, VertexBufferSize);
	}

	if (SourceLODNode->IsSubNodeExists("Draws"))
		return false;

	ZEMLWriterNode DrawsNode, DrawNode;
	DestinationLODNode->OpenNode("Draws", DrawsNode);
	DrawsNode.OpenNode("Draw", DrawNode);

	DrawNode.WriteUInt32("Offset", 0);
	DrawNode.WriteUInt32("Count", (ZEUInt32)VertexCount);

	ZEInt32 MaterialID = SourceLODNode->ReadInt32("MaterialId");

	if (MaterialID < Materials.GetCount())
	{
		DrawNode.WriteString("MaterialName", Materials[MaterialID].Name);
		DrawNode.WriteString("MaterialFilePath", Materials[MaterialID].FilePath);
	}

	DrawNode.CloseNode();
	DrawsNode.CloseNode();

	return true;
}

bool ZECVModelConverterV2::ConvertMesh(ZEMLReaderNode* SourceMeshNode, ZEMLWriterNode* DestinationMeshNode)
{
	zeDebugCheck(SourceMeshNode == NULL || DestinationMeshNode == NULL || !SourceMeshNode->IsValid(), "Invalid paramters");

	zeLog("Processing Mesh: \"%s\".", SourceMeshNode->ReadString("Name").ToCString());
	
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
		if (!ConvertPhysicalBody(&SourcePhysicalBodyNode, &DestinationPhysicalBodyNode))
			return false;

		DestinationPhysicalBodyNode.CloseNode();
	}

	ZEMLReaderNode SourceLODsNode = SourceMeshNode->GetNode("LODs");
	if (!SourceLODsNode.IsValid())
		return true;

	ZEMLWriterNode DestinationLODsNode;
	DestinationMeshNode->OpenNode("LODs", DestinationLODsNode);
	
	ZESize LODCount = SourceLODsNode.GetNodeCount("LOD");
	for (ZESize I = 0; I < LODCount; I++)
	{
		ZEMLWriterNode DestinationLODNode;
		DestinationLODsNode.OpenNode("LOD", DestinationLODNode);
		if (!ConvertMeshLOD(&SourceLODsNode.GetNode("LOD", I), &DestinationLODNode))
			return false;

		DestinationLODNode.CloseNode();
	}

	DestinationLODsNode.CloseNode();

	return true;
}

bool ZECVModelConverterV2::ConvertHelper(ZEMLReaderNode* SourceHelperNode, ZEMLWriterNode* DestinationHelperNode)
{
	zeDebugCheck(SourceHelperNode == NULL || DestinationHelperNode == NULL || !SourceHelperNode->IsValid(), "Invalid parameters");

	zeLog("Processing Helper: \"%s\".", SourceHelperNode->ReadString("Name").ToCString());

	DestinationHelperNode->WriteString("Name", SourceHelperNode->ReadString("Name"));
	DestinationHelperNode->WriteInt32("OwnerType", SourceHelperNode->ReadInt32("OwnerType"));
	DestinationHelperNode->WriteInt32("OwnerId", SourceHelperNode->ReadInt32("OwnerId", -1));
	DestinationHelperNode->WriteVector3("Position", SourceHelperNode->ReadVector3("Position"));
	DestinationHelperNode->WriteQuaternion("Rotation", SourceHelperNode->ReadQuaternion("Rotation"));
	DestinationHelperNode->WriteVector3("Scale", SourceHelperNode->ReadVector3("Scale"));

	if (SourceHelperNode->IsPropertyExists("UserDefinedProperties"))
		DestinationHelperNode->WriteString("UserDefinedProperties", SourceHelperNode->ReadString("UserDefinedProperties"));

	return true;
}

bool ZECVModelConverterV2::ConvertAnimation(ZEMLReaderNode* SourceAnimationNode, ZEMLWriterNode* DestinationAnimationNode)
{
	zeDebugCheck(SourceAnimationNode == NULL || DestinationAnimationNode == NULL || !SourceAnimationNode->IsValid(), "Invalid parameters");

	zeLog("Processing Animation: \"%s\".", SourceAnimationNode->ReadString("Name").ToCString());

	DestinationAnimationNode->WriteString("Name", SourceAnimationNode->ReadString("Name"));
	DestinationAnimationNode->WriteFloat("FramesPerSecond", SourceAnimationNode->ReadFloat("FramesPerSecond", 30.0f));
	DestinationAnimationNode->WriteUInt32("BoneKeyCount", SourceAnimationNode->ReadUInt32("BoneKeyCount"));
	DestinationAnimationNode->WriteUInt32("MeshKeyCount", SourceAnimationNode->ReadUInt32("MeshKeyCount"));

	ZEArray<ZEMDResourceAnimationKey> FrameKeys;
	FrameKeys.SetCount(SourceAnimationNode->ReadDataSize("Frames") / sizeof(ZEMDResourceAnimationKey));
	if (!SourceAnimationNode->ReadDataItems("Frames", FrameKeys.GetCArray(), sizeof(ZEMDResourceAnimationKey), FrameKeys.GetCount()))
	{
		zeError("Cannot read frames from source file.");
		return false;
	}

	DestinationAnimationNode->WriteData("Frames", FrameKeys.GetCArray(), FrameKeys.GetCount() * sizeof(ZEMDResourceAnimationKey));

	return true;
}

bool ZECVModelConverterV2::ConvertModel(ZEMLReaderNode* Unserializer, ZEMLWriterNode* Serializer)
{
	zeDebugCheck(!Unserializer->IsValid(), "Invalid parameters");

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
			
			if (!ConvertBone(&SourceBonesNode.GetNode("Bone", I), &DestinationBoneNode))
				return false;

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

		TootleInit();
		for (ZESize I = 0; I < MeshCount; I++)
		{
			ZEMLWriterNode DestinationMeshNode;
			DestinationMeshesNode.OpenNode("Mesh", DestinationMeshNode);
			
			if (!ConvertMesh(&SourceMeshesNode.GetNode("Mesh", I), &DestinationMeshNode))
			{
				TootleCleanup();
				return false;
			}

			DestinationMeshNode.CloseNode();
		}
		TootleCleanup();

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
			
			if (ConvertAnimation(&SourceAnimationsNode.GetNode("Animation", I), &DestinationAnimationNode))
				return false;

			DestinationAnimationNode.CloseNode();
		}

		DestinationAnimationsNode.CloseNode();
	}

	return true;
}

const char* ZECVModelConverterV2::GetName()
{
	return "ZECVModelConverterV2";
}

const char* ZECVModelConverterV2::GetExtension()
{
	return "ZEModel";
}
ZEUInt ZECVModelConverterV2::GetSourceVersion()
{
	return 1;
}
ZEUInt ZECVModelConverterV2::GetDestinationVersion()
{
	return 2;
}

bool ZECVModelConverterV2::GetMajorVersionConversion()
{
	return true;
}

bool ZECVModelConverterV2::Convert(const ZEString& SourceFileName, const ZEString& DestinationFileName)
{
	ZELog::GetInstance()->SetLogFileEnabled(true);
	ZELog::GetInstance()->SetLogFileMinimumLevel(ZE_LOG_INFO);
	ZELog::GetInstance()->SetLogFilePath(DestinationFileName + ".ZECVConvert.log");

	zeLog("Converting ZEModel file. Source File: \"%s\", Destination File: \"%s\".", SourceFileName.ToCString(), DestinationFileName.ToCString());

	ZEMLReader Unserializer;
	Unserializer.Open(SourceFileName);
	ZEMLReaderNode SourceModelNode = Unserializer.GetRootNode();

	if (SourceModelNode.GetName() != "ZEModel")
	{
		zeError("Cannot load model. Corrupted model file. File Name: \"%s\".", SourceFileName.ToCString());
		Unserializer.Close();
		return false;
	}

	zeLog("Checking source version.\n");

	ZEUInt8 SourceFileMajorVersion = SourceModelNode.ReadUInt8("MajorVersion");
	ZEUInt8 SourceFileMinorVersion = SourceModelNode.ReadUInt8("MinorVersion");

	if (SourceFileMajorVersion != 1 || SourceFileMinorVersion != 0)
	{
		if (SourceFileMajorVersion == 2 && SourceFileMinorVersion == 0)
		{
			zeLog("Material file is already converted.\n");
			return true;
		}

		zeError("Cannot load model. Different model file version detected. File Name: \"%s\".", SourceFileName.ToCString());
		Unserializer.Close();
		return false;
	}

	ZEFile DestinationFile;
	if (!DestinationFile.Open(DestinationFileName, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Cannot open destination ZEModel file. File Name: \"%s\".", DestinationFileName.ToCString());
		return false;
	}

	ZEMLWriter Serializer;
	Serializer.Open(DestinationFileName);
	ZEMLWriterNode DestinationModelNode;
	Serializer.OpenRootNode("ZEModel", DestinationModelNode);

	DestinationModelNode.WriteUInt8("MajorVersion", 2);
	DestinationModelNode.WriteUInt8("MinorVersion", 0);

	if (!ConvertModel(&SourceModelNode, &DestinationModelNode))
	{
		zeError("Conversation has failed. File Name: \"%s\".", DestinationFileName.ToCString());
		return false;
	}

	DestinationModelNode.CloseNode();
	Serializer.Close();
	Unserializer.Close();

	zeLog("Convert completed. File Name: \"%s\".", DestinationFileName.ToCString());

	ZELog::GetInstance()->SetLogFileEnabled(false);

	return true;
}
