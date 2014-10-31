//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNode.cpp
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

#include "ZEModelAnimationNode.h"
#include "ZEModelAnimationGraph.h"

void ZEModelAnimationNode::SetOwner(ZEModelAnimationGraph* owner)
{
	Owner = owner;
}

const ZEModelAnimationMap* ZEModelAnimationNode::GetMap()
{
	if (OutputNode == NULL)
		return NULL;

	return GenerateMap();
}

bool ZEModelAnimationNode::GetOutput(ZEModelAnimationFrame& output)
{
	if (!GenerateOutput(output))
		return false;

	const ZEModelAnimationMap* map = GetMap();

	if (map == NULL)
		return false;

	ZESize boneKeyCount = output.BoneKeys.GetCount();
	if (map->BoneItems.GetCount() != boneKeyCount)
		return false;

	ZESize meshKeyCount = output.MeshKeys.GetCount();
	if (map->MeshItems.GetCount() != meshKeyCount)
		return false;

	for (ZESize I = 0; I < boneKeyCount; I++)
		if (strcmp(map->BoneItems[I], output.BoneKeys[I].Name) != 0)
			return false;

	for (ZESize I = 0; I < meshKeyCount; I++)
		if (strcmp(map->MeshItems[I], output.MeshKeys[I].Name) != 0)
			return false;

	return true;
}

void ZEModelAnimationNode::Process(float elapsedTime)
{
	ProcessSelf(elapsedTime);
}

void ZEModelAnimationNode::Initialize()
{
	if (!Initialized)
	{
		Initialized = true;
		InitializeSelf();
	}
}

void ZEModelAnimationNode::Deinitialize()
{
	if (Initialized)
	{
		DeinitializeSelf();
		Initialized = false;
	}
}

ZEModelAnimationNode::ZEModelAnimationNode()
{
	Owner = NULL;
	OutputNode = NULL;
	Initialized = false;
	Enabled = true;
}

ZEModelAnimationNode::~ZEModelAnimationNode()
{

}

ZEModelAnimationGraph* ZEModelAnimationNode::GetOwner() const
{
	return Owner;
}

bool ZEModelAnimationNode::AddInputNode(ZEModelAnimationNode* node)
{
	if (node == NULL)
		return false;

	if (node->Owner == NULL)
		return false;

	if (this->Owner == NULL)
		return false;

	if (node->Owner != this->Owner)
		return false;

	if (InputNodes.Exists(node))
		return false;

	InputNodes.Add(node);

	node->SetOutputNode(this);

	return true;
}

bool ZEModelAnimationNode::SetInputNode(ZESize index, ZEModelAnimationNode* node)
{
	if (index >= InputNodes.GetCount())
		return false;
	
	if (this->Owner == NULL)
		return false;
	
	if (node != NULL)
	{
		if (node->Owner == NULL)
			return false;

		if (node->Owner != this->Owner)
			return false;

		if (InputNodes.Exists(node))
			return false;
	}

	if (InputNodes[index] != NULL)
	{
		InputNodes[index]->SetOutputNode(NULL);
		InputNodes.DeleteValue(InputNodes[index]);
	}

	InputNodes.SetItem(index, node);
	
	if (node != NULL)
		node->SetOutputNode(this);

	return true;

}

ZEModelAnimationNode* ZEModelAnimationNode::GetInputNode(ZESize index) const
{
	return InputNodes[index];
}

const ZEArray<ZEModelAnimationNode*>& ZEModelAnimationNode::GetInputNodes() const
{
	return InputNodes;
}

void ZEModelAnimationNode::SetInputNodeCount(ZESize count)
{
	for (ZESize I = 0; I < InputNodes.GetCount(); I++)
		InputNodes[I]->SetOutputNode(NULL);

	InputNodes.SetCount(count);
	InputNodes.Fill(NULL);
}

bool ZEModelAnimationNode::RemoveInputNode(ZEModelAnimationNode* node)
{
	if (node == NULL)
		return false;

	if (node->Owner == NULL)
		return false;

	if (this->Owner == NULL)
		return false;

	if (node->Owner != this->Owner)
		return false;

	if (!InputNodes.Exists(node))
		return false;

	node->SetOutputNode(NULL);

	InputNodes.DeleteValue(node);

	return true;
}

const ZEModelAnimationMap* ZEModelAnimationNode::GenerateMap()
{
	return OutputNode->GenerateMap();
}

void ZEModelAnimationNode::InitializeSelf()
{

}

void ZEModelAnimationNode::DeinitializeSelf()
{

}

bool ZEModelAnimationNode::SetName(const ZEString& name)
{
	if (Initialized)
		return false;

	this->Name = Name;

	return true;
}

const ZEString&	ZEModelAnimationNode::GetName() const
{
	return Name;
}

void ZEModelAnimationNode::SetEnabled(bool enabled)
{
	Enabled = enabled;
}

bool ZEModelAnimationNode::GetEnabled() const
{
	return Enabled;
}

bool ZEModelAnimationNode::SetOutputNode(ZEModelAnimationNode* node)
{
	if (OutputNode == node)
		return false;

// 	if (this->Owner == NULL)
// 		return false;
// 
// 	if (node != NULL && (node->Owner != this->Owner || node->Owner == NULL))
// 		return false;

	if (OutputNode != NULL)
		OutputNode->RemoveInputNode(this);

	OutputNode = node;

	return true;
}

const ZEModelAnimationNode* ZEModelAnimationNode::GetOutputNode() const
{
	return OutputNode;
}

void ZEModelAnimationNode::Destroy()
{
	delete this;
}

