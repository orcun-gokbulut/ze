//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSResourceLoadable.cpp
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

#include "ZERSResourceLoadable.h"

#include "ZELog.h"
#include "ZEError.h"
#include "ZEFile\ZEPathInfo.h"

ZETaskResult ZERSResourceLoadable::ManageStatesFunction(ZETaskThread* TaskThread, void* Parameters)
{
	if (LoadState == ZERS_LS_LOADED && TargetState == ZERS_LS_NOT_LOADED)
	{
		LoadState = ZERS_LS_UNLOADING;
	}
	else if (LoadState == ZERS_LS_NOT_LOADED && TargetState == ZERS_LS_LOADED)
	{
		LoadState = ZERS_LS_LOADING;
	}
	
	if (LoadState == ZERS_LS_LOADING)
	{
		ZETaskResult Result = LoadInternal();
		if (Result == ZE_TR_DONE)
		{
			zeLog("Resource loaded. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());
			LoadState = ZERS_LS_LOADED;
			return ZE_TR_DONE;
		}
		else if (Result == ZE_TR_FAILED)
		{
			zeError("Resource loading failed. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());
			LoadState = ZERS_LS_ERROR_LOADING;
			return ZE_TR_DONE;
		}
		else
		{
			 return Result;
		}
	}
	else if (LoadState == ZERS_LS_UNLOADING)
	{
		ZETaskResult Result = UnloadInternal();
		if (Result == ZE_TR_DONE)
		{
			zeLog("Resource unloaded. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());
			LoadState = ZERS_LS_NOT_LOADED;
			return ZE_TR_DONE;
		}
		else if (Result == ZE_TR_FAILED)
		{
			zeError("Resource unloading failed. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());
			LoadState = ZERS_LS_ERROR_UNLOADING;
			return ZE_TR_DONE;
		}
		else
		{
			return Result;
		}
	}

	return ZE_TR_DONE;
}

ZETaskResult ZERSResourceLoadable::LoadInternal()
{
	return ZE_TR_DONE;
}

ZETaskResult ZERSResourceLoadable::UnloadInternal()
{
	return ZE_TR_DONE;
}

ZERSResourceLoadable::ZERSResourceLoadable()
{
	LoadState = ZERS_LS_NOT_LOADED;
	TargetState = ZERS_LS_NOT_LOADED;
	FileNameHash = 0;
	ManageStatesTask.SetPool(ZE_TPI_IO);
	ManageStatesTask.SetFunction(ZEDelegateMethod(ZETaskFunction, ZERSResourceLoadable, ManageStatesFunction, this));
}

ZERSResourceLoadable::~ZERSResourceLoadable()
{
	Unload();
}

ZERSResourceType ZERSResourceLoadable::GetType() const
{
	return ZERS_RT_LOADABLE;
}

ZERSLoadState ZERSResourceLoadable::GetLoadState() const
{
	return LoadState;
}

const ZEString& ZERSResourceLoadable::GetFileName() const
{
	return FileName;
}

bool ZERSResourceLoadable::IsLoaded()
{
	return LoadState == ZERS_LS_LOADED;
}

bool ZERSResourceLoadable::IsFailed()
{
	return LoadState == ZERS_LS_ERROR_LOADING || LoadState == ZERS_LS_ERROR_UNLOADING;
}

void ZERSResourceLoadable::Load(const ZEString& FileName)
{
	if (LoadState == ZERS_LS_LOADING || LoadState == ZERS_LS_LOADED)
		return;

	zeCheckError(IsShared(), ZE_VOID, "Shared resources cannot be loaded again. File Name: \"%s\".", FileName.ToCString());

	zeLog("Loading resource. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());

	this->FileName = ZEPathInfo(FileName).Normalize();
	FileNameHash = this->FileName.Lower().Hash();

	TargetState = ZERS_LS_LOADED;
	ManageStatesTask.Run();
}

void ZERSResourceLoadable::Unload()
{
	if (LoadState == ZERS_LS_NOT_LOADED || LoadState == ZERS_LS_UNLOADING)
		return;

	zeLog("Unloading resource. Resource Class: \"%s\", File Name: \"%s\".", GetClass()->GetName(), this->FileName.ToCString());
	
	TargetState = ZERS_LS_NOT_LOADED;
	ManageStatesTask.Run();
}

void ZERSResourceLoadable::WaitLoad()
{
	while (LoadState == ZERS_LS_LOADING);
}

void ZERSResourceLoadable::WaitUnload()
{
	while (LoadState == ZERS_LS_ERROR_UNLOADING);
}
