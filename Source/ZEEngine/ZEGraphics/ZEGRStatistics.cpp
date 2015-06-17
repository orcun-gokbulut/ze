//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRStatistics.cpp
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

#include "ZEGRStatistics.h"

#include <memory.h>

void ZEGRStatistics::Clean()
{
	memset(&DrawStatistics, 0, sizeof(ZEGRStatisticsDrawCall));
	memset(&MemoryStatistics, 0, sizeof(ZEGRStatisticsMemory));
	memset(&ObjectStatistics, 0, sizeof(ZEGRStatisticsObjects));

}

void ZEGRStatistics::Reset()
{
	DrawStatistics.DrawCall = 0;
	DrawStatistics.InstancedDrawCall = 0;
	DrawStatistics.DrawedPrimtivies = 0;
	DrawStatistics.StateChanges = 0;
}

ZEGRStatistics::ZEGRStatistics()
{
	Clean();
}

const ZEGRStatisticsDrawCall& ZEGRStatistics::GetDrawStatistics()
{
	return DrawStatistics;
}

const ZEGRStatisticsMemory& ZEGRStatistics::GetMemoryStatistics()
{
	return MemoryStatistics;
}

const ZEGRStatisticsObjects& ZEGRStatistics::GetObjectStatistics()
{
	return ObjectStatistics;
}
