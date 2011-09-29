//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProfiler.h
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
#ifndef __ZE_PROFILER_H__
#define __ZE_PROFILER_H__

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"

class ZEProfilerManager;

class ZEProfiler
{
	protected:
		ZEString					Name;
		ZEProfiler*					ParentProfiler;
		ZEProfilerManager*			Manager;
		bool						ProfilerActive;

		ZEINT64						StartTime;
		ZEINT64						EndTime;
		ZEINT64						PassedTime;

		ZEINT64 					TotalCount;
		ZEINT64						FrameCount;
		ZEINT64						TotalMinimumTime;
		ZEINT64						FrameMinimumTime;
		ZEINT64						TotalMaximumTime;
		ZEINT64						FrameMaximumTime;
		ZEINT64						TotalTime;
		ZEINT64						FrameTotalTime;


	public:
		void						SetName(const ZEString& Name);
		const ZEString&				GetName();

		void						SetManager(ZEProfilerManager* Manager);

		ZEProfiler*					GetParent();
		void						SetParent(ZEProfiler* ParentProfiler);

		void						Start();
		void						Stop();

		ZEINT64						GetFrameCount();
		ZEINT64						GetFrameMinimumTime();
		ZEINT64						GetFrameMaximumTime();
		ZEINT64						GetFrameTotalTime();
		ZEINT64						GetFrameAverageTime();

		ZEINT64						GetTotalCount();
		ZEINT64						GetTotalMinimumTime();
		ZEINT64						GetTotalTime();
		ZEINT64						GetTotalAverageTime();
		ZEINT64						GetTotalMaximumTime();

		void						ResetFrame();
		void						ResetTotal();	

									ZEProfiler();
									~ZEProfiler();
};

#endif
