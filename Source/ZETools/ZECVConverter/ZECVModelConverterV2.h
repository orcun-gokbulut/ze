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
		static void							ConvertVertexBase(ZEMDVertexV2& Output, const ZEMDVertexV1& Input);
		static void							ConvertVertexBase(ZEMDVertexSkinV2& Output, const ZEMDVertexSkinV1& Input);
		template<typename ZEVertexTypeV1, typename ZEVertexTypeV2>
		static void							VerifyVertexNormals(const ZEVertexTypeV1& InputVertex, const ZEVertexTypeV2& OutputVertex, ZESize Index);
		template<typename ZEVertexTypeV1, typename ZEVertexTypeV2>
		static void							ConvertVertexNormals(const ZEVertexTypeV1& InputVertex, ZEVertexTypeV2& OutputVertex, ZESize Index, const ZEVector3& TriangleTangent, const ZEVector3& TriangleBinormal);
		template<typename ZEVertexTypeV1, typename ZEVertexTypeV2>
		static void							ConvertVertexData(ZEArray<ZEVertexTypeV2>& Output, const ZEArray<ZEVertexTypeV1>& Input, bool& DegenerateFaceDetected);

		static bool							CompareVertexes(const ZEMDVertexV2& VertexA, const ZEMDVertexV2& VertexB);
		static bool							CompareVertexes(const ZEMDVertexSkinV2& VertexA, const ZEMDVertexSkinV2& VertexB);
		template<typename ZEVertexType>
		static void							GenerateIndices(const ZEArray<ZEVertexType>& Vertices, ZEArray<ZEUInt32>& Indices, ZEArray<ZEVertexType>& IndexedVertices);

		static bool							ConvertPhysicalBody(ZEMLReaderNode* SourcePhysicalBodyNode, ZEMLWriterNode* DestinationPhysicalBodyNode);
		static bool							ConvertPhysicalJoint(ZEMLReaderNode* SourceJointNode, ZEMLWriterNode* DestinationJointNode);
		static bool							ConvertBone(ZEMLReaderNode* SourceBoneNode, ZEMLWriterNode* DestinationBoneNode);
		static bool							ConvertMeshBoundingBox(ZEMLReaderNode* SourceMeshNode, ZEMLWriterNode* DestinationBoundingBoxNode);
		static bool							ConvertMeshLOD(ZEMLReaderNode* SourceLODNode, ZEMLWriterNode* DestinationLODNode);
		static bool							ConvertMesh(ZEMLReaderNode* SourceMeshNode, ZEMLWriterNode* DestinationMeshNode);
		static bool							ConvertHelper(ZEMLReaderNode* SourceHelperNode, ZEMLWriterNode* DestinationHelperNode);
		static bool							ConvertAnimation(ZEMLReaderNode* SourceAnimationNode, ZEMLWriterNode* DestinationAnimationNode);
		static bool							ConvertModel(ZEMLReaderNode* Unserializer, ZEMLWriterNode* Serializer);

	public:
		virtual ZECVVersion					GetSourceVersion() const;
		virtual ZECVVersion					GetDestinationVersion() const;

		virtual ZECVResult					Convert(const ZEString& SourceFileName, const ZEString& DestinationFileName);
};

