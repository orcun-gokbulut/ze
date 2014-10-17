//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationClip.cpp
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

#include "ZEModelAnimationClip.h"
#include "ZEModel.h"


ZEModelAnimationClip::ZEModelAnimationClip()
{
	IsInitialized = false;
	FrameCount = 0;
	ReferenceCount = 0;
};

ZEModelAnimationClip::~ZEModelAnimationClip()
{

};

void ZEModelAnimationClip::AddReference()
{
	if (ReferenceCount == 0)
		Initialize();

	ReferenceCount++;
}

void ZEModelAnimationClip::RemoveReference()
{
	ReferenceCount--;

	if (ReferenceCount == 0)
		Deinitialize();
}

bool ZEModelAnimationClip::AddAnimationSequence(const ZEModelResourceAnimation& animation)
{
	if (IsInitialized)
		return false;

	AnimationSequence* sequence = Sequences.Add();
	strcpy(sequence->Name,animation.Name);
	//sequence->Frames = animation.Frames;
	sequence->FrameCount = sequence->Frames.GetCount();
	sequence->StartFrame = FrameCount;
	sequence->EndFrame = sequence->StartFrame + sequence->FrameCount - 1;

	//Frames.Resize(FrameCount + sequence->FrameCount);

	//for (ZESize I = 0; I < sequence->FrameCount; I++)
	//{
	//	Frames[sequence->StartFrame + I] = &(sequence->Frames[I]);
	//}

	FrameCount += sequence->FrameCount;

	return true;

};

