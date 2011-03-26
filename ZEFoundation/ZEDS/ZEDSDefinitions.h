//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSDefinitions.h
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
#ifndef	__ZEDS_DEFINITIONS_H__
#define __ZEDS_DEFINITIONS_H__

//#include "CompileOptions.h"

#ifdef ZE_DEBUG_ENABLED
	#define ZEDS_DEBUG_MODE
#endif

#ifdef ZEDS_DEBUG_MODE
	#ifdef ZE_ZINEK_ENGINE
		#include "ZECore\ZEError.h"
		#define zedsAssert(Condition, ...) zeAssert(Condition, __VA_ARGS__) 
		#define zedsWarningAssert(Condition, ...) zeWarningAssert(Condition, __VA_ARGS__)
	#else	
		void stdAssert(const char* Function, const char* File, int Line, const char* Message, ...);
		void stdWarningAssert(const char* Function, const char* File, int Line, const char* Message, ...);
		#define zedsAssert(Condition, ...) if (Condition) stdAssert(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
		#define zedsWarningAssert(Condition, ...) if (Condition) stdWarningAssert(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
	#endif
#else
	#define zedsAssert(Condition, Message)
	#define zedsWarningAssert(Condition, Message)
#endif

#include <memory.h>

#ifndef NULL
	#define NULL 0
#endif


template <typename Type>
inline void ObjectConcat(Type* Destiantion, Type* Source1, size_t Count1, Type* Source2, size_t Count2)
{
	memcpy(Destination, Source1, Count1 * sizeof(Type));
	memcpy(Destination + Count1, Source2, Count2 * sizeof(Type));
}

template <typename Type>
inline void ObjectCopy(Type* Destination, Type* Source, size_t Count)
{
	//memcpy(Destination, Source, Count * sizeof(Type));
	while(Count--)
		*Destination++ = *Source++;
}

#endif





