//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInitializableAsync.cpp
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

#include "ZEInitializableAsync.h"

#include "ZEMeta\ZEClass.h"

void ZEInitializableAsync::InitializationDone()
{
	zeDebugCheck(InitializationState != ZE_IS_INITIALIZING, "InitializeDone called while State != ZE_IS_INITIALIZING");
	if (InitializationState != ZE_IS_INITIALIZING)
		return;

	#ifdef ZE_DEBUG_ENABLE
	if (!CallChainCompleted)
		zeWarning("Initialization chain is not complete. Class Name: \"%s\".", GetClass() != NULL ? GetClass()->GetName() : "<Non-ZEObject>");	
	#endif

	InitializationState = ZE_IS_INITIALIZED;

	OnInitialized(this);
}

void ZEInitializableAsync::InitializationFailed()
{
	zeDebugCheck(InitializationState != ZE_IS_INITIALIZING, "InitializeDone called while State != ZE_IS_INITIALIZING");
	if (InitializationState != ZE_IS_INITIALIZING)
		return;

	InitializationState = ZE_IS_ERROR_INITIALIZATION;
	zeError("Initialization failed. Class Name: \"%s\".", GetClass() != NULL ? GetClass()->GetName() : "<Non-ZEObject>");

	OnInitializationFailed(this);
}

void ZEInitializableAsync::DeinitializationDone()
{
	zeDebugCheck(InitializationState != ZE_IS_DEINITIALIZING, "InitializeDone called while State != ZE_IS_INITIALIZING");
	if (InitializationState != ZE_IS_DEINITIALIZING)
		return;

	#ifdef ZE_DEBUG_ENABLE
	if (!CallChainCompleted)
		zeWarning("Deinitialization chain is not complete. Class Name: \"%s\".", GetClass() != NULL ? GetClass()->GetName() : "<Non-ZEObject>");	
	#endif

	InitializationState = ZE_IS_UNINITIALIZED;

	OnDeinitialized(this);
}

void ZEInitializableAsync::DeinitializationFailed()
{
	zeDebugCheck(InitializationState != ZE_IS_DEINITIALIZING, "InitializeDone called while State != ZE_IS_INITIALIZING");
	if (InitializationState != ZE_IS_DEINITIALIZING)
		return;

	InitializationState = ZE_IS_ERROR_DEINITIALIZATION;

	OnDeinitializationFailed(this);
}

ZETaskResult ZEInitializableAsync::UpdateStateFunction(ZETaskThread* Thread, ZESize InstanceIndex, void* Parameters)
{
	// Persistent States
	if (InitializationState == ZE_IS_UNINITIALIZED)
	{
		ReinitializeFlag = false;

		if (InitializationTargetState == ZE_IS_INITIALIZED)
		{
			InitializationState = ZE_IS_INITIALIZING;
			return UpdateStateFunction(Thread, 0, Parameters);
		}

	}
	else if (InitializationState == ZE_IS_INITIALIZED)
	{
		if (ReinitializeFlag || InitializationTargetState == ZE_IS_UNINITIALIZED)
			InitializationState = ZE_IS_DEINITIALIZING;
	}
	
	// Transition States
	if (InitializationState == ZE_IS_INITIALIZING)
	{
		ZEInitializationResult Result = InitializeInternal();
		if (Result == ZE_IR_DONE)
		{
			InitializationDone();
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_IR_ASYNC)
		{
			return ZE_TR_DONE;
		}
		else if (Result == ZE_IR_COOPERATING)
		{
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_TR_FAILED)
		{
			InitializationFailed();
			return ZE_TR_FAILED;
		}
	}
	else if (InitializationState == ZE_IS_DEINITIALIZING)
	{
		ZEInitializationResult Result = DeinitializeInternal();
		if (Result == ZE_TR_DONE)
		{
			DeinitializationDone();
			return ZE_TR_COOPERATING;
		}
		else if (Result == ZE_IR_ASYNC)
		{
			return ZE_TR_DONE;
		}
		else if (Result == ZE_IR_COOPERATING)
		{
			return ZE_TR_COOPERATING;
		}
		else
		{
			DeinitializationFailed();
			return ZE_TR_FAILED;
		}
	}

	return ZE_TR_DONE;
}

void ZEInitializableAsync::UpdateState()
{
	//UpdateStateTask.Run();
	while (UpdateStateFunction(NULL, 0, NULL) != ZE_TR_COOPERATING);
}

ZEInitializationResult ZEInitializableAsync::InitializeInternal()
{
	#ifdef ZE_DEBUG_ENABLE
		CallChainCompleted = true;
	#endif

	return ZE_IR_DONE;
}

ZEInitializationResult ZEInitializableAsync::DeinitializeInternal()
{
	#ifdef ZE_DEBUG_ENABLE
		CallChainCompleted = true;
	#endif

	return ZE_IR_DONE;
}

ZEClass* ZEInitializableAsync::GetClass() const
{
	return NULL;
}

ZEInitializationState ZEInitializableAsync::GetInitializationState() const
{
	return InitializationState;
}

bool ZEInitializableAsync::IsInitialized() const
{
	return InitializationState == ZE_IS_INITIALIZED;
}

bool ZEInitializableAsync::IsInitializedOrInitializing() const
{
	return InitializationState == ZE_IS_INITIALIZED || InitializationState == ZE_IS_INITIALIZING || (InitializationTargetState  == ZE_IS_INITIALIZED && InitializationTargetState != ZE_IS_ERROR_INITIALIZATION);
}

void ZEInitializableAsync::Initialize()
{
	if (InitializationTargetState == ZE_IS_INITIALIZED)
		return;

	InitializationTargetState = ZE_IS_INITIALIZED;

	UpdateState();
}

void ZEInitializableAsync::Deinitialize()
{
	if (InitializationTargetState == ZE_IS_UNINITIALIZED)
		return;

	InitializationTargetState = ZE_IS_UNINITIALIZED;

	UpdateState();
}

void ZEInitializableAsync::Reinitialize()
{
	ReinitializeFlag = true;

	if (InitializationTargetState <= ZE_IS_INITIALIZED)
		InitializationTargetState = ZE_IS_INITIALIZED;

	UpdateState();
}

ZEInitializableAsync::ZEInitializableAsync()
{
	InitializationState = ZE_IS_UNINITIALIZED;
	InitializationTargetState = ZE_IS_UNINITIALIZED;
	ReinitializeFlag = false;
	UpdateStateTask.SetFunction(ZETaskFunction::Create<ZEInitializableAsync, &ZEInitializableAsync::UpdateStateFunction>(this));
}

ZEInitializableAsync::~ZEInitializableAsync()
{
	if (IsInitializedOrInitializing() || InitializationState == ZE_IS_ERROR_DEINITIALIZATION)
		zeError("Destroying Initializable object that is still initialized or deinitializing. Class Name: \"%s\".", GetClass() != NULL ? GetClass()->GetName() : "<Non-ZEObject>");	
}
