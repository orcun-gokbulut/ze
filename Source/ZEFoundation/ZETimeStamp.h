//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETimeStamp.h
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
#ifndef __ZE_TIME_STAMP_H__
#define __ZE_TIME_STAMP_H__

#include "ZETypes.h"
#include <time.h>

#define ZE_ILVALID_TIME ((ZEInt64)-1)
class ZETimeStamp
{
	public:
		ZEInt64					Value;

		ZEUInt					GetHour() const;
		ZEUInt					GetMinute() const;
		ZEUInt					GetSecond() const;
		ZEUInt					GetMillisecond() const;
		ZEUInt					GetMicrosecond() const;
		bool					GetAM() const;
		bool					GetPM() const;

		ZEUInt					GetDay() const;
		ZEUInt					GetMonth() const;
		ZEUInt					GetYear() const;
		ZEUInt					GetDayOfTheWeek() const;
		ZEUInt					GetDayOfTheYear() const;

		void					SetHour(ZEUInt Hour);
		void					SetMinute(ZEUInt Minute);
		void					SetSecond(ZEUInt Second);
		void					SetMillisecond(ZEUInt Millisecond);
		void					SetMicroseconds(ZEUInt Microsecond);
		
		void					SetDate(ZEUInt Year, ZEUInt Month, ZEUInt Day);
		void					SetTime(ZEUInt Hour, ZEUInt Minute, ZEUInt Second, ZEUInt Millisecond = 0, ZEUInt Microsecond = 0);
		void					SetDateTime(ZEUInt Year, ZEUInt Month, ZEUInt Day, ZEUInt Hour, ZEUInt Minute, ZEUInt Second, ZEUInt Millisecond = 0, ZEUInt Microsecond = 0);

		void					AddDays(ZEInt Days);
		void					AddHours(ZEInt Hours);
		void					AddMinutes(ZEInt Minutes);
		void					AddSeconds(ZEInt Seconds);
		void					AddMilliseconds(ZEInt Milliseconds);
		void					AddMicroseconds(ZEInt Microseconds);

		ZETimeStamp				operator+(ZETimeStamp Other) const;
		ZETimeStamp				operator-(ZETimeStamp Other) const;

		bool					operator==(ZETimeStamp Other) const;
		bool					operator!=(ZETimeStamp Other) const;
		bool					operator<(ZETimeStamp Other) const;
		bool					operator>(ZETimeStamp Other) const;
		bool					operator<=(ZETimeStamp Other) const;
		bool					operator>=(ZETimeStamp Other) const;

		ZETimeStamp				operator+=(ZETimeStamp Other);
		ZETimeStamp				operator-=(ZETimeStamp Other);

		ZETimeStamp				operator=(ZETimeStamp Other);
		ZETimeStamp				operator=(const tm& Other);
		ZETimeStamp				operator=(const time_t& Other);
		
								operator tm();
								operator time_t();

		tm						ToTM() const;
		time_t					ToCTime() const;

		void					FromTM(const tm& TM);
		void					FromCTime(time_t Time);
		
		void					Clear();

								ZETimeStamp();
								ZETimeStamp(ZEInt64 Value);
								ZETimeStamp(ZEUInt Year, ZEUInt Day, ZEUInt Month, 
									ZEUInt Hour = 0, ZEUInt Minute = 0, ZEUInt Second = 0, ZEUInt Millisecond = 0, ZEUInt Microsecond = 0);

		static ZETimeStamp		CreateDateTime(ZEUInt Year, ZEUInt Month, ZEUInt Day, ZEUInt Hour, ZEUInt Minute, ZEUInt Second, ZEUInt Millisecond = 0, ZEUInt Microsecond = 0);
		static ZETimeStamp		CreateTime(ZEUInt Hour, ZEUInt Minute, ZEUInt Second, ZEUInt Millisecond = 0, ZEUInt Microsecond = 0);
		static ZETimeStamp		CreateDate(ZEUInt Year, ZEUInt Month, ZEUInt Day);

		static ZETimeStamp		Now();
};

#endif
