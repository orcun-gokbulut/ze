//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECVModelConverterV2.h
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

#include "ZECVConverter.h"

#include "ZEPacking.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"


ZEPackStruct(
struct ZEMDVertexV1
{
	ZEVector3						Position;
	ZEVector3						Normal;
	ZEVector3						Tangent;
	ZEVector3						Binormal;
	ZEVector2						Texcoords;
});

ZEPackStruct(
struct ZEMDVertexSkinV1
{
	ZEVector3						Position;
	ZEVector3						Normal;
	ZEVector3						Tangent;
	ZEVector3						Binormal;
	ZEVector2						Texcoords;
	ZEUInt8							Indices[4];
	ZEVector4						Weights;
});

ZEPackStruct(
struct ZEMDVertexV2
{
	ZEVector3						Position;
	float							Reserved0;
	ZEInt16							Normal[2];
	ZEInt16							Tangent[2];
	ZEVector2						Texcoords;
});

ZEPackStruct(
struct ZEMDVertexSkinV2
{
	ZEVector3						Position;
	ZEUInt8							Indices[4];
	ZEInt16							Normal[2];
	ZEInt16							Tangent[2];
	ZEVector2						Texcoords;
	ZEVector4						Weights;
});

class ZECVModelConverterV2 : public ZECVConverter
{
	private:
		static void							GenerateIndices(const ZEArray<ZEMDVertexV2>& Vertices, ZEArray<ZEUInt32>& Indices, ZEArray<ZEMDVertexV2>& IndexedVertices);
		static void							GenerateIndices(const ZEArray<ZEMDVertexSkinV2>& Vertices, ZEArray<ZEUInt32>& Indices, ZEArray<ZEMDVertexSkinV2>& IndexedVertices);
		static bool							ConvertPhysicalBody(ZEMLReaderNode* SourcePhysicalBodyNode, ZEMLWriterNode* DestinationPhysicalBodyNode);
		static bool							ConvertPhysicalJoint(ZEMLReaderNode* SourceJointNode, ZEMLWriterNode* DestinationJointNode);
		static bool							ConvertBone(ZEMLReaderNode* SourceBoneNode, ZEMLWriterNode* DestinationBoneNode);
		static bool							ConvertMeshBoundingBox(ZEMLReaderNode* SourceMeshNode, ZEMLWriterNode* DestinationBoundingBoxNode);
		static void							ConvertVertexData(ZEArray<ZEMDVertexV2>& Output, const ZEArray<ZEMDVertexV1>& Input);
		static void							ConvertVertexData(ZEArray<ZEMDVertexSkinV2>& Output, const ZEArray<ZEMDVertexSkinV1>& Input);
		static bool							ConvertMeshLOD(ZEMLReaderNode* SourceLODNode, ZEMLWriterNode* DestinationLODNode);
		static bool							ConvertMesh(ZEMLReaderNode* SourceMeshNode, ZEMLWriterNode* DestinationMeshNode);
		static bool							ConvertHelper(ZEMLReaderNode* SourceHelperNode, ZEMLWriterNode* DestinationHelperNode);
		static bool							ConvertAnimation(ZEMLReaderNode* SourceAnimationNode, ZEMLWriterNode* DestinationAnimationNode);
		static bool							ConvertModel(ZEMLReaderNode* Unserializer, ZEMLWriterNode* Serializer);

	public:
		virtual const char*					GetName();
		virtual const char*					GetExtension();
		virtual ZEUInt						GetSourceVersion();
		virtual ZEUInt						GetDestinationVersion();
		virtual bool						GetMajorVersionConversion();

		virtual bool						Convert(const ZEString& SourceFileName, const ZEString& DestinationFileName);
};

