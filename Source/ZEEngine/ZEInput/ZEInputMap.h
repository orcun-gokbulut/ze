//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputMap.h
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
#ifndef	__ZE_INPUT_MAP_H__
#define __ZE_INPUT_MAP_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEInputEvent.h"

#define ZE_INPUT_MAP_MAX_INPUT_ACTION_COUNT 50

class ZEInputMap
{
	friend class ZEInputModule;
	private:
		ZEInputAction				Actions[ZE_INPUT_MAP_MAX_INPUT_ACTION_COUNT];
		ZESize						ActionCount;

	public:
		ZEArray<ZEInputEvent>		Events;	

		ZESize						GetActionCount() const;
		const ZEInputAction*		GetActions() const;

		bool						CheckAction(ZEInt ActionId) const;
		bool						CheckAction(ZEInt ActionId, ZEInputAction& Action) const;

		void						AddAction(const ZEInput& Input, ZEInputState State, ZEInt ActionId);
		void						AddAction(const ZEString& InputString, ZEInputState State, ZEInt ActionId);
		void						AddButtonAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);
		void						AddAxisAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign, ZEInputState State, ZEInt ActionId);
		void						AddPOVAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);
		void						AddSwitchAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);
		void						AddVectorAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);
		void						AddQuaternionAction(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, ZEInt ActionId);

		void						AddCallback(const ZEInput& Input, ZEInputState State, const ZEInputCallback& Callback);
		void						AddCallback(const ZEString& InputString, ZEInputState State, const ZEInputCallback& Callback);
		void						AddButtonCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);
		void						AddAxisCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputAxisSign Sign, ZEInputState State, const ZEInputCallback& Callback);
		void						AddPOVCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);
		void						AddSwitchCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);
		void						AddVectorCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);
		void						AddQuaternionCallback(const ZEString& DeviceName, ZEUInt32 Index, ZEInputState State, const ZEInputCallback& Callback);

		void						Update();

									ZEInputMap();
};

#endif
