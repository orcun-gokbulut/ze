//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDebugDrawer.h
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
#ifndef __ZE_DEBUG_DRAWER_H__
#define __ZE_DEBUG_DRAWER_H__

#include "ZEEntity.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZERenderer/ZECanvas.h"

#define ZE_DDE_NONE									0
#define ZE_DDE_ENTITY_WORLD_BOUNDING_BOX			1
#define ZE_DDE_LIGHT_RANGE							2
#define ZE_DDE_ALL									3

class ZEAABBox;
class ZEMatrix4x4;
class ZERectangle3D;
class ZERNMaterial;
class ZEUIDebugDrawTag;

class ZEDebugDrawer : public ZEEntity
{
	private:
		ZEVector4				DrawColor;
		ZEFlags					DebugDrawElements;

		ZERNMaterial*				Material;
		ZECanvas				DrawCanvas;
		ZERNCommand			RenderCommand;

	protected:
		ZEEntity*				Target;
		ZEUIDebugDrawTag*		EntityTag;

		void					DrawOrientedBoundingBox(const ZEAABBox& BoundingBox, const ZEMatrix4x4& Transform, const ZEVector4& Color, ZECanvas& Canvas);
		void					DrawAxisAlignedBoundingBox(const ZEAABBox& BoundingBox, const ZEVector4& Color, ZECanvas& Canvas);
		void					DrawBoundingSphere(const ZEBSphere& BoundingSphere, const ZEVector4& Color, ZECanvas& Canvas);
		void					DrawLineSegment(const ZEVector3& StartPosition, const ZEVector3& EndPosition, const ZEVector4& Color, ZECanvas& Canvas);
		void					DrawRectangle(const ZERectangle3D& Rectangle, const ZEVector4& Color, ZECanvas& Canvas);

		virtual void			DebugDrawEntity();

		virtual bool			InitializeSelf();
		virtual	bool			DeinitializeSelf();

								ZEDebugDrawer();
		virtual					~ZEDebugDrawer();

	public:
		virtual void			SetTarget(ZEEntity* Target);
		ZEEntity*				GetTarget();

		void					SetDebugDrawColor(ZEVector4 Color);
		ZEVector4				GetDebugDrawColor() const;

		void					SetDebugDrawElements(ZEUInt Elements);
		ZEUInt					GetDebugDrawElements() const;

		virtual ZEDrawFlags		GetDrawFlags() const;

		static ZEDebugDrawer*	CreateInstance();

};

#endif
