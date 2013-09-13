//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFLTReader.cpp
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

#include "ZEFLTReader.h"
#include "ZEFLTHandlerCallbacks.h"
#include "ZEFLTResourceNode.h"

const char* ZEFLTReader::GetRecordName(ZEUInt8 OpCode)
{
	return Handlers[OpCode].RecordName;
};

ZEFLTRecordType ZEFLTReader::GetRecordType(ZEUInt8 OpCode)
{
	return Handlers[OpCode].RecordType;
};

bool ZEFLTReader::ReadRecord()
{
	PreviousRecordIdentifier.Opcode = CurrentRecordIdentifier.Opcode;
	PreviousRecordIdentifier.Size = CurrentRecordIdentifier.Size;

	if (ResourceFile->Read(&CurrentRecordIdentifier, sizeof(ZEFLTRecordIdentifier), 1) == 1)
	{
		if (Handlers[CurrentRecordIdentifier.Opcode].RecordType == ZE_FLT_RECORD_UNUSED)
		{
			zeLog("Unsupported FLT record. Opcode: %d", (ZEInt16)CurrentRecordIdentifier.Opcode);
			SkipRecord();
		}
		else
		{
			ResourceFile->Seek(-sizeof(ZEFLTRecordIdentifier), ZE_SF_CURRENT);
			Handlers[CurrentRecordIdentifier.Opcode].Callback(ResourceFile, &CurrentNode, PreviousRecordIdentifier.Opcode, PreviousRecordIdentifier.Size, CurrentRecordIdentifier.Opcode, CurrentRecordIdentifier.Size);
		}
	}
	else
	{
		if(!ResourceFile->Eof())
		{
			zeError("Can not read FLT file.");
			return false;
		}
	}

	return true;
};

void ZEFLTReader::SkipRecord()
{
	ZESize RemainingRecordSize = CurrentRecordIdentifier.Size - sizeof(ZEFLTRecordIdentifier);
	ResourceFile->Seek(RemainingRecordSize, ZE_SF_CURRENT);
};

ZEFLTResourceNode* ZEFLTReader::GetRootNode()
{
	return RootNode;
};

ZEFLTResourceNode* ZEFLTReader::GetCurrentNode()
{
	return CurrentNode;
};

ZEFLTReader::ZEFLTReader(ZEFile* File)
{
	ResourceFile = File;

	CursorPointer = 0;

	memset(&CurrentRecordIdentifier, 0, sizeof(ZEFLTRecordIdentifier));
	memset(&PreviousRecordIdentifier, 0, sizeof(ZEFLTRecordIdentifier));

	RootNode = ZEFLTResourceNodeHeader::CreateInstance();
	CurrentNode = RootNode;

	RegisterHandlers();

};

ZEFLTReader::~ZEFLTReader()
{

};

