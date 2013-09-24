//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFLTHandlerCallbacks.cpp
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

#include "ZEFLTHandlerCallbacks.h"
#include "ZEFLTResourceNode.h"
#include "ZEFLTOpcodes.h"
#include "ZEFile/ZEFile.h"

void ReadRecordHeader(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordHeader* Header = new ZEFLTRecordHeader();

	memcpy(Header, BufferAtNode, NodeLength);

	(*Node)->SetRecord(Header);
};

void ReadRecordGroup(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordGroup* Group = new ZEFLTRecordGroup();

	memcpy(Group, BufferAtNode, NodeLength);

	(*Node)->SetRecord(Group);
};

void ReadRecordObject(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordObject* Object = new ZEFLTRecordObject();

	memcpy(Object, BufferAtNode, NodeLength);

	(*Node)->SetRecord(Object);
};

void ReadRecordFace(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordFace* Face = new ZEFLTRecordFace();

	memcpy(Face, BufferAtNode, NodeLength);

	(*Node)->SetRecord(Face);
};

void ReadRecordMesh(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordMesh* Mesh = new ZEFLTRecordMesh();

	memcpy(Mesh, BufferAtNode, NodeLength);

	(*Node)->SetRecord(Mesh);
};

void ReadRecordLocalVertexPool(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordLocalVertexPool* LocalVertexPool = new ZEFLTRecordLocalVertexPool();

	memcpy(LocalVertexPool, BufferAtNode, 12);

	LocalVertexPool->Vertices.SetCount(LocalVertexPool->NumberOfVertices);
	
	memcpy(LocalVertexPool->Vertices.GetCArray(), BufferAtNode + 12, LocalVertexPool->NumberOfVertices * sizeof(ZEFLTRecordLocalVertexPoolVertex));

	(*Node)->SetRecord(LocalVertexPool);

};

void ReadRecordMeshPrimitive(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordMeshPrimitive* MeshPrimitive = new ZEFLTRecordMeshPrimitive();

	memcpy(MeshPrimitive, BufferAtNode, 12);

	MeshPrimitive->Indices.SetCount(MeshPrimitive->VertexCount);
	MeshPrimitive->Indices.Fill(NULL);

	for (ZESize I = 0; I < MeshPrimitive->VertexCount; I++)
	{
		memcpy(&MeshPrimitive->Indices[I], (BufferAtNode + 12) + I * MeshPrimitive->IndexSize, MeshPrimitive->IndexSize);
	}

	(*Node)->SetRecord(MeshPrimitive);
};

void ReadRecordIndexedLightPoint(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordIndexedLightPoint* IndexedLightPoint = new ZEFLTRecordIndexedLightPoint();

	memcpy(IndexedLightPoint, BufferAtNode, NodeLength);

	(*Node)->SetRecord(IndexedLightPoint);
};

void ReadRecordLightPoint(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordLightPoint* LightPoint = new ZEFLTRecordLightPoint();

	memcpy(LightPoint, BufferAtNode, NodeLength);

	(*Node)->SetRecord(LightPoint);
};

void ReadRecordLightPointSystem(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordLightPointSystem* LightPointSystem = new ZEFLTRecordLightPointSystem();

	memcpy(LightPointSystem, BufferAtNode, NodeLength);

	(*Node)->SetRecord(LightPointSystem);
};

void ReadRecordDegreeOfFreedom(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordDegreeOfFreedom* DegreeOfFreedom = new ZEFLTRecordDegreeOfFreedom();

	memcpy(DegreeOfFreedom, BufferAtNode, NodeLength);

	(*Node)->SetRecord(DegreeOfFreedom);
};

void ReadRecordVertexList(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZESize NumberOfVertices = (NodeLength - sizeof(ZEFLTRecordBase)) / 4;

	ZEFLTRecordVertexList* VertexList = new ZEFLTRecordVertexList();

	memcpy(VertexList, BufferAtNode, sizeof(ZEFLTRecordBase));

	VertexList->VertexOffsets.SetCount(NumberOfVertices);

	memcpy(VertexList->VertexOffsets.GetCArray(), BufferAtNode + sizeof(ZEFLTRecordBase), NumberOfVertices * sizeof(ZEInt32));

	(*Node)->SetRecord(VertexList);
};

void ReadRecordMorphList(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZESize NumberOfVertices = (NodeLength - sizeof(ZEFLTRecordBase)) / 8;

	ZEFLTRecordMorphList* MorphVertexList = new ZEFLTRecordMorphList();

	memcpy(MorphVertexList, BufferAtNode, sizeof(ZEFLTRecordBase));

	MorphVertexList->VertexOffsets.SetCount(NumberOfVertices);

	memcpy(MorphVertexList->VertexOffsets.GetCArray(), BufferAtNode + sizeof(ZEFLTRecordBase), NumberOfVertices * sizeof(ZEFLTRecordMorphListVertex));

	(*Node)->SetRecord(MorphVertexList);
};

void ReadRecordBinarySeparatingPlane(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBinarySeparatingPlane* BinarySeparatingPlane = new ZEFLTRecordBinarySeparatingPlane();

	memcpy(BinarySeparatingPlane, BufferAtNode, NodeLength);

	(*Node)->SetRecord(BinarySeparatingPlane);
};

void ReadRecordExternalReference(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExternalReference* ExternalReference = new ZEFLTRecordExternalReference();

	memcpy(ExternalReference, BufferAtNode, NodeLength);

	(*Node)->SetRecord(ExternalReference);
};

void ReadRecordLevelOfDetail(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordLevelOfDetail* LevelOfDetail = new ZEFLTRecordLevelOfDetail();

	memcpy(LevelOfDetail, BufferAtNode, NodeLength);

	(*Node)->SetRecord(LevelOfDetail);
};

