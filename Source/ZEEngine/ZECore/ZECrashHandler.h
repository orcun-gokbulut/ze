//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECrashHandler.h
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

#include "ZEDS/ZEString.h"
#include "ZEVersion.h"
#include "ZEExport.ZEEngine.h"
#include "ZEModule.h"

enum ZECRMemoryDumpType
{
	ZECR_CDT_MINIMAL,
	ZECR_CDT_NORMAL,
	ZECR_CDT_FULL
};

ZE_ENUM(ZECrashReason)
{
	ZE_CR_NONE,
	ZE_CR_CRITICIAL_ERROR,
	ZE_CR_UNHANDLED_EXCEPTION,
	ZE_CR_UNHANDLED_SYSTEM_EXCEPTION,
	ZE_CR_ACCESS_VIOLATION,
	ZE_CR_STACK_OVERFLOW,
	ZE_CR_PREMATURE_TERMINATION,
	ZE_CR_OUT_OF_MEMORY,
	ZE_CR_PURE_VIRTUAL_CALL,
	ZE_CR_INDEX_OUT_OF_BOUNDS,
	ZE_CR_INVALID_CALL,
	ZE_CR_PAGE_ERROR,
	ZE_CR_ABORT,
	ZE_CR_WATCH_DOG_TIMER,
	ZE_CR_DIVISION_BY_ZERO,
	ZE_CR_ILLEGAL_INSTRUCTION,
	ZE_CR_OTHER
};

struct ZECRReportParameters;

class ZE_EXPORT_ZEENGINE ZECrashHandler : public ZEModule
{
	ZE_OBJECT
	friend class ZECore;
	private:
		bool						ExecuteCrashReporter;
		ZELock						CrashLock;

		void						RegisterHandlers();
		void						UnregisterHandlers();

		bool						InitializeInternal();
		bool						DeinitializeInternal();

		void						GenerateParameters(ZECRReportParameters& Parameters);

									ZECrashHandler();
									~ZECrashHandler();

	public:
		void						SetExecuteCrashReporter(bool Enabled);
		bool						GetExecuteCrashReporter() const;

		void						Crashed(ZECrashReason Reason);

		static ZECrashHandler*		CreateInstance();
};
