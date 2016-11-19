//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMainWindow.h
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

#include "ZEDCore/ZEDComponent.h"

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDCommand.h"
#include "ZEDMenuManager.h"
#include "ZEDWindow.h"

#include <QMainWindow>

class ZEDMenu;
class ZEDWindow;
class ZEDToolbar;
class ZEDViewport;
class ZEDMenuManager;
class ZEDToolbarManager;
class Ui_ZEDMainWindow;

enum ZEDWindowDefault
{
	ZED_WD_NONE					= 0x00,
	ZED_WD_VISIBLE				= 0x01,
	ZED_WD_DOCK_LEFT			= 0x10,
	ZED_WD_DOCK_RIGHT			= 0x20,
	ZED_WD_DOCK_TOP				= 0x30,
	ZED_WD_DOCK_BOTTOM			= 0x40,
	ZED_WD_STACK_LEFT			= 0x50,
	ZED_WD_STACK_RIGHT			= 0x60,
	ZED_WD_STACK_TOP			= 0x70,
	ZED_WD_STACK_BOTTOM			= 0x80,

};

typedef ZEFlags ZEDWindowDefaults;

enum ZEDToolbarDefault
{
	ZED_TD_NONE					= 0x00,
	ZED_TD_VISIBLE				= 0x01,
	ZED_TD_PLACE_LEFT			= 0x10,
	ZED_TD_PLACE_RIGHT			= 0x20,
	ZED_TD_PLACE_TOP			= 0x30,
	ZED_TD_PLACE_BOTTOM			= 0x40,
};

typedef ZEFlags ZEDToolbarDefaults;

class ZEDMainWindow : public QObject, public ZEDComponent
{
	Q_OBJECT
	private:
		Ui_ZEDMainWindow*					Form;
		QMainWindow*						MainWindow;
		ZEDViewport*						Viewport;

		ZEArray<ZEDToolbar*>				Toolbars;
		ZEArray<ZEDWindow*>					Windows;

		ZEDMenuManager*						MenuManager;
		ZEDToolbarManager*					ToolbarManager;

		bool								eventFilter(QObject* Object, QEvent* Event);

		void								ToolbarMenuCallback(ZEDMenu* Menu);

		virtual bool						InitializeInternal();

											ZEDMainWindow();
											~ZEDMainWindow();

	public:
		QMainWindow*						GetMainWindow();
		ZEDMenuManager*						GetMenuManager();
		ZEDToolbarManager*					GetToolbarManager();

		const ZEArray<ZEDWindow*>&			GetWindows();
		void								AddWindow(ZEDWindow* Widget, ZEDWindowDefaults = ZED_WD_VISIBLE);
		void								RemoveWindow(ZEDWindow* Widget);

		const ZEArray<ZEDToolbar*>&			GetToolbars();
		void								AddToolbar(ZEDToolbar* Toolbar, ZEDToolbarDefaults Default = ZED_TD_VISIBLE | ZED_TD_PLACE_TOP);
		void								RemoveToolbar(ZEDToolbar* Toolbar);

		void								SetViewport(ZEDViewport* Viewport);
		ZEDViewport*						GetViewport();

		static ZEDMainWindow*				CreateInstance();
};