bool ZEModelAnimationClip::AddAnimationSequence(const char* name, const ZEModel& model)
{
	if (IsInitialized)
		return false;

	if (model.GetModelResource() == NULL)
		return false;

	//if (model.GetMapper() == NULL)
	//	return false;

	//if (Mapper == NULL)
	//Mapper = model.GetMapper();

	const ZEModelResource* resource = model.GetModelResource();

	for (ZESize I = 0; I < resource->GetAnimations().GetCount(); I++)
	{
		if (strnicmp(name, resource->GetAnimations()[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			const ZEModelResourceAnimation* animation = &resource->GetAnimations()[I];
			AnimationSequence* sequence = Sequences.Add();
			strcpy(sequence->Name,animation->Name);
			//sequence->Frames = animation->Frames;
			sequence->FrameCount = sequence->Frames.GetCount();
			sequence->StartFrame = FrameCount;
			sequence->EndFrame = sequence->StartFrame + sequence->FrameCount - 1;

			//Frames.Resize(FrameCount + sequence->FrameCount);

			//for (ZESize I = 0; I < sequence->FrameCount; I++)
			//{
			//	Frames[sequence->StartFrame + I] = &(sequence->Frames[I]);
			//}

			FrameCount += sequence->FrameCount;

			return true;
		}
	}

	return false;

};

bool ZEModelAnimationClip::InsertAnimationSequence(ZESize index, const ZEModelResourceAnimation& animation)
{
	if (IsInitialized)
		return false;

	if (index >= Sequences.GetCount())
		return false;

	AnimationSequence* sequence = Sequences.Insert(index);
	strcpy(sequence->Name,animation.Name);
	//sequence->Frames = animation.Frames;
	sequence->FrameCount = sequence->Frames.GetCount();

	for (ZESize I = 0; I < index; I++)
		sequence->StartFrame += Sequences[I].FrameCount;

	sequence->EndFrame = sequence->StartFrame + sequence->FrameCount - 1;

	for (ZESize I = index + 1; I < Sequences.GetCount(); I++)
	{
		Sequences[I].StartFrame += sequence->FrameCount;
		Sequences[I].EndFrame += sequence->FrameCount;
	}

	//ZEModelAnimationFrame** insertedFrames = Frames.MassInsert(sequence->StartFrame, sequence->FrameCount);

	//for (ZESize I = 0; I < sequence->FrameCount; I++)
	//{
	//	 *(insertedFrames + I) = &(sequence->Frames[I]);
	//}

	FrameCount += sequence->FrameCount;

	return true;
};

bool ZEModelAnimationClip::InsertAnimationSequence(ZESize index, const char* name, const ZEModel& model)
{
	if (IsInitialized)
		return false;

	if (index >= Sequences.GetCount())
		return false;

	if (model.GetModelResource() == NULL)
		return false;

	//if (model.GetMapper() == NULL)
	//	return false;

	//if (Mapper == NULL)
	//Mapper = model.GetMapper();

	const ZEModelResource* resource = model.GetModelResource();

	for (ZESize I = 0; I < resource->GetAnimations().GetCount(); I++)
	{
		if (strnicmp(name, resource->GetAnimations()[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			const ZEModelResourceAnimation* animation = &resource->GetAnimations()[I];
			AnimationSequence* sequence = Sequences.Insert(index);
			strcpy(sequence->Name,animation->Name);
			//sequence->Frames = animation->Frames;
			sequence->FrameCount = sequence->Frames.GetCount();

			for (ZESize I = 0; I < index; I++)
				sequence->StartFrame += Sequences[I].FrameCount;

			sequence->EndFrame = sequence->StartFrame + sequence->FrameCount - 1;

			for (ZESize I = index + 1; I < Sequences.GetCount(); I++)
			{
				Sequences[I].StartFrame += sequence->FrameCount;
				Sequences[I].EndFrame += sequence->FrameCount;
			}

			//ZEModelAnimationFrame** insertedFrames = Frames.MassInsert(sequence->StartFrame, sequence->FrameCount);

			//for (ZESize I = 0; I < sequence->FrameCount; I++)
			//{
			//	 *(insertedFrames + I) = &(sequence->Frames[I]);
			//}

			FrameCount += sequence->FrameCount;

			return true;
		}

	}

	return false;
};

AnimationSequence* ZEModelAnimationClip::GetAnimationSequence(const char* name)
{
	for (ZESize I = 0; I < Sequences.GetCount(); I++)
	{
		if (strnicmp(name, Sequences[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
			return &Sequences[I];
	}

	return NULL;
};

AnimationSequence* ZEModelAnimationClip::GetAnimationSequence(ZESize index)
{
	if (index >= Sequences.GetCount())
		return NULL;

	return &Sequences[index];
};

ZESize ZEModelAnimationClip::GetSequenceCount() const
{
	return Sequences.GetCount();
};

bool ZEModelAnimationClip::RemoveAnimationSequence(const char* name)
{
	if (IsInitialized)
		return false;

	for (ZESize I = 0; I < Sequences.GetCount(); I++)
	{
		if (strnicmp(name, Sequences[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			for (ZESize J = I + 1; J < Sequences.GetCount(); J++)
			{
				Sequences[J].StartFrame -= Sequences[I].FrameCount;
				Sequences[J].EndFrame -= Sequences[I].FrameCount;
			}

			FrameCount -= Sequences[I].FrameCount;
			Sequences.DeleteAt(I);

			return true;
		}
	}

	return false;
};

bool ZEModelAnimationClip::RemoveAnimationSequence(ZESize index)
{
	if (IsInitialized || index >= Sequences.GetCount())
		return false;

	for (ZESize I = index + 1; I < Sequences.GetCount(); I++)
	{
		Sequences[I].StartFrame -= Sequences[index].FrameCount;
		Sequences[I].EndFrame -= Sequences[index].FrameCount;
	}

	FrameCount -= Sequences[index].FrameCount;
	Sequences.DeleteAt(index);

	return true;
};

bool ZEModelAnimationClip::RemoveAnimationSequence(ZESize startIndex, ZESize endIndex)
{
	if (IsInitialized)
		return false;

	if (startIndex >= Sequences.GetCount() || endIndex >= Sequences.GetCount())
		return false;

	if (startIndex > endIndex)
		return false;

	ZESize removedFrameCount = 0;

	for (ZESize I = startIndex; I <= endIndex; I++)
	{
		removedFrameCount += Sequences[startIndex].FrameCount;
		Sequences.DeleteAt(startIndex);
	}

	for (ZESize I = startIndex; I < Sequences.GetCount(); I++)
	{
		Sequences[I].StartFrame -= removedFrameCount;
		Sequences[I].EndFrame -= removedFrameCount;
	}

	FrameCount -= removedFrameCount;

	return true;
};

ZEModelAnimationFrame* ZEModelAnimationClip::GetFrame(ZESize index)
{
	if (index >= FrameCount)
		return NULL;

	return Frames[index];
};

ZESize ZEModelAnimationClip::GetFrameCount() const
{
	return FrameCount;
}

void ZEModelAnimationClip::Initialize()
{
	if (IsInitialized)
		return;

	IsInitialized = true;

	Frames.SetCount(FrameCount);
	ZESize clipFrameCursor = 0;

	for (ZESize I = 0; I < Sequences.GetCount(); I++)
	{
		for (ZESize J = 0; J < Sequences[I].FrameCount; I++)
		{
			Frames[clipFrameCursor + J] = &(Sequences[I].Frames[J]);
		}

		clipFrameCursor += Sequences[I].FrameCount;
	}

};

void ZEModelAnimationClip::Deinitialize()
{
	if (!IsInitialized)
		return;

	Frames.Clear();
	IsInitialized = false;
};

ZEModelAnimationClip* ZEModelAnimationClip::CreateInstance()
{
	return new ZEModelAnimationClip();
};
