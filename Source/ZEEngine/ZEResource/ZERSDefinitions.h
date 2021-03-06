//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSDefinitions.h
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

#pragma  once

#include "ZEMeta/ZEEnumerator.h"

#define ZERS_MEMORY_POOL_COUNT 2

ZE_ENUM(ZERSMemoryPool)
{
	ZERS_MP_CPU,
	ZERS_MP_GPU
};

ZE_ENUM( ZERSResourceType)
{
	ZERS_RT_NONE,
	ZERS_RT_NORMAL,
	ZERS_RT_LOADABLE
};

ZE_ENUM(ZERSResourceState)
{
	ZERS_RS_ERROR_UNLOADING		= -3,
	ZERS_RS_ERROR_LOADING		= -2,
	ZERS_RS_ERROR_STAGING		= -1,
	ZERS_RS_NONE				= 0,
	ZERS_RS_DESTROYED			= 1,
	ZERS_RS_DESTROYING			= 2,
	ZERS_RS_STAGING				= 3,
	ZERS_RS_STAGED				= 4,
	ZERS_RS_UNLOADING			= 5,
	ZERS_RS_LOADING				= 6,
	ZERS_RS_ITERATING			= 7,
	ZERS_RS_LOADED				= 8
};

class ZERSResource;
typedef ZERSResource* (_cdecl *ZERSInstanciator)(const void*);
