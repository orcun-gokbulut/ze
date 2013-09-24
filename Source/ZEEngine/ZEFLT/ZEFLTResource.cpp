//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFLTResource.cpp
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

#include "ZEFLTResource.h"
#include "ZEFLTReader.h"
#include "ZECore/ZEResourceManager.h"
#include "ZEFile/ZEPathUtils.h"

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

bool ZEFLTResource::ReadOpenflightSceneFromFile(ZEFile* ResourceFile)
{
	ZEFLTReader Reader;

	if (!Reader.SetResourceFile(ResourceFile))
	{
		zeError("Can not read FLT file.");
		return false;
	}

	ZEInt64 ReadCursor = 0;

	while(ReadCursor != ResourceFile->GetSize())
	{
		ReadCursor = Reader.ReadRecord();
	}

	Node = Reader.GetRootNode();

	return true;

};

ZEFLTResource::~ZEFLTResource()
{

};

const char* ZEFLTResource::GetResourceType() const
{
	return "Openflight";
};

ZEFLTResource* ZEFLTResource::LoadResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	bool Result;
	ZEFile ResourceFile;
	NewPath = ZEPathUtils::GetSimplifiedPath(NewPath, false);

	Result = ResourceFile.Open(NewPath, ZE_FOM_READ, ZE_FCM_NONE);
	if (Result)
	{
		ZEFLTResource* NewResource = new ZEFLTResource();
		NewResource->SetFileName(NewPath);
		NewResource->Cached = false;
		NewResource->ReferenceCount = 0;
		if (!NewResource->ReadOpenflightSceneFromFile(&ResourceFile))
		{
			zeError("Can not load Openflight scene file. (FileName : \"%s\")", FileName.ToCString());
			ResourceFile.Close();
			delete NewResource;
			return NULL;
		}
		else
		{
			ResourceFile.Close();
			return NewResource;
		}
	}
	else
	{
		zeError("Openflight scene file does not exists. (FileName : \"%s\")", FileName.ToCString());
		return NULL;
	}
};

ZEFLTResource* ZEFLTResource::LoadSharedResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	NewPath = ZEPathUtils::GetSimplifiedPath(NewPath, false);

	ZEFLTResource* Resource = (ZEFLTResource*)zeResources->GetResource(NewPath);
	
	if (Resource != NULL)
		return Resource;
	else
	{
		Resource = LoadResource(NewPath);
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
};

void ZEFLTResource::CacheResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	NewPath = ZEPathUtils::GetSimplifiedPath(NewPath, false);

	ZEFLTResource* Resource = (ZEFLTResource*)zeResources->GetResource(NewPath);
	if (Resource != NULL)
		Resource->Cached = true;
	else
	{
		Resource = LoadResource(NewPath);
		Resource->Cached = true;
		Resource->ReferenceCount = 0;
		zeResources->AddResource(Resource);
	}
};
