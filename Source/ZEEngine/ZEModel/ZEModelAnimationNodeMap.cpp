//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNodeMap.cpp
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

#include "ZEModelAnimationNodeMap.h"

const ZEModelAnimationMap* ZEModelAnimationNodeMap::GenerateMap()
{
	return &CurrentMap;
}

bool ZEModelAnimationNodeMap::GenerateOutput(ZEModelAnimationFrame& output)
{
	if (InputNodes[0] == NULL)
		return false;

	ZEModelAnimationFrame temporaryOutput;
	if (InputNodes[0]->GetOutput(temporaryOutput))
		return false;

	ZESize BoneCount = TargetMap.BoneItems.GetCount();
	output.BoneKeys.SetCount(BoneCount);

	for (ZESize I = 0; I < BoneCount; I++)
	{
		output.BoneKeys[I] = temporaryOutput.BoneKeys[BoneLinks[I]];
		strncpy(output.BoneKeys[I].Name, TargetMap.BoneItems[I], ZE_MDLF_MAX_NAME_SIZE);
		output.BoneKeys[I].ItemId = I;
	}

	ZESize MeshCount = TargetMap.MeshItems.GetCount();
	output.MeshKeys.SetCount(MeshCount);

	for (ZESize I = 0; I < MeshCount; I++)
	{
		output.MeshKeys[I] = temporaryOutput.MeshKeys[MeshLinks[I]];
		strncpy(output.MeshKeys[I].Name, TargetMap.MeshItems[I], ZE_MDLF_MAX_NAME_SIZE);
		output.MeshKeys[I].ItemId = I;
	}

	return true;
}

void ZEModelAnimationNodeMap::ProcessSelf(float elapsedTime)
{

}

ZEModelAnimationNodeMap::ZEModelAnimationNodeMap()
{
	SetInputNodeCount(1);
}

ZEModelAnimationNodeMap::~ZEModelAnimationNodeMap()
{

}

bool ZEModelAnimationNodeMap::SetInputNode(ZEModelAnimationNode* node)
{
	if (!ZEModelAnimationNode::SetInputNode(0, node))
		return false;

	return true;
}

const ZEModelAnimationNode* ZEModelAnimationNodeMap::GetInputNode() const
{
	return InputNodes[0];
}

void ZEModelAnimationNodeMap::SetTargetMap(const ZEModelAnimationMap& map)
{
	ZESize boneItemCount = map.BoneItems.GetCount();
	TargetMap.BoneItems.SetCount(boneItemCount);
	ZESize meshItemCount = map.MeshItems.GetCount();
	TargetMap.MeshItems.SetCount(meshItemCount);

	for (ZESize I = 0; I < boneItemCount; I++)
		TargetMap.BoneItems[I] = map.BoneItems[I];

	for (ZESize I = 0; I < meshItemCount; I++)
		TargetMap.MeshItems[I] = map.MeshItems[I];
}

void ZEModelAnimationNodeMap::SetCurrentMap(const ZEModelAnimationMap& map)
{
	ZESize boneItemCount = map.BoneItems.GetCount();
	CurrentMap.BoneItems.SetCount(boneItemCount);
	ZESize meshItemCount = map.MeshItems.GetCount();
	CurrentMap.MeshItems.SetCount(meshItemCount);

	for (ZESize I = 0; I < boneItemCount; I++)
		CurrentMap.BoneItems[I] = map.BoneItems[I];

	for (ZESize I = 0; I < meshItemCount; I++)
		CurrentMap.MeshItems[I] = map.MeshItems[I];
}

void ZEModelAnimationNodeMap::SetBoneLinks(const ZEArray<ZEInt32>& links)
{
	ZESize linkCount = links.GetCount();
	BoneLinks.SetCount(linkCount);

	for (ZESize I = 0; I < linkCount; I++)
		BoneLinks[I] = links[I];
};

void ZEModelAnimationNodeMap::SetMeshLinks(const ZEArray<ZEInt32>& links)
{
	ZESize linkCount = links.GetCount();
	MeshLinks.SetCount(linkCount);

	for (ZESize I = 0; I < linkCount; I++)
		MeshLinks[I] = links[I];
};

//void ZEModelAnimationNodeMap::MapBone(ZEString targetName, ZEString currentName)
//{
//	ZEModelAnimationMapItem item;
//	item.IndexBased = false;
//	item.CurrentName = currentName;
//	item.TargetName = targetName;
//
//	Map.BoneItems.Add(item);
//}
//
//void ZEModelAnimationNodeMap::MapMesh(ZEString targetName, ZEString currentName)
//{
//	ZEModelAnimationMapItem item;
//	item.IndexBased = false;
//	item.CurrentName = currentName;
//	item.TargetName = targetName;
//
//	Map.MeshItems.Add(item);
//}

ZEModelAnimationNodeMap* ZEModelAnimationNodeMap::CreateInstance()
{
	return new ZEModelAnimationNodeMap();
}
