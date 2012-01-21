//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture3DResource.cpp
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
#include "ZETexture3D.h"
#include "ZEGraphicsModule.h"
#include "ZETexture3DResource.h"
#include "ZECore/ZEResourceManager.h"


static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	ZEUInt ConstLength = strlen("resources\\") - 1;

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

const char* ZETexture3DResource::GetResourceType() const
{
	return "Volume Texture Resource";
}

ZETextureType ZETexture3DResource::GetTextureType() const
{
	return ZE_TT_3D;
}

const ZETexture3D* ZETexture3DResource::GetTexture() const
{
	return Texture;
}

ZETexture3DResource::ZETexture3DResource()
{
	Texture = NULL;
}

ZETexture3DResource::~ZETexture3DResource()
{
	if (Texture != NULL)
		Texture->Destroy();
}

void ZETexture3DResource::CacheResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	zeAssert(true, "NOT IMPLEMENTED YET");
}

ZETexture3DResource* ZETexture3DResource::LoadSharedResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	zeAssert(true, "NOT IMPLEMENTED YET");

	return NULL;
}

ZETexture3DResource* ZETexture3DResource::LoadResource(const ZEString& FileName, const ZETextureOptions* UserOptions)
{
	zeAssert(true, "NOT IMPLEMENTED YET");

	return NULL;
}





