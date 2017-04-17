//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEvent.h
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

#include "ZEMTMethodSignatureGenerator.h"
#include "ZEEventDelegate.h"
#include "ZEObject.h"
#include "ZEExport.ZEFoundation.h"

#define ZE_EVENT(Name, Parameters) ZEEvent<void Parameters> Name; 

struct ZEMTEventStackItem
{
	ZEMTEventStackItem* Previous;
	bool Acquired;
};

class ZE_EXPORT_ZEFOUNDATION ZEMTEventBase
{
	friend class ZEObject;
	private:
		bool								Suppressed;

		virtual void						CloneConnections(ZEObject* SourceObject, ZEObject* NewObject) = 0;

	protected:
		static void							BeginDistribution(ZEMTEventStackItem* StackItem);
		static void							EndDistribution();

	public:
		virtual const ZEMTMethodSignature&	GetSignature() const = 0;

		void								SetSuppressed(bool Suppressed);
		bool								GetSuppressed() const;

		virtual void						DisconnectObject(ZEObject* Object) = 0;

		static bool							IsAcquired();
		static void							Acquire();
		static void							Unacquire();

											ZEMTEventBase();
};

template <typename TSignature> 
class ZEEvent;

#define ZE_MACRO_INCLUDE_FILE_NAME "ZEMeta/ZEEventImp.h"
#define ZE_MACRO_INCLUDE_COUNT 30
#include "ZEMacro/ZEMacroIncludeRepeater.h"
#undef ZE_MACRO_INCLUDE_FILE_NAME
#undef ZE_MACRO_INCLUDE_COUNT
