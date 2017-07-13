//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPlugin.cpp
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

#include "ZEPlugin.h"

#include "ZEError.h"
#include "ZEVersion.h"
#include "ZEDS/ZEFormat.h"
#include "ZEMeta/ZEMTProvider.h"
#include "ZEMeta/ZEMTDeclaration.h"
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZEFile/ZEFileInfo.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


ZEPlugin::ZEPlugin() : CoreLink(this)
{
	Data = NULL;
	Core = NULL;
}

ZEPlugin::~ZEPlugin()
{
	if (Core != NULL)
		zeCriticalError("Cannot deconstruct plugin it is still loaded to the core. Plugin Name: \"%s\".", GetName());
}

const char* ZEPlugin::GetName() const
{
	return "";
}

ZEVersion ZEPlugin::GetVersion() const
{
	return ZEVersion(1, 0, 0);
}

ZECore* ZEPlugin::GetCore() const
{
	return Core;
}

ZEVersion ZEPlugin::GetEngineVersion() const
{
	return ZEVersion::GetZinekVersion();
}

const ZEString& ZEPlugin::GetFileName() const
{
	return FileName;
}

ZESharedLibraryData* ZEPlugin::GetData() const
{
	return Data;
}

ZEMTDeclaration* const* ZEPlugin::GetDeclarations() const
{
	return NULL;
}

ZESize ZEPlugin::GetDeclarationCount() const
{
	return 0;
}

void ZEPlugin::Destroy()
{
	delete this;
}
