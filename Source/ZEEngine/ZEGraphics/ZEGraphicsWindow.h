//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsWindow.h
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

#ifndef __ZE_GRAPHICS_WINDOW_H__
#define __ZE_GRAPHICS_WINDOW_H__

#include "ZETypes.h"
#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZERectangle.h"
#include "ZEThread/ZEMutex.h"

class ZEGraphicsWindow;

class ZEGraphicsCursor
{
	protected:
		static bool			Locked;
		static bool			Visible;

							ZEGraphicsCursor();
							~ZEGraphicsCursor();
	public:
		static bool			SetVisible(bool Value);
		static bool			GetVisible();

		static bool			SetPosition(ZEInt PosX, ZEInt PosY);
		static bool			GetPosition(ZEInt& PosX, ZEInt& PosY);
		static bool			GetPosition(ZEGraphicsWindow* Window, ZEInt& PosX, ZEInt& PosY);

		static bool			SetLockRectangle(const ZERectangle* Rectangle);
		static bool			GetLockRectangle(ZERectangle& Rectangle);

		static bool			GetLocked();

		static bool			CollisionCheck(const ZERectangle& Rectangle);
		static bool			CollisionCheck(const ZEGraphicsWindow* Window);
};

enum ZEWindowType
{
	ZE_GWT_CAPTION		= 0,
	ZE_GWT_POPUP		= 1,
	ZE_GWT_UNKNOWN		= 2
};
 
struct ZEWindowStyle
{
	ZEWindowType		Type;
	union
	{		
		struct ZEWindowCaption
		{
			bool		OnTop;
			bool		Resizable;
			bool		Minimizable;
			bool		Maximizable;
		}	Caption;
		
		struct ZEWindowPopup
		{
			bool		OnTop;
			bool		Bordered;
			bool		Resizable;
		}	Popup;
		
		struct ZEWindowUnknown
		{
			ZEUInt32	Properties0;
			ZEUInt32	Properties1;
		}	Unknown;
	};
};

#define ZE_GWF_NONE					0
#define ZE_GWF_CREATED				1 << 0
#define ZE_GWF_DESTROYED			1 << 1
#define ZE_GWF_RESIZED				1 << 2
#define ZE_GWF_WINDOWED				1 << 3
#define ZE_GWF_FULLSCREEN			1 << 4
typedef ZEFlagsBase<ZEUInt32>		ZEGraphicsWindowFlags;

class ZETexture2D;
class ZERenderTarget;
class ZEGraphicsMonitor;
class ZEDepthStencilBuffer;

class ZEGraphicsWindow
{
	protected:
		static ZEUInt						WindowCount;
		static ZEGraphicsWindow*			LastCursorLock;

		ZEUInt								Id;

		void*								Handle;
		ZEString							Title;
		ZEWindowStyle						Style;

		ZEMutex								FlagLock;
		ZEGraphicsWindowFlags				Flags;

		ZEInt								Width;
		ZEInt								Height;

		ZEInt								PositionX;
		ZEInt								PositionY;

		bool								Enabled;
		bool								Focused;
		bool								Minimized;
		bool								Maximized;

		bool								FullScreen;
		const ZEGraphicsMonitor*			FullScreenMonitor;

		bool								VSynchEnabed;

		virtual void						OnEnable();
		virtual void						OnDisable();
		virtual void						OnCreate();
		virtual void						OnDestroy();
		virtual void						OnFocusGain();
		virtual void						OnFocusLoose();
		virtual void						OnRestore();
		virtual void						OnMinimize();
		virtual void						OnMaximize();
		virtual void						OnWindowed();
		virtual void						OnFullScreen();
		virtual void						OnMove();
		virtual void						OnSize();
		
											ZEGraphicsWindow();
		virtual								~ZEGraphicsWindow();

	public:
		ZEUInt								GetId() const;
		void*								GetHandle() const;
		
		virtual bool						SetStyle(const ZEWindowStyle& Style);
		const ZEWindowStyle&				GetStyle() const;
		
		virtual bool						SetTitle(const char* String);
		const char*							GetTitle() const;

		virtual bool						SetPosition(ZEInt X, ZEInt Y);
		void								GetPosition(ZEInt& X, ZEInt& Y) const;
		void								GetPosition(ZEVector2& Position) const;

		virtual bool						SetSize(ZEInt Width, ZEInt Height);
		void								GetSize(ZEInt& Width, ZEInt& Height) const;
		void								GetSize(ZEVector2& Size) const;

		void								GetRectangle(ZERectangle& Rectangle) const;

		virtual bool						SetFullScreen(bool Value, const ZEGraphicsMonitor* Monitor = NULL);
		bool								GetFullScreen() const;

		virtual bool						SetVSynchEnabed(bool Value);
		bool								GetVSynchEnabed() const;

		const ZEGraphicsMonitor*			GetContainingMonitor() const;

		virtual void						Enable();
		virtual void						Disable();
		bool								IsDisabled() const;
		
		virtual void						Focus();
		bool								IsFocused() const;

		virtual bool						Restore();
		bool								IsRestored() const;
		virtual bool						Maximize();
		bool								IsMaximized() const;
		virtual bool						Minimize();
		bool								IsMinimized() const;

		virtual bool						SetCursorLock(bool Value);
		bool								GetCursorLock() const;

		virtual ZESSize						HandleMessage(ZEUInt32 Message, ZESize Param1, ZESSize Param2);

		virtual bool						Initialize();
		virtual bool						Initialize(void* Handle);
		bool								IsInitialized() const;

		virtual bool						DeInitialize();
		virtual void						Destroy();

		virtual bool						Update();
		virtual bool						Present() = 0;

		virtual const ZETexture2D*			GetBackBufferTexture() = 0;
		virtual const ZERenderTarget*		GetBackBuffer() = 0;
		virtual const ZEDepthStencilBuffer*	GetDepthBuffer() = 0;

		static ZEUInt						GetWindowCount();
		static ZEGraphicsWindow*			CreateInstance();
};

#endif
