//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECanvas.h
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
#ifndef __ZE_CANVAS_H__
#define __ZE_CANVAS_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEVertexTypes.h"
#include "ZEDS/ZEString.h"
#include "ZEVertexBuffer.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEQuaternion.h"

class ZECanvasVertex
{
	private:
		static ZEVertexDeclaration*		VertexDeclaration;

	public:
		ZEVector3						Position;
		ZEVector3						Normal;		
		ZEVector2						Texcoord;
		ZEVector4						Color;

		static ZEVertexDeclaration*		GetVertexDeclaration();
};

class ZECanvas : public ZEDynamicVertexBuffer 
{
	public:
		ZESmartArray<ZECanvasVertex>	Vertices;
		ZESmartArray<ZEMatrix4x4>		TransformationStack;
		ZEMatrix4x4						Transformation;
		ZEVector4						Color;		

		ZEVector3						Translation;
		ZEQuaternion					Rotation;
		ZEVector3						Scale;

	public:
		void							PushTransformation();
		void							PopTransformation();

		void							SetTransfomation(const ZEMatrix4x4& Matrix);
		const ZEMatrix4x4&				GetTransformation();

		void							SetTranslation(const ZEVector3& Translation);
		const ZEVector3&				GetTranslation();

		void							SetRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion&				GetRotation();

		void							SetScale(const ZEVector3& Scale);
		const ZEVector3&				GetScale();

		void							SetColor(const ZEVector4& Color);
		const ZEVector4&				GetColor();

		// Transformations
		void							ApplyTransformationBefore(const ZEMatrix4x4& Matrix);
		void							ApplyTransformationAfter(const ZEMatrix4x4& Matrix);

		void							ApplyTranslation(const ZEVector3& Translation);
		void							ApplyRotation(const ZEQuaternion& Rotation);
		void							ApplyScale(const ZEVector3& Scale);

		// 2D Primitives
		void							AddPoint(const ZEVector3& Point);
		void							AddLine(const ZEVector3& Point0, const ZEVector3& Point1);
		void							AddCircle(float Radius, ZEUInt Segments);

		// 3D Wireframe primitives
		void							AddWireframePlane(float Width, float Length, ZEUInt VSegment, ZEUInt HSegment);
		void							AddWireframeBox(float Width, float Height, float Length);
		void							AddWireframeSphere(float Radius, ZEUInt HSegments, ZEUInt VSegments);
		void							AddWireframeTriangle(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2);
		void							AddWireframeQuad(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3);
		void							AddWireframeCylinder(float Radius, float Height, ZEUInt HSegments, ZEUInt VSegments, bool Caps);
		void							AddWireframePyramid(float Width, float Height, float Length);
		void							AddWireframeCone(float Radius, ZEUInt Segments, float Height);
		void							AddWireframeConvexPolygon(const ZEVector3* Vertices, ZESize VertexCount);

		// 3D Primitives
		void							AddBox(float Width, float Height, float Length);
		void							AddSphere(float Radius, ZEUInt HSegment, ZEUInt VSegment);
		void							AddTriangle(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2);
		void							AddQuad(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3);
		void							AddDisc(float Radius, ZEUInt Segments);
		void							AddPyramid(float Width, float Height, float Length);
		void							AddPlane(float Width, float Length);
		void							AddCone(float Radius, ZEUInt Segments, float Height);
		void							AddConvexPolygon(const ZEVector3* Vertices, ZESize VertexCount);

		// Custom Vertices
		void							AddVertex(const ZECanvasVertex& Vertex);
		void							AddVertices(const ZECanvasVertex* Vertices, ZESize Count);
		
		void							CalculateBoundingBox(ZEAABBox& BoundingBox);

		bool							IsEmpty();
		void							Clean();

		ZEStaticVertexBuffer*			CreateStaticVertexBuffer();

		bool							LoadFromFile(const ZEString& FileName);
		void							SaveToFile(const ZEString& FileName);

		virtual ZESize					GetBufferSize();
		virtual void*					GetVertexBuffer();
		
										ZECanvas();
};

#endif