void ReadRecordSound(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordSound* Sound = new ZEFLTRecordSound();

	memcpy(Sound, BufferAtNode, NodeLength);

	(*Node)->SetRecord(Sound);
};

void ReadRecordLightSource(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordLightSource* LightSource = new ZEFLTRecordLightSource();

	memcpy(LightSource, BufferAtNode, NodeLength);

	(*Node)->SetRecord(LightSource);
};

void ReadRecordClipRegion(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordClipRegion* ClipRegion = new ZEFLTRecordClipRegion();

	memcpy(ClipRegion, BufferAtNode, NodeLength);

	(*Node)->SetRecord(ClipRegion);
};

void ReadRecordText(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordText* Text = new ZEFLTRecordText();

	memcpy(Text, BufferAtNode, NodeLength);

	(*Node)->SetRecord(Text);
};

void ReadRecordSwitch(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordSwitch* Switch = new ZEFLTRecordSwitch();
	memcpy(Switch, BufferAtNode, 28);

	Switch->MaskWords.SetCount(Switch->NumberOfMasks * Switch->NumberOfWordsPerMask);
	memcpy(Switch->MaskWords.GetCArray(), BufferAtNode + 28, (Switch->NumberOfMasks * Switch->NumberOfWordsPerMask) * sizeof(ZEUInt32));

	(*Node)->SetRecord(Switch);
};

