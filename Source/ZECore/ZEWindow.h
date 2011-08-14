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
#ifndef	__ZE_WINDOW_H__
#define __ZE_WINDOW_H__

#include "ZESystemMessageHandler.h"

#define zeWindow ZEWindow::GetInstance()

enum ZEWindowType
{
	ZE_WT_DEFAULT			= 0,
	ZE_WT_FIXEDSIZE			= 1,
	ZE_WT_RESIZABLE			= 2,
	ZE_WT_COMPONENT			= 3,
	ZE_WT_FULLSCREEN		= 4,
};

class ZEWindowSystemMessageHandler : public ZESystemMessageHandler
{
	public:
		virtual bool			Callback(MSG* Message);
};

class ZEWindow
{
	protected:
		int						WindowPositionLeft, WindowPositionTop;
		int						WindowWidth, WindowHeight;
		ZEWindowType			WindowType;
		void*					WindowHandle;
		ZEWindowSystemMessageHandler SystemMessageHandler;

		bool					CreateMainWindow(const char* WindowTitle);
		bool					DestroyMainWindow();

	public:
		void					WindowGainedFocus();
		void					WindowLostFocus();
		void					WindowDestroyed();
		void					WindowResized(int Width, int Height);

		void					SetWindowType(ZEWindowType WindowType);
		ZEWindowType			GetWindowType();

		bool					SetComponentWindowHandle(void* Handle);

		void					SetWindowPosition(int Left, int Top);
		void					GetWindowPosition(int& Left, int& Top);

		void					SetWindowSize(int Width, int Height);
		void					GetWindowSize(int& Width, int& Height);

		void					ShowWindow();
		void					HideWindow();

		void*					GetHandle();
		void					ProcessMessages();

		bool					Initialize();
		void					Deinitialize();

		static ZEWindow*		GetInstance();
							
								ZEWindow();
								~ZEWindow();
};
#endif





