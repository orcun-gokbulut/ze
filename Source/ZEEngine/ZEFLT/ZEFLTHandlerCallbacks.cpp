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

void ReadRecordHeader(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordHeader* Header = new ZEFLTRecordHeader();

	File->Read(Header, CurrentLength, 1);

	(*Node)->SetRecord(Header);
};

void ReadRecordGroup(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordGroup* Group = new ZEFLTRecordGroup();

	File->Read(Group, CurrentLength, 1);

	(*Node)->SetRecord(Group);
};

void ReadRecordObject(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordObject* Object = new ZEFLTRecordObject();

	File->Read(Object, CurrentLength, 1);

	(*Node)->SetRecord(Object);
};

void ReadRecordFace(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordFace* Face = new ZEFLTRecordFace();

	File->Read(Face, CurrentLength, 1);

	(*Node)->SetRecord(Face);
};

void ReadRecordMesh(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordMesh* Mesh = new ZEFLTRecordMesh();

	File->Read(Mesh, CurrentLength, 1);

	(*Node)->SetRecord(Mesh);
};

void ReadRecordLocalVertexPool(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordLocalVertexPool* LocalVertexPool = new ZEFLTRecordLocalVertexPool();

	File->Read(LocalVertexPool, 12, 1);

	LocalVertexPool->Vertices.SetCount(LocalVertexPool->NumberOfVertices);
	ZEFLTRecordLocalVertexPoolVertex Vertex;

	for (ZESize I = 0; I < LocalVertexPool->NumberOfVertices; I++)
	{
		File->Read(&Vertex, sizeof(ZEFLTRecordLocalVertexPoolVertex), 1);
		LocalVertexPool->Vertices[I] = Vertex;
	}

	(*Node)->SetRecord(LocalVertexPool);

};

void ReadRecordMeshPrimitive(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordMeshPrimitive* MeshPrimitive = new ZEFLTRecordMeshPrimitive();

	File->Read(MeshPrimitive, 12, 1);

	MeshPrimitive->Indices.SetCount(MeshPrimitive->VertexCount);
	MeshPrimitive->Indices.Fill(NULL);
	ZEBigEndian<ZEInt32> Index;

	for (ZESize I = 0; I < MeshPrimitive->VertexCount; I++)
	{
		File->Read(&Index, sizeof(ZEInt32), 1);
		MeshPrimitive->Indices[I] = Index;
	}

	(*Node)->SetRecord(MeshPrimitive);
};

void ReadRecordIndexedLightPoint(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordIndexedLightPoint* IndexedLightPoint = new ZEFLTRecordIndexedLightPoint();

	File->Read(IndexedLightPoint, CurrentLength, 1);

	(*Node)->SetRecord(IndexedLightPoint);
};

void ReadRecordLightPoint(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordLightPoint* LightPoint = new ZEFLTRecordLightPoint();

	File->Read(LightPoint, CurrentLength, 1);

	(*Node)->SetRecord(LightPoint);
};

void ReadRecordLightPointSystem(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordLightPointSystem* LightPointSystem = new ZEFLTRecordLightPointSystem();

	File->Read(LightPointSystem, CurrentLength, 1);

	(*Node)->SetRecord(LightPointSystem);
};

void ReadRecordDegreeOfFreedom(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordDegreeOfFreedom* DegreeOfFreedom = new ZEFLTRecordDegreeOfFreedom();

	File->Read(DegreeOfFreedom, CurrentLength, 1);

	(*Node)->SetRecord(DegreeOfFreedom);
};

void ReadRecordVertexList(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZESize NumberOfVertices = (CurrentLength - sizeof(ZEFLTRecordBase)) / 4;

	ZEFLTRecordVertexList* VertexList = new ZEFLTRecordVertexList();
	File->Read(VertexList, sizeof(ZEFLTRecordBase), 1);

	VertexList->VertexOffsets.SetCount(NumberOfVertices);
	ZEBigEndian<ZEInt32> Offset;

	for (ZESize I = 0; I < NumberOfVertices; I++)
	{
		File->Read(&Offset, sizeof(ZEInt32), 1);
		VertexList->VertexOffsets[I] = Offset;
	}

	(*Node)->SetRecord(VertexList);
};

void ReadRecordMorphList(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZESize NumberOfVertices = (CurrentLength - sizeof(ZEFLTRecordBase)) / 8;

	ZEFLTRecordMorphList* MorphVertexList = new ZEFLTRecordMorphList();
	File->Read(MorphVertexList, sizeof(ZEFLTRecordBase), 1);

	MorphVertexList->VertexOffsets.SetCount(NumberOfVertices);
	ZEFLTRecordMorphListVertex Offsets;

	for (ZESize I = 0; I < NumberOfVertices; I++)
	{
		File->Read(&Offsets, sizeof(ZEFLTRecordMorphListVertex), 1);
		MorphVertexList->VertexOffsets[I] = Offsets;
	}

	(*Node)->SetRecord(MorphVertexList);
};

void ReadRecordBinarySeparatingPlane(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBinarySeparatingPlane* BinarySeparatingPlane = new ZEFLTRecordBinarySeparatingPlane();

	File->Read(BinarySeparatingPlane, CurrentLength, 1);

	(*Node)->SetRecord(BinarySeparatingPlane);
};

