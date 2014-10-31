//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationGraph.cpp
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

#include "ZEModelAnimationGraph.h"

ZEModelAnimationGraph::ZEModelAnimationGraph()
{
	Initialized = false;
	ResultNode = ZEModelAnimationNodeMap::CreateInstance();
	ResultNode->SetOwner(this);
}

ZEModelAnimationGraph::~ZEModelAnimationGraph()
{
	ResultNode->Destroy();
}

bool ZEModelAnimationGraph::SetRootNode(ZEModelAnimationNode* node)
{
	if (!Nodes.Exists(node))
		return false;

	ResultNode->SetInputNode(node);
	
	return true;
}

const ZEModelAnimationNode* ZEModelAnimationGraph::GetRootNode() const
{
	return ResultNode->GetInputNode();
}

void ZEModelAnimationGraph::RegisterAnimationNode(ZEModelAnimationNode* node)
{
	if (Nodes.Exists(node))
		return;

	node->SetOwner(this);
	Nodes.Add(node);

	if (Initialized)
		node->Initialize();
}

void ZEModelAnimationGraph::UnregisterAnimationNode(ZEModelAnimationNode* node)
{
	if (!Nodes.Exists(node))
		return;

	if (Initialized)
		node->Deinitialize();

	Nodes.DeleteValue(node);
	node->SetOwner(NULL);
}

ZEModelAnimationNode* ZEModelAnimationGraph::GetAnimationNode(const char* name)
{
	for (ZEInt I = 0; I < Nodes.GetCount(); I++)
	{
		if (strcmp(Nodes[I]->GetName(), name) == 0)
			return Nodes[I];
	}

	return NULL;
}

void ZEModelAnimationGraph::Process(float elapsedtime)
{
	for (ZEInt I = 0; I < Nodes.GetCount(); I++)
	{
		Nodes[I]->Process(elapsedtime);
	}
}

void ZEModelAnimationGraph::Initialize(const ZEModelResource* modelResource)
{
	Initialized = true;

	ZESize boneCount = modelResource->GetBones().GetCount();
	ZESize meshCount = modelResource->GetMeshes().GetCount();

	ModelMap.BoneItems.SetCount(boneCount);
	ModelMap.MeshItems.SetCount(meshCount);

	for (ZESize I = 0; I < boneCount; I++)
		ModelMap.BoneItems[I] = modelResource->GetBones()[I].Name;


	for (ZESize I = 0; I < meshCount; I++)
		ModelMap.MeshItems[I] = modelResource->GetMeshes()[I].Name;

	ZEArray<ZEInt32> links;

	links.SetCount(boneCount);

	for (ZESize I = 0; I < boneCount; I++)
		links[I] = (ZEInt32)I;

	ResultNode->SetBoneLinks(links);

	links.SetCount(meshCount);

	for (ZESize I = 0; I < meshCount; I++)
		links[I] = (ZEInt32)I;

	ResultNode->SetMeshLinks(links);

	ResultNode->SetTargetMap(ModelMap);
	ResultNode->SetCurrentMap(ModelMap);

	for (ZESize I = 0; I < Nodes.GetCount(); I++)
		Nodes[I]->Initialize();
}

void ZEModelAnimationGraph::Deinitialize()
{
	Initialized = false;

	for (ZESize I = 0; I < Nodes.GetCount(); I++)
		Nodes[I]->Deinitialize();
}
