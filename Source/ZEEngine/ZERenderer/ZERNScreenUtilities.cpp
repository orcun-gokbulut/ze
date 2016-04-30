//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNScreenUtilities.cpp
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

#include "ZERNScreenUtilities.h"

#include "ZEMath/ZEMatrix.h"
#include "ZERNView.h"

ZERay ZERNScreenUtilities::ScreenToWorld(const ZERNView& View, const ZEVector2& ScreenCoords)
{
	ZEVector4 ClipCoords = ZEVector4(ScreenToClip(View, ScreenCoords), 0.0f, 1.0f);
	ZEVector4 Vector;
	ZEMatrix4x4::Transform(Vector, View.InvViewProjectionTransform, ClipCoords);

	ZERay Ray;
	Ray.p = View.Position;
	Ray.v = Vector.ToVector3().Normalize();

	return Ray;
}

ZERay ZERNScreenUtilities::ScreenToView(const ZERNView& View, const ZEVector2& ScreenCoords)
{
	ZEVector4 ClipCoords = ZEVector4(ScreenToClip(View, ScreenCoords), 0.0f, 1.0f);
	ZEVector4 Vector;
	ZEMatrix4x4::Transform(Vector, View.InvProjectionTransform, ClipCoords);

	ZERay Ray;
	Ray.p = View.Position;
	Ray.v = Vector.ToVector3().Normalize();
	
	return Ray;
}

ZEVector2 ZERNScreenUtilities::ScreenToClip(const ZERNView& View, const ZEVector2& ScreenCoords)
{
	ZEVector2 ClipCoords;

	ClipCoords.x = 2.0f * (ScreenCoords.x - View.Viewport.GetX()) / View.Viewport.GetWidth() - 1.0f;
	ClipCoords.y = 1.0f - 2.0f * (ScreenCoords.y - View.Viewport.GetY()) / View.Viewport.GetHeight();

	return ClipCoords;
}

ZEVector2 ZERNScreenUtilities::WorldToScreen(const ZERNView& View, const ZEVector3& WorldCoords)
{
	return ViewToScreen(View, View.ViewTransform * WorldCoords);
}

ZEVector2 ZERNScreenUtilities::ViewToScreen(const ZERNView& View, const ZEVector3& ViewCoords)
{
	ZEVector4 Output4;
	ZEMatrix4x4::Transform(Output4, View.ViewProjectionTransform, ZEVector4(ViewCoords, 1.0f));
	
	ZEVector2 ClipCoords;
	ClipCoords.x = Output4.x / Output4.w;
	ClipCoords.y = Output4.y / Output4.w;

	return ClipToScreen(View, ClipCoords);
}

ZEVector2 ZERNScreenUtilities::ClipToScreen(const ZERNView& View, const ZEVector2& ClipCoords)
{
	ZEVector2 ScreenCoords;

	ScreenCoords.x = View.Viewport.GetX() + (ClipCoords.x + 1.0f) * (View.Viewport.GetWidth() / 2.0f);
	ScreenCoords.y = View.Viewport.GetY() + (1.0f - ClipCoords.y) * (View.Viewport.GetHeight() / 2.0f);

	return ScreenCoords;
}
