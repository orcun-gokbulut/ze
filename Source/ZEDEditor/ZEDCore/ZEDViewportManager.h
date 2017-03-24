//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewportManager.h
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

#include "ZEDComponent.h"

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEDelegate.h"
#include "ZEMeta/ZEEvent.h"

enum ZEDKeyboardKey;
enum ZEDMouseButton;
class ZEDViewport;
class ZERNPreRenderParameters;
class ZERNView;

class ZEDViewportManager : public ZEDComponent
{
	ZE_OBJECT
	private:
		ZEArray<ZEDViewport*>				Viewports;

											ZEDViewportManager();
											~ZEDViewportManager();

	public:
		const ZEArray<ZEDViewport*>			GetViewports();
		void								RegisterViewport(ZEDViewport* Viewport);
		void								UnregisterViewport(ZEDViewport* Viewport);


		void								Render();

		ZE_EVENT(							OnViewportFocusGained,(ZEDViewport* Viewport));
		ZE_EVENT(							OnViewportFocusLost,(ZEDViewport* Viewport));
		ZE_EVENT(							OnViewportDestroying,(ZEDViewport* Viewport));
		ZE_EVENT(							OnViewportDestroyed,(ZEDViewport* Viewport));
		ZE_EVENT(							OnViewportViewChangedEvent,(ZEDViewport* Viewport, const ZERNView& View));
		ZE_EVENT(							OnViewportKeyboardKeyPressed,(ZEDViewport* Viewport, ZEDKeyboardKey Key));
		ZE_EVENT(							OnViewportKeyboardKeyPressing,(ZEDViewport* Viewport, ZEDKeyboardKey Key));
		ZE_EVENT(							OnViewportKeyboardKeyReleased,(ZEDViewport* Viewport, ZEDKeyboardKey Key));
		ZE_EVENT(							OnViewportMouseClick,(ZEDViewport* Viewport, ZEDMouseButton Button));
		ZE_EVENT(							OnViewportMouseDoubleClick,(ZEDViewport* Viewport, ZEDMouseButton Button));
		ZE_EVENT(							OnViewportMouseMoved,(ZEDViewport* Viewport, const ZEVector2& Position));
		ZE_EVENT(							OnViewportMouseWheelMoved,(ZEDViewport* Viewport, ZEInt Value));
		ZE_EVENT(							OnViewportMouseButtonPressed,(ZEDViewport* Viewport, ZEDMouseButton Button));
		ZE_EVENT(							OnViewportMouseButtonPressing,(ZEDViewport* Viewport, ZEDMouseButton Button));
		ZE_EVENT(							OnViewportMouseButtonReleased,(ZEDViewport* Viewport, ZEDMouseButton Button));
		ZE_EVENT(							OnViewportPreRender,(ZEDViewport* Viewport, const ZERNPreRenderParameters& Parameters));

		static ZEDViewportManager*			CreateInstance();
};
