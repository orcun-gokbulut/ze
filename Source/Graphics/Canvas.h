//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Canvas.h
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

#include "VertexTypes.h"
#include "VertexBuffer.h"
#include "ZEDS/Array.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/Quaternion.h"
#include "ZEMath/AABoundingBox.h"

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
		void							ApplyTranslation(const ZEVector3& Translation);
		void							ApplyRotation(const ZEQuaternion& Rotation);
		void							ApplyScale(const ZEVector3& Scale);

		// 2D Primitives
		void							AddPoint(const ZEVector3& Point);
		void							AddLine(const ZEVector3& Point0, const ZEVector3& Point1);
		void							AddCircle(float Radius, unsigned int Segments);

		// 3D Wireframe primitives
		void							AddWireframePlane(float Width, float Length, unsigned int VSegment, unsigned int HSegment);
		void							AddWireframeBox(float Width, float Height, float Length);
		void							AddWireframeSphere(float Radius, unsigned int HSegments, unsigned int VSegments);
		void							AddWireframeQuad(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3);
		void							AddWireframeCylinder(float Radius, float Height, unsigned int HSegments, unsigned int VSegments, bool Caps);
		void							AddWireframePyramid(float Width, float Height, float Length);

		// 3D Primitives
		void							AddBox(float Width, float Height, float Length);
		void							AddSphere(float Radius, unsigned int HSegment, unsigned int VSegment);
		void							AddQuad(const ZEVector3& P0, const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3);
		void							AddDisc(float Radius, unsigned int Segments);
		void							AddPyramid(float Width, float Height, float Length);
		void							AddPlane(float Width, float Length);

		// Custom Vertices
		void							AddVertex(const ZECanvasVertex& Vertex);
		void							AddVertices(const ZECanvasVertex* Vertices, size_t Count);
		
		void							CalculateBoundingBox(ZEAABoundingBox& BoundingBox);

		bool							IsEmpty();
		void							Clean();

		ZEStaticVertexBuffer*			CreateStaticVertexBuffer();

		bool							LoadFromFile(const char* FileName);
		void							SaveToFile(const char* Filename);

		virtual unsigned int			GetBufferSize();
		virtual void*					GetVertexBuffer();
		
										ZECanvas();
};

#endif
