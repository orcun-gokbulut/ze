//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEStateMachine.cpp
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

#include "ZEStateMachine.h"
#include "ZEState.h"
#include "ZEStateTransition.h"

void ZEStateMachine::Pushed()
{

}

void ZEStateMachine::Popped()
{
	SetCurrentState(NULL);
}

const ZEArray<ZEState*>& ZEStateMachine::GetStates()
{
	return States;
}


ZEState* ZEStateMachine::GetState(const ZEString& Name)
{
	ZEUInt NameHash = Name.Hash();
	for (ZESize I = 0; I < States.GetCount(); I++)
	{
		if (States[I]->NameHash == NameHash &&
			States[I]->Name == Name)
		{
			return States[I];
		}
	}

	return NULL;
}

bool ZEStateMachine::AddState(ZEState* State)
{
	zeCheckError(State == NULL, false, "Cannot add state. State is NULL.");
	zeCheckError(State->StateMachine != NULL, false, "Cannot add state. State is already added to a state machine. State Name: \"%s\".", State->GetName().ToCString());

	State->StateMachine = this;
	States.Add(State);

	return true;
}

bool ZEStateMachine::RemoveState(ZEState* State)
{
	zeCheckError(State == NULL, false, "Cannot remove state. State is NULL.");
	zeCheckError(State->StateMachine != NULL, false, "Cannot remove state. State does not belong to this state machine. State Name: \"%s\".", State->GetName().ToCString());
	
	State->StateMachine = NULL;
	States.RemoveValue(State);
	return true;
}

void ZEStateMachine::SetCurrentState(const ZEString& TargetStateName)
{
	ZEState* TargetState = GetState(TargetStateName);
	zeCheckError(TargetState == NULL, ZE_VOID, "Cannot change current state. Cannot find TargetState. Target State Name: \"%s\".", TargetStateName.ToCString());

	SetCurrentState(TargetState);
}

void ZEStateMachine::SetCurrentState(ZEState* TargetState)
{
	zeCheckError(TargetState != NULL && TargetState->StateMachine == NULL, ZE_VOID, "Cannot change current state. TargetState does not belong to this state machine.");

	if (CurrentState != NULL)
		CurrentState->Left(NULL);

	CurrentState = TargetState;

	if (TargetState != NULL)
		TargetState->Entered(NULL);
}

const ZEState* ZEStateMachine::GetCurrentState()
{
	return CurrentState;
}

bool ZEStateMachine::TransferInnerState(const ZEString& TargetStateName)
{
	ZEState* TargetState = GetState(TargetStateName);
	zeCheckError(TargetState == NULL, false, "Cannot transfer state. Cannot find TargetState. Target State Name: \"%s\".", TargetStateName.ToCString());

	return Transfer(TargetState);	
}

bool ZEStateMachine::TransferInnerState(ZEState* TargetState)
{
	zeCheckError(TargetState == NULL, false, "Cannot transition to inner state. Inner TargetState cannot be NULL.");
	zeCheckError(TargetState->StateMachine != this, false, "Cannot transition to inner state. TargetState does not belong to this state machine.");
	zeCheckError(TargetState == CurrentState, false, "Cannot transition to inner state. Recursion detected.");

	ZEStateTransition PlaceHolderTransition;
	ZEStateTransition* Transition = NULL;
	if (CurrentState != NULL)
	{
		if (CurrentState->GetTransitionRule() == ZE_STL_ALLOW_LIST)
		{
			const ZEArray<ZEStateTransition*>& TransitionList = CurrentState->GetTransitionList();
			for (ZESize I = 0; I < TransitionList.GetCount(); I++)
			{
				if (TransitionList[I]->GetType() != ZE_STT_HIERARCHICAL && TransitionList[I]->GetType() == ZE_STT_BOTH)
					continue;

				if (TransitionList[I]->GetTargetState() != TargetState)
					continue;

				Transition = NULL;
				break;
			}
		}
		else if (CurrentState->GetTransitionRule() == ZE_STL_DENY_LIST)
		{
			const ZEArray<ZEStateTransition*>& TransitionList = CurrentState->GetTransitionList();
			for (ZESize I = 0; I < TransitionList.GetCount(); I++)
			{
				if (TransitionList[I]->GetType() != ZE_STT_HIERARCHICAL && TransitionList[I]->GetType() == ZE_STT_BOTH)
					continue;

				if (TransitionList[I]->GetTargetState() != TargetState)
					continue;
				
				return false;
			}
		}

		ZEStateTransition PlaceHolderTransition;
		if (Transition == NULL)
		{
			PlaceHolderTransition.State = CurrentState;
			PlaceHolderTransition.TargetState = TargetState;
			Transition = &PlaceHolderTransition;
		}
	}

	bool Rejected;
	TargetState->Entering(Transition, Rejected);

	if (Rejected)
		return false;

	StateStack.Push(CurrentState);

	CurrentState = TargetState;
	if (CurrentState != NULL)
		CurrentState->Entered(Transition);

	return true;
}