void ReadRecordExtension(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{

};

void ReadRecordMultitexture(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordMultitexture* Multitexture = new ZEFLTRecordMultitexture();
	memcpy(Multitexture, BufferAtNode, sizeof(ZEFLTRecordBase) + 4);
	
	ZESize NumberOfLayers = (NodeLength - 8) / sizeof(ZEFLTRecordMultitextureLayer);
	Multitexture->Layers.SetCount(NumberOfLayers);

	memcpy(Multitexture->Layers.GetCArray(), BufferAtNode + (sizeof(ZEFLTRecordBase) + 4), NumberOfLayers * sizeof(ZEFLTRecordMultitextureLayer));

	ZEFLTResourceNode* MultitextureNode = ZEFLTResourceNode::CreateInstance();

	MultitextureNode->SetRecord(Multitexture);
	(*Node)->RegisterAncillary(MultitextureNode);
};

void ReadRecordUVList(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBase UVListBase;
	memcpy(&UVListBase, BufferAtNode, sizeof(ZEFLTRecordBase));

	ZEFLTResourceNode* UVListNode = ZEFLTResourceNode::CreateInstance();

	if (PreceedingOpCode == ZE_FLT_OPCODE_VERTEX_LIST)
	{
		ZEFLTRecordUVList<ZEFLTRecordUVListItemVertex>* UVList = new ZEFLTRecordUVList<ZEFLTRecordUVListItemVertex>();
		UVList->Opcode = UVListBase.Opcode;
		UVList->Length = UVListBase.Length;

		memcpy(&UVList->AttributeMask, BufferAtNode + sizeof(ZEFLTRecordBase), sizeof(ZEInt32));

		ZESize NumberOfCoordinates = (NodeLength - 8) / sizeof(ZEFLTRecordUVListItemVertex);
		UVList->TextureCoordinates.SetCount(NumberOfCoordinates);

		memcpy(UVList->TextureCoordinates.GetCArray(), BufferAtNode + (sizeof(ZEFLTRecordBase) + sizeof(ZEInt32)), NumberOfCoordinates * sizeof(ZEFLTRecordUVListItemVertex));

		UVListNode->SetRecord(UVList);
	}
	else
	{
		ZEFLTRecordUVList<ZEFLTRecordUVListItemMorph>* UVList = new ZEFLTRecordUVList<ZEFLTRecordUVListItemMorph>();
		UVList->Opcode = UVListBase.Opcode;
		UVList->Length = UVListBase.Length;

		memcpy(&UVList->AttributeMask, BufferAtNode + sizeof(ZEFLTRecordBase), sizeof(ZEInt32));

		ZESize NumberOfCoordinates = (NodeLength - 8) / sizeof(ZEFLTRecordUVListItemMorph);
		UVList->TextureCoordinates.SetCount(NumberOfCoordinates);

		memcpy(UVList->TextureCoordinates.GetCArray(), BufferAtNode + (sizeof(ZEFLTRecordBase) + sizeof(ZEInt32)), NumberOfCoordinates * sizeof(ZEFLTRecordUVListItemMorph));

		UVListNode->SetRecord(UVList);
	}

	(*Node)->RegisterAncillary(UVListNode);
};

void ReadRecordReplicate(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordReplicate* Replicate = new ZEFLTRecordReplicate();

	memcpy(Replicate, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ReplicateNode = ZEFLTResourceNode::CreateInstance();

	ReplicateNode->SetRecord(Replicate);
	(*Node)->RegisterAncillary(ReplicateNode);
};

void ReadRecordMatrix(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordMatrix* Matrix = new ZEFLTRecordMatrix();

	memcpy(Matrix, BufferAtNode, NodeLength);

	ZEFLTResourceNode* MatrixNode = ZEFLTResourceNode::CreateInstance();

	MatrixNode->SetRecord(Matrix);
	(*Node)->RegisterAncillary(MatrixNode);
};

void ReadRecordRotateAboutEdge(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordRotateAboutEdge* RotateAboutEdge = new ZEFLTRecordRotateAboutEdge();

	memcpy(RotateAboutEdge, BufferAtNode, NodeLength);

	ZEFLTResourceNode* RotateAboutEdgeNode = ZEFLTResourceNode::CreateInstance();

	RotateAboutEdgeNode->SetRecord(RotateAboutEdge);
	(*Node)->RegisterAncillary(RotateAboutEdgeNode);
};

void ReadRecordTranslate(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordTranslate* Translate = new ZEFLTRecordTranslate();

	memcpy(Translate, BufferAtNode, NodeLength);

	ZEFLTResourceNode* TranslateNode = ZEFLTResourceNode::CreateInstance();

	TranslateNode->SetRecord(Translate);
	(*Node)->RegisterAncillary(TranslateNode);
};

void ReadRecordScale(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordScale* Scale = new ZEFLTRecordScale();

	memcpy(Scale, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ScaleNode = ZEFLTResourceNode::CreateInstance();

	ScaleNode->SetRecord(Scale);
	(*Node)->RegisterAncillary(ScaleNode);
};

void ReadRecordRotateAboutPoint(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordRotateAboutPoint* RotateAboutPoint = new ZEFLTRecordRotateAboutPoint();

	memcpy(RotateAboutPoint, BufferAtNode, NodeLength);

	ZEFLTResourceNode* RotateAboutPointNode = ZEFLTResourceNode::CreateInstance();

	RotateAboutPointNode->SetRecord(RotateAboutPoint);
	(*Node)->RegisterAncillary(RotateAboutPointNode);
};

void ReadRecordRotateScaleToPoint(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordRotateScaleToPoint* RotateScaleToPoint = new ZEFLTRecordRotateScaleToPoint();

	memcpy(RotateScaleToPoint, BufferAtNode, NodeLength);

	ZEFLTResourceNode* RotateScaleToPointNode = ZEFLTResourceNode::CreateInstance();

	RotateScaleToPointNode->SetRecord(RotateScaleToPoint);
	(*Node)->RegisterAncillary(RotateScaleToPointNode);
};

void ReadRecordPut(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPut* Put = new ZEFLTRecordPut();

	memcpy(Put, BufferAtNode, NodeLength);

	ZEFLTResourceNode* PutNode = ZEFLTResourceNode::CreateInstance();

	PutNode->SetRecord(Put);
	(*Node)->RegisterAncillary(PutNode);
};

void ReadRecordGeneralMatrix(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordGeneralMatrix* GeneralMatrix = new ZEFLTRecordGeneralMatrix();

	memcpy(GeneralMatrix, BufferAtNode, NodeLength);

	ZEFLTResourceNode* GeneralMatrixNode = ZEFLTResourceNode::CreateInstance();

	GeneralMatrixNode->SetRecord(GeneralMatrix);
	(*Node)->RegisterAncillary(GeneralMatrixNode);
};

void ReadRecordVector(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordVector* Vector = new ZEFLTRecordVector();

	memcpy(Vector, BufferAtNode, NodeLength);

	ZEFLTResourceNode* VectorNode = ZEFLTResourceNode::CreateInstance();

	VectorNode->SetRecord(Vector);
	(*Node)->RegisterAncillary(VectorNode);
};

void ReadRecordBoundingBox(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBoundingBox* BoundingBox = new ZEFLTRecordBoundingBox();

	memcpy(BoundingBox, BufferAtNode, NodeLength);

	ZEFLTResourceNode* BoundingBoxNode = ZEFLTResourceNode::CreateInstance();

	BoundingBoxNode->SetRecord(BoundingBox);
	(*Node)->RegisterAncillary(BoundingBoxNode);
};

void ReadRecordBoundingSphere(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBoundingSphere* BoundingSphere = new ZEFLTRecordBoundingSphere();

	memcpy(BoundingSphere, BufferAtNode, NodeLength);

	ZEFLTResourceNode* BoundingSphereNode = ZEFLTResourceNode::CreateInstance();

	BoundingSphereNode->SetRecord(BoundingSphere);
	(*Node)->RegisterAncillary(BoundingSphereNode);
};

void ReadRecordBoundingCylinder(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBoundingCylinder* BoundingCylinder = new ZEFLTRecordBoundingCylinder();

	memcpy(BoundingCylinder, BufferAtNode, NodeLength);

	ZEFLTResourceNode* BoundingCylinderNode = ZEFLTResourceNode::CreateInstance();

	BoundingCylinderNode->SetRecord(BoundingCylinder);
	(*Node)->RegisterAncillary(BoundingCylinderNode);
};

void ReadRecordBoundingConvexHull(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBoundingConvexHull* ConvexHull = new ZEFLTRecordBoundingConvexHull();

	memcpy(ConvexHull, BufferAtNode, sizeof(ZEFLTRecordBase) + 4);
	ConvexHull->Triangles.SetCount(ConvexHull->NumberOfTriangles);

	memcpy(ConvexHull->Triangles.GetCArray(), BufferAtNode + (sizeof(ZEFLTRecordBase) + 4), ConvexHull->NumberOfTriangles * sizeof(ZEFLTRecordBoundingConvexHullTriangles));

	ZEFLTResourceNode* ConvexHullNode = ZEFLTResourceNode::CreateInstance();

	ConvexHullNode->SetRecord(ConvexHull);
	(*Node)->RegisterAncillary(ConvexHullNode);
};

void ReadRecordBoundingVolumeCenter(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBoundingVolumeCenter* VolumeCenter = new ZEFLTRecordBoundingVolumeCenter();

	memcpy(VolumeCenter, BufferAtNode, NodeLength);

	ZEFLTResourceNode* VolumeCenterNode = ZEFLTResourceNode::CreateInstance();

	VolumeCenterNode->SetRecord(VolumeCenter);
	(*Node)->RegisterAncillary(VolumeCenterNode);
};

void ReadRecordBoundingVolumeOrientation(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBoundingVolumeOrientation* VolumeOrientation = new ZEFLTRecordBoundingVolumeOrientation();

	memcpy(VolumeOrientation, BufferAtNode, NodeLength);

	ZEFLTResourceNode* VolumeOrientationNode = ZEFLTResourceNode::CreateInstance();

	VolumeOrientationNode->SetRecord(VolumeOrientation);
	(*Node)->RegisterAncillary(VolumeOrientationNode);
};

void ReadRecordExtensionFieldBoolean(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtensionFieldBoolean* ExtensionFieldBoolean = new ZEFLTRecordExtensionFieldBoolean();

	memcpy(ExtensionFieldBoolean, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtensionFieldBooleanNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldBooleanNode->SetRecord(ExtensionFieldBoolean);
	(*Node)->RegisterAncillary(ExtensionFieldBooleanNode);
};

void ReadRecordExtensionFieldInteger(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtensionFieldInteger* ExtensionFieldInteger = new ZEFLTRecordExtensionFieldInteger();

	memcpy(ExtensionFieldInteger, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtensionFieldIntegerNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldIntegerNode->SetRecord(ExtensionFieldInteger);
	(*Node)->RegisterAncillary(ExtensionFieldIntegerNode);
};

void ReadRecordExtensionFieldFloat(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtensionFieldFloat* ExtensionFieldFloat = new ZEFLTRecordExtensionFieldFloat();

	memcpy(ExtensionFieldFloat, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtensionFieldFloatNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldFloatNode->SetRecord(ExtensionFieldFloat);
	(*Node)->RegisterAncillary(ExtensionFieldFloatNode);
};

void ReadRecordExtensionFieldDouble(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtensionFieldDouble* ExtensionFieldDouble = new ZEFLTRecordExtensionFieldDouble();

	memcpy(ExtensionFieldDouble, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtensionFieldDoubleNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldDoubleNode->SetRecord(ExtensionFieldDouble);
	(*Node)->RegisterAncillary(ExtensionFieldDoubleNode);
};

void ReadRecordExtensionFieldString(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtensionFieldString* ExtensionFieldString = new ZEFLTRecordExtensionFieldString();

	memcpy(ExtensionFieldString, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtensionFieldStringNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldStringNode->SetRecord(ExtensionFieldString);
	(*Node)->RegisterAncillary(ExtensionFieldStringNode);
};

void ReadRecordExtensionFieldXML(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtensionFieldXML* ExtensionFieldXML = new ZEFLTRecordExtensionFieldXML();

	memcpy(ExtensionFieldXML, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtensionFieldXMLNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldXMLNode->SetRecord(ExtensionFieldXML);
	(*Node)->RegisterAncillary(ExtensionFieldXMLNode);
};

void ReadRecordContinuation(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBase ContinuationBase;
	memcpy(&ContinuationBase, BufferAtNode, sizeof(ZEFLTRecordBase));

	if (PreceedingOpCode == ZE_FLT_OPCODE_LOCAL_VERTEX_POOL)
	{
		ZEFLTRecordContinuation<ZEFLTRecordLocalVertexPoolVertex>* Continuation = new ZEFLTRecordContinuation<ZEFLTRecordLocalVertexPoolVertex>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZESize NumberOfVertices = (Continuation->Length - 4) / sizeof(ZEFLTRecordLocalVertexPoolVertex);
		Continuation->Data.SetCount(NumberOfVertices);

		memcpy(Continuation->Data.GetCArray(), BufferAtNode + sizeof(ZEFLTRecordBase), NumberOfVertices * sizeof(ZEFLTRecordLocalVertexPoolVertex));

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);
	}
	else if (PreceedingOpCode == ZE_FLT_OPCODE_MESH_PRIMITIVE)
	{
		ZEFLTRecordContinuation<ZEBigEndian<ZEInt32>>* Continuation = new ZEFLTRecordContinuation<ZEBigEndian<ZEInt32>>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZEUInt16 IndexSize = ((ZEFLTRecordMeshPrimitive*)((*Node)->GetRecord()))->IndexSize;

		ZESize NumberOfVertices = (Continuation->Length - 4) / IndexSize;
		Continuation->Data.SetCount(NumberOfVertices);
		Continuation->Data.Fill(NULL);

		for (ZESize I = 0; I < NumberOfVertices; I++)
		{
			memcpy(&Continuation->Data[I], BufferAtNode + (sizeof(ZEFLTRecordBase) + I * IndexSize), IndexSize);
		}

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);
	}
	else if (PreceedingOpCode == ZE_FLT_OPCODE_VERTEX_LIST)
	{
		ZEFLTRecordContinuation<ZEBigEndian<ZEInt32>>* Continuation = new ZEFLTRecordContinuation<ZEBigEndian<ZEInt32>>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZESize NumberOfVertices = (Continuation->Length - 4) / sizeof(ZEInt32);
		Continuation->Data.SetCount(NumberOfVertices);

		memcpy(Continuation->Data.GetCArray(), BufferAtNode + sizeof(ZEFLTRecordBase), NumberOfVertices * sizeof(ZEInt32));

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);

	}
	else if (PreceedingOpCode == ZE_FLT_OPCODE_MORPH_VERTEX_LIST)
	{
		ZEFLTRecordContinuation<ZEFLTRecordMorphListVertex>* Continuation = new ZEFLTRecordContinuation<ZEFLTRecordMorphListVertex>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZESize NumberOfVertices = (Continuation->Length - 4) / sizeof(ZEFLTRecordMorphListVertex);
		Continuation->Data.SetCount(NumberOfVertices);

		memcpy(Continuation->Data.GetCArray(), BufferAtNode + sizeof(ZEFLTRecordBase), NumberOfVertices * sizeof(ZEFLTRecordMorphListVertex));

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);
	}
	else if (PreceedingOpCode == ZE_FLT_OPCODE_EXTENSION)
	{
		//File->Seek(ContinuationBase.Length - 4, ZE_SF_CURRENT);
	}
	else if (PreceedingOpCode == ZE_FLT_OPCODE_BOUNDING_CONVEX_HULL)
	{
		ZEFLTRecordContinuation<ZEFLTRecordBoundingConvexHullTriangles>* Continuation = new ZEFLTRecordContinuation<ZEFLTRecordBoundingConvexHullTriangles>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZESize NumberOfTriangles = (Continuation->Length - 4) / sizeof(ZEFLTRecordBoundingConvexHullTriangles);
		Continuation->Data.SetCount(NumberOfTriangles);

		memcpy(Continuation->Data.GetCArray(), BufferAtNode + sizeof(ZEFLTRecordBase), NumberOfTriangles * sizeof(ZEFLTRecordBoundingConvexHullTriangles));

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);
	}
	else if (PreceedingOpCode == ZE_FLT_OPCODE_NAME_TABLE)
	{
		ZEFLTRecordContinuation<ZEFLTRecordNameTableEntry>* Continuation = new ZEFLTRecordContinuation<ZEFLTRecordNameTableEntry>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;
		ZEUInt32 ContinuationCursor = sizeof(ZEFLTRecordBase);

		ZEFLTRecordNameTableEntry TempNameTableEntry;
		ZEInt32	TempEntryLength;

		while (ContinuationCursor != Continuation->Length)
		{
			memcpy(&TempEntryLength, BufferAtNode + ContinuationCursor, sizeof(ZEInt32));
			memcpy(&TempNameTableEntry, BufferAtNode + ContinuationCursor, TempEntryLength);
			Continuation->Data.Add(TempNameTableEntry);
			ContinuationCursor += TempEntryLength;
		}

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);

	}
	else
	{
		return;
	}

};

void ReadRecordVertexPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPaletteVertex* VertexPalette = new ZEFLTRecordPaletteVertex();

	memcpy(VertexPalette, BufferAtNode, NodeLength);

	ZEFLTResourceNode* VertexPaletteNode = ZEFLTResourceNode::CreateInstance();

	VertexPaletteNode->SetRecord(VertexPalette);
	(*Node)->RegisterAncillary(VertexPaletteNode);
};

void ReadRecordVertexWithColor(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordVertexWithColor* VertexWithColor = new ZEFLTRecordVertexWithColor();

	memcpy(VertexWithColor, BufferAtNode, NodeLength);

	ZEFLTResourceNode* VertexWithColorNode = ZEFLTResourceNode::CreateInstance();

	VertexWithColorNode->SetRecord(VertexWithColor);
	(*Node)->RegisterAncillary(VertexWithColorNode);
};

void ReadRecordVertexWithColorNormal(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordVertexWithColorNormal* VertexWithColorNormal = new ZEFLTRecordVertexWithColorNormal();

	memcpy(VertexWithColorNormal, BufferAtNode, NodeLength);

	ZEFLTResourceNode* VertexWithColorNormalNode = ZEFLTResourceNode::CreateInstance();

	VertexWithColorNormalNode->SetRecord(VertexWithColorNormal);
	(*Node)->RegisterAncillary(VertexWithColorNormalNode);
};

void ReadRecordVertexWithColorUV(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordVertexWithColorUV* VertexWithColorUV = new ZEFLTRecordVertexWithColorUV();

	memcpy(VertexWithColorUV, BufferAtNode, NodeLength);

	ZEFLTResourceNode* VertexWithColorUVNode = ZEFLTResourceNode::CreateInstance();

	VertexWithColorUVNode->SetRecord(VertexWithColorUV);
	(*Node)->RegisterAncillary(VertexWithColorUVNode);
};

void ReadRecordVertexWithColorNormalUV(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordVertexWithColorNormalUV* VertexWithColorNormalUV = new ZEFLTRecordVertexWithColorNormalUV();

	memcpy(VertexWithColorNormalUV, BufferAtNode, NodeLength);

	ZEFLTResourceNode* VertexWithColorNormalUVNode = ZEFLTResourceNode::CreateInstance();

	VertexWithColorNormalUVNode->SetRecord(VertexWithColorNormalUV);
	(*Node)->RegisterAncillary(VertexWithColorNormalUVNode);
};

void ReadRecordColorPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPaletteColor* ColorPalette = new ZEFLTRecordPaletteColor();

	memcpy(ColorPalette, BufferAtNode, sizeof(ZEInt16) + sizeof(ZEUInt16) + 128 * sizeof(char) + 1024 * sizeof(ZEInt32));

	if (ColorPalette->Length > 4228)
	{
		memcpy(&ColorPalette->NumberofColorNames, BufferAtNode + 4228, sizeof(ZEInt32));

		ColorPalette->ColorNames.SetCount(ColorPalette->NumberofColorNames);
		ZEUInt32 ColorPaletteCursor = 4228;
		ZEUInt16 ColorNameLength;

		for (ZESize I = 0; I < ColorPalette->NumberofColorNames; I++)
		{
			memcpy(&ColorNameLength, BufferAtNode + ColorPaletteCursor, sizeof(ZEInt16));
			memcpy(&ColorPalette->ColorNames[I], BufferAtNode + ColorPaletteCursor, ColorNameLength);
			ColorPaletteCursor += ColorNameLength;
		}
	}
	else
	{
		ColorPalette->NumberofColorNames = 0;
		ColorPalette->ColorNames.Clear();
	}

	((ZEFLTResourceNodeHeader*)(*Node))->SetColorPalette(ColorPalette);

};

void ReadRecordNameTable(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordNameTable* NameTable = new ZEFLTRecordNameTable();

	memcpy(NameTable, BufferAtNode, 10);

	NameTable->Names.SetCount(NameTable->NumberOfNames);
	ZEUInt32 NameTableCursor = 10;
	ZEUInt16 NameEntryLength;

	for (ZESize I = 0; I < NameTable->NumberOfNames; I++)
	{
		memcpy(&NameEntryLength, BufferAtNode + NameTableCursor, sizeof(ZEInt32));
		memcpy(&NameTable->Names[I], BufferAtNode + NameTableCursor, NameEntryLength);
		NameTableCursor += NameEntryLength;
	};

	ZEFLTResourceNode* NameTableNode = ZEFLTResourceNode::CreateInstance();

	NameTableNode->SetRecord(NameTable);
	(*Node)->RegisterAncillary(NameTableNode);
};

void ReadRecordMaterialPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPaletteMaterial* MaterialPalette = new ZEFLTRecordPaletteMaterial();

	memcpy(MaterialPalette, BufferAtNode, NodeLength);

	((ZEFLTResourceNodeHeader*)(*Node))->RegisterStandardMaterial(MaterialPalette);

};