void ReadRecordExternalReference(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExternalReference* ExternalReference = new ZEFLTRecordExternalReference();

	File->Read(ExternalReference, CurrentLength, 1);

	(*Node)->SetRecord(ExternalReference);
};

void ReadRecordLevelOfDetail(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordLevelOfDetail* LevelOfDetail = new ZEFLTRecordLevelOfDetail();

	File->Read(LevelOfDetail, CurrentLength, 1);

	(*Node)->SetRecord(LevelOfDetail);
};

void ReadRecordSound(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordSound* Sound = new ZEFLTRecordSound();

	File->Read(Sound, CurrentLength, 1);

	(*Node)->SetRecord(Sound);
};

void ReadRecordLightSource(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordLightSource* LightSource = new ZEFLTRecordLightSource();

	File->Read(LightSource, CurrentLength, 1);

	(*Node)->SetRecord(LightSource);
};

void ReadRecordClipRegion(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordClipRegion* ClipRegion = new ZEFLTRecordClipRegion();

	File->Read(ClipRegion, CurrentLength, 1);

	(*Node)->SetRecord(ClipRegion);
};

void ReadRecordText(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordText* Text = new ZEFLTRecordText();

	File->Read(Text, CurrentLength, 1);

	(*Node)->SetRecord(Text);
};

void ReadRecordSwitch(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordSwitch* Switch = new ZEFLTRecordSwitch();
	File->Read(Switch, 28, 1);

	Switch->MaskWords.SetCount(Switch->NumberOfMasks * Switch->NumberOfWordsPerMask);
	ZEBigEndian<ZEUInt32> Words;

	for (ZESize I = 0; I < Switch->MaskWords.GetCount(); I++)
	{
		File->Read(&Words, sizeof(ZEUInt32), 1);
		Switch->MaskWords[I] = Words;
	}

	(*Node)->SetRecord(Switch);
};

