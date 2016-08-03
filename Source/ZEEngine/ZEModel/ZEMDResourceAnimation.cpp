//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceAnimation.cpp
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

#include "ZEMDResourceAnimation.h"

#include "ZEML/ZEMLReader.h"
#include "ZEMDResource.h"

ZEMDResourceAnimation::ZEMDResourceAnimation() : Link(this)
{
	Resource = NULL;
	FramesPerSecond = 30; 
}

ZEMDResourceAnimation::~ZEMDResourceAnimation()
{
	if (GetResource() != NULL)
		GetResource()->RemoveAnimation(this);
}

ZEMDResource* ZEMDResourceAnimation::GetResource()
{
	return Resource;
}

const ZEMDResource* ZEMDResourceAnimation::GetResource() const
{
	return Resource;
}

void ZEMDResourceAnimation::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEMDResourceAnimation::GetName() const
{
	return Name;
}

void ZEMDResourceAnimation::SetFrames(ZEArray<ZEMDResourceAnimationFrame>& Frames)
{
	this->Frames = Frames;
}

const ZEArray<ZEMDResourceAnimationFrame>& ZEMDResourceAnimation::GetFrames() const
{
	return Frames;
}

void ZEMDResourceAnimation::AddFrame(const ZEMDResourceAnimationFrame& Frame)
{
	Frames.AddByRef(Frame);
}

void ZEMDResourceAnimation::RemoveFrame(ZESize Index)
{
	Frames.Remove(Index);
}


void ZEMDResourceAnimation::SetFramesPerSecond(float FramesPerSecond)
{
	this->FramesPerSecond = FramesPerSecond;
}

float ZEMDResourceAnimation::GetFramesPerSecond() const
{
	return FramesPerSecond;
}

bool ZEMDResourceAnimation::Unserialize(const ZEMLReaderNode& AnimationNode)
{
	zeCheckError(!AnimationNode.IsValid(), false, "Invalid Animation node.");
	zeCheckError(AnimationNode.GetName() == "Animation", false, "Invalid Animation node name.");
	
	SetName(AnimationNode.ReadString("Name"));
	FramesPerSecond = AnimationNode.ReadFloat("FramesPerSecond", 30);
	ZEUInt BoneKeyCount = AnimationNode.ReadUInt32("BoneKeyCount");
	ZEUInt MeshKeyCount = AnimationNode.ReadUInt32("MeshKeyCount");
	ZEUInt FrameKeyCount = BoneKeyCount + MeshKeyCount;

	Frames.SetCount(((ZESize)AnimationNode.ReadDataSize("Frames") / sizeof(ZEMDResourceAnimationKey)) / (ZESize)FrameKeyCount);

	for (ZESize I = 0; I < Frames.GetCount(); I++)
	{
		ZEMDResourceAnimationFrame* CurrentAnimationFrame = &Frames[I];
		CurrentAnimationFrame->BoneKeys.SetCount(BoneKeyCount);

		if (BoneKeyCount != 0)
			if (!AnimationNode.ReadDataItems("Frames", CurrentAnimationFrame->BoneKeys.GetCArray(), sizeof(ZEMDResourceAnimationKey), BoneKeyCount, (I * FrameKeyCount) * sizeof(ZEMDResourceAnimationKey)))
				return false;

		CurrentAnimationFrame->MeshKeys.SetCount(MeshKeyCount);

		if (MeshKeyCount != 0)
			if (!AnimationNode.ReadDataItems("Frames", CurrentAnimationFrame->MeshKeys.GetCArray(), sizeof(ZEMDResourceAnimationKey), MeshKeyCount, (I * FrameKeyCount) * sizeof(ZEMDResourceAnimationKey) + BoneKeyCount * sizeof(ZEMDResourceAnimationKey)))
				return false;
	}

	return true;
}

bool ZEMDResourceAnimation::Serialize(ZEMLWriterNode& AnimationNode) const
{
	return false;
}

ZEMDResourceAnimation* ZEMDResourceAnimation::CreateInstance()
{
	return new ZEMDResourceAnimation();
}
