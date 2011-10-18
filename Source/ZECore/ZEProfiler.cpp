//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProfiler.cpp
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

#include "ZEProfiler.h"
#include "ZEProfilerCounter.h"

#include <memory.h>

ZEProfiler::ZEProfiler(void)
{
}

ZEProfiler::~ZEProfiler(void)
{
}

ZEProfilerCounter* ZEProfiler::GetCounter(const ZEString& Name)
{
	if(Stack.GetSize() > 0)
	{
		for (size_t I = 0; I < Counters.GetCount(); I++)
			if (Stack.GetLastItem() == Counters[I]->GetParent() && Counters[I]->GetName() == Name)
				return Counters[I];
	}

	ZEProfilerCounter* NewProfiler = new ZEProfilerCounter();
	Counters.Add(NewProfiler);
	Stack.Add(NewProfiler);
	NewProfiler->SetName(Name);
	NewProfiler->SetManager(this);
	NewProfiler->SetParent(Stack.GetLastItem());
	return NewProfiler;
}

ZEProfilerCounter* ZEProfiler::GetCounterForData(const ZEString& Name)
{
	for (size_t I = 0; I < Counters.GetCount(); I++)
		if (Counters[I]->GetName() == Name)
			return Counters[I];

	return NULL;
}

const ZEArray<ZEProfilerCounter*>& ZEProfiler::GetCounters()
{
	return Counters;
}

ZEProfiler* ZEProfiler::GetInstance()
{
	static ZEProfiler Manager;
	return &Manager;
}
