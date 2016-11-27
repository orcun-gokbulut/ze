//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInitializable.cpp
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

#include "ZEInitializable.h"

#include "ZEMeta\ZEClass.h"

bool ZEInitializable::InitializeInternal()
{
	#ifdef ZE_DEBUG_ENABLE
	CallChainCompleted = true;
	#endif

	return true;
}

bool ZEInitializable::DeinitializeInternal()
{
	#ifdef ZE_DEBUG_ENABLE
	CallChainCompleted = true;
	#endif

	return true;
}

ZEClass* ZEInitializable::GetClass() const
{
	return NULL;
}

ZEInitializationState ZEInitializable::GetInitializationState() const
{
	return InitializationState;
}

bool ZEInitializable::IsInitialized() const
{
	return InitializationState == ZE_IS_INITIALIZED;
}

bool ZEInitializable::IsInitializedOrInitializing() const
{
	return InitializationState == ZE_IS_INITIALIZED || InitializationState == ZE_IS_INITIALIZING;
}

bool ZEInitializable::Initialize()
{
	if (InitializationState == ZE_IS_INITIALIZED)
		return true;

	if (InitializationState != ZE_IS_UNINITIALIZED && InitializationState != ZE_IS_ERROR_INITIALIZATION)
		return false;

	InitializationState = ZE_IS_INITIALIZING;

	#ifdef ZE_DEBUG_ENABLE
	CallChainCompleted = false;
	#endif

	if (!InitializeInternal())
	{
		zeError("Initialization failed. Class Name: \"%s\".", GetClass() != NULL ? GetClass()->GetName() : "<Non-ZEObject>");
		InitializationState = ZE_IS_ERROR_INITIALIZATION;
		return false;
	}

	#ifdef ZE_DEBUG_ENABLE
	if (!CallChainCompleted)
		zeWarning("Initialization chain is not complete. Class Name: \"%s\".", GetClass() != NULL ? GetClass()->GetName() : "<Non-ZEObject>");	
	#endif

	InitializationState = ZE_IS_INITIALIZED;

	return true;
}

bool ZEInitializable::Deinitialize()
{
	if (InitializationState == ZE_IS_UNINITIALIZED)
		return true;

	if (InitializationState != ZE_IS_INITIALIZED && InitializationState == ZE_IS_ERROR_DEINITIALIZATION)
		return false;

	InitializationState = ZE_IS_DEINITIALIZING;

	#ifdef ZE_DEBUG_ENABLE
	CallChainCompleted = false;
	#endif

	if (!DeinitializeInternal())
	{
		zeError("Deinitalization failed. Class Name: %s.", GetClass() != NULL ? GetClass()->GetName() : "<Non-ZEObject>");
		InitializationState = ZE_IS_ERROR_DEINITIALIZATION;
		return false;
	}

	#ifdef ZE_DEBUG_ENABLE
	if (!CallChainCompleted)
		zeWarning("Initialization chain is not complete. Class Name: \"%s\".", GetClass() != NULL ? GetClass()->GetName() : "<Non-ZEObject>");	
	#endif
	
	InitializationState = ZE_IS_UNINITIALIZED;

	return true;
}

bool ZEInitializable::Reinitialize()
{
	if (!Deinitialize())
		return false;

	if (!Initialize())
		return false;

	return true;
}

ZEInitializable::ZEInitializable()
{
	InitializationState = ZE_IS_UNINITIALIZED;
}

ZEInitializable::~ZEInitializable()
{
	DeinitializeInternal();
	//zeDebugCheck(InitializationState != ZE_IS_UNINITIALIZED, "Object is not deinitialized. Class Name: \"%s\".", GetClass() != NULL ? GetClass()->GetName() : "<Non-ZEObject>");
}