void ReadRecordExtension(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{

};

void ReadRecordMultitexture(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordMultitexture* Multitexture = new ZEFLTRecordMultitexture();
	File->Read(Multitexture, sizeof(ZEFLTRecordBase) + 4, 1);
	
	ZESize NumberOfLayers = (CurrentLength - 8) / sizeof(ZEFLTRecordMultitextureLayer);
	Multitexture->Layers.SetCount(NumberOfLayers);

	for (ZESize I = 0; I < NumberOfLayers; I++)
	{
		File->Read(&Multitexture->Layers[I], sizeof(ZEFLTRecordMultitextureLayer), 1);
	}

	ZEFLTResourceNode* MultitextureNode = ZEFLTResourceNode::CreateInstance();

	MultitextureNode->SetRecord(Multitexture);
	(*Node)->RegisterAncillary(MultitextureNode);
};

void ReadRecordUVList(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBase UVListBase;
	File->Read(&UVListBase, sizeof(ZEFLTRecordBase), 1);

	ZEFLTResourceNode* UVListNode = ZEFLTResourceNode::CreateInstance();

	if (PreviousOpCode == ZE_FLT_OPCODE_VERTEX_LIST)
	{
		ZEFLTRecordUVList<ZEFLTRecordUVListItemVertex>* UVList = new ZEFLTRecordUVList<ZEFLTRecordUVListItemVertex>();
		UVList->Opcode = UVListBase.Opcode;
		UVList->Length = UVListBase.Length;

		File->Read(&UVList->AttributeMask, sizeof(ZEInt32), 1);

		ZESize NumberOfCoordinates = (CurrentLength - 8) / sizeof(ZEFLTRecordUVListItemVertex);
		UVList->TextureCoordinates.SetCount(NumberOfCoordinates);

		for (ZESize I = 0; I < NumberOfCoordinates; I++)
		{
			File->Read(&UVList->TextureCoordinates[I], sizeof(ZEFLTRecordUVListItemVertex), 1);
		}

		UVListNode->SetRecord(UVList);
	}
	else
	{
		ZEFLTRecordUVList<ZEFLTRecordUVListItemMorph>* UVList = new ZEFLTRecordUVList<ZEFLTRecordUVListItemMorph>();
		UVList->Opcode = UVListBase.Opcode;
		UVList->Length = UVListBase.Length;

		File->Read(&UVList->AttributeMask, sizeof(ZEInt32), 1);

		ZESize NumberOfCoordinates = (CurrentLength - 8) / sizeof(ZEFLTRecordUVListItemMorph);
		UVList->TextureCoordinates.SetCount(NumberOfCoordinates);

		for (ZESize I = 0; I < NumberOfCoordinates; I++)
		{
			File->Read(&UVList->TextureCoordinates[I], sizeof(ZEFLTRecordUVListItemMorph), 1);
		}

		UVListNode->SetRecord(UVList);
	}

	(*Node)->RegisterAncillary(UVListNode);
};

void ReadRecordReplicate(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordReplicate* Replicate = new ZEFLTRecordReplicate();

	File->Read(Replicate, CurrentLength, 1);

	ZEFLTResourceNode* ReplicateNode = ZEFLTResourceNode::CreateInstance();

	ReplicateNode->SetRecord(Replicate);
	(*Node)->RegisterAncillary(ReplicateNode);
};

void ReadRecordMatrix(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordMatrix* Matrix = new ZEFLTRecordMatrix();

	File->Read(Matrix, CurrentLength, 1);

	ZEFLTResourceNode* MatrixNode = ZEFLTResourceNode::CreateInstance();

	MatrixNode->SetRecord(Matrix);
	(*Node)->RegisterAncillary(MatrixNode);
};

void ReadRecordRotateAboutEdge(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordRotateAboutEdge* RotateAboutEdge = new ZEFLTRecordRotateAboutEdge();

	File->Read(RotateAboutEdge, CurrentLength, 1);

	ZEFLTResourceNode* RotateAboutEdgeNode = ZEFLTResourceNode::CreateInstance();

	RotateAboutEdgeNode->SetRecord(RotateAboutEdge);
	(*Node)->RegisterAncillary(RotateAboutEdgeNode);
};

void ReadRecordTranslate(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordRotateAboutEdge* RotateAboutEdge = new ZEFLTRecordRotateAboutEdge();

	File->Read(RotateAboutEdge, CurrentLength, 1);

	ZEFLTResourceNode* RotateAboutEdgeNode = ZEFLTResourceNode::CreateInstance();

	RotateAboutEdgeNode->SetRecord(RotateAboutEdge);
	(*Node)->RegisterAncillary(RotateAboutEdgeNode);
};

void ReadRecordScale(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordScale* Scale = new ZEFLTRecordScale();

	File->Read(Scale, CurrentLength, 1);

	ZEFLTResourceNode* ScaleNode = ZEFLTResourceNode::CreateInstance();

	ScaleNode->SetRecord(Scale);
	(*Node)->RegisterAncillary(ScaleNode);
};

void ReadRecordRotateAboutPoint(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordRotateAboutPoint* RotateAboutPoint = new ZEFLTRecordRotateAboutPoint();

	File->Read(RotateAboutPoint, CurrentLength, 1);

	ZEFLTResourceNode* RotateAboutPointNode = ZEFLTResourceNode::CreateInstance();

	RotateAboutPointNode->SetRecord(RotateAboutPoint);
	(*Node)->RegisterAncillary(RotateAboutPointNode);
};

void ReadRecordRotateScaleToPoint(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordRotateScaleToPoint* RotateScaleToPoint = new ZEFLTRecordRotateScaleToPoint();

	File->Read(RotateScaleToPoint, CurrentLength, 1);

	ZEFLTResourceNode* RotateScaleToPointNode = ZEFLTResourceNode::CreateInstance();

	RotateScaleToPointNode->SetRecord(RotateScaleToPoint);
	(*Node)->RegisterAncillary(RotateScaleToPointNode);
};

void ReadRecordPut(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPut* Put = new ZEFLTRecordPut();

	File->Read(Put, CurrentLength, 1);

	ZEFLTResourceNode* PutNode = ZEFLTResourceNode::CreateInstance();

	PutNode->SetRecord(Put);
	(*Node)->RegisterAncillary(PutNode);
};

void ReadRecordGeneralMatrix(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordGeneralMatrix* GeneralMatrix = new ZEFLTRecordGeneralMatrix();

	File->Read(GeneralMatrix, CurrentLength, 1);

	ZEFLTResourceNode* GeneralMatrixNode = ZEFLTResourceNode::CreateInstance();

	GeneralMatrixNode->SetRecord(GeneralMatrix);
	(*Node)->RegisterAncillary(GeneralMatrixNode);
};

void ReadRecordVector(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordVector* RecordVector = new ZEFLTRecordVector();

	File->Read(RecordVector, CurrentLength, 1);

	ZEFLTResourceNode* RecordVectorNode = ZEFLTResourceNode::CreateInstance();

	RecordVectorNode->SetRecord(RecordVector);
	(*Node)->RegisterAncillary(RecordVectorNode);
};

void ReadRecordBoundingBox(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBoundingBox* BoundingBox = new ZEFLTRecordBoundingBox();

	File->Read(BoundingBox, CurrentLength, 1);

	ZEFLTResourceNode* BoundingBoxNode = ZEFLTResourceNode::CreateInstance();

	BoundingBoxNode->SetRecord(BoundingBox);
	(*Node)->RegisterAncillary(BoundingBoxNode);
};

void ReadRecordBoundingSphere(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBoundingSphere* BoundingSphere = new ZEFLTRecordBoundingSphere();

	File->Read(BoundingSphere, CurrentLength, 1);

	ZEFLTResourceNode* BoundingSphereNode = ZEFLTResourceNode::CreateInstance();

	BoundingSphereNode->SetRecord(BoundingSphere);
	(*Node)->RegisterAncillary(BoundingSphereNode);
};

void ReadRecordBoundingCylinder(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBoundingCylinder* BoundingCylinder = new ZEFLTRecordBoundingCylinder();

	File->Read(BoundingCylinder, CurrentLength, 1);

	ZEFLTResourceNode* BoundingCylinderNode = ZEFLTResourceNode::CreateInstance();

	BoundingCylinderNode->SetRecord(BoundingCylinder);
	(*Node)->RegisterAncillary(BoundingCylinderNode);
};

void ReadRecordBoundingConvexHull(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBoundingConvexHull* ConvexHull = new ZEFLTRecordBoundingConvexHull();

	File->Read(ConvexHull, sizeof(ZEFLTRecordBase) + 4, 1);
	ConvexHull->Triangles.SetCount(ConvexHull->NumberOfTriangles);

	for(ZESize I = 0; I < ConvexHull->NumberOfTriangles; I++)
		File->Read(&ConvexHull->Triangles[I], sizeof(ZEFLTRecordBoundingConvexHullTriangles), 1);

	ZEFLTResourceNode* ConvexHullNode = ZEFLTResourceNode::CreateInstance();

	ConvexHullNode->SetRecord(ConvexHull);
	(*Node)->RegisterAncillary(ConvexHullNode);
};

void ReadRecordBoundingVolumeCenter(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBoundingVolumeCenter* VolumeCenter = new ZEFLTRecordBoundingVolumeCenter();

	File->Read(VolumeCenter, CurrentLength, 1);

	ZEFLTResourceNode* VolumeCenterNode = ZEFLTResourceNode::CreateInstance();

	VolumeCenterNode->SetRecord(VolumeCenter);
	(*Node)->RegisterAncillary(VolumeCenterNode);
};

void ReadRecordBoundingVolumeOrientation(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBoundingVolumeOrientation* VolumeOrientation = new ZEFLTRecordBoundingVolumeOrientation();

	File->Read(VolumeOrientation, CurrentLength, 1);

	ZEFLTResourceNode* VolumeOrientationNode = ZEFLTResourceNode::CreateInstance();

	VolumeOrientationNode->SetRecord(VolumeOrientation);
	(*Node)->RegisterAncillary(VolumeOrientationNode);
};

void ReadRecordExtensionFieldBoolean(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtensionFieldBoolean* ExtensionFieldBoolean = new ZEFLTRecordExtensionFieldBoolean();

	File->Read(ExtensionFieldBoolean, CurrentLength, 1);

	ZEFLTResourceNode* ExtensionFieldBooleanNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldBooleanNode->SetRecord(ExtensionFieldBoolean);
	(*Node)->RegisterAncillary(ExtensionFieldBooleanNode);
};

void ReadRecordExtensionFieldInteger(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtensionFieldInteger* ExtensionFieldInteger = new ZEFLTRecordExtensionFieldInteger();

	File->Read(ExtensionFieldInteger, CurrentLength, 1);

	ZEFLTResourceNode* ExtensionFieldIntegerNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldIntegerNode->SetRecord(ExtensionFieldInteger);
	(*Node)->RegisterAncillary(ExtensionFieldIntegerNode);
};

void ReadRecordExtensionFieldFloat(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtensionFieldFloat* ExtensionFieldFloat = new ZEFLTRecordExtensionFieldFloat();

	File->Read(ExtensionFieldFloat, CurrentLength, 1);

	ZEFLTResourceNode* ExtensionFieldFloatNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldFloatNode->SetRecord(ExtensionFieldFloat);
	(*Node)->RegisterAncillary(ExtensionFieldFloatNode);
};

void ReadRecordExtensionFieldDouble(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtensionFieldDouble* ExtensionFieldDouble = new ZEFLTRecordExtensionFieldDouble();

	File->Read(ExtensionFieldDouble, CurrentLength, 1);

	ZEFLTResourceNode* ExtensionFieldDoubleNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldDoubleNode->SetRecord(ExtensionFieldDouble);
	(*Node)->RegisterAncillary(ExtensionFieldDoubleNode);
};

void ReadRecordExtensionFieldString(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtensionFieldString* ExtensionFieldString = new ZEFLTRecordExtensionFieldString();

	File->Read(ExtensionFieldString, CurrentLength, 1);

	ZEFLTResourceNode* ExtensionFieldStringNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldStringNode->SetRecord(ExtensionFieldString);
	(*Node)->RegisterAncillary(ExtensionFieldStringNode);
};

void ReadRecordExtensionFieldXML(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtensionFieldXML* ExtensionFieldXML = new ZEFLTRecordExtensionFieldXML();

	File->Read(ExtensionFieldXML, CurrentLength, 1);

	ZEFLTResourceNode* ExtensionFieldXMLNode = ZEFLTResourceNode::CreateInstance();

	ExtensionFieldXMLNode->SetRecord(ExtensionFieldXML);
	(*Node)->RegisterAncillary(ExtensionFieldXMLNode);
};

void ReadRecordContinuation(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBase ContinuationBase;
	File->Read(&ContinuationBase, sizeof(ZEFLTRecordBase), 1);

	if (PreviousOpCode == ZE_FLT_OPCODE_LOCAL_VERTEX_POOL)
	{
		ZEFLTRecordContinuation<ZEFLTRecordLocalVertexPoolVertex>* Continuation = new ZEFLTRecordContinuation<ZEFLTRecordLocalVertexPoolVertex>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZESize NumberOfVertices = (Continuation->Length - 4) / sizeof(ZEFLTRecordLocalVertexPoolVertex);
		Continuation->Data.SetCount(NumberOfVertices);

		ZEFLTRecordLocalVertexPoolVertex Vertex;

		for (ZESize I = 0; I < NumberOfVertices; I++)
		{
			File->Read(&Vertex, sizeof(ZEFLTRecordLocalVertexPoolVertex), 1);
			Continuation->Data[I] = Vertex;
		};

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);
	}
	else if (PreviousOpCode == ZE_FLT_OPCODE_MESH_PRIMITIVE)
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
			File->Read(&Continuation->Data[I], IndexSize, 1);
		};

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);
	}
	else if (PreviousOpCode == ZE_FLT_OPCODE_VERTEX_LIST)
	{
		ZEFLTRecordContinuation<ZEBigEndian<ZEInt32>>* Continuation = new ZEFLTRecordContinuation<ZEBigEndian<ZEInt32>>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZESize NumberOfVertices = (Continuation->Length - 4) / sizeof(ZEInt32);
		Continuation->Data.SetCount(NumberOfVertices);

		for (ZESize I = 0; I < NumberOfVertices; I++)
		{
			File->Read(&Continuation->Data[I], sizeof(ZEInt32), 1);
		};

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);

	}
	else if (PreviousOpCode == ZE_FLT_OPCODE_MORPH_VERTEX_LIST)
	{
		ZEFLTRecordContinuation<ZEFLTRecordMorphListVertex>* Continuation = new ZEFLTRecordContinuation<ZEFLTRecordMorphListVertex>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZESize NumberOfVertices = (Continuation->Length - 4) / sizeof(ZEFLTRecordMorphListVertex);
		Continuation->Data.SetCount(NumberOfVertices);

		for (ZESize I = 0; I < NumberOfVertices; I++)
		{
			File->Read(&Continuation->Data[I], sizeof(ZEFLTRecordMorphListVertex), 1);
		};

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);
	}
	else if (PreviousOpCode == ZE_FLT_OPCODE_EXTENSION)
	{
		File->Seek(ContinuationBase.Length - 4, ZE_SF_CURRENT);
	}
	else if (PreviousOpCode == ZE_FLT_OPCODE_BOUNDING_CONVEX_HULL)
	{
		ZEFLTRecordContinuation<ZEFLTRecordBoundingConvexHullTriangles>* Continuation = new ZEFLTRecordContinuation<ZEFLTRecordBoundingConvexHullTriangles>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZESize NumberOfTriangles = (Continuation->Length - 4) / sizeof(ZEFLTRecordBoundingConvexHullTriangles);
		Continuation->Data.SetCount(NumberOfTriangles);

		for (ZESize I = 0; I < NumberOfTriangles; I++)
		{
			File->Read(&Continuation->Data[I], sizeof(ZEFLTRecordBoundingConvexHullTriangles), 1);
		};

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);
	}
	else if (PreviousOpCode == ZE_FLT_OPCODE_NAME_TABLE)
	{
		ZEFLTRecordContinuation<ZEFLTRecordNameTableEntry>* Continuation = new ZEFLTRecordContinuation<ZEFLTRecordNameTableEntry>();
		Continuation->Opcode = ContinuationBase.Opcode;
		Continuation->Length = ContinuationBase.Length;

		ZESize NumberOfEntries= (Continuation->Length - 4) / sizeof(ZEFLTRecordNameTableEntry);
		Continuation->Data.SetCount(NumberOfEntries);

		for (ZESize I = 0; I < NumberOfEntries; I++)
		{
			File->Read(&Continuation->Data[I].Length, sizeof(ZEInt32), 1);
			File->Read(&Continuation->Data[I].NameIndex, sizeof(ZEUInt16), 1);
			File->Read(&Continuation->Data[I].NameString, Continuation->Data[I].Length - 6, 1);
		};

		ZEFLTResourceNode* ContinuationNode = ZEFLTResourceNode::CreateInstance();
		ContinuationNode->SetRecord(Continuation);
		(*Node)->RegisterAncillary(ContinuationNode);

	}
	else
	{

		File->Seek(ContinuationBase.Length - 4, ZE_SF_CURRENT);

	}

};

void ReadRecordVertexPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPaletteVertex* VertexPalette = new ZEFLTRecordPaletteVertex();

	File->Read(VertexPalette, CurrentLength, 1);

	ZEFLTResourceNode* VertexPaletteNode = ZEFLTResourceNode::CreateInstance();

	VertexPaletteNode->SetRecord(VertexPalette);
	(*Node)->RegisterAncillary(VertexPaletteNode);
};

void ReadRecordVertexWithColor(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordVertexWithColor* VertexWithColor = new ZEFLTRecordVertexWithColor();

	File->Read(VertexWithColor, CurrentLength, 1);

	ZEFLTResourceNode* VertexWithColorNode = ZEFLTResourceNode::CreateInstance();

	VertexWithColorNode->SetRecord(VertexWithColor);
	(*Node)->RegisterAncillary(VertexWithColorNode);
};

void ReadRecordVertexWithColorNormal(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordVertexWithColorNormal* VertexWithColorNormal = new ZEFLTRecordVertexWithColorNormal();

	File->Read(VertexWithColorNormal, CurrentLength, 1);

	ZEFLTResourceNode* VertexWithColorNormalNode = ZEFLTResourceNode::CreateInstance();

	VertexWithColorNormalNode->SetRecord(VertexWithColorNormal);
	(*Node)->RegisterAncillary(VertexWithColorNormalNode);
};

