//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEState.cpp
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

#include "ZEState.h"
#include "ZEStateMachine.h"
#include "ZEStateTransition.h"

void ZEState::Entering(ZEStateTransition* Transition, bool& Reject)
{
	Reject = false;
	OnEntering(Transition, Reject);
}

void ZEState::Entered(ZEStateTransition* Transition)
{
	OnEntered(Transition);
}

void ZEState::Looping()
{
	OnLooping(this);
}

void ZEState::Leaving(ZEStateTransition* Transition, bool& Reject)
{
	Reject = false;
	OnLeaving(Transition, Reject);
}

void ZEState::Left(ZEStateTransition* Transition)
{
	OnLeft(Transition);
}

ZEStateMachine* ZEState::GetStateMachine() const
{
	return StateMachine;
}

void ZEState::SetName(const ZEString& Name)
{
	this->Name = Name;
	NameHash = Name.Hash();
}

const ZEString& ZEState::GetName() const
{
	return Name;
}


void ZEState::SetTransitionRule(ZEStateTransitionRule Rule)
{
	TransitionRule = Rule;
}

ZEStateTransitionRule ZEState::GetTransitionRule() const
{
	return TransitionRule;
}

const ZEArray<ZEStateTransition*>& ZEState::GetTransitionList() const
{
	return TransitionList;
}

bool ZEState::AddToTransitionList(ZEStateTransition* Transition)
{
	zeCheckError(Transition == NULL, "Cannot add transition to state. Transition is NULL. State Name: \"%s\".", GetName().ToCString());
	zeCheckError(Transition->State != NULL, "Cannot add transition to state. Transition is already added to a state. State Name: \"%s\". Transition Name: \"%s\".", 
		GetName().ToCString(), Transition->GetName().ToCString());

	Transition->State = this;
	TransitionList.Add(Transition);

	return true;
}

bool ZEState::RemoveFromTransitionList(ZEStateTransition* Transition)
{
	zeCheckError(Transition == NULL, "Cannot remove transition from state. Transition is NULL. State Name: \"%s\".", GetName().ToCString());
	zeCheckError(Transition->State != this, "Cannot remove transition from state. Transition does not belong to this state. State Name: \"%s\". Transition Name: \"%s\".", 
		GetName().ToCString(), Transition->GetName().ToCString());
		
	Transition->State = NULL;
	TransitionList.RemoveValue(Transition);

	return true;
}

void ZEState::Tick()
{
	ZEStateTransition* AutoTransitionTarget = NULL;
	for (ZESize I = 0; I < TransitionList.GetCount(); I++)
	{
		ZEStateTransition* Transition = TransitionList[I];

		if (!Transition->GetAutoTransition())
			continue;

		if (AutoTransitionTarget != NULL && AutoTransitionTarget->GetAutoTransitionPriority() > Transition->GetAutoTransitionPriority())
			continue;

		if (!Transition->CheckTransitionCondition())
			continue;

		AutoTransitionTarget = Transition;
	}

	if (AutoTransitionTarget != NULL)
		GetStateMachine()->Transfer(AutoTransitionTarget->GetTargetState());
	else
		Looping();
}

ZEState::ZEState()
{
	NameHash = 0;
	StateMachine = NULL;
	TransitionRule = ZE_STL_NONE;
}

ZEState::~ZEState()
{
	if(StateMachine == NULL)
		StateMachine->RemoveState(this);

	while (TransitionList.GetCount() != 0)
		delete TransitionList.GetFirstItem();
}