bool ZEStateMachine::ExitInnerState()
{
	ZEStateTransition Transition;
	Transition.State = CurrentState;
	Transition.TargetState = StateStack.GetFirstItem();
	Transition.Type = ZE_STT_HIERARCHICAL;

	bool Rejected;
	CurrentState->Leaving(&Transition, Rejected);

	if (Rejected)
		return false;

	CurrentState->Left(&Transition);

	CurrentState = StateStack.Pop();

	return true;
}

bool ZEStateMachine::Transfer(const ZEString& TargetStateName)
{
	ZEState* TargetState = GetState(TargetStateName);
	zeCheckError(TargetState == NULL, false, "Cannot transfer state. Cannot find TargetState. Target State Name: \"%s\".", TargetStateName.ToCString());

	return Transfer(TargetState);
}

bool ZEStateMachine::Transfer(ZEState* TargetState)
{
	zeCheckError(TargetState != NULL && TargetState->StateMachine != this, false, "Cannot change current state. TargetState does not belong to this state machine.");

	if (TargetState == CurrentState)
	{
		TargetState->Looping();
		return true;
	}
	
	ZEStateTransition PlaceHolderTransition;
	ZEStateTransition* Transition = NULL;
	if(CurrentState != NULL)
	{
		if (CurrentState->GetTransitionRule() == ZE_STL_ALLOW_LIST)
		{
			const ZEArray<ZEStateTransition*>& TransitionList = CurrentState->GetTransitionList();
			for (ZESize I = 0; I < TransitionList.GetCount(); I++)
			{
				if (TransitionList[I]->GetType() != ZE_STT_PLANAR && TransitionList[I]->GetType() == ZE_STT_BOTH)
					continue;

				if (TransitionList[I]->GetTargetState() != TargetState)
					continue;

				Transition = NULL;
				break;
			}
		}
		else if (CurrentState->GetTransitionRule() == ZE_STL_DENY_LIST)
		{
			const ZEArray<ZEStateTransition*>& TransitionList = CurrentState->GetTransitionList();
			for (ZESize I = 0; I < TransitionList.GetCount(); I++)
			{
				if (TransitionList[I]->GetType() != ZE_STT_PLANAR && TransitionList[I]->GetType() == ZE_STT_BOTH)
					continue;

				if (TransitionList[I]->GetTargetState() != TargetState)
					continue;

				return false;
			}
		}

		ZEStateTransition PlaceHolderTransition;
		if (Transition == NULL)
		{
			PlaceHolderTransition.State = CurrentState;
			PlaceHolderTransition.TargetState = TargetState;
			Transition = &PlaceHolderTransition;
		}
		
		bool Rejected;
		CurrentState->Leaving(Transition, Rejected);

		if (Rejected)
			return false;
	}

	bool Rejected;
	if (TargetState != NULL)
		TargetState->Entering(Transition, Rejected);

	if (Rejected)
		return false;

	ZEState* OldState = CurrentState;
	CurrentState = TargetState;

	if (OldState != NULL)
		OldState->Left(Transition);

	if (CurrentState != NULL)
		CurrentState->Entered(Transition);

	return true;
}

void ZEStateMachine::Tick()
{
	/*for (ZESize I = 0; I < StateStack.GetCount(); I++)
	{
		if (StateStack[I] == NULL)
			continue;

		StateStack[I]->Tick();
	}*/

	if (CurrentState != NULL)
		CurrentState->Tick();
}

ZEStateMachine::ZEStateMachine()
{
	CurrentState = NULL;
}

ZEStateMachine::~ZEStateMachine()
{
	while (States.GetCount() != 0)
		delete States.GetFirstItem();
}