void ZEFLTReader::RegisterHandlers()
{
	Handlers.SetCount(256);

	//Handlers.Fill(NULL);

	for(ZESize I = 0; I < Handlers.GetCount(); I++)
	{
		Handlers[I].RecordName = NULL;
		Handlers[I].RecordType = ZE_FLT_RECORD_UNUSED;
		Handlers[I].Callback.Clear();
	}

	Handlers[ZE_FLT_OPCODE_HEADER].RecordName								= "Header";
	Handlers[ZE_FLT_OPCODE_HEADER].RecordType								= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_HEADER].Callback									= ZEFLTRecordCallback::Create<&ReadRecordHeader>();
	Handlers[ZE_FLT_OPCODE_GROUP].RecordName								= "Group";
	Handlers[ZE_FLT_OPCODE_GROUP].RecordType								= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_GROUP].Callback									= ZEFLTRecordCallback::Create<&ReadRecordGroup>();
	Handlers[ZE_FLT_OPCODE_OBJECT].RecordName								= "Object";
	Handlers[ZE_FLT_OPCODE_OBJECT].RecordType								= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_OBJECT].Callback									= ZEFLTRecordCallback::Create<&ReadRecordObject>();
	Handlers[ZE_FLT_OPCODE_FACE].RecordName									= "Face";
	Handlers[ZE_FLT_OPCODE_FACE].RecordType									= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_FACE].Callback									= ZEFLTRecordCallback::Create<&ReadRecordFace>();
	Handlers[ZE_FLT_OPCODE_PUSH_LEVEL].RecordName							= "Push Level";
	Handlers[ZE_FLT_OPCODE_PUSH_LEVEL].RecordType							= ZE_FLT_RECORD_CONTROL;
	Handlers[ZE_FLT_OPCODE_PUSH_LEVEL].Callback								= ZEFLTRecordCallback::Create<&ReadRecordPushLevel>();
	Handlers[ZE_FLT_OPCODE_POP_LEVEL].RecordName							= "Pop Level";
	Handlers[ZE_FLT_OPCODE_POP_LEVEL].RecordType							= ZE_FLT_RECORD_CONTROL;
	Handlers[ZE_FLT_OPCODE_POP_LEVEL].Callback								= ZEFLTRecordCallback::Create<&ReadRecordPopLevel>();
	Handlers[ZE_FLT_OPCODE_DOF].RecordName									= "Degree of Freedom";
	Handlers[ZE_FLT_OPCODE_DOF].RecordType									= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_DOF].Callback									= ZEFLTRecordCallback::Create<&ReadRecordDegreeOfFreedom>();
	//Handlers[ZE_FLT_OPCODE_PUSH_SUBFACE].RecordName							= "Push Subface";
	//Handlers[ZE_FLT_OPCODE_PUSH_SUBFACE].RecordType							= ZE_FLT_RECORD_CONTROL;
	//Handlers[ZE_FLT_OPCODE_PUSH_SUBFACE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordPushSubface>();
	//Handlers[ZE_FLT_OPCODE_POP_SUBFACE].RecordName							= "Pop Subface";
	//Handlers[ZE_FLT_OPCODE_POP_SUBFACE].RecordType							= ZE_FLT_RECORD_CONTROL;
	//Handlers[ZE_FLT_OPCODE_POP_SUBFACE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordPopSubface>();
	//Handlers[ZE_FLT_OPCODE_PUSH_EXTENSION].RecordName						= "Push Extension";
	//Handlers[ZE_FLT_OPCODE_PUSH_EXTENSION].RecordType						= ZE_FLT_RECORD_CONTROL;
	//Handlers[ZE_FLT_OPCODE_PUSH_EXTENSION].Callback							= ZEFLTRecordCallback::Create<&ReadRecordPushExtension>();
	//Handlers[ZE_FLT_OPCODE_POP_EXTENSION].RecordName						= "Pop Extension";
	//Handlers[ZE_FLT_OPCODE_POP_EXTENSION].RecordType						= ZE_FLT_RECORD_CONTROL;
	//Handlers[ZE_FLT_OPCODE_POP_EXTENSION].Callback							= ZEFLTRecordCallback::Create<&ReadRecordPopExtension>();

	Handlers[ZE_FLT_OPCODE_CONTINUATION].RecordName							= "Continuation";
	Handlers[ZE_FLT_OPCODE_CONTINUATION].RecordType							= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_CONTINUATION].Callback							= ZEFLTRecordCallback::Create<&ReadRecordContinuation>();
	Handlers[ZE_FLT_OPCODE_COLOR_PALETTE].RecordName						= "Color Palette";
	Handlers[ZE_FLT_OPCODE_COLOR_PALETTE].RecordType						= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_COLOR_PALETTE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordColorPalette>();

	Handlers[ZE_FLT_OPCODE_MATRIX].RecordName								= "Matrix";
	Handlers[ZE_FLT_OPCODE_MATRIX].RecordType								= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_MATRIX].Callback									= ZEFLTRecordCallback::Create<&ReadRecordMatrix>();
	Handlers[ZE_FLT_OPCODE_VECTOR].RecordName								= "Vector";
	Handlers[ZE_FLT_OPCODE_VECTOR].RecordType								= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_VECTOR].Callback									= ZEFLTRecordCallback::Create<&ReadRecordVector>();
	Handlers[ZE_FLT_OPCODE_MULTITEXTURE].RecordName							= "Multitexture";
	Handlers[ZE_FLT_OPCODE_MULTITEXTURE].RecordType							= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_MULTITEXTURE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordMultitexture>();
	Handlers[ZE_FLT_OPCODE_UV_LIST].RecordName								= "UV List";
	Handlers[ZE_FLT_OPCODE_UV_LIST].RecordType								= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_UV_LIST].Callback								= ZEFLTRecordCallback::Create<&ReadRecordUVList>();
	Handlers[ZE_FLT_OPCODE_BINARY_SEPARATING_PLANE].RecordName				= "Binary Seperating Plane";
	Handlers[ZE_FLT_OPCODE_BINARY_SEPARATING_PLANE].RecordType				= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_BINARY_SEPARATING_PLANE].Callback				= ZEFLTRecordCallback::Create<&ReadRecordBinarySeparatingPlane>();
	Handlers[ZE_FLT_OPCODE_REPLICATE].RecordName							= "Replicate";
	Handlers[ZE_FLT_OPCODE_REPLICATE].RecordType							= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_REPLICATE].Callback								= ZEFLTRecordCallback::Create<&ReadRecordReplicate>();
	//Handlers[ZE_FLT_OPCODE_INSTANCE_REFERENCE].RecordName					= "Instance Reference";
	//Handlers[ZE_FLT_OPCODE_INSTANCE_REFERENCE].RecordType					= ZE_FLT_RECORD_CONTROL;
	//Handlers[ZE_FLT_OPCODE_INSTANCE_REFERENCE].Callback						= ZEFLTRecordCallback::Create<&ReadRecordInstanceReference>();
	//Handlers[ZE_FLT_OPCODE_INSTANCE_DEFINITION].RecordName					= "Instance Definition";
	//Handlers[ZE_FLT_OPCODE_INSTANCE_DEFINITION].RecordType					= ZE_FLT_RECORD_CONTROL;
	//Handlers[ZE_FLT_OPCODE_INSTANCE_DEFINITION].Callback					= ZEFLTRecordCallback::Create<&ReadRecordInstanceDefinition>();
	Handlers[ZE_FLT_OPCODE_EXTERNAL_REFERENCE].RecordName					= "External Reference";
	Handlers[ZE_FLT_OPCODE_EXTERNAL_REFERENCE].RecordType					= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_EXTERNAL_REFERENCE].Callback						= ZEFLTRecordCallback::Create<&ReadRecordExternalReference>();
	Handlers[ZE_FLT_OPCODE_TEXTURE_PALETTE].RecordName						= "Texture Palette";
	Handlers[ZE_FLT_OPCODE_TEXTURE_PALETTE].RecordType						= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_TEXTURE_PALETTE].Callback						= ZEFLTRecordCallback::Create<&ReadRecordTexturePalette>();
	Handlers[ZE_FLT_OPCODE_VERTEX_PALETTE].RecordName						= "Vertex Palette";
	Handlers[ZE_FLT_OPCODE_VERTEX_PALETTE].RecordType						= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_VERTEX_PALETTE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordVertexPalette>();

	Handlers[ZE_FLT_OPCODE_VERTEX_C].RecordName								= "Vertex with Color";
	Handlers[ZE_FLT_OPCODE_VERTEX_C].RecordType								= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_VERTEX_C].Callback								= ZEFLTRecordCallback::Create<&ReadRecordVertexWithColor>();
	Handlers[ZE_FLT_OPCODE_VERTEX_CN].RecordName							= "Vertex with Color and Normal";
	Handlers[ZE_FLT_OPCODE_VERTEX_CN].RecordType							= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_VERTEX_CN].Callback								= ZEFLTRecordCallback::Create<&ReadRecordVertexWithColorNormal>();
	Handlers[ZE_FLT_OPCODE_VERTEX_CNT].RecordName							= "Vertex with Color, Normal and UV";
	Handlers[ZE_FLT_OPCODE_VERTEX_CNT].RecordType							= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_VERTEX_CNT].Callback								= ZEFLTRecordCallback::Create<&ReadRecordVertexWithColorNormalUV>();
	Handlers[ZE_FLT_OPCODE_VERTEX_CT].RecordName							= "Vertex with Color and UV";
	Handlers[ZE_FLT_OPCODE_VERTEX_CT].RecordType							= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_VERTEX_CT].Callback								= ZEFLTRecordCallback::Create<&ReadRecordVertexWithColorUV>();
	Handlers[ZE_FLT_OPCODE_VERTEX_LIST].RecordName							= "Vertex List";
	Handlers[ZE_FLT_OPCODE_VERTEX_LIST].RecordType							= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_VERTEX_LIST].Callback							= ZEFLTRecordCallback::Create<&ReadRecordVertexList>();
	Handlers[ZE_FLT_OPCODE_LOD].RecordName									= "Level of Detail";
	Handlers[ZE_FLT_OPCODE_LOD].RecordType									= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_LOD].Callback									= ZEFLTRecordCallback::Create<&ReadRecordLevelOfDetail>();
	Handlers[ZE_FLT_OPCODE_BOUNDING_BOX].RecordName							= "Bounding Box";
	Handlers[ZE_FLT_OPCODE_BOUNDING_BOX].RecordType							= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_BOUNDING_BOX].Callback							= ZEFLTRecordCallback::Create<&ReadRecordBoundingBox>();
	Handlers[ZE_FLT_OPCODE_ROTATE_ABOUT_EDGE].RecordName					= "Rotate About Edge";
	Handlers[ZE_FLT_OPCODE_ROTATE_ABOUT_EDGE].RecordType					= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_ROTATE_ABOUT_EDGE].Callback						= ZEFLTRecordCallback::Create<&ReadRecordRotateAboutEdge>();
	Handlers[ZE_FLT_OPCODE_TRANSLATE].RecordName							= "Translate";
	Handlers[ZE_FLT_OPCODE_TRANSLATE].RecordType							= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_TRANSLATE].Callback								= ZEFLTRecordCallback::Create<&ReadRecordTranslate>();
	Handlers[ZE_FLT_OPCODE_SCALE].RecordName								= "Scale";
	Handlers[ZE_FLT_OPCODE_SCALE].RecordType								= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_SCALE].Callback									= ZEFLTRecordCallback::Create<&ReadRecordScale>();
	Handlers[ZE_FLT_OPCODE_ROTATE_ABOUT_POINT].RecordName					= "Rotate About Point";
	Handlers[ZE_FLT_OPCODE_ROTATE_ABOUT_POINT].RecordType					= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_ROTATE_ABOUT_POINT].Callback						= ZEFLTRecordCallback::Create<&ReadRecordRotateAboutPoint>();
	Handlers[ZE_FLT_OPCODE_ROTATE_SCALE_TO_POINT].RecordName				= "Rotate and/or Scale to Point";
	Handlers[ZE_FLT_OPCODE_ROTATE_SCALE_TO_POINT].RecordType				= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_ROTATE_SCALE_TO_POINT].Callback					= ZEFLTRecordCallback::Create<&ReadRecordRotateScaleToPoint>();
	Handlers[ZE_FLT_OPCODE_PUT].RecordName									= "Put";
	Handlers[ZE_FLT_OPCODE_PUT].RecordType									= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_PUT].Callback									= ZEFLTRecordCallback::Create<&ReadRecordPut>();
	Handlers[ZE_FLT_OPCODE_EYEPOINT_AND_TRACKPLANE_PALETTE].RecordName		= "Eyepoint and Trackplane Palette";
	Handlers[ZE_FLT_OPCODE_EYEPOINT_AND_TRACKPLANE_PALETTE].RecordType		= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EYEPOINT_AND_TRACKPLANE_PALETTE].Callback		= ZEFLTRecordCallback::Create<&ReadRecordEyepointTrackPlanePalette>();
	Handlers[ZE_FLT_OPCODE_MESH].RecordName									= "Mesh";
	Handlers[ZE_FLT_OPCODE_MESH].RecordType									= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_MESH].Callback									= ZEFLTRecordCallback::Create<&ReadRecordMesh>();
	Handlers[ZE_FLT_OPCODE_LOCAL_VERTEX_POOL].RecordName					= "Local Vertex Pool";
	Handlers[ZE_FLT_OPCODE_LOCAL_VERTEX_POOL].RecordType					= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_LOCAL_VERTEX_POOL].Callback						= ZEFLTRecordCallback::Create<&ReadRecordLocalVertexPool>();
	Handlers[ZE_FLT_OPCODE_MESH_PRIMITIVE].RecordName						= "Mesh Primitive";
	Handlers[ZE_FLT_OPCODE_MESH_PRIMITIVE].RecordType						= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_MESH_PRIMITIVE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordMeshPrimitive>();
	Handlers[ZE_FLT_OPCODE_MORPH_VERTEX_LIST].RecordName					= "Morph Vertex List";
	Handlers[ZE_FLT_OPCODE_MORPH_VERTEX_LIST].RecordType					= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_MORPH_VERTEX_LIST].Callback						= ZEFLTRecordCallback::Create<&ReadRecordMorphList>();
	Handlers[ZE_FLT_OPCODE_SOUND].RecordName								= "Sound";
	Handlers[ZE_FLT_OPCODE_SOUND].RecordType								= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_SOUND].Callback									= ZEFLTRecordCallback::Create<&ReadRecordSound>();
	Handlers[ZE_FLT_OPCODE_SOUND_PALETTE].RecordName						= "Sound Palette";
	Handlers[ZE_FLT_OPCODE_SOUND_PALETTE].RecordType						= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_SOUND_PALETTE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordSoundPalette>();

	Handlers[ZE_FLT_OPCODE_GENERAL_MATRIX].RecordName						= "General Matrix";
	Handlers[ZE_FLT_OPCODE_GENERAL_MATRIX].RecordType						= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_GENERAL_MATRIX].Callback							= ZEFLTRecordCallback::Create<&ReadRecordGeneralMatrix>();
	Handlers[ZE_FLT_OPCODE_TEXT].RecordName									= "Text";
	Handlers[ZE_FLT_OPCODE_TEXT].RecordType									= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_TEXT].Callback									= ZEFLTRecordCallback::Create<&ReadRecordText>();
	Handlers[ZE_FLT_OPCODE_SWITCH].RecordName								= "Switch";
	Handlers[ZE_FLT_OPCODE_SWITCH].RecordType								= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_SWITCH].Callback									= ZEFLTRecordCallback::Create<&ReadRecordSwitch>();
	Handlers[ZE_FLT_OPCODE_CLIP_REGION].RecordName							= "Clip Region";
	Handlers[ZE_FLT_OPCODE_CLIP_REGION].RecordType							= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_CLIP_REGION].Callback							= ZEFLTRecordCallback::Create<&ReadRecordClipRegion>();
	//Handlers[ZE_FLT_OPCODE_EXTENSION].RecordName							= "Extension";
	//Handlers[ZE_FLT_OPCODE_EXTENSION].RecordType							= ZE_FLT_RECORD_PRIMARY;
	//Handlers[ZE_FLT_OPCODE_EXTENSION].Callback								= ZEFLTRecordCallback::Create<&ReadRecordExtension>();
	Handlers[ZE_FLT_OPCODE_LIGHT_SOURCE].RecordName							= "Light Source";
	Handlers[ZE_FLT_OPCODE_LIGHT_SOURCE].RecordType							= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_LIGHT_SOURCE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordLightSource>();
	Handlers[ZE_FLT_OPCODE_LIGHT_SOURCE_PALETTE].RecordName					= "Light Source Palette";
	Handlers[ZE_FLT_OPCODE_LIGHT_SOURCE_PALETTE].RecordType					= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_LIGHT_SOURCE_PALETTE].Callback					= ZEFLTRecordCallback::Create<&ReadRecordLightSourcePalette>();
	Handlers[ZE_FLT_OPCODE_BOUNDING_SPHERE].RecordName						= "Bounding Sphere";
	Handlers[ZE_FLT_OPCODE_BOUNDING_SPHERE].RecordType						= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_BOUNDING_SPHERE].Callback						= ZEFLTRecordCallback::Create<&ReadRecordBoundingSphere>();
	Handlers[ZE_FLT_OPCODE_BOUNDING_CYLINDER].RecordName					= "Bounding Cylinder";
	Handlers[ZE_FLT_OPCODE_BOUNDING_CYLINDER].RecordType					= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_BOUNDING_CYLINDER].Callback						= ZEFLTRecordCallback::Create<&ReadRecordBoundingCylinder>();
	Handlers[ZE_FLT_OPCODE_BOUNDING_CONVEX_HULL].RecordName					= "Bounding Convex Hull";
	Handlers[ZE_FLT_OPCODE_BOUNDING_CONVEX_HULL].RecordType					= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_BOUNDING_CONVEX_HULL].Callback					= ZEFLTRecordCallback::Create<&ReadRecordBoundingConvexHull>();
	Handlers[ZE_FLT_OPCODE_BOUNDING_VOLUME_CENTER].RecordName				= "Bounding Volume Center";
	Handlers[ZE_FLT_OPCODE_BOUNDING_VOLUME_CENTER].RecordType				= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_BOUNDING_VOLUME_CENTER].Callback					= ZEFLTRecordCallback::Create<&ReadRecordBoundingVolumeCenter>();
	Handlers[ZE_FLT_OPCODE_BOUNDING_VOLUME_ORIENTATION].RecordName			= "Bounding Volume Orientation";
	Handlers[ZE_FLT_OPCODE_BOUNDING_VOLUME_ORIENTATION].RecordType			= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_BOUNDING_VOLUME_ORIENTATION].Callback			= ZEFLTRecordCallback::Create<&ReadRecordBoundingVolumeOrientation>();
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT].RecordName							= "Light Point";
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT].RecordType							= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT].Callback							= ZEFLTRecordCallback::Create<&ReadRecordLightPoint>();
	Handlers[ZE_FLT_OPCODE_TEXTURE_MAPPING_PALETTE].RecordName				= "Texture Mapping Palette";
	Handlers[ZE_FLT_OPCODE_TEXTURE_MAPPING_PALETTE].RecordType				= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_TEXTURE_MAPPING_PALETTE].Callback				= ZEFLTRecordCallback::Create<&ReadRecordTextureMappingPalette>();
	Handlers[ZE_FLT_OPCODE_MATERIAL_PALETTE].RecordName						= "Material Palette";
	Handlers[ZE_FLT_OPCODE_MATERIAL_PALETTE].RecordType						= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_MATERIAL_PALETTE].Callback						= ZEFLTRecordCallback::Create<&ReadRecordMaterialPalette>();
	Handlers[ZE_FLT_OPCODE_NAME_TABLE].RecordName							= "Name Table";
	Handlers[ZE_FLT_OPCODE_NAME_TABLE].RecordType							= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_NAME_TABLE].Callback								= ZEFLTRecordCallback::Create<&ReadRecordNameTable>();
	//Handlers[ZE_FLT_OPCODE_PUSH_ATTRIBUTE].RecordName						= "Push Attribute";
	//Handlers[ZE_FLT_OPCODE_PUSH_ATTRIBUTE].RecordType						= ZE_FLT_RECORD_CONTROL;
	//Handlers[ZE_FLT_OPCODE_PUSH_ATTRIBUTE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordPushAttribute>();
	//Handlers[ZE_FLT_OPCODE_POP_ATTRIBUTE].RecordName						= "Pop Attribute";
	//Handlers[ZE_FLT_OPCODE_POP_ATTRIBUTE].RecordType						= ZE_FLT_RECORD_CONTROL;
	//Handlers[ZE_FLT_OPCODE_POP_ATTRIBUTE].Callback							= ZEFLTRecordCallback::Create<&ReadRecordPopAttribute>();
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT_APPEARANCE_PALETTE].RecordName		= "Light Point Appearance Palette";
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT_APPEARANCE_PALETTE].RecordType		= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT_APPEARANCE_PALETTE].Callback			= ZEFLTRecordCallback::Create<&ReadRecordLightPointAppearancePalette>();
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT_ANIMATION_PALETTE].RecordName		= "Light Point Animation Palette";
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT_ANIMATION_PALETTE].RecordType		= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT_ANIMATION_PALETTE].Callback			= ZEFLTRecordCallback::Create<&ReadRecordLightPointAnimationPalette>();
	Handlers[ZE_FLT_OPCODE_INDEXED_LIGHT_POINT].RecordName					= "Indexed Light Point";
	Handlers[ZE_FLT_OPCODE_INDEXED_LIGHT_POINT].RecordType					= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_INDEXED_LIGHT_POINT].Callback					= ZEFLTRecordCallback::Create<&ReadRecordIndexedLightPoint>();
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT_SYSTEM].RecordName					= "Light Point System";
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT_SYSTEM].RecordType					= ZE_FLT_RECORD_PRIMARY;
	Handlers[ZE_FLT_OPCODE_LIGHT_POINT_SYSTEM].Callback						= ZEFLTRecordCallback::Create<&ReadRecordLightPointSystem>();

	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_HEADER].RecordName				= "Extended Material Header";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_HEADER].RecordType				= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_HEADER].Callback				= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialHeader>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_AMBIENT].RecordName			= "Extended Material Ambient";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_AMBIENT].RecordType			= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_AMBIENT].Callback				= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialAmbient>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_DIFFUSE].RecordName			= "Extended Material Diffuse";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_DIFFUSE].RecordType			= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_DIFFUSE].Callback				= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialDiffuse>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_SPECULAR].RecordName			= "Extended Material Specular";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_SPECULAR].RecordType			= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_SPECULAR].Callback				= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialSpecular>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_EMISSIVE].RecordName			= "Extended Material Emissive";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_EMISSIVE].RecordType			= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_EMISSIVE].Callback				= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialEmissive>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_ALPHA].RecordName				= "Extended Material Alpha";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_ALPHA].RecordType				= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_ALPHA].Callback				= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialAlpha>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_LIGHT_MAP].RecordName			= "Extended Material Light Map";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_LIGHT_MAP].RecordType			= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_LIGHT_MAP].Callback			= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialLightMap>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_NORMAL_MAP].RecordName			= "Extended Material Normal Map";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_NORMAL_MAP].RecordType			= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_NORMAL_MAP].Callback			= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialNormalMap>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_BUMP_MAP].RecordName			= "Extended Material Bump Map";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_BUMP_MAP].RecordType			= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_BUMP_MAP].Callback				= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialBumpMap>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_SHADOW_MAP].RecordName			= "Extended Material Shadow Map";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_SHADOW_MAP].RecordType			= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_SHADOW_MAP].Callback			= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialShadowMap>();
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_REFLECTION_MAP].RecordName		= "Extended Material Reflection Map";
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_REFLECTION_MAP].RecordType		= ZE_FLT_RECORD_ANCILLARY;
	Handlers[ZE_FLT_OPCODE_EXTENDED_MATERIAL_REFLECTION_MAP].Callback		= ZEFLTRecordCallback::Create<&ReadRecordExtendedMaterialReflectionMap>();

};
