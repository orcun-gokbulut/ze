//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWindow.h
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

#include "ZEInitializable.h"
#include "ZETypes.h"
#include "ZEDS\ZEString.h"
#include "ZEMath\ZEVector.h"
#include "ZEPointer/ZEHolder.h"

class ZEGROutput;

class ZEWindow : public ZEInitializable
{
	friend class ZEWindowCallback;
	private:
		void*					Handle;
		ZEHolder<ZEGROutput>	Output;

		ZEString				Title;
		ZEInt					Left;
		ZEUInt					Top;
		ZEUInt					Width;
		ZEUInt					Height;
		void*					Instance;
		
		bool					Visible;
		bool					BorderlessMode;
		bool					Resizable;
		bool					Fullscreen;
		
		bool					HideCursor;
		bool					LockCursor;

		bool					CreateWindow_();
		bool					DestroyWindow();

		void					ChangeWindowConfiguration();
		void					ChangeWidowGeometry();

		virtual bool			InitializeInternal();
		virtual bool			DeinitializeInternal();

								ZEWindow();
								~ZEWindow();

	public:
		void*					GetHandle();
		ZEGROutput*				GetOutput();

		void					SetTitle(const ZEString& Title);
		const ZEString&			GetTitle();

		void					SetLeft(ZEInt Left);
		ZEInt					GetLeft();

		void					SetTop(ZEInt Top);
		ZEInt					GetTop();

		void					SetWidth(ZEUInt Width);
		ZEUInt					GetWidth();

		void					SetHeight(ZEUInt Height);
		ZEUInt					GetHeigth();

		void					SetVisible(bool Visible);
		bool					GetVisible();

		void					SetResizable(bool True);
		bool					GetResizable();

		void					SetBorderlessMode(bool Enabled);
		bool					GetBorderlessMode();

		void					SetFullScreen(bool Enabled);
		bool					GetFullscreen();

		void					SetHideCursor(bool Visibility);
		bool					GetHideCursor();

		void					SetLockCursor(bool Enabled);
		bool					GetLockCursor();

		ZEVector2				GetCursorPosition();

		void					Minimize();
		void					Restore();
		void					Maximize();

		void					Destroy();

		static ZEWindow*		CreateInstance();

};
