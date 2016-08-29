//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDAnimationData.cpp
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

#include "ZEMDAnimationData.h"
#include "ZEMDAnimationKey.h"

#include "ZEError.h"
#include "ZEML\ZEMLReader.h"


// ZEMDAnimationDataMapping
//////////////////////////////////////////////////////////////////////////////////////

void ZEMDAnimationDataMapping::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEMDAnimationDataMapping::GetName() const
{
	return Name;
}

void ZEMDAnimationDataMapping::SetPositionInitial(const ZEVector3& Position)
{
	PositionQuantizationInitial = Position;
}

const ZEVector3& ZEMDAnimationDataMapping::GetPositionQuantizationInitial() const
{
	return PositionQuantizationInitial;
}

void ZEMDAnimationDataMapping::SetPositionDelta(const ZEVector3& Delta)
{
	this->PositionQuantizationDelta = Delta;
}

const ZEVector3& ZEMDAnimationDataMapping::SetPositionDelta() const
{
	return PositionQuantizationDelta;
}

void ZEMDAnimationDataMapping::SetScaleInitial(const ZEVector3& Scale)
{
	ScaleQuantizationInitial = Scale;
}

const ZEVector3& ZEMDAnimationDataMapping::GetScaleQuantizationInitial() const
{
	return ScaleQuantizationInitial;
}

void ZEMDAnimationDataMapping::SetScaleDelta(const ZEVector3& Delta)
{
	ScaleQuantizationDelta = Delta;
}

const ZEVector3& ZEMDAnimationDataMapping::SetScaleDelta() const
{
	return ScaleQuantizationDelta;
}


// ZEMDAnimationDataMapping
//////////////////////////////////////////////////////////////////////////////////////

bool ZEMDAnimationData::UpdateCounters()
{
	ZESize Offset = 0;
	while(Offset < Keys.GetCount())
	{	
		const ZEMDAnimationKey* Key = reinterpret_cast<const ZEMDAnimationKey*>(&Keys[Offset]);
		if (Key->GetType() == ZEMD_AKT_RAW)
			Offset += sizeof(ZEMDAnimationKeyRaw);
		else if (Key->GetType() == ZEMD_AKT_QUANTIZED)
			Offset += sizeof(ZEMDAnimationKeyQuantized);
		else
		{
			zeError("Corrupted animation data. Unknown key type. Animation Name: \"%s\".", GetName().ToCString());
			return false;
		}

		if (Offset >= Keys.GetCount())
		{
			zeError("Corrupted animation data. Buffer overrun. Animation Name: \"%s\".", GetName().ToCString());
			return false;
		}

		if (Key->GetIndex() >= Mappings.GetCount())
		{
			zeError("Corrupted animation data. Key index is too large. Animation Name: \"%s\".", GetName().ToCString());
			return false;
		}

		KeyCount++;
	}

	return true;
}

void ZEMDAnimationData::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEMDAnimationData::GetName() const
{
	return Name;
}

void ZEMDAnimationData::SetFramesPerSecond(float FramesPerSecond)
{
	this->FramesPerSecond = FramesPerSecond;
}

float ZEMDAnimationData::GetFramesPerSecond()
{
	return FramesPerSecond;
}

float ZEMDAnimationData::GetFrameCount()
{
	return FrameCount;
}

void ZEMDAnimationData::SetKeysData(const void* Data, ZESize Size)
{
	if (Data == NULL || Size == 0)
	{
		Keys.Clear();
		KeyCount = 0;
		return;
	}

	Keys.SetCount(Size);
	memcpy(Keys.GetCArray(), Data, Size);
}

const void* ZEMDAnimationData::GetKeysData()
{
	return Keys.GetCArray();
}

ZESize ZEMDAnimationData::GetKeysDataSize()
{
	return Keys.GetCount();
}

const ZEArray<ZEMDAnimationDataMapping>& ZEMDAnimationData::GetMappings() const
{
	return Mappings;
}

void ZEMDAnimationData::AddMapping(const ZEMDAnimationDataMapping& Mapping)
{
	zeCheckError(Mappings.GetCount() >= 1024, ZE_VOID, "Cannot add new mapping. Maximum index count (1024 Mapping) has been reached. Animation Name: \"%s\".", GetName().ToCString());

	ze_for_each(Current, Mappings)
	{
		if (Current->GetName() == Mapping.GetName())
		{
			zeError("Cannot add mapping. Another mapping with the same name is already added to mappings. Animation Name: \"%s\".", GetName().ToCString());
			return;
		}
	}

	Mappings.Add(Mapping);
}

