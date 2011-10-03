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

bool ZEStateMachine::AddState(ZEState* State)
{
	ZEState TemporaryState;
	memcpy(&TemporaryState, State, sizeof(ZEState));
	ZEStateMachine::StateArray.Add(TemporaryState);
	return true;
}

bool ZEStateMachine::AddTransaction(ZEState* From, ZEState* To, int Priority)
{
	ZETransaction TemporaryTransaction;
	TemporaryTransaction.Initialize(From, To);
	TemporaryTransaction.SetPriority(Priority);
	ZEStateMachine::TransactionArray.Add(TemporaryTransaction);
	return true;
}

bool ZEStateMachine::SetCurrentState(const ZEString& Name)
{
	for(size_t I = 0; I < StateArray.GetSize(); I++)
	{
		if(StateArray.GetItem(I).GetName().Equals(Name))
		{
			CurrentState = &StateArray.GetItem(I);
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
	for(size_t I = 0; I < TransactionArray.GetSize(); I++)
	{
		if(TransactionArray.GetItem(I).GetFromState() == CurrentState)
		{
			if(TransactionArray.GetItem(I).Evaluates())
			{
				if(TransactionArray.GetItem(I).GetFromState()->OnLeaving(&TransactionArray.GetItem(I)))
				{
					if(TransactionArray.GetItem(I).GetToState()->OnEntering(&TransactionArray.GetItem(I)))
					{
						if(TransactionArray.GetItem(I).GetPriority() >TheBiggestPriority)
						{
							TheBiggestPriority = TransactionArray.GetItem(I).GetPriority();
							TheChosenOne = I;
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
	for(size_t I = 0; I < StateArray.GetSize(); I++)
	{
		if(StateArray.GetItem(I).GetName().Equals(Name))
		{
			return &StateArray.GetItem(I);
		}
	}

	return NULL;
}
