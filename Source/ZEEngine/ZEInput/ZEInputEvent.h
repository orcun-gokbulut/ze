//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputEvent.h
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
#ifndef	__ZE_INPUT_EVENT_H__
#define __ZE_INPUT_EVENT_H__

#include "ZETypes.h"
#include "ZEInput.h"
#include "ZEDS/ZEDelegate.h"
#include "ZEDS/ZEString.h"

class ZEInputEvent;

class ZEInputAction : public ZEInputValue
{
	public:
		ZEInt						Id;
		ZEInputEvent*				From;
};

typedef ZEDelegate<void (const ZEInputAction&)> ZEInputCallback;

class ZEInputEvent
{
	private:
		bool						Processed;

	public: 
		ZEInput						Input;
		ZEInputState				State;
		ZEInt						ActionId;
		ZEInputCallback				Callback;

		bool						IsAvailable() const;

		bool						Check() const;
		bool						Check(ZEInputAction& Action) const;

									ZEInputEvent();

		static ZEInputEvent			CreateAction(const ZEInput& Input, ZEInputState State, ZEInt ActionId);
		static ZEInputEvent			CreateAction(const ZEString& InputString, ZEInputState State, ZEInt ActionId);
		static ZEInputEvent			CreateButtonAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);
		static ZEInputEvent			CreateAxisAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign, ZEInputState State, ZEInt ActionId);
		static ZEInputEvent			CreatePOVAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);
		static ZEInputEvent			CreateSwitchAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);
		static ZEInputEvent			CreateVectorAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);
		static ZEInputEvent			CreateQuaternionAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);
		static ZEInputEvent			CreateCursorAction(const ZEString& DeviceName, ZEInputState State, ZEInt ActionId);
		static ZEInputEvent			CreateTextAction(const ZEString& DeviceName, ZEInt ActionId);

		static ZEInputEvent			CreateCallback(const ZEInput& Input, ZEInputState State, const ZEInputCallback& Callback);
		static ZEInputEvent			CreateCallback(const ZEString& InputString, ZEInputState State, const ZEInputCallback& Callback);
		static ZEInputEvent			CreateButtonCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);
		static ZEInputEvent			CreateAxisCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign, ZEInputState State, const ZEInputCallback& Callback);
		static ZEInputEvent			CreatePOVCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);
		static ZEInputEvent			CreateSwitchCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);
		static ZEInputEvent			CreateVectorCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);
		static ZEInputEvent			CreateQuaternionCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);
		static ZEInputEvent			CreateCursorCallback(const ZEString& DeviceName, ZEInputState State, const ZEInputCallback& Callback);
		static ZEInputEvent			CreateTextCallback(const ZEString& DeviceName, const ZEInputCallback& Callback);
};

#endif
