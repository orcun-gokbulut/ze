//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInitializableAsync.h
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

#include "ZEInitializable.h"
#include "ZEThread/ZETask.h"
#include "ZEMeta/ZEEvent.h"
#include "ZEExport.ZEFoundation.h"

#define ZE_INITIALIZABLE_FUNCTION_CHAIN(BaseClass, Function) do {ZEInitializationResult Result = __super::Function(); if (Result != ZE_IR_DONE) return Result;} while(false)
#define ZE_INITIALIZABLE_INITIALIZE_CHAIN(BaseClass) ZE_INITIALIZABLE_FUNCTION_CHAIN(BaseClass, InitializeInternal)
#define ZE_INITIALIZABLE_DEINITIALIZE_CHAIN(BaseClass) ZE_INITIALIZABLE_FUNCTION_CHAIN(BaseClass, DeinitializeInternal)

enum ZEInitializationResult
{
	ZE_IR_FAILED,
	ZE_IR_DONE,
	ZE_IR_ASYNC,
	ZE_IR_COOPERATING
};

class ZE_EXPORT_ZEFOUNDATION ZEInitializableAsync
{
	private:
		#ifdef ZE_DEBUG_ENABLE
		bool							CallChainCompleted;
		#endif

		volatile ZEInitializationState	InitializationState;
		volatile ZEInitializationState	InitializationTargetState;
		volatile bool					ReinitializeFlag;
		ZETask							UpdateStateTask;

		void							InitializationDone();
		void							InitializationFailed();

		void							DeinitializationDone();
		void							DeinitializationFailed();

		ZETaskResult					UpdateStateFunction(ZETaskThread* Thread, ZESize InstanceIndex, void* Parameters);
		void							UpdateState();

	protected:
		virtual ZEInitializationResult	InitializeInternal();
		virtual ZEInitializationResult	DeinitializeInternal();

	public:
		virtual ZEClass*				GetClass() const;

		ZEInitializationState			GetInitializationState() const;

		bool							IsInitialized() const;
		bool							IsInitializedOrInitializing() const;
		bool							IsFailed() const;

		void							Initialize();
		void							Deinitialize();
		void							Reinitialize();

		ZE_EVENT(						OnInitialized, (ZEInitializableAsync*));
		ZE_EVENT(						OnDeinitialized, (ZEInitializableAsync*));
		ZE_EVENT(						OnInitializationFailed, (ZEInitializableAsync*));
		ZE_EVENT(						OnDeinitializationFailed, (ZEInitializableAsync*));

										ZEInitializableAsync();
		virtual							~ZEInitializableAsync();
};