void ReadRecordExtendedMaterialHeader(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialHeader* ExtendedMaterialHeader = new ZEFLTRecordExtendedMaterialHeader();

	memcpy(ExtendedMaterialHeader, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialHeaderNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialHeaderNode->SetRecord(ExtendedMaterialHeader);
	(*Node)->RegisterAncillary(ExtendedMaterialHeaderNode);
};

void ReadRecordExtendedMaterialAmbient(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialAmbient* ExtendedMaterialAmbient = new ZEFLTRecordExtendedMaterialAmbient();

	memcpy(ExtendedMaterialAmbient, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialAmbientNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialAmbientNode->SetRecord(ExtendedMaterialAmbient);
	(*Node)->RegisterAncillary(ExtendedMaterialAmbientNode);
};

void ReadRecordExtendedMaterialDiffuse(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialDiffuse* ExtendedMaterialDiffuse = new ZEFLTRecordExtendedMaterialDiffuse();

	memcpy(ExtendedMaterialDiffuse, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialDiffuseNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialDiffuseNode->SetRecord(ExtendedMaterialDiffuse);
	(*Node)->RegisterAncillary(ExtendedMaterialDiffuseNode);
};

void ReadRecordExtendedMaterialSpecular(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialSpecular* ExtendedMaterialSpecular = new ZEFLTRecordExtendedMaterialSpecular();

	memcpy(ExtendedMaterialSpecular, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialSpecularNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialSpecularNode->SetRecord(ExtendedMaterialSpecular);
	(*Node)->RegisterAncillary(ExtendedMaterialSpecularNode);
};

void ReadRecordExtendedMaterialEmissive(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialEmissive* ExtendedMaterialEmissive = new ZEFLTRecordExtendedMaterialEmissive();

	memcpy(ExtendedMaterialEmissive, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialEmissiveNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialEmissiveNode->SetRecord(ExtendedMaterialEmissive);
	(*Node)->RegisterAncillary(ExtendedMaterialEmissiveNode);
};

void ReadRecordExtendedMaterialAlpha(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialAlpha* ExtendedMaterialAlpha = new ZEFLTRecordExtendedMaterialAlpha();

	memcpy(ExtendedMaterialAlpha, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialAlphaNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialAlphaNode->SetRecord(ExtendedMaterialAlpha);
	(*Node)->RegisterAncillary(ExtendedMaterialAlphaNode);
};

void ReadRecordExtendedMaterialLightMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialLightMap* ExtendedMaterialLightMap = new ZEFLTRecordExtendedMaterialLightMap();

	memcpy(ExtendedMaterialLightMap, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialLightMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialLightMapNode->SetRecord(ExtendedMaterialLightMap);
	(*Node)->RegisterAncillary(ExtendedMaterialLightMapNode);
};

void ReadRecordExtendedMaterialNormalMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialNormalMap* ExtendedMaterialNormalMap = new ZEFLTRecordExtendedMaterialNormalMap();

	memcpy(ExtendedMaterialNormalMap, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialNormalMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialNormalMapNode->SetRecord(ExtendedMaterialNormalMap);
	(*Node)->RegisterAncillary(ExtendedMaterialNormalMapNode);
};

void ReadRecordExtendedMaterialBumpMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialBumpMap* ExtendedMaterialBumpMap = new ZEFLTRecordExtendedMaterialBumpMap();

	memcpy(ExtendedMaterialBumpMap, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialBumpMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialBumpMapNode->SetRecord(ExtendedMaterialBumpMap);
	(*Node)->RegisterAncillary(ExtendedMaterialBumpMapNode);
};

void ReadRecordExtendedMaterialShadowMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialShadowMap* ExtendedMaterialShadowMap = new ZEFLTRecordExtendedMaterialShadowMap();

	memcpy(ExtendedMaterialShadowMap, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialShadowMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialShadowMapNode->SetRecord(ExtendedMaterialShadowMap);
	(*Node)->RegisterAncillary(ExtendedMaterialShadowMapNode);
};

void ReadRecordExtendedMaterialReflectionMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordExtendedMaterialReflectionMap* ExtendedMaterialReflectionMap = new ZEFLTRecordExtendedMaterialReflectionMap();

	memcpy(ExtendedMaterialReflectionMap, BufferAtNode, NodeLength);

	ZEFLTResourceNode* ExtendedMaterialReflectionMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialReflectionMapNode->SetRecord(ExtendedMaterialReflectionMap);
	(*Node)->RegisterAncillary(ExtendedMaterialReflectionMapNode);
};

void ReadRecordTexturePalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPaletteTexture* TexturePalette = new ZEFLTRecordPaletteTexture();

	memcpy(TexturePalette, BufferAtNode, NodeLength);

	((ZEFLTResourceNodeHeader*)(*Node))->RegisterTexture(TexturePalette);

};

void ReadRecordEyepointTrackPlanePalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPaletteEyePointsAndTrackPlanes* EyePointsAndTrackPlanesPalette = new ZEFLTRecordPaletteEyePointsAndTrackPlanes();

	memcpy(EyePointsAndTrackPlanesPalette, BufferAtNode, sizeof(ZEFLTRecordBase) + sizeof(ZEInt32));

	EyePointsAndTrackPlanesPalette->EyePoints.SetCount(10);
	EyePointsAndTrackPlanesPalette->TrackPlanes.SetCount(10);
	ZEUInt32 EyeTrackCursor = sizeof(ZEFLTRecordBase) + sizeof(ZEInt32);

	for (ZESize I = 0; I < 10; I++)
	{
		memcpy(&EyePointsAndTrackPlanesPalette->EyePoints[I], BufferAtNode + EyeTrackCursor, sizeof(ZEFLTRecordEyePoint));
		EyeTrackCursor += sizeof(ZEFLTRecordEyePoint);
	}

	for (ZESize I = 0; I < 10; I++)
	{
		memcpy(&EyePointsAndTrackPlanesPalette->TrackPlanes[I], BufferAtNode + EyeTrackCursor, sizeof(ZEFLTRecordTrackPlane));
		EyeTrackCursor += sizeof(ZEFLTRecordTrackPlane);
	}

	ZEFLTResourceNode* EyePointsAndTrackPlanesPaletteNode = ZEFLTResourceNode::CreateInstance();

	EyePointsAndTrackPlanesPaletteNode->SetRecord(EyePointsAndTrackPlanesPalette);
	(*Node)->RegisterAncillary(EyePointsAndTrackPlanesPaletteNode);
};

void ReadRecordSoundPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordBase SoundPaletteBase;
	ZEBigEndian<ZEInt32> Subtype;

	memcpy(&SoundPaletteBase, BufferAtNode, sizeof(ZEFLTRecordBase));
	memcpy(&Subtype, BufferAtNode + sizeof(ZEFLTRecordBase), sizeof(ZEInt32));
	ZEUInt32 SoundPaletteCursor = sizeof(ZEFLTRecordBase) + sizeof(ZEInt32);

	ZEFLTResourceNode* SoundPaletteNode = ZEFLTResourceNode::CreateInstance();

	if (Subtype == 1) //Signifies Header
	{
		ZEFLTRecordPaletteSoundHeader* SoundPaletteHeader = new ZEFLTRecordPaletteSoundHeader();
		SoundPaletteHeader->Opcode = SoundPaletteBase.Opcode;
		SoundPaletteHeader->Length = SoundPaletteBase.Length;
		SoundPaletteHeader->Subtype = Subtype;

		memcpy(&SoundPaletteHeader->MaxNumber, BufferAtNode + SoundPaletteCursor, sizeof(ZEInt32));
		SoundPaletteCursor += sizeof(ZEInt32);
		memcpy(&SoundPaletteHeader->ActualNumber, BufferAtNode + SoundPaletteCursor, sizeof(ZEInt32));
		SoundPaletteCursor += sizeof(ZEInt32);
		memcpy(&SoundPaletteHeader->TotalLength, BufferAtNode + SoundPaletteCursor, sizeof(ZEInt32));
		SoundPaletteCursor += sizeof(ZEInt32);
		memcpy(&SoundPaletteHeader->Reserved, BufferAtNode + SoundPaletteCursor, 3 * sizeof(ZEInt32));
		SoundPaletteCursor += 3 * sizeof(ZEInt32);

		SoundPaletteHeader->Indices.SetCount(SoundPaletteHeader->ActualNumber);

		for (ZESize I = 0; I < SoundPaletteHeader->ActualNumber; I++)
		{
			memcpy(&SoundPaletteHeader->Indices[I], BufferAtNode + SoundPaletteCursor, sizeof(ZEFLTRecordPaletteSoundIndex));
			SoundPaletteCursor += sizeof(ZEFLTRecordPaletteSoundIndex);
		}

		SoundPaletteNode->SetRecord(SoundPaletteHeader);
	}
	else //Subtype 2 - Signifies Data
	{
		ZEFLTRecordPaletteSoundData* SoundPaletteData = new ZEFLTRecordPaletteSoundData();
		SoundPaletteData->Opcode = SoundPaletteBase.Opcode;
		SoundPaletteData->Length = SoundPaletteBase.Length;
		SoundPaletteData->Subtype = Subtype;

		memcpy(&SoundPaletteData->TotalLength, BufferAtNode + SoundPaletteCursor, sizeof(ZEInt32));
		SoundPaletteCursor += sizeof(ZEInt32);
		SoundPaletteData->PackedFileNames.SetCount(SoundPaletteData->Length - SoundPaletteCursor);
		memcpy(SoundPaletteData->PackedFileNames.GetCArray(), BufferAtNode + SoundPaletteCursor, SoundPaletteData->Length - SoundPaletteCursor);

		SoundPaletteNode->SetRecord(SoundPaletteData);
	}

	(*Node)->RegisterAncillary(SoundPaletteNode);

};

void ReadRecordLightSourcePalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPaletteLightSource* LightSourcePalette = new ZEFLTRecordPaletteLightSource();

	memcpy(LightSourcePalette, BufferAtNode, NodeLength);

	ZEFLTResourceNode* LightSourcePaletteNode = ZEFLTResourceNode::CreateInstance();

	LightSourcePaletteNode->SetRecord(LightSourcePalette);
	(*Node)->RegisterAncillary(LightSourcePaletteNode);
};

void ReadRecordLightPointAppearancePalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPaletteLightPointAppearance* LightAppearancePalette = new ZEFLTRecordPaletteLightPointAppearance();

	memcpy(LightAppearancePalette, BufferAtNode, NodeLength);

	ZEFLTResourceNode* LightAppearancePaletteNode = ZEFLTResourceNode::CreateInstance();

	LightAppearancePaletteNode->SetRecord(LightAppearancePalette);
	(*Node)->RegisterAncillary(LightAppearancePaletteNode);
};

void ReadRecordLightPointAnimationPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPaletteLightPointAnimation* LightAnimationPalette = new ZEFLTRecordPaletteLightPointAnimation();

	memcpy(LightAnimationPalette, BufferAtNode, 1340);

	LightAnimationPalette->Sequences.SetCount(LightAnimationPalette->NumberOfSequences);
	ZEUInt32 LightAnimationPaletteCursor = 1340;

	for (ZESize I = 0; I < LightAnimationPalette->NumberOfSequences; I++)
	{
		memcpy(&LightAnimationPalette->Sequences[I], BufferAtNode + LightAnimationPaletteCursor, sizeof(ZEFLTRecordPaletteLightPointAnimationSequence));
		LightAnimationPaletteCursor += sizeof(ZEFLTRecordPaletteLightPointAnimationSequence);
	}

	ZEFLTResourceNode* LightAnimationPaletteNode = ZEFLTResourceNode::CreateInstance();

	LightAnimationPaletteNode->SetRecord(LightAnimationPalette);
	(*Node)->RegisterAncillary(LightAnimationPaletteNode);
};

void ReadRecordTextureMappingPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTRecordPaletteTextureMapping* TextureMappingPalette = new ZEFLTRecordPaletteTextureMapping();

	memcpy(TextureMappingPalette, BufferAtNode, 168);
	ZEUInt32 TextureMappingPaletteCursor = 168;

	if (TextureMappingPalette->Type == 1)
	{
		ZEFLTRecordPaletteTextureMapping3PointPut* ThreePointPutParameters = new ZEFLTRecordPaletteTextureMapping3PointPut();
		memcpy(ThreePointPutParameters, BufferAtNode + TextureMappingPaletteCursor, sizeof(ZEFLTRecordPaletteTextureMapping3PointPut));
		TextureMappingPalette->Parameters = ThreePointPutParameters;
		TextureMappingPaletteCursor += sizeof(ZEFLTRecordPaletteTextureMapping3PointPut);
	}
	else if (TextureMappingPalette->Type == 2)
	{
		ZEFLTRecordPaletteTextureMapping4PointPut* FourPointPutParameters = new ZEFLTRecordPaletteTextureMapping4PointPut();
		memcpy(FourPointPutParameters, BufferAtNode + TextureMappingPaletteCursor, sizeof(ZEFLTRecordPaletteTextureMapping4PointPut));
		TextureMappingPalette->Parameters = FourPointPutParameters;
		TextureMappingPaletteCursor += sizeof(ZEFLTRecordPaletteTextureMapping4PointPut);
	}
	else if (TextureMappingPalette->Type == 4)
	{
		ZEFLTRecordPaletteTextureMappingSphericalProject* SphericalParameters = new ZEFLTRecordPaletteTextureMappingSphericalProject();
		memcpy(SphericalParameters, BufferAtNode + TextureMappingPaletteCursor, sizeof(ZEFLTRecordPaletteTextureMappingSphericalProject));
		TextureMappingPalette->Parameters = SphericalParameters;
		TextureMappingPaletteCursor += sizeof(ZEFLTRecordPaletteTextureMappingSphericalProject);
	}
	else if (TextureMappingPalette->Type == 5)
	{
		ZEFLTRecordPaletteTextureMappingRadialProject* RadialParameters = new ZEFLTRecordPaletteTextureMappingRadialProject();
		memcpy(RadialParameters, BufferAtNode + TextureMappingPaletteCursor, sizeof(ZEFLTRecordPaletteTextureMappingRadialProject));
		TextureMappingPalette->Parameters = RadialParameters;
		TextureMappingPaletteCursor += sizeof(ZEFLTRecordPaletteTextureMappingRadialProject);
	}
	else
	{
		TextureMappingPalette->Parameters = NULL;
	}

	if (TextureMappingPalette->WarpedFlag == 1)
	{
		ZEFLTRecordPaletteTextureMappingWarped* WarpedParameters = new ZEFLTRecordPaletteTextureMappingWarped();
		memcpy(WarpedParameters, BufferAtNode + TextureMappingPaletteCursor, sizeof(ZEFLTRecordPaletteTextureMappingWarped));
		TextureMappingPalette->WarpedParameters = WarpedParameters;
	}
	else
	{
		TextureMappingPalette->WarpedParameters = NULL;
	}

	ZEFLTResourceNode* TextureMappingNode = ZEFLTResourceNode::CreateInstance();

	TextureMappingNode->SetRecord(TextureMappingPalette);
	(*Node)->RegisterAncillary(TextureMappingNode);
};

void ReadRecordPushLevel(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	ZEFLTResourceNode* Child = ZEFLTResourceNode::CreateInstance();

	(*Node)->RegisterChild(Child);

	*Node = Child;
};

void ReadRecordPopLevel(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{
	*Node = (*Node)->GetParent();
};

void ReadRecordPushSubface(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{

};

void ReadRecordPopSubface(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{

};

void ReadRecordPushExtension(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{

};

void ReadRecordPopExtension(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{

};

void ReadRecordInstanceReference(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{

};

void ReadRecordInstanceDefinition(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{

};

void ReadRecordPushAttribute(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{

};

void ReadRecordPopAttribute(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength)
{

};
