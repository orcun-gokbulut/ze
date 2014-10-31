//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNodeStateMachine.cpp
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

#include "ZEModelAnimationNodeStateMachine.h"
#include "ZEModelAnimationGraph.h"
#include "ZEModelAnimationNodeState.h"
#include "ZEModelAnimationNodeTransition.h"

bool ZEModelAnimationNodeStateMachine::GenerateOutput(ZEModelAnimationFrame& output)
{
	if (CurrentState == NULL)
		return false;
	
	return CurrentState->GetOutput(output);
}

void ZEModelAnimationNodeStateMachine::ProcessSelf(float elapsedTime)
{

}

ZEModelAnimationNodeStateMachine::ZEModelAnimationNodeStateMachine()
{
	CurrentState = NULL;
	CurrentlyInTransition = false;
}

ZEModelAnimationNodeStateMachine::~ZEModelAnimationNodeStateMachine()
{

}

bool ZEModelAnimationNodeStateMachine::SetCurrentState(ZEModelAnimationNodeState* state, bool forced)
{
	if (state == NULL)
		return false;

	if (!InputNodes.Exists(state))
		return false;

	if (forced)
	{
		if (CurrentlyInTransition)
		{
			((ZEModelAnimationNodeTransition*)CurrentState)->Reset();
			CurrentlyInTransition = false;
		}

		CurrentState = state;
		return true;
	}

	if(CurrentState == NULL)
	{
		CurrentState = state;
		return true;
	}

	if (CurrentlyInTransition)
	{
		if (((ZEModelAnimationNodeTransition*)CurrentState)->IsTriggered())
			return false;

		CurrentState = state;
		CurrentlyInTransition = false;
		return true;
	}
	
	ZEModelAnimationNodeTransition* transition = ((ZEModelAnimationNodeState*)CurrentState)->GetTransition(state);

	if(transition == NULL)
		return false;

	CurrentState = transition;
	transition->Trigger();
	CurrentlyInTransition = true;

	return true;

}

ZEModelAnimationNodeState* ZEModelAnimationNodeStateMachine::GetCurrentState() const
{
	return (CurrentlyInTransition) ? ((ZEModelAnimationNodeTransition*)CurrentState)->GetDestinationNode() : ((ZEModelAnimationNodeState*)CurrentState);
}

ZEModelAnimationNodeState* ZEModelAnimationNodeStateMachine::CreateStateNode()
{
	if (GetOwner() == NULL)
		return NULL;

	ZEModelAnimationNodeState* stateNode = new ZEModelAnimationNodeState();
	GetOwner()->RegisterAnimationNode(stateNode);
	AddInputNode(stateNode);
	States.Add(stateNode);

	return stateNode;
}

ZEModelAnimationNodeState* ZEModelAnimationNodeStateMachine::GetStateNode(const ZEString& name) const
{
	for (ZESize I = 0; I < States.GetCount(); I++)
		if (strcmp(States[I]->GetName(), name) == 0)
			return (ZEModelAnimationNodeState*)States[I];
	
	return NULL;
}

const ZEArray<ZEModelAnimationNodeState*>& ZEModelAnimationNodeStateMachine::GetStates() const
{
	return States;
}

bool ZEModelAnimationNodeStateMachine::DeleteStateNode(ZEModelAnimationNodeState* node)
{
	if (GetOwner() == NULL)
		return false;

	if (!RemoveInputNode(node))
		return false;

	States.DeleteValue(node);

	if (CurrentState == node)
		CurrentState = NULL;

	ZESize count = node->Transitions.GetCount();

	for (ZESize I = 0; I < count; I++)
		DeleteTransitionNode(node->Transitions[0]->GetSourceNode(),node->Transitions[0]->GetDestinationNode());
			
	for (ZESize I = 0; I < Transitions.GetCount(); I++)
	{
		if (Transitions[I]->GetDestinationNode() == node)
		{
			DeleteTransitionNode(Transitions[I]);
			I--;
		}
	}

	GetOwner()->UnregisterAnimationNode(node);
	node->Destroy();

	return true;
}

bool ZEModelAnimationNodeStateMachine::CreateTransitionNode(ZEModelAnimationNodeState* sourceState, ZEModelAnimationNodeState* destinationState, float transitionTimeInSeconds)
{
	if (sourceState->GetOutputNode() != this || destinationState->GetOutputNode() != this)
		return false;

	ZEModelAnimationNodeTransition* transition = new ZEModelAnimationNodeTransition();
	GetOwner()->RegisterAnimationNode(transition);

	if (!transition->SetSourceNode(sourceState) || !transition->SetDestinationNode(destinationState))
	{
		GetOwner()->UnregisterAnimationNode(transition);
		transition->Destroy(); 
		return false;
	}

	AddInputNode(transition);
	Transitions.Add(transition);
	sourceState->Transitions.Add(transition);
	transition->SetTransitionTime(transitionTimeInSeconds);

	return true;
}

ZEModelAnimationNodeTransition* ZEModelAnimationNodeStateMachine::GetTransitionNode(ZEModelAnimationNodeState* sourceState, ZEModelAnimationNodeState* destinationState) const
{
	return sourceState->GetTransition(destinationState);
}

const ZEArray<ZEModelAnimationNodeTransition*>& ZEModelAnimationNodeStateMachine::GetTransitions() const
{
	return Transitions;
}

bool ZEModelAnimationNodeStateMachine::DeleteTransitionNode(ZEModelAnimationNodeState* sourceState, ZEModelAnimationNodeState* destinationState)
{
	if (sourceState->GetOutputNode() != this || destinationState->GetOutputNode() != this)
		return false;

	ZEModelAnimationNodeTransition* transition = sourceState->GetTransition(destinationState);
	
	return DeleteTransitionNode(transition);
}

bool ZEModelAnimationNodeStateMachine::DeleteTransitionNode(ZEModelAnimationNodeTransition* node)
{
	if (node == NULL)
		return false;

	if (!Transitions.Exists(node))
		return false;

	if (node->IsTriggered())
		SetCurrentState(node->GetDestinationNode(), true);

	node->GetSourceNode()->Transitions.DeleteValue(node);
	RemoveInputNode(node);
	Transitions.DeleteValue(node);
	GetOwner()->UnregisterAnimationNode(node);
	node->Destroy();

	return true;
}

ZEModelAnimationNodeStateMachine* ZEModelAnimationNodeStateMachine::CreateInstance()
{
	return new ZEModelAnimationNodeStateMachine();
}
