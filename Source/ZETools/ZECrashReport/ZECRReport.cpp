//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRReport.cpp
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

#include "ZECRReport.h"
#include "ZECRCollector.h"

const ZEArray<ZECRCollector*>& ZECRReport::GetCollectors()
{
	return Collectors;
}

ZECRCollector* ZECRReport::GetCollector(const ZEString& Name)
{
	for (ZESize I = 0; I < Collectors.GetCount(); I++)
	{
		if (Name == Collectors[I]->GetName())
			return Collectors[I];
	}

	return NULL;
}

bool ZECRReport::AddCollector(ZECRCollector* Collector)
{
	zeCheckError(Collector == NULL, false, "Cannot remove collector. Collector is NULL.");
	zeCheckError(Collector->Report != NULL, false, "Cannot remove collector. Collector has been already added to a report.");

	Collectors.Add(Collector);

	return true;
}

void ZECRReport::RemoveCollector(ZECRCollector* Collector)
{
	zeCheckError(Collector == NULL, ZE_VOID, "Cannot remove collector. Collector is NULL.");
	zeCheckError(Collector->Report != this, ZE_VOID, "Cannot remove collector. Collector does not belong to this report.");

	Collectors.RemoveValue(Collector);
}

void ZECRReport::Generate(const ZECRReportParameters* Parameters)
{
	for (ZESize I = 0; I < Collectors.GetCount(); I++)
		Collectors[I]->Generate(Parameters);
}

void ZECRReport::CleanUp()
{
	for (ZESize I = 0; I < Collectors.GetCount(); I++)
		Collectors[I]->CleanUp();
}

ZECRReport::~ZECRReport()
{
	CleanUp();
}
