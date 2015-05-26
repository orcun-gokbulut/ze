//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTag.h
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
#ifndef __ZED_TAG_H__
#define __ZED_TAG_H__

#include "ZEMeta/ZEObject.h"
#include "ZEGraphics/ZECanvas.h"
#include "ZEGraphics/ZERenderCommand.h"


class ZECanvas;
class ZEDrawParameters;
class ZERectangle3D;
class ZESimpleMaterial;
class ZEMLWriterNode;
class ZEMLReaderNode;

class ZEDTag
{
	friend class ZEDSelection;

	private:
		ZEObject* Object;
		ZEDSelection* Selection;

		ZEDTag* ParentTag;
		ZEArray<ZEDTag*> ChildTags;
		ZEArray<ZEDTag*> ComponentTags;
		
	protected:
		ZEMaterial* TagMaterial;
		ZECanvas TagCanvas;
		ZERenderCommand TagRenderCommand;

		void SetSelection(ZEDSelection* Selection);
		ZEDSelection* GetSelection();

		void SetParentTag(ZEDTag* Tag);
		ZEDTag* GetParentTag();

		void DrawOrientedBoundingBox(const ZEAABBox& BoundingBox, const ZEMatrix4x4& Transform, const ZEVector4& Color, ZECanvas& Canvas);
		void DrawAxisAlignedBoundingBox(const ZEAABBox& BoundingBox, const ZEVector4& Color, ZECanvas& Canvas);
		void DrawBoundingSphere(const ZEBSphere& BoundingSphere, const ZEVector4& Color, ZECanvas& Canvas);
		void DrawLineSegment(const ZEVector3& StartPosition, const ZEVector3& EndPosition, const ZEVector4& Color, ZECanvas& Canvas);
		void DrawRectangle(const ZERectangle3D& Rectangle, const ZEVector4& Color, ZECanvas& Canvas);

		virtual bool InitializeSelf();
		virtual bool DeinitializeSelf();

		ZEDTag();
		virtual ~ZEDTag();

	public:
		virtual void SetObject(ZEObject* Object);
		virtual ZEObject* GetObject();

		void SetIcon(ZESimpleMaterial* Material);
		const ZESimpleMaterial* GetIcon();

		virtual ZEAABBox GetBoundingBox() = 0;
		virtual ZEAABBox GetWorldBoundingBox() = 0;

		virtual void SetPosition(const ZEVector3& NewPosition) = 0;
		virtual ZEVector3 GetPosition() = 0;
		virtual void SetWorldPosition(const ZEVector3& NewPosition) = 0;
		virtual ZEVector3 GetWorldPosition() = 0;

		virtual void SetRotation(const ZEQuaternion& NewRotation) = 0;
		virtual ZEQuaternion GetRotation() = 0;
		virtual void SetWorldRotation(const ZEQuaternion& NewRotation) = 0;
		virtual ZEQuaternion GetWorldRotation() = 0;

		virtual void SetScale(const ZEVector3& NewScale) = 0;
		virtual ZEVector3 GetScale() = 0;
		virtual void SetWorldScale(const ZEVector3& NewScale) = 0;
		virtual ZEVector3 GetWorldScale() = 0;

		ZEArray<ZEDTag*>& GetChildTags();
		ZEDTag* GetChildTag(ZESize Index);

		virtual void AddChildTag(ZEDTag* Tag);
		virtual void RemoveChildTag(ZEDTag* Tag);

		ZEArray<ZEDTag*>& GetComponentTags();
		ZEDTag* GetComponentTag(ZESize Index);
		void AddComponentTag(ZEDTag* Tag);
		void RemoveComponentTag(ZEDTag* Tag);

		virtual bool Save(ZEMLWriterNode* Serializer);
		virtual bool Restore(ZEMLReaderNode* Unserializer);

		virtual void Tick(float Time);
		virtual void Draw(ZEDrawParameters* DrawParameters);
};

#endif