void ZEMDAnimationData::RemoveMapping(ZESize Index)
{
	zeCheckError(Index >= Mappings.GetCount(), ZE_VOID, "Cannot remove mappings. Index is out of range. Animation Name: \"%s\".", GetName().ToCString());
	Mappings.Remove(Index);
}

ZESize ZEMDAnimationData::GetKeyCount() const
{
	return KeyCount;
}

ZEMDAnimationKey* ZEMDAnimationData::GetKey(ZESize Index)
{
	ZESize Offset = 0;
	while(Offset < Keys.GetCount())
	{	
		ZEMDAnimationKey* Key = reinterpret_cast<ZEMDAnimationKey*>(&Keys[Offset]);

		if (Index == 0)
			return Key;

		ZESize KeySize = Key->GetSize();
		zeCheckError(KeySize == 0, NULL, "Cannot get key. Corrupted key data (unknwon key type). Animation Name: \"%s\".", GetName().ToCString());
		
		Offset += KeySize;
		Index--;
	}

	return NULL;
}

const ZEMDAnimationKey* ZEMDAnimationData::GetKey(ZESize Index) const
{
	ZESize Offset = 0;
	while(Offset < Keys.GetCount())
	{	
		const ZEMDAnimationKey* Key = reinterpret_cast<const ZEMDAnimationKey*>(&Keys[Offset]);

		if (Index == 0)
			return Key;

		ZESize KeySize = Key->GetSize();
		zeCheckError(KeySize == 0, NULL, "Cannot get key. Corrupted key data (unknwon key type). Animation Name: \"%s\".", GetName().ToCString());

		Offset += KeySize;
		Index--;
	}

	return NULL;
}

void ZEMDAnimationData::AddKey(const ZEMDAnimationKey* Key)
{
	switch (Key->GetType())
	{
		case ZEMD_AKT_RAW:
			Keys.MassAdd(reinterpret_cast<const ZEBYTE*>(Key), sizeof(ZEMDAnimationKeyRaw));
			break;


		case ZEMD_AKT_QUANTIZED:
			Keys.MassAdd(reinterpret_cast<const ZEBYTE*>(Key), sizeof(ZEMDAnimationKeyQuantized));
		
		default:
			zeError("Cannot add key to animation. Unknown key type.");
			return;
	}

	UpdateCounters();
}

void ZEMDAnimationData::RemoveKey(ZESize Index)
{
	zeCheckError(Index >= KeyCount, ZE_VOID, "Cannot remove key from animation. Index is out of range. Animation Name: \"%s\".", GetName().ToCString());
	
	ZEMDAnimationKey* AnimationKey = GetKey(Index);
	ZESize KeySize = AnimationKey->GetSize();
	
	zeCheckError(KeySize == 0, ZE_VOID, "Cannot get key. Corrupted key data (unknwon key type). Animation Name: \"%s\".", GetName().ToCString());

	Keys.RemoveRange(reinterpret_cast<ZEBYTE*>(AnimationKey) - Keys.GetCArray(), KeySize);

	UpdateCounters();
}

bool ZEMDAnimationData::Unserialize(ZEMLReaderNode* AnimationDataNode)
{
	SetName(AnimationDataNode->ReadString("Name"));
	SetFramesPerSecond(AnimationDataNode->ReadFloat("FramesPerSecond", 30.0f));
	Keys.SetCount(AnimationDataNode->ReadDataSize("Keys"));
	
	ZEMLReaderNode MappingsNode = AnimationDataNode->GetNode("Mappings");
	if (MappingsNode.IsValid())
	{
		Mappings.SetCount(MappingsNode.GetNodeCount("Mapping"));
		for (ZESize I = 0; I < Mappings.GetCount(); I++)
		{
			ZEMLReaderNode MappingNode = MappingsNode.GetNode("Mappings", I);
			Mappings[I].SetName(MappingNode.ReadString("Name"));
			Mappings[I].SetPositionInitial(MappingNode.ReadVector3("PositionInitial"));
			Mappings[I].SetPositionDelta(MappingNode.ReadVector3("PositionDelta"));
			Mappings[I].SetScaleInitial(MappingNode.ReadVector3("ScaleInitial"));
			Mappings[I].SetScaleDelta(MappingNode.ReadVector3("ScaleInitial"));
		}
	}

	AnimationDataNode->ReadData("Keys", Keys.GetCArray(), Keys.GetCount());

	return true;
}

bool ZEMDAnimationData::Serialize(ZEMLWriterNode* AnimationDataNode)
{
	return false;
}
