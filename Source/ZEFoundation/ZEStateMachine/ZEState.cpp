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

void ZEState::OnEntering(ZEState* From, ZEState* To, bool& Cancel)
{
	Cancel = false;
}

void ZEState::OnEntered(ZEState* From, ZEState* To)
{
}

void ZEState::OnLeaving(ZEState* From, ZEState* To, bool& Cancel)
{
	Cancel = false;
}

void ZEState::OnLeft(ZEState* From, ZEState* To)
{
}

void ZEState::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEState::GetName()
{
	return Name;
}

const ZEStateMachine& ZEState::GetOwner()
{
	return *Owner;
}

const ZEArray<ZEState*>& ZEState::GetTransitions()
{
	return Transitions;
}

bool ZEState::AddTransition(ZEState* State)
{
	zeDebugCheck(State == NULL, "State cannot be NULL.");

	if (State == NULL)
		return false;

	if (Owner == NULL)
		return false;

	if (Transitions.Exists(State))
		return false;

	if (Owner != State->Owner)
		return false;

	Transitions.Add(State);
	return true;
}

bool ZEState::RemoveTransition(ZEState* State)
{
	zeDebugCheck(State == NULL, "State cannot be NULL.");

	if (State == NULL)
		return false;

	if(!Transitions.Exists(State))
		return false;

	Transitions.DeleteValue(State);
	return true;
}

ZEState::ZEState()
{
	Name = "";
	Owner = NULL;
}

ZEState::~ZEState()
{
	if(Owner == NULL)
		return;

	for (ZESize I = 0; I < Owner->States.GetCount(); I++)
		Owner->States[I]->RemoveTransition(this);

	Owner->States.DeleteValue(this);
}

void ZEDelegatedState::OnEntering(ZEState* From, ZEState* To, bool& Cancel)
{
	Cancel = false;

	if (EnteringEvent != NULL)
		EnteringEvent(From, To, Cancel);
}

void ZEDelegatedState::OnEntered(ZEState* From, ZEState* To)
{
	if (EnteredEvent != NULL)
		EnteredEvent(From, To);
}

void ZEDelegatedState::OnLeaving(ZEState* From, ZEState* To, bool& Cancel)
{
	Cancel = false;

	if (LeavingEvent != NULL)
		LeavingEvent(From, To, Cancel);
}

void ZEDelegatedState::OnLeft(ZEState* From, ZEState* To)
{
	if (LeftEvent != NULL)
		LeftEvent(From, To);
}

void ZEDelegatedState::SetEnteringEvent(const ZEStateEventEntering& Event)
{
	EnteringEvent = Event;
}

const ZEStateEventEntering& ZEDelegatedState::GetEnteringEvent()
{
	return EnteringEvent;
}

void ZEDelegatedState::SetEnteredEvent(const ZEStateEventEntered& Event)
{
	EnteredEvent = Event;
}

const ZEStateEventEntered& ZEDelegatedState::GetEnteredEvent()
{
	return EnteredEvent;
}

void ZEDelegatedState::SetLeavingEvent(const ZEStateEventLeaving& Event)
{
	LeavingEvent = Event;
}

const ZEStateEventLeaving& ZEDelegatedState::GetLeavingEvent()
{
	return LeavingEvent;
}

void ZEDelegatedState::SetLeftEvent(const ZEStateEventLeft& Event)
{
	LeftEvent = Event;
}

const ZEStateEventLeft& ZEDelegatedState::GetLeftEvent()
{
	return LeftEvent;
}
