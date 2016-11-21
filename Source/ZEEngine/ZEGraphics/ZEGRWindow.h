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

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"
#include "ZEDestroyable.h"

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

class ZEGROutput;

class ZEGRWindow : public ZEObject, public ZEInitializable, public ZEDestroyable
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEGRWindow)
	private:
		ZEUInt							Id;
		static ZEUInt					WindowCount;
		static ZEGRWindow*				LastCursorLock;

		ZEHolder<ZEGROutput>			Output;
		void*							Handle;

		ZEUInt							Width;
		ZEUInt							Height;

		ZEInt							Left;
		ZEInt							Top;

		bool							Enabled;
		bool							Visible;

		ZEString						Title;
		bool							TitleBar;
		bool							IconVisible;
		bool							CloseButton;
		bool							MinimizeButton;
		bool							MaximizeButton;
		bool							Resizable;
		bool							Bordered;
		bool							ShowInTaskbar;
		bool							AlwaysOnTop;
		bool							QuitWhenClosed;
		bool							ValidateQuit;
		bool							CursorVisible;
		bool							CursorLocked;
		bool							ManageInputAcquisition;
		bool							CursorVisibleState;
		bool							CursorLockedState;

		bool							FullScreen;
		bool							VSynchEnabled;

		bool							Focused;
		bool							Minimized;
		bool							Maximized;

		void							UpdateStyle();

	protected:
		virtual void					OnEnabled();
		virtual void					OnDisabled();
		virtual void					OnCreate();
		virtual void					OnDestroy();
		virtual void					OnFocusGain();
		virtual void					OnFocusLoose();
		virtual void					OnRestore();
		virtual void					OnMinimize();
		virtual void					OnMaximize();
		virtual void					OnWindowed();
		virtual void					OnFullScreen();
		virtual void					OnMove();
		virtual void					OnSize();

		virtual bool					InitializeInternal();
		virtual bool					DeinitializeInternal();

										ZEGRWindow();
										~ZEGRWindow();

	public:
		ZEUInt							GetId() const;
		void*							GetHandle() const;
		
		void							SetPosition(ZEInt Left, ZEInt Top);

		void							SetLeft(ZEInt Left);
		ZEInt							GetLeft() const;

		void							SetTop(ZEInt Top);
		ZEInt							GetTop() const;

		void							SetSize(ZEUInt Width, ZEUInt Height);

		void							SetWidth(ZEUInt Width);
		ZEUInt							GetWidth() const;
		
		void							SetHeight(ZEUInt Height);
		ZEUInt							GetHeight() const;

		void							SetFullScreen(bool FullScreen);
		bool							GetFullScreen() const;

		void							SetVSynchEnabled(bool Enabled);
		bool							GetVSynchEnabled() const;

		void							SetVisible(bool Visible);
		bool							GetVisible() const;

		void							SetEnabled(bool Enable);
		bool							GetEnabled() const;

		void							SetTitle(const ZEString& Title);
		const ZEString&					GetTitle() const;

		void							SetTitleBar(bool Enabled);
		bool							GetTitleBar() const;

		void							SetIconVisible(bool Visible);
		bool							GetIconVisible() const;

		void							SetMinimizeButton(bool Enabled);
		bool							GetMinimizeButton() const;

		void							SetMaximizeButton(bool Enabled);
		bool							GetMaximizeButton() const;

		void							SetCloseButton(bool Enabled);
		bool							GetCloseButton() const;

		void							SetResizable(bool Enabled);
		bool							GetResizable() const;

		void							SetBordered(bool Enabled);
		bool							GetBordered() const;

		void							SetShowInTaskbar(bool Enabled);
		bool							GetShowInTaskbar() const;

		void							SetAlwaysOnTop(bool Enabled);
		bool							GetAlwaysOnTop() const;

		void							SetQuitWhenClosed(bool Enabled);
		bool							GetQuitWhenClosed() const;
		
		void							SetValidateQuit(bool Enabled);
		bool							GetValidateQuit() const;

		void							SetCursorVisible(bool Visible);
		bool							GetCursorVisible() const;

		void							SetCursorLocked(bool Enabled);
		bool							GetCursorLocked() const;

		void							SetManageInputAcquisition(bool Enabled);
		bool							GetManageInputAcquisition() const;

		ZEGROutput*						GetOutput() const;
		
		bool							GetFocused() const;
		bool							GetMinimized() const;
		bool							GetMaximized() const;
		bool							GetRestored() const;

		void							Focus();
		void							Minimize();
		void							Maximize();
		void							Restore();

		void							WrapperResized(ZEUInt Width, ZEUInt Height);

		static ZEUInt					GetWindowCount();
		virtual ZESSize					HandleMessage(ZEUInt32 Message, ZESize Param1, ZESSize Param2);

		static ZEGRWindow*				WrapHandle(void* ExistingHandle);
		static ZEGRWindow*				CreateInstance();
};
