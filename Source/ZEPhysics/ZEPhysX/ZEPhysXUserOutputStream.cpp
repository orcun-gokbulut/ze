//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXUserOutputStream.cpp
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

#include "ZEPhysXUserOutputStream.h"
#include "ZEError.h"
#include "ZECore/ZEConsole.h"

void ZEPhysXUserOutputStream::reportError(NxErrorCode Code, const char* Message, const char* File, int Line)
{
	switch(Code)
	{
		case NXE_NO_ERROR:
			break;
		default:
		case NXE_OUT_OF_MEMORY:
		case NXE_INTERNAL_ERROR:
			zeCriticalError("PhysX", "Error Code: %d, Message: \"%s\", File: \"%s\", Line: %d.", Code, Message, File, Line);
			break;

		case NXE_INVALID_PARAMETER:
		case NXE_INVALID_OPERATION:
		case NXE_ASSERTION:
			zeError("PhysX", "Assert ! Error Code: %d, Message: \"%s\", File: \"%s\", Line: %d.", Code, Message, File, Line);
			break;

		case NXE_DB_PRINT:
			zeLog("PhysX", Message);
			break;

		case NXE_DB_INFO:
			zeNotice("PhysX", "Error Code: %d, Message: \"%s\", File: \"%s\", Line: %d.", Code, Message, File, Line);
			break;

		case NXE_DB_WARNING:
			zeWarning("PhysX", "Error Code: %d, Message: \"%s\", File: \"%s\", Line: %d.", Code, Message, File, Line);
			break;
	}
}

NxAssertResponse ZEPhysXUserOutputStream::reportAssertViolation(const char* Message, const char* File, int Line)
{
	zeCriticalError("PhysX", "Message: \"%s\", File: \"%s\", Line: %d.", Message, File, Line);
	return NX_AR_CONTINUE;
}

void ZEPhysXUserOutputStream::print(const char* Message)
{
	zeLog("PhysX", Message);
}