void ReadRecordVertexWithColorUV(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordVertexWithColorUV* VertexWithColorUV = new ZEFLTRecordVertexWithColorUV();

	File->Read(VertexWithColorUV, CurrentLength, 1);

	ZEFLTResourceNode* VertexWithColorUVNode = ZEFLTResourceNode::CreateInstance();

	VertexWithColorUVNode->SetRecord(VertexWithColorUV);
	(*Node)->RegisterAncillary(VertexWithColorUVNode);
};

void ReadRecordVertexWithColorNormalUV(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordVertexWithColorNormalUV* VertexWithColorNormalUV = new ZEFLTRecordVertexWithColorNormalUV();

	File->Read(VertexWithColorNormalUV, CurrentLength, 1);

	ZEFLTResourceNode* VertexWithColorNormalUVNode = ZEFLTResourceNode::CreateInstance();

	VertexWithColorNormalUVNode->SetRecord(VertexWithColorNormalUV);
	(*Node)->RegisterAncillary(VertexWithColorNormalUVNode);
};

void ReadRecordColorPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPaletteColor* ColorPalette = new ZEFLTRecordPaletteColor();

	File->Read(ColorPalette, sizeof(ZEInt16) + sizeof(ZEUInt16) + 128 * sizeof(char) + 1024 * sizeof(ZEInt32), 1);

	if (ColorPalette->Length > 4228)
	{
		File->Read(&ColorPalette->NumberofColorNames, sizeof(ZEInt32), 1);
		ColorPalette->ColorNames.SetCount(ColorPalette->NumberofColorNames);

		for (ZESize I = 0; I < ColorPalette->NumberofColorNames; I++)
		{
			File->Read(&ColorPalette->ColorNames[I], 4 * sizeof(ZEInt16), 1);
			File->Read(&ColorPalette->ColorNames[I].ColorName, ColorPalette->ColorNames[I].Length - (4 * sizeof(ZEInt16)), 1);
		}
	}
	else
	{
		ColorPalette->NumberofColorNames = 0;
		ColorPalette->ColorNames.Clear();
	}

	((ZEFLTResourceNodeHeader*)(*Node))->SetColorPalette(ColorPalette);

};

