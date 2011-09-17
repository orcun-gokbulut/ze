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

ZEStateMachine::ZEStateMachine(void)
{
}

ZEStateMachine::~ZEStateMachine(void)
{
}

bool ZEStateMachine::AddState(ZEState *State)
{
	ZEState TemporaryState;
	memcpy(&TemporaryState,State,sizeof(ZEState));
	ZEStateMachine::StateArray.Add(TemporaryState);
	return true;
}

bool ZEStateMachine::AddTransaction(ZEState *From, ZEState *To, int Priority)
{
	ZETransaction TemporaryTransaction;
	TemporaryTransaction.Initialize(From,To);
	TemporaryTransaction.SetPriority(Priority);
	ZEStateMachine::TransactionArray.Add(TemporaryTransaction);
	return true;
}

bool ZEStateMachine::SetCurrentState(const ZEString& Name)
{
	for(int i=0;i<StateArray.GetSize();i++)
	{
		if(StateArray.GetItem(i).GetName().Equals(Name))
		{
			CurrentState = &StateArray.GetItem(i);
			return true;
		}
	}
	return false;
}

bool ZEStateMachine::Process()
{
	int TheBiggestPriority = 0;
	int TheChosenOne = -1;

	//Find transaction with higher priority
	for(int i=0;i<TransactionArray.GetSize();i++)
	{
		if(TransactionArray.GetItem(i).GetFromState() == CurrentState)
		{
			if(TransactionArray.GetItem(i).Evaluates())
			{
				if(TransactionArray.GetItem(i).GetFromState()->OnLeaving(&TransactionArray.GetItem(i)))
				{
					if(TransactionArray.GetItem(i).GetToState()->OnEntering(&TransactionArray.GetItem(i)))
					{
						if(TransactionArray.GetItem(i).GetPriority() >TheBiggestPriority)
						{
							TheBiggestPriority = TransactionArray.GetItem(i).GetPriority();
							TheChosenOne = i;
						}
					}
				}
			}
		}
	}
	if(TheChosenOne != -1)
	{
		TransactionArray.GetItem(TheChosenOne).GetFromState()->OnLeave(&TransactionArray.GetItem(TheChosenOne));
		TransactionArray.GetItem(TheChosenOne).GetToState()->OnEnter(&TransactionArray.GetItem(TheChosenOne));
		ZEStateMachine::CurrentState = TransactionArray.GetItem(TheChosenOne).GetToState();
		return true;
	}
	else
		return false;
}

ZEState* ZEStateMachine::GetCurrentState()
{
	return ZEStateMachine::CurrentState;
}

ZEState* ZEStateMachine::GetState(ZEString Name)
{
	for(int i=0;i<StateArray.GetSize();i++)
	{
		if(StateArray.GetItem(i).GetName().Equals(Name))
		{
			return &StateArray.GetItem(i);
		}
	}
}
