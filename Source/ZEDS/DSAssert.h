//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DSAssert.h
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
#ifndef	__ZEDS_ASSERT_H__
#define __ZEDS_ASSERT_H__

	#ifdef ZEDEBUG_ENABLED
	#define ZEMATH_DEBUG_MODE
	#endif

	#ifdef ZEMATH_DEBUG_MODE
		#ifdef __ZINEK_ENGINE__
			void __ZEDS_ASSERT(const char* Function, const char* File, int Line, const char* Message, ...);
			void __ZEDS_WARNING(const char* Function, const char* File, int Line, const char* Message, ...);
			#define ZEDS_ASSERT(Condition, ...) if (Condition) {__ZEDS_ASSERT(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);}
			#define ZEDS_WARNING(Condition, ...) if (Condition) {__ZEDS_ASSERT(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);}
		#elif defined(__ZETOOLSDK__)
			#define ZEMATH_ASSERT(Condition, ...) 
			#define ZEMATH_WARNING(Condition, ...)
		#else
			#define ZEMATH_ASSERT(Condition, ...) 
			#define ZEMATH_WARNING(Condition, ...)
		#endif
	#else
		#define ZEDS_ASSERT(Condition, ...) 
		#define ZEDS_WARNING(Condition, ...)
	#endif

#endif
