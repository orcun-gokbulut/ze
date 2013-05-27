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

const ZEArray<ZEState*>& ZEStateMachine::GetStates()
{
	return States;
}

bool ZEStateMachine::AddState(ZEState* State)
{
	zeDebugCheck(State == NULL, "State cannot be NULL.");

	if (State == NULL)
		return false;

	if(States.Exists(State))
		return false;

	if(State->Owner != NULL)
		return false;

	States.Add(State);
	State->Owner = this;

	return true;
}

bool ZEStateMachine::DeleteState(ZEState* State)
{
	zeDebugCheck(State == NULL, "State cannot be NULL.");

	if (State == NULL)
		return false;

	if (State->Owner != this)
		return false;

	delete State;

	return true;
}

bool ZEStateMachine::SetCurrentState(ZEState* TargetState, bool Forced)
{
	zeDebugCheck(TargetState == NULL, "TargetState cannot be NULL.");

	if (TargetState == NULL)
		return false;

	zeDebugCheck(TargetState->Owner != this, "TargetState is not member of this state machine.");

	if (Forced)
	{
		CurrentState = TargetState;
		return true;
	}

	if (TargetState->Owner != this)
		return false;

	if(CurrentState != NULL)
	{
		if(!CurrentState->Transitions.Exists(TargetState))
			return false;

		bool CancelLeaving;
		CurrentState->OnLeaving(CurrentState, TargetState, CancelLeaving);
	
		if (CancelLeaving)
			return false;
	}

	bool CancelEntering;
	TargetState->OnEntering(CurrentState, TargetState, CancelEntering);

	if (CancelEntering)
		return false;

	ZEState* OldState = CurrentState;
	CurrentState = TargetState;

	if (OldState != NULL)
		OldState->OnLeft(OldState, CurrentState);

	CurrentState->OnEntered(OldState, CurrentState);

	return true;
}

const ZEState* ZEStateMachine::GetCurrentState()
{
	return CurrentState;
}

ZEStateMachine::ZEStateMachine()
{
	CurrentState = NULL;
}

ZEStateMachine::~ZEStateMachine()
{
	for (ZESize I = 0; I < States.GetCount(); I++)
	{
		delete States[I];
	}
}
