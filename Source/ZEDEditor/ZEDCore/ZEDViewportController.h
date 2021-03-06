//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewportController.h
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

enum ZEDKeyboardKey;
enum ZEDMouseButton;
class ZEVector2;
class ZEDViewport;
class ZEDViewportMouseEvent;
class ZEDViewportKeyboardEvent;

class ZEDViewportController : public ZEDComponent
{
	ZE_OBJECT
	private:
		bool								LockCamera;
		float								StepSize;
		float								MouseSensivity;
		bool								Active;
		float								Rx, Ry, Rz;

		void								Viewport_OnKeyboardKeyPressing(ZEDViewport* Viewport, ZEDKeyboardKey Key);
		void								Viewport_OnMouseButtonPressed(ZEDViewport* Viewport, ZEDMouseButton Button);
		void								Viewport_OnMouseButtonReleased(ZEDViewport* Viewport, ZEDMouseButton Button);
		void								Viewport_OnMouseMoved(ZEDViewport* Viewport, const ZEVector2& Position);

		virtual bool						InitializeInternal() override;
		virtual bool						DeinitializeInternal() override;

											ZEDViewportController();
		virtual								~ZEDViewportController();

	public:
		void								SetLockCamera(bool Enabled);
		bool								GetLockCamera();

		void								SetStepSize(float StepSize);
		float								GetStepSize();

		void								SetMouseSensivity(float Sensivity);
		float								GetMouseSensivity();

		static ZEDViewportController*		CreateInstance();
};
