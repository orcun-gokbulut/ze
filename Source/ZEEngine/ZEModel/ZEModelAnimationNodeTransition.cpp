//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNodeTransition.cpp
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

#include "ZEModelAnimationNodeTransition.h"
#include "ZEModelAnimationNodeState.h"
#include "ZEModelAnimationNodeStateMachine.h"

void ZEModelAnimationNodeTransition::SetInputNodeCount(ZESize count)
{
	InputNodes.SetCount(count);
	InputNodes.Fill(NULL);
}

bool ZEModelAnimationNodeTransition::AddInputNode(ZEModelAnimationNode* node)
{
	if (node == NULL)
		return false;

	if (node->GetOwner() == NULL)
		return false;

	if (this->GetOwner() == NULL)
		return false;

	if (node->GetOwner() != this->GetOwner())
		return false;

	if (InputNodes.Exists(node))
		return false;

	InputNodes.Add(node);

	return true;
}

bool ZEModelAnimationNodeTransition::SetInputNode(ZESize index, ZEModelAnimationNode* node)
{
	if (index >= InputNodes.GetCount())
		return false;

	if (this->GetOwner() == NULL)
		return false;

	if (node != NULL)
	{
		if (node->GetOwner() == NULL)
			return false;

		if (node->GetOwner() != this->GetOwner())
			return false;

		if (InputNodes.Exists(node))
			return false;
	}

	if (InputNodes[index] != NULL)
		InputNodes.DeleteValue(InputNodes[index]);

	InputNodes.SetItem(index, node);

	return true;
}

bool ZEModelAnimationNodeTransition::RemoveInputNode(ZEModelAnimationNode* node)
{
	if (node == NULL)
		return false;

	if (node->GetOwner() == NULL)
		return false;

	if (this->GetOwner() == NULL)
		return false;

	if (node->GetOwner() != this->GetOwner())
		return false;

	if (!InputNodes.Exists(node))
		return false;

	InputNodes.DeleteValue(node);

	return true;
}

bool ZEModelAnimationNodeTransition::GenerateOutput(ZEModelAnimationFrame& output)
{
	return ZEModelAnimationNodeBlend::GenerateOutput(output);
}

void ZEModelAnimationNodeTransition::ProcessSelf(float elapsedTime)
{
	if (Triggered)
	{
		RemainingTransitionTime -= elapsedTime;
		SetBlendFactor(RemainingTransitionTime/TotalTransitionTime);

		if (RemainingTransitionTime <= 0.0f)
		{
			Reset();
			((ZEModelAnimationNodeStateMachine*)GetOutputNode())->SetCurrentState((ZEModelAnimationNodeState*)InputNodes[1]);
		}
	}
}

bool ZEModelAnimationNodeTransition::IsTriggered() const
{
	return Triggered;
}

void ZEModelAnimationNodeTransition::SetTransitionTime(float seconds)
{
	TotalTransitionTime = seconds * 1000.0f;
	RemainingTransitionTime = TotalTransitionTime;
}

float ZEModelAnimationNodeTransition::GetTransitionTime() const
{
	return TotalTransitionTime / 1000.0f;
}

float ZEModelAnimationNodeTransition::GetRemainingTransitionTime() const
{
	return RemainingTransitionTime / 1000.0f;
}

void ZEModelAnimationNodeTransition::Reset()
{
	Triggered = false;
	RemainingTransitionTime = TotalTransitionTime;
}

bool ZEModelAnimationNodeTransition::Trigger()
{
	if (Triggered)
		return false;

	Triggered = true;
	SetBlendFactor(0.0f);

	return true;
}

bool ZEModelAnimationNodeTransition::SetDestinationNode(ZEModelAnimationNodeState* node)
{
	if (node == NULL)
		return false;

	return SetInputNodeB((ZEModelAnimationNode*)node);
}

ZEModelAnimationNodeState* ZEModelAnimationNodeTransition::GetDestinationNode() const
{
	return (ZEModelAnimationNodeState*)GetInputNodeB();
}

bool ZEModelAnimationNodeTransition::SetSourceNode(ZEModelAnimationNodeState* node)
{
	if (node == NULL)
		return false;

	return SetInputNodeA((ZEModelAnimationNode*)node);
}

ZEModelAnimationNodeState* ZEModelAnimationNodeTransition::GetSourceNode() const
{
	return (ZEModelAnimationNodeState*)GetInputNodeA();
}

ZEModelAnimationNodeTransition::ZEModelAnimationNodeTransition()
{
	Triggered = false;
	TotalTransitionTime = 0.0f;
	RemainingTransitionTime = 0.0f;
	SetBlendFactor(0.0f);
}

ZEModelAnimationNodeTransition::~ZEModelAnimationNodeTransition()
{

}
