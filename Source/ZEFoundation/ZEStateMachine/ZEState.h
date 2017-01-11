//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEState.h
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

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEDelegate.h"
#include "ZEMeta/ZEEnumerator.h"
#include "ZEMeta/ZEEvent.h"

class ZEStateMachine;
class ZEStateTransition;

ZE_ENUM(ZEStateTransitionRule)
{
	ZE_STL_NONE,
	ZE_STL_ALLOW_LIST,
	ZE_STL_DENY_LIST
};

class ZEState : public ZEObject
{
	ZE_OBJECT
	friend class ZEStateMachine;
	private:
		ZEString							Name;
		ZEUInt								NameHash;
		ZEStateMachine*						StateMachine;
		ZEStateTransitionRule				TransitionRule;
		ZEArray<ZEStateTransition*>			TransitionList;

		void								StriptStateFromTransition(ZEState* State);

	protected:
		virtual void						Entering(ZEStateTransition* Transition, bool& Cancel);
		virtual void						Entered(ZEStateTransition* Transition);
		virtual void						Looping();
		virtual void						Leaving(ZEStateTransition* Transition, bool& Cancel);
		virtual void						Left(ZEStateTransition* Transition);

	public:
		ZEStateMachine*						GetStateMachine() const;

		void								SetName(const ZEString& Name);
		const ZEString&						GetName() const;

		void								SetTransitionRule(ZEStateTransitionRule Rule);
		ZEStateTransitionRule				GetTransitionRule() const;

		const ZEArray<ZEStateTransition*>&	GetTransitionList() const;
		virtual bool						AddToTransitionList(ZEStateTransition* Transition);
		virtual bool						RemoveFromTransitionList(ZEStateTransition* Transition);

		ZE_EVENT(							OnEntering,(ZEStateTransition* Transition, bool& Reject));
		ZE_EVENT(							OnEntered,(ZEStateTransition* Transition));
		ZE_EVENT(							OnLooping,(ZEState* Transition));
		ZE_EVENT(							OnLeaving,(ZEStateTransition* Transition, bool& Reject));
		ZE_EVENT(							OnLeft,(ZEStateTransition* Transition));

		void								Tick();

											ZEState();
		virtual								~ZEState();
};