void ReadRecordNameTable(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordNameTable* NameTable = new ZEFLTRecordNameTable();

	File->Read(NameTable, 10, 1);

	NameTable->Names.SetCount(NameTable->NumberOfNames);

	for (ZESize I = 0; I < NameTable->NumberOfNames; I++)
	{
		File->Read(&NameTable->Names[I], sizeof(ZEFLTRecordNameTableEntry), 1);
	};

	ZEFLTResourceNode* NameTableNode = ZEFLTResourceNode::CreateInstance();

	NameTableNode->SetRecord(NameTable);
	(*Node)->RegisterAncillary(NameTableNode);
};

void ReadRecordMaterialPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPaletteMaterial* MaterialPalette = new ZEFLTRecordPaletteMaterial();

	File->Read(MaterialPalette, CurrentLength, 1);

	((ZEFLTResourceNodeHeader*)(*Node))->RegisterStandardMaterial(MaterialPalette);

};

void ReadRecordExtendedMaterialHeader(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialHeader* ExtendedMaterialHeader = new ZEFLTRecordExtendedMaterialHeader();

	File->Read(ExtendedMaterialHeader, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialHeaderNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialHeaderNode->SetRecord(ExtendedMaterialHeader);
	(*Node)->RegisterAncillary(ExtendedMaterialHeaderNode);
};

void ReadRecordExtendedMaterialAmbient(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialAmbient* ExtendedMaterialAmbient = new ZEFLTRecordExtendedMaterialAmbient();

	File->Read(ExtendedMaterialAmbient, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialAmbientNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialAmbientNode->SetRecord(ExtendedMaterialAmbient);
	(*Node)->RegisterAncillary(ExtendedMaterialAmbientNode);
};

void ReadRecordExtendedMaterialDiffuse(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialDiffuse* ExtendedMaterialDiffuse = new ZEFLTRecordExtendedMaterialDiffuse();

	File->Read(ExtendedMaterialDiffuse, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialDiffuseNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialDiffuseNode->SetRecord(ExtendedMaterialDiffuse);
	(*Node)->RegisterAncillary(ExtendedMaterialDiffuseNode);
};

void ReadRecordExtendedMaterialSpecular(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialSpecular* ExtendedMaterialSpecular = new ZEFLTRecordExtendedMaterialSpecular();

	File->Read(ExtendedMaterialSpecular, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialSpecularNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialSpecularNode->SetRecord(ExtendedMaterialSpecular);
	(*Node)->RegisterAncillary(ExtendedMaterialSpecularNode);
};

void ReadRecordExtendedMaterialEmissive(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialEmissive* ExtendedMaterialEmissive = new ZEFLTRecordExtendedMaterialEmissive();

	File->Read(ExtendedMaterialEmissive, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialEmissiveNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialEmissiveNode->SetRecord(ExtendedMaterialEmissive);
	(*Node)->RegisterAncillary(ExtendedMaterialEmissiveNode);
};

void ReadRecordExtendedMaterialAlpha(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialAlpha* ExtendedMaterialAlpha = new ZEFLTRecordExtendedMaterialAlpha();

	File->Read(ExtendedMaterialAlpha, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialAlphaNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialAlphaNode->SetRecord(ExtendedMaterialAlpha);
	(*Node)->RegisterAncillary(ExtendedMaterialAlphaNode);
};

void ReadRecordExtendedMaterialLightMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialLightMap* ExtendedMaterialLightMap = new ZEFLTRecordExtendedMaterialLightMap();

	File->Read(ExtendedMaterialLightMap, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialLightMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialLightMapNode->SetRecord(ExtendedMaterialLightMap);
	(*Node)->RegisterAncillary(ExtendedMaterialLightMapNode);
};

void ReadRecordExtendedMaterialNormalMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialNormalMap* ExtendedMaterialNormalMap = new ZEFLTRecordExtendedMaterialNormalMap();

	File->Read(ExtendedMaterialNormalMap, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialNormalMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialNormalMapNode->SetRecord(ExtendedMaterialNormalMap);
	(*Node)->RegisterAncillary(ExtendedMaterialNormalMapNode);
};

void ReadRecordExtendedMaterialBumpMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialBumpMap* ExtendedMaterialBumpMap = new ZEFLTRecordExtendedMaterialBumpMap();

	File->Read(ExtendedMaterialBumpMap, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialBumpMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialBumpMapNode->SetRecord(ExtendedMaterialBumpMap);
	(*Node)->RegisterAncillary(ExtendedMaterialBumpMapNode);
};

void ReadRecordExtendedMaterialShadowMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialShadowMap* ExtendedMaterialShadowMap = new ZEFLTRecordExtendedMaterialShadowMap();

	File->Read(ExtendedMaterialShadowMap, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialShadowMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialShadowMapNode->SetRecord(ExtendedMaterialShadowMap);
	(*Node)->RegisterAncillary(ExtendedMaterialShadowMapNode);
};

void ReadRecordExtendedMaterialReflectionMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordExtendedMaterialReflectionMap* ExtendedMaterialReflectionMap = new ZEFLTRecordExtendedMaterialReflectionMap();

	File->Read(ExtendedMaterialReflectionMap, CurrentLength, 1);

	ZEFLTResourceNode* ExtendedMaterialReflectionMapNode = ZEFLTResourceNode::CreateInstance();

	ExtendedMaterialReflectionMapNode->SetRecord(ExtendedMaterialReflectionMap);
	(*Node)->RegisterAncillary(ExtendedMaterialReflectionMapNode);
};

void ReadRecordTexturePalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPaletteTexture* TexturePalette = new ZEFLTRecordPaletteTexture();

	File->Read(TexturePalette, CurrentLength, 1);

	((ZEFLTResourceNodeHeader*)(*Node))->RegisterTexture(TexturePalette);

};

void ReadRecordEyepointTrackPlanePalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPaletteEyePointsAndTrackPlanes* EyePointsAndTrackPlanesPalette = new ZEFLTRecordPaletteEyePointsAndTrackPlanes();

	File->Read(EyePointsAndTrackPlanesPalette, sizeof(ZEFLTRecordBase) + sizeof(ZEInt32), 1);

	EyePointsAndTrackPlanesPalette->EyePoints.SetCount(10);
	EyePointsAndTrackPlanesPalette->TrackPlanes.SetCount(10);

	for (ZESize I = 0; I < 10; I++)
		File->Read(&EyePointsAndTrackPlanesPalette->EyePoints[I], sizeof(ZEFLTRecordEyePoint), 1);

	for (ZESize I = 0; I < 10; I++)
		File->Read(&EyePointsAndTrackPlanesPalette->TrackPlanes[I], sizeof(ZEFLTRecordTrackPlane), 1);

	ZEFLTResourceNode* EyePointsAndTrackPlanesPaletteNode = ZEFLTResourceNode::CreateInstance();

	EyePointsAndTrackPlanesPaletteNode->SetRecord(EyePointsAndTrackPlanesPalette);
	(*Node)->RegisterAncillary(EyePointsAndTrackPlanesPaletteNode);
};

void ReadRecordSoundPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordBase SoundPaletteBase;
	ZEBigEndian<ZEInt32> Subtype;

	File->Read(&SoundPaletteBase, sizeof(ZEFLTRecordBase), 1);
	File->Read(&Subtype, sizeof(ZEInt32), 1);

	ZEFLTResourceNode* SoundPaletteNode = ZEFLTResourceNode::CreateInstance();

	if (Subtype == 1) //Signifies Header
	{
		ZEFLTRecordPaletteSoundHeader* SoundPaletteHeader = new ZEFLTRecordPaletteSoundHeader();
		SoundPaletteHeader->Opcode = SoundPaletteBase.Opcode;
		SoundPaletteHeader->Length = SoundPaletteBase.Length;
		SoundPaletteHeader->Subtype = Subtype;

		File->Read(&SoundPaletteHeader->MaxNumber, sizeof(ZEInt32), 1);
		File->Read(&SoundPaletteHeader->ActualNumber, sizeof(ZEInt32), 1);
		File->Read(&SoundPaletteHeader->TotalLength, sizeof(ZEInt32), 1);
		File->Read(&SoundPaletteHeader->Reserved, 3 * sizeof(ZEInt32), 1);

		SoundPaletteHeader->Indices.SetCount(SoundPaletteHeader->ActualNumber);

		for (ZESize I = 0; I < SoundPaletteHeader->ActualNumber; I++)
			File->Read(&SoundPaletteHeader->Indices[I], sizeof(ZEFLTRecordPaletteSoundIndex), 1);

		SoundPaletteNode->SetRecord(SoundPaletteHeader);
	}
	else //Subtype 2 - Signifies Data
	{
		ZEFLTRecordPaletteSoundData* SoundPaletteData = new ZEFLTRecordPaletteSoundData();
		SoundPaletteData->Opcode = SoundPaletteBase.Opcode;
		SoundPaletteData->Length = SoundPaletteBase.Length;
		SoundPaletteData->Subtype = Subtype;

		File->Read(&SoundPaletteData->TotalLength, sizeof(ZEInt32), 1);
		SoundPaletteData->PackedFileNames.SetCount(SoundPaletteData->Length - (sizeof(ZEFLTRecordBase) + 2 * sizeof(ZEInt32)));
		File->Read(SoundPaletteData->PackedFileNames.GetCArray(), 1, SoundPaletteData->Length - (sizeof(ZEFLTRecordBase) + 2 * sizeof(ZEInt32)));

		SoundPaletteNode->SetRecord(SoundPaletteData);
	}

	(*Node)->RegisterAncillary(SoundPaletteNode);

};

void ReadRecordLightSourcePalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPaletteLightSource* LightSourcePalette = new ZEFLTRecordPaletteLightSource();

	File->Read(LightSourcePalette, CurrentLength, 1);

	ZEFLTResourceNode* LightSourcePaletteNode = ZEFLTResourceNode::CreateInstance();

	LightSourcePaletteNode->SetRecord(LightSourcePalette);
	(*Node)->RegisterAncillary(LightSourcePaletteNode);
};

void ReadRecordLightPointAppearancePalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPaletteLightPointAppearance* LightAppearancePalette = new ZEFLTRecordPaletteLightPointAppearance();

	File->Read(LightAppearancePalette, CurrentLength, 1);

	ZEFLTResourceNode* LightAppearancePaletteNode = ZEFLTResourceNode::CreateInstance();

	LightAppearancePaletteNode->SetRecord(LightAppearancePalette);
	(*Node)->RegisterAncillary(LightAppearancePaletteNode);
};

void ReadRecordLightPointAnimationPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPaletteLightPointAnimation* LightAnimationPalette = new ZEFLTRecordPaletteLightPointAnimation();

	File->Read(LightAnimationPalette, 1340, 1);

	LightAnimationPalette->Sequences.SetCount(LightAnimationPalette->NumberOfSequences);

	for (ZESize I = 0; I < LightAnimationPalette->NumberOfSequences; I++)
		File->Read(&LightAnimationPalette->Sequences[I], sizeof(ZEFLTRecordPaletteLightPointAnimationSequence), 1);

	ZEFLTResourceNode* LightAnimationPaletteNode = ZEFLTResourceNode::CreateInstance();

	LightAnimationPaletteNode->SetRecord(LightAnimationPalette);
	(*Node)->RegisterAncillary(LightAnimationPaletteNode);
};

void ReadRecordTextureMappingPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	ZEFLTRecordPaletteTextureMapping* TextureMappingPalette = new ZEFLTRecordPaletteTextureMapping();

	File->Read(TextureMappingPalette, 168, 1);

	if (TextureMappingPalette->Type == 1)
	{
		ZEFLTRecordPaletteTextureMapping3PointPut* ThreePointPutParameters = new ZEFLTRecordPaletteTextureMapping3PointPut();
		File->Read(ThreePointPutParameters, sizeof(ZEFLTRecordPaletteTextureMapping3PointPut), 1);
		TextureMappingPalette->Parameters = ThreePointPutParameters;
	}
	else if (TextureMappingPalette->Type == 2)
	{
		ZEFLTRecordPaletteTextureMapping4PointPut* FourPointPutParameters = new ZEFLTRecordPaletteTextureMapping4PointPut();
		File->Read(FourPointPutParameters, sizeof(ZEFLTRecordPaletteTextureMapping4PointPut), 1);
		TextureMappingPalette->Parameters = FourPointPutParameters;
	}
	else if (TextureMappingPalette->Type == 4)
	{
		ZEFLTRecordPaletteTextureMappingSphericalProject* SphericalParameters = new ZEFLTRecordPaletteTextureMappingSphericalProject();
		File->Read(SphericalParameters, sizeof(ZEFLTRecordPaletteTextureMappingSphericalProject), 1);
		TextureMappingPalette->Parameters = SphericalParameters;
	}
	else if (TextureMappingPalette->Type == 5)
	{
		ZEFLTRecordPaletteTextureMappingRadialProject* RadialParameters = new ZEFLTRecordPaletteTextureMappingRadialProject();
		File->Read(RadialParameters, sizeof(ZEFLTRecordPaletteTextureMappingRadialProject), 1);
		TextureMappingPalette->Parameters = RadialParameters;
	}
	else
	{
		TextureMappingPalette->Parameters = NULL;
	}

	if (TextureMappingPalette->WarpedFlag == 1)
	{
		ZEFLTRecordPaletteTextureMappingWarped* WarpedParameters = new ZEFLTRecordPaletteTextureMappingWarped();
		File->Read(WarpedParameters, sizeof(ZEFLTRecordPaletteTextureMappingWarped), 1);
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

void ReadRecordPushLevel(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	File->Seek(sizeof(ZEFLTRecordBase), ZE_SF_CURRENT);

	ZEFLTResourceNode* Child = ZEFLTResourceNode::CreateInstance();

	(*Node)->RegisterChild(Child);

	*Node = Child;
};

void ReadRecordPopLevel(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{
	File->Seek(sizeof(ZEFLTRecordBase), ZE_SF_CURRENT);

	*Node = (*Node)->GetParent();
};

void ReadRecordPushSubface(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{

};

void ReadRecordPopSubface(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{

};

void ReadRecordPushExtension(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{

};

void ReadRecordPopExtension(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{

};

void ReadRecordInstanceReference(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{

};

void ReadRecordInstanceDefinition(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{

};

void ReadRecordPushAttribute(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{

};

void ReadRecordPopAttribute(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength)
{

};
