//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEntityMacros.h
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

#pragma once

#include "ZEError.h"

#define ZE_ENTITY_FUNCTION_CHAIN(BaseClass, Function) do {ZEEntityResult Result = __super::Function(); if (Result != ZE_ER_DONE) return Result;} while(false)
#define ZE_ENTITY_LOAD_CHAIN(BaseClass) ZE_ENTITY_FUNCTION_CHAIN(BaseClass, LoadInternal)
#define ZE_ENTITY_UNLOAD_CHAIN(BaseClass) ZE_ENTITY_FUNCTION_CHAIN(BaseClass, UnloadInternal)
#define ZE_ENTITY_INITIALIZE_CHAIN(BaseClass) ZE_ENTITY_FUNCTION_CHAIN(BaseClass, InitializeInternal)
#define ZE_ENTITY_DEINITIALIZE_CHAIN(BaseClass) ZE_ENTITY_FUNCTION_CHAIN(BaseClass, DeinitializeInternal)
#define ZE_ENTITY_DESTROY_CHAIN(BaseClass) ZE_ENTITY_FUNCTION_CHAIN(BaseClass, DestroyInternal)

#define ZE_ENTITY_FENCE(Entity, TargetState, FailedReturn) \
	if (Entity != NULL) \
	{ \
		if (Resource->IsFailed()) \
		{ \
			return FailedReturn; \
		} \
		else if (Entity->GetState() < TargetState) \
		{ \
			return ZE_ER_WAIT; \
		} \
	} \
	else

#define ZE_ENTITY_RESOURCE_FENCE(Resource, TargetState, FailedReturn, LoadPercentageStart, LoadPercentageEnd) \
	if (Resource != NULL) \
	{ \
		if (Resource->IsFailed()) \
		{ \
			return FailedReturn; \
		} \
		else if (Resource->GetState() < TargetState) \
		{ \
			ZEUInt Percentage = LoadPercentageStart + (Resource->GetLoadPercentage() * (LoadPercentageEnd - LoadPercentageStart)) / 100; \
			if (Percentage < GetLocalLoadingPercentage()) \
				SetLocalLoadingPercentage(Percentage); \
			return ZE_ER_WAIT; \
		} \
	} \
	else

#define ZE_ENTITY_RESOURCE_FENCE_PROGRESS(Resource, TargetState, FailedReturn, LoadPercentageStart, LoadPercentageEnd) \
	if (Resource != NULL) \
	{ \
		if (Resource->IsFailed()) \
		{ \
			return FailedReturn; \
		} \
		else if (Resource->GetState() < TargetState) \
		{ \
			ZEUInt Percentage = LoadPercentageStart + (Resource->GetLoadProgress() * (LoadPercentageEnd - LoadPercentageStart)) / 100; \
			if (Percentage < GetLocalLoadingPercentage()) \
				SetLocalLoadingPercentage(Percentage); \
			return ZE_ER_WAIT; \
		} \
	} \
	else

#define ZE_ENTITY_FENCE_LOADED(Entity, FailedReturn) ZE_ENTITY_FENCE(Entity, ZE_ES_LOADED, FailedReturn)
#define ZE_ENTITY_FENCE_INITIALIZED(Entity, FailedReturn) ZE_ENTITY_FENCE(Entity, ZE_ES_INITIALIZED, FailedReturn)
#define ZE_ENTITY_RESOURCE_FENCE_STAGED(Resource, FailedReturn) ZE_ENTITY_RESOURCE_FENCE(Resource, ZERS_RS_STAGED, FailedReturn)
#define ZE_ENTITY_RESOURCE_FENCE_LOADED(Resource, FailedReturn) ZE_ENTITY_RESOURCE_FENCE(Resource, ZERS_RS_LOADED, FailedReturn)
#define ZE_ENTITY_RESOURCE_FENCE_LOADED_PROGRESS(Resource, FailedReturn, LoadPercentageStart, LoadPercentageEnd) ZE_ENTITY_RESOURCE_FENCE_PROGRESS(Resource, ZERS_RS_LOADED, FailedReturn, LoadPercentageStart, LoadPercentageEnd)
