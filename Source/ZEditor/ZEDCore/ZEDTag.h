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
#include "ZEGame/ZEEntity.h"
#include "ZEUI/ZEUIFrameControl.h"
#include "ZEGraphics/ZECanvas.h"
#include "ZEGraphics/ZERenderCommand.h"

class ZECanvas;
class ZERectangle3D;

class ZEDTag : public ZEEntity
{
	private:
		ZEUIFrameControl Icon;
		ZEVector4 Color;

		bool Selected;
		ZEObject* Object;

	protected:
		ZEMaterial* TagMaterial;
		ZECanvas TagCanvas;
		ZERenderCommand TagRenderCommand;

		void SetIcon(const ZEUIMaterial* Material);
		const ZEUIMaterial* GetIcon();

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

		void SetSelected(bool Value);
		bool GetSelected();

		void SetDrawColor(const ZEVector4& Color);
		const ZEVector4& GetDrawColor();

		virtual void SetPosition(const ZEVector3& NewPosition);
		virtual void SetRotation(const ZEQuaternion& NewRotation);
		virtual void SetScale(const ZEVector3& NewScale);

		virtual void Destroy();

		virtual bool Save(ZEMLWriterNode* Serializer);
		virtual bool Restore(ZEMLReaderNode* Unserializer);

		virtual void Tick(float Time);
		virtual void Draw(ZEDrawParameters* DrawParameters);

		static ZEDTag* CreateInstance();
};

#endif
