//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsResource.cpp
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

#include "ZEError.h"
#include "ZEGraphicsResource.h"
#include "ZETexture/ZETextureData.h"

/********************************************************************/
/*							ZEShadowCopy							*/
/********************************************************************/

#define G_SHADOW_COPY_COUNT			ZE_RENDER_BUFFER_COUNT
#define G_SHADOW_COPY_ALIGNMENT		16

void ZEShadowCopy::SetSynchEnabled(bool Enabled)
{
	SynchEnabled = Enabled;
}

bool ZEShadowCopy::GetSynchEnabled() const
{
	return SynchEnabled;
}

ZEUInt ZEShadowCopy::GetRowCount() const
{
	return RowCount;
}

ZEUInt ZEShadowCopy::GetSliceCount() const
{
	return SliceCount;
}

ZESize ZEShadowCopy::GetRowSize() const
{
	return RowSize;
}

ZESize ZEShadowCopy::GetSliceSize() const
{
	return SliceSize;
}

ZEUInt ZEShadowCopy::GetCurrentIndex() const
{
	return CurrentIndex;
}

void* ZEShadowCopy::GetData()
{
	Changed[CurrentIndex] = true;
	return Shadows[CurrentIndex];
}

const void* ZEShadowCopy::GetConstData() const
{
	return Shadows[CurrentIndex];
}

void ZEShadowCopy::SetChanged(bool Value)
{
	Changed[CurrentIndex] = Value;
}

bool ZEShadowCopy::GetChanged() const
{
	return Changed[CurrentIndex];
}

void* ZEShadowCopy::GetData(ZEUInt Index)
{
	zeDebugCheck(Index >= G_SHADOW_COPY_COUNT, "Index out of range");

	Changed[Index] = true;
	return Shadows[(ZESize)Index];
}

const void* ZEShadowCopy::GetConstData(ZEUInt Index) const
{
	zeDebugCheck(Index >= G_SHADOW_COPY_COUNT, "Index out of range");
	return Shadows[(ZESize)Index];
}

void ZEShadowCopy::SetChanged(ZEUInt Index, bool Value)
{
	zeDebugCheck(Index >= G_SHADOW_COPY_COUNT, "Index out of range");	
	Changed[(ZESize)Index] = Value;
}

bool ZEShadowCopy::GetChanged(ZEUInt Index) const
{
	zeDebugCheck(Index >= G_SHADOW_COPY_COUNT, "Index out of range");

	return Changed[(ZESize)Index];
}

void ZEShadowCopy::MoveNext()
{
	ZESize Current = CurrentIndex;
	ZESize Next = (Current + 1) % G_SHADOW_COPY_COUNT;

	if (SynchEnabled)
	{
		memcpy(Shadows[Next], Shadows[Current], SliceCount * SliceSize);
	}

	CurrentIndex = (ZEUInt)Next;
}

bool ZEShadowCopy::CreateBuffer(ZESize RowSize, const void* InitialData)
{
	zeDebugCheck(RowSize == 0, "Cannot create with zero row size.");

	for (ZESize I = 0; I < G_SHADOW_COPY_COUNT; ++I)
	{
		Shadows[I] = _aligned_malloc(RowSize, G_SHADOW_COPY_ALIGNMENT);

		if (InitialData != NULL)
		{
			memcpy(Shadows[I], InitialData, RowSize);
		}
	}

	this->RowCount = 1;
	this->RowSize = RowSize;
	this->SliceCount = 1;
	this->SliceSize = RowSize;

	return true;
}

bool ZEShadowCopy::CreateTexture(ZEUInt RowCount, ZESize RowSize, ZEUInt SliceCount, ZESize SliceSize, ZETextureData* InitialData)
{
	zeDebugCheck(RowCount == 0, "Cannot create with zero row pitch.");
	zeDebugCheck(SliceCount == 0, "Cannot create with zero slice pitch.");
	zeDebugCheck(RowSize == 0, "Cannot create with zero row pitch.");
	zeDebugCheck(SliceSize == 0, "Cannot create with zero slice pitch.");
	zeDebugCheck(RowCount * RowSize != SliceSize, "Inconsistent input data.");

	for (ZESize I = 0; I < G_SHADOW_COPY_COUNT; ++I)
	{
		Shadows[I] = _aligned_malloc(SliceSize * SliceCount, G_SHADOW_COPY_ALIGNMENT);

		if (InitialData != NULL)
		{
			for (ZESize SurfaceN = 0; SurfaceN < SliceCount; ++ SurfaceN)
			{
				void* Destination = (ZEUInt8*)Shadows[I] + SurfaceN * SliceSize;
				InitialData->GetSurfaces()[SurfaceN].GetLevels()[0].CopyTo(Destination, RowSize * SliceCount);
			}
		}
	}

	this->RowCount = RowCount;
	this->SliceCount = SliceCount;
	this->RowSize = RowSize;
	this->SliceSize = SliceSize;

	return true;
}

ZEShadowCopy::ZEShadowCopy()
{
	RowSize = 0;
	SliceSize = 0,
	CurrentIndex = 0;
	SynchEnabled = false;

	memset(&Shadows, 0, sizeof(void*) * ZE_MAX_SHADOW_COPY_COUNT);
	memset(&Changed, 0, sizeof(bool ) * ZE_MAX_SHADOW_COPY_COUNT);
}

ZEShadowCopy::~ZEShadowCopy()
{
	for (ZESize I = 0; I < ZE_MAX_SHADOW_COPY_COUNT; ++I)
	{
		if (Shadows[I] != NULL)
		{
			_aligned_free(Shadows[I]);
			Shadows[I] = NULL;
		}
	}
}

/********************************************************************/
/*							ZEGraphicsResource						*/
/********************************************************************/
bool ZEGraphicsResource::GetIsCreated() const
{
	return State.IsCreated;
}

bool ZEGraphicsResource::GetIsStatic() const
{
	return State.IsStatic;
}

bool ZEGraphicsResource::GetIsLocked() const
{
	return State.IsLocked;
}

bool ZEGraphicsResource::GetIsDeleted() const
{
	return State.IsDestroyed;
}

void ZEGraphicsResource::SetDebugName(const char* Name)
{
#ifdef ZE_DEBUG_ENABLE
	DebugName = Name;
#endif
}

const char* ZEGraphicsResource::GetDebugName() const
{
#ifdef ZE_DEBUG_ENABLE
	return DebugName.ToCString();
#else
	return NULL;
#endif
}

void ZEGraphicsResource::Destroy()
{
	if (State.IsOccupied)
	{
		State.IsDestroyed = true;
		return;
	}

	delete this;
}

ZEGraphicsResource::ZEGraphicsResource()
{
	State.IsCreated = false;
	State.IsStatic = false;
	State.IsLocked = false;

	State.IsOccupied = false;
	State.IsDestroyed = false;
}

ZEGraphicsResource::~ZEGraphicsResource()
{

}
