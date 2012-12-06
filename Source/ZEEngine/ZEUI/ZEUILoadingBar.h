//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUILoadingBar.h
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
#ifndef __ZE_UI_LOADING_BAR__
#define __ZE_UI_LOADING_BAR__

#include "ZEUIControl.h"
#include "ZEUIRectangle.h"
#include "ZEGraphics/ZEUIMaterial.h"

enum UVMode
{
	ZE_UI_UVM_TILE,
	ZE_UI_UVM_STRECH
};

class ZEUILoadingBar : public ZEUIControl
{
	private:

		ZEUIRectangle					Bar;
		ZEUIRectangle					Frame;

		ZEUIMaterial*					BarMaterial;
		ZEUIMaterial*					FrameMaterial; 

		UVMode							ModeBar;
		UVMode							ModeFrame;

		int								Value;

		void							SetLoadingBarBar();

	public:

		void							SetBarColor(ZEVector4 Color);
		void							SetFrameColor(ZEVector4 Color);

		void							SetBarTexture(ZEString FileName);
		void							SetFrameTexture(ZEString FileName);

		void							SetValue(int Value);
		int								GetValue();

		void							SetFrameUVMode(UVMode Mode);
		UVMode							GetFrameUVMode();

		void							SetBarUVMode(UVMode Mode);
		UVMode							GetBarUVMode();
			
		virtual void					SetPosition(float X, float Y);
		virtual void					SetPosition(const ZEVector2& Position);

		virtual void					SetSize(const ZEVector2& Size);

		virtual void					SetWidth(float Width);

		virtual void					SetHeight(float Height);

		virtual void					Draw(ZEUIRenderer* Renderer);
		virtual void					Tick(float ElapsedTime);

		virtual ZEMaterial*				GetMaterial() const;
		virtual void					SetMaterial(ZEMaterial* Material);

										ZEUILoadingBar();
										~ZEUILoadingBar();
};

#endif
