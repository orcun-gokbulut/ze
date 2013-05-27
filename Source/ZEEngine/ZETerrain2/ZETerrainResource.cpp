//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainResource.cpp
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

#include "ZETerrainResource.h"
#include "ZECore\ZEResourceManager.h"

static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	ZESize ConstLength = strlen("resources\\") - 1;

	if (Path[0] == '\\' || Path[0] == '/')
		NewString = NewString.SubString(1, Path.GetLength() - 1);

	// If it is guaranteed that there is no "resources\\" string in beginning
	if (NewString.GetLength() - 1 < ConstLength)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}
	// Else check if there is "resources\\" in the beginning
	else if (_stricmp("resources\\", Path.SubString(0, ConstLength)) != 0)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}

	return NewString;
}


const char* ZETerrainResource::GetResourceType() const
{
	return "Terrain Resource";
}

ZETerrainResource::ZETerrainResource()
{
	memset(&Info, 0, sizeof(ZETerrainBlockInfo));
}

const ZETerrainBlockInfo& ZETerrainResource::GetInfo()
{
	return Info;
}

ZETerrainStreamedResource::ZETerrainStreamedResource()
{

}

ZETerrainStreamedResource::~ZETerrainStreamedResource()
{
	File.Close();
}

bool ZETerrainStreamedResource::GetBlock(ZETerrainBlock& Output, ZEUInt Level, ZEInt64 PositionX, ZEInt64 PositionY)
{
	// Binary Search
	ZESize Start = 0;
	ZESize End = Index.GetCount() - 1;
	while(Start <= End)
	{
		ZESize MidPoint = Start + (End - Start) / 2;
		ZETerrainBlockIndex CurrentIndex = Index[MidPoint];

		if (Level > CurrentIndex.Level)
		{
			Start = MidPoint + 1;
		}
		else if (Level < CurrentIndex.Level)
		{
			End = MidPoint - 1;
		}
		else
		{
			if (PositionX > CurrentIndex.PositionX)
			{
				Start = MidPoint + 1;
			}
			else if (PositionX < CurrentIndex.PositionX)
			{
				End = MidPoint - 1;
			}
			else
			{
				if (PositionY > CurrentIndex.PositionY)
				{
					Start = MidPoint + 1;
				}
				else if (PositionY < CurrentIndex.PositionY)
				{
					End = MidPoint - 1;
				}
				else
				{
					File.Seek(CurrentIndex.Offset, ZE_SF_BEGINING);
					ZETerrainBlock* Block = new ZETerrainBlock();
					if (!Block->Load(&File))
					{
						delete Block;
						return NULL;
					}
					return Block;
				}
			}
		}
	}

	return NULL;
}

ZETerrainResource* ZETerrainStreamedResource::LoadSharedResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);
	ZETerrainStreamedResource* Resource = (ZETerrainStreamedResource*)zeResources->GetResource(NewPath);

	if (Resource != NULL)
		return Resource;
	else
	{
		Resource = (ZETerrainStreamedResource*)LoadResource(NewPath);
		if (Resource != NULL)
		{
			Resource->Shared = true;
			Resource->Cached = false;
			Resource->ReferenceCount = 1;
			zeResources->AddResource(Resource);
			return Resource;
		}
		else
			return NULL;
	}
}

void ZETerrainStreamedResource::CacheResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZETerrainStreamedResource* Resource = (ZETerrainStreamedResource*)zeResources->GetResource(NewPath);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		Resource = (ZETerrainStreamedResource*)LoadResource(NewPath);
		Resource->Cached = true;
		Resource->ReferenceCount = 0;
		zeResources->AddResource(Resource);
	}
}

ZETerrainResource* ZETerrainStreamedResource::LoadResource(const ZEString& FileName)
{
	ZEString NewPath = FileName;//ConstructResourcePath(FileName);

	ZEFile IndexFile;
	if (!IndexFile.Open(NewPath + ".Index.ZETerrain" , ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Can not open terrain index file.");
		return NULL;
	}

	ZETerrainStreamedResource* NewResource = new ZETerrainStreamedResource();
	NewResource->SetFileName(NewPath);
	NewResource->Cached = false;
	NewResource->ReferenceCount = 0;

	if (IndexFile.Read(&NewResource->Info, sizeof(ZETerrainBlockInfo), 1) != 1)
	{
		zeError("Can not read block index count. Can not read terrain file.");
		delete NewResource;
		return NULL;
	}

	ZEUInt32 IndexCount;
	if (IndexFile.Read(&IndexCount, sizeof(IndexCount), 1) != 1)
	{
		zeError("Can not read block index count. Can not read terrain file.");
		delete NewResource;
		return NULL;
	}

	NewResource->Index.SetCount(IndexCount);
	if (IndexFile.Read(NewResource->Index.GetCArray(), sizeof(ZETerrainBlockIndex), IndexCount) != IndexCount)
	{
		zeError("Can not read block indexes. Can not read terrain file.");
		delete NewResource;
		return NULL;
	}

	IndexFile.Close();

	if (!NewResource->File.Open(NewPath + ".Data.ZETerrain" , ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Can not open terrain data file.");
		delete NewResource;
		return NULL;
	}

	return NewResource;
}
