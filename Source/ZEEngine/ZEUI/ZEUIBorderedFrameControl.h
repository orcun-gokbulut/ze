//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIBorderedFrameControl.h
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

#include "ZEUIControl.h"
#include "ZEUIFrameControl.h"
#include "ZEUIFont.h"
#include "ZEExport.ZEEngine.h"

#define	ZEUI_BFC_EL_MAINAREA		1
#define	ZEUI_BFC_EL_LEFTUPCORNER	2
#define	ZEUI_BFC_EL_LEFTDOWNCORNER	4
#define	ZEUI_BFC_EL_RIGHTUPCORNER	8
#define	ZEUI_BFC_EL_RIGHTDOWNCORNER	16
#define	ZEUI_BFC_EL_TOPEDGE			32
#define	ZEUI_BFC_EL_BOTTOMEDGE		64
#define	ZEUI_BFC_EL_LEFTEDGE		128
#define	ZEUI_BFC_EL_RIGHTEDGE		256
#define	ZEUI_BFC_EL_ALL				512

struct ZE_EXPORT_ZEENGINE ZEUIControlStyle
{
	bool							BorderVisibility;
	ZEInt32							BorderWidth;
	ZEInt32							ElementVisibilities;
	bool							TileTextures;

	ZEVector4						BoderEdgeColor;
	ZEVector4						BorderCornerColor;
	ZEVector4						BackGroundColor;
	ZEVector4						ForeGroundColor;
	ZEVector4						HoveredColor;
	ZEVector4						PressedColor;
	ZEVector4						DisabledColor;

	ZEVector2						BorderTopLeftCornerUVLeftUp;
	ZEVector2						BorderTopLeftCornerUVRightDown;
	ZEVector2						BorderLeftDownCornerUVLeftUp;
	ZEVector2						BorderLeftDownCornerUVRightDown;
	ZEVector2						BorderRightUpCornerUVLeftUp;
	ZEVector2						BorderRightUpCornerUVRightDown;
	ZEVector2						BorderRightDownCornerUVLeftUp;
	ZEVector2						BorderRightDownCornerUVRightDown;
	ZEVector2						BorderTopEdgeUVLeftUp;
	ZEVector2						BorderTopEdgeUVRightDown;
	ZEVector2						BorderBottomEdgeUVLeftUp;
	ZEVector2						BorderBottomEdgeUVRightDown;
	ZEVector2						BorderLeftEdgeUVLeftUp;
	ZEVector2						BorderLeftEdgeUVRightDown;
	ZEVector2						BorderRightEdgeUVLeftUp;
	ZEVector2						BorderRightEdgeUVRightDown;
	ZEVector2						MainAreaUVLeftUp;
	ZEVector2						MainAreaUVRightDown;

	ZEGRTexture*					BackgroundTexture;
	ZEGRTexture*					BorderCornerTexture;
	ZEGRTexture*					BorderEdgeTexture;
	ZEGRTexture*					HoveredTexture;
	ZEGRTexture*					PressedTexture;
	ZEGRTexture*					DisabledTexture;

	ZEUIFont*					Font;

	ZEUIControlStyle()
	{
		BorderVisibility	= true;
		BorderWidth			= 4;
		ElementVisibilities = ZEUI_BFC_EL_ALL;
		TileTextures		= true;

		BoderEdgeColor		= ZEVector4::One;
		BorderCornerColor	= ZEVector4::One;
		BackGroundColor		= ZEVector4::One;
		ForeGroundColor		= ZEVector4::One;
		HoveredColor		= ZEVector4::One;
		PressedColor		= ZEVector4::One;
		DisabledColor		= ZEVector4::One;

		BorderTopLeftCornerUVLeftUp			= ZEVector2::Zero;
		BorderTopLeftCornerUVRightDown		= ZEVector2::One;
		BorderLeftDownCornerUVLeftUp		= ZEVector2::Zero;
		BorderLeftDownCornerUVRightDown		= ZEVector2::One;
		BorderRightUpCornerUVLeftUp			= ZEVector2::Zero;
		BorderRightUpCornerUVRightDown		= ZEVector2::One;
		BorderRightDownCornerUVLeftUp		= ZEVector2::Zero;
		BorderRightDownCornerUVRightDown	= ZEVector2::One;
		BorderTopEdgeUVLeftUp				= ZEVector2::Zero;
		BorderTopEdgeUVRightDown			= ZEVector2::One;
		BorderBottomEdgeUVLeftUp			= ZEVector2::Zero;
		BorderBottomEdgeUVRightDown			= ZEVector2::One;
		BorderLeftEdgeUVLeftUp				= ZEVector2::Zero;
		BorderLeftEdgeUVRightDown			= ZEVector2::One;
		BorderRightEdgeUVLeftUp				= ZEVector2::Zero;
		BorderRightEdgeUVRightDown			= ZEVector2::One;
		MainAreaUVLeftUp					= ZEVector2::Zero;
		MainAreaUVRightDown					= ZEVector2::One;

		BackgroundTexture	= NULL;
		BorderCornerTexture = NULL;
		BorderEdgeTexture	= NULL;
		HoveredTexture		= NULL;
		PressedTexture		= NULL;
		DisabledTexture		= NULL;

		Font				= NULL;		
	}
};

class ZE_EXPORT_ZEENGINE ZEUIBorderedFrameControl : public ZEUIControl
{
	ZE_OBJECT

	protected:

		ZEUIFrameControl			MainArea;
		ZEUIFrameControl			LeftUpCornerArea;
		ZEUIFrameControl			LeftDownCornerArea;
		ZEUIFrameControl			RightUpCornerArea;
		ZEUIFrameControl			RightDownCornerArea;
		ZEUIFrameControl			TopEdgeArea;
		ZEUIFrameControl			LeftEdgeArea;
		ZEUIFrameControl			RightEdgeArea;
		ZEUIFrameControl			BottomEdgeArea;

	public:

		virtual void				SetHeight(float Height);
		virtual void				SetWidth(float Width);

		virtual void				AddChildControl(ZEUIControl* Control);

		void						SetElementVisibilities(ZEInt32 Elements, bool Visibility);
		ZEInt32						GetVisibleElements();

		void						SetBorderThickness(float Thickness);

									ZEUIBorderedFrameControl();
};
