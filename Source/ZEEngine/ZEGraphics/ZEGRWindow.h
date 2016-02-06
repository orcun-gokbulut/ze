//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRWindow.h
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
#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZERectangle.h"
#include "ZEPointer/ZEHolder.h"

enum ZEGRWindowType : ZEUInt8
{
	ZEGR_WT_NORMAL		= 0,
	ZEGR_WT_POPUP		= 1
};

class ZEGRMonitor;
class ZEGROutput;

class ZEGRWindowStyle
{
	public:
		ZEGRWindowType			Type;
		bool					TitleBar;
		bool					WindowMenu;
		bool					MinimizeButton;
		bool					MaximizeButton;
		bool					Resizable;
		bool					Bordered;
		bool					ShowInTaskbar;
		bool					AlwaysOnTop;

								ZEGRWindowStyle();
};

class ZEGRWindow : public ZEInitializable
{
	private:
		ZEUInt					Id;
		static ZEUInt			WindowCount;
		static ZEGRWindow*		LastCursorLock;

		ZEHolder<ZEGROutput>	Output;

		void*					Handle;
		ZEString				Title;
		ZEGRWindowStyle			Style;

		ZEUInt					Width;
		ZEUInt					Height;

		ZEInt					PositionX;
		ZEInt					PositionY;

		bool					Enabled;
		bool					Focused;
		bool					Minimized;
		bool					Maximized;

		bool					FullScreen;
		bool					VSynchEnable;

	protected:
		virtual void			OnEnabled();
		virtual void			OnDisabled();
		virtual void			OnCreate();
		virtual void			OnDestroy();
		virtual void			OnFocusGain();
		virtual void			OnFocusLoose();
		virtual void			OnRestore();
		virtual void			OnMinimize();
		virtual void			OnMaximize();
		virtual void			OnWindowed();
		virtual void			OnFullScreen();
		virtual void			OnMove();
		virtual void			OnSize();

		virtual bool			InitializeSelf();
		virtual void			DeinitializeSelf();

								ZEGRWindow();
		virtual					~ZEGRWindow();

	public:
		ZEUInt					GetId() const;
		void*					GetHandle() const;
		
		void					SetStyle(const ZEGRWindowStyle& Style);
		const ZEGRWindowStyle&	GetStyle() const;
		
		void					SetTitle(const ZEString& Title);
		const ZEString&			GetTitle() const;

		ZEUInt					GetWidth() const;
		ZEUInt					GetHeight() const;

		ZEInt					GetPositionX() const;
		ZEInt					GetPositionY() const;

		void					SetPosition(ZEInt X, ZEInt Y);
		void					GetPosition(ZEInt& X, ZEInt& Y) const;
		void					GetPosition(ZEVector2& Position) const;

		void					SetSize(ZEInt Width, ZEInt Height);
		void					GetSize(ZEInt& Width, ZEInt& Height) const;
		void					GetSize(ZEVector2& Size) const;

		ZERectangle				GetRectangle() const;

		void					SetFullScreen(bool FullScreen);
		bool					GetFullScreen() const;

		void					SetVSynchEnable(bool VSynchEnable);
		bool					GetVSynchEnable() const;

		void					SetEnable(bool Enable);
		bool					GetEnable() const;
		
		ZEGRMonitor*			GetContainingMonitor() const;
		ZEGROutput*				GetOutput() const;

		void					SetCursorLock(bool Enabled);
		bool					GetCursorLock() const;

		bool					GetFocused() const;
		bool					GetMinimized() const;
		bool					GetMaximized() const;
		bool					GetRestored() const;

		void					Focus();
		void					Minimize();
		void					Maximize();
		void					Restore();

		virtual ZESSize			HandleMessage(ZEUInt32 Message, ZESize Param1, ZESSize Param2);

		virtual bool			InitializeEmbedded(void* ExistingHandle);
		void					Show();
		virtual void			Destroy();

		static ZEUInt			GetWindowCount();
		static ZEGRWindow*		CreateInstance();
};
