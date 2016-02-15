//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETimeStamp.cpp
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

#include "ZETimeStamp.h"
#include "ZEPlatform.h"

ZEUInt ZETimeStamp::GetHour() const
{
	return (Value / (1000LL * 1000LL * 60LL * 60LL)) % 24LL;
}

ZEUInt ZETimeStamp::GetMinute() const
{
	return (Value / (1000LL * 1000LL * 60LL)) % 60LL;
}

ZEUInt ZETimeStamp::GetSecond() const
{
	return (Value / (1000LL * 1000LL)) % 60LL;
}

ZEUInt ZETimeStamp::GetMillisecond() const
{
	return (Value / 1000LL) % 1000LL;
}

ZEUInt ZETimeStamp::GetMicrosecond() const
{
	return Value % 1000LL;
}

bool ZETimeStamp::GetAM() const
{
	return GetHour() < 12;
}

bool ZETimeStamp::GetPM() const
{
	return GetHour() >= 12;
}

ZEUInt ZETimeStamp::GetDay() const
{
	tm Temp = ToTM();
	return Temp.tm_mday;
}

ZEUInt ZETimeStamp::GetMonth() const
{
	tm Temp = ToTM();
	return Temp.tm_mon + 1;
}

ZEUInt ZETimeStamp::GetYear() const
{
	tm Temp = ToTM();
	return Temp.tm_year + 1900;
}

ZEUInt ZETimeStamp::GetDayOfTheWeek() const
{
	return (Value / (1000LL * 1000LL * 60LL * 60LL * 24LL) + 4) % 7;
}

ZEUInt ZETimeStamp::GetDayOfTheYear() const
{
	tm Temp = ToTM();
	return Temp.tm_yday;
}

void ZETimeStamp::SetYear(ZEUInt Year)
{
	SetDateTime(Year, GetMonth(), GetDay(), GetHour(), GetMinute(), GetSecond(), GetMillisecond(), GetMicrosecond());
}

void ZETimeStamp::SetMonth(ZEUInt Month)
{
	SetDateTime(GetYear(), Month, GetDay(), GetHour(), GetMinute(), GetSecond(), GetMillisecond(), GetMicrosecond());
}

void ZETimeStamp::SetDay(ZEUInt Day)
{
	SetDateTime(GetYear(), GetMonth(), GetDay(), GetHour(), GetMinute(), GetSecond(), GetMillisecond(), GetMicrosecond());
}

void ZETimeStamp::SetHour(ZEUInt Hour)
{
	AddHoursSelf((ZEInt)Hour - (ZEInt)GetHour());
}

void ZETimeStamp::SetMinute(ZEUInt Minute)
{
	AddMinutesSelf((ZEInt)Minute - (ZEInt)GetHour());
}

void ZETimeStamp::SetSecond(ZEUInt Second)
{
	AddSecondsSelf((ZEInt)Second - (ZEInt)GetSecond());
}

void ZETimeStamp::SetMillisecond(ZEUInt Millisecond)
{
	AddMillisecondsSelf((ZEInt)Millisecond - (ZEInt)GetMillisecond());
}

void ZETimeStamp::SetMicroseconds(ZEUInt Microseconds)
{
	AddMicrosecondsSelf((ZEInt)Microseconds - (ZEInt)GetMicrosecond());
}

ZETimeStamp ZETimeStamp::AddDays(ZEInt Days) const
{
	return ZETimeStamp(Value + (ZEInt64)Days * 24LL * 60LL * 60LL * 1000LL * 1000LL);
}

ZETimeStamp ZETimeStamp::AddHours(ZEInt Hours)  const
{
	return ZETimeStamp(Value + (ZEInt64)Hours * 60LL * 60LL * 1000LL * 1000LL);
}

ZETimeStamp ZETimeStamp::AddMinutes(ZEInt Minutes) const
{
	return ZETimeStamp(Value + (ZEInt64)Minutes * 60LL * 1000LL * 1000LL);
}

ZETimeStamp ZETimeStamp::AddSeconds(ZEInt Seconds) const
{
	return ZETimeStamp(Value + (ZEInt64)Seconds * 1000LL * 1000LL);
}

ZETimeStamp ZETimeStamp::AddMilliseconds(ZEInt Milliseconds) const
{
	return ZETimeStamp(Value + (ZEInt64)Milliseconds * 1000LL);
}

ZETimeStamp ZETimeStamp::AddMicroseconds(ZEInt Microseconds) const
{
	return ZETimeStamp(Value +(ZEInt64) Microseconds);
}

void ZETimeStamp::AddDaysSelf(ZEInt Days)
{
	Value += (ZEInt64)Days * 24LL * 60LL * 60LL * 1000LL * 1000LL;
}

void ZETimeStamp::AddHoursSelf(ZEInt Hours) 
{
	Value += (ZEInt64)Hours * 60LL * 60LL * 1000LL * 1000LL;
}

void ZETimeStamp::AddMinutesSelf(ZEInt Minutes)
{
	Value += (ZEInt64)Minutes * 60LL * 1000LL * 1000LL;
}

void ZETimeStamp::AddSecondsSelf(ZEInt Seconds)
{
	Value += (ZEInt64)Seconds * 1000LL * 1000LL;
}

void ZETimeStamp::AddMillisecondsSelf(ZEInt Milliseconds)
{
	Value += (ZEInt64)Milliseconds * 1000LL;
}

void ZETimeStamp::AddMicrosecondsSelf(ZEInt Microseconds)
{
	Value += (ZEInt64)Microseconds;
}

void ZETimeStamp::SetDate(ZEUInt Year, ZEUInt Month, ZEUInt Day)
{
	SetDateTime(Year, Month, Day, 0, 0, 0, 0, 0);
}

void ZETimeStamp::SetTime(ZEUInt Hour, ZEUInt Minute, ZEUInt Second, ZEUInt Millisecond, ZEUInt Microsecond)
{
	Value = Hour * 60LL * 60LL * 1000LL * 1000LL +
		Minute * 60LL * 1000LL * 1000LL +
		Second * 1000LL * 1000LL +
		Millisecond * 1000LL +
		Microsecond;
}

void ZETimeStamp::SetDateTime(ZEUInt Year, ZEUInt Month, ZEUInt Day, ZEUInt Hour, ZEUInt Minute, ZEUInt Second, ZEUInt Millisecond, ZEUInt Microsecond)
{
	tm TempTM;
    TempTM.tm_year = Year - 1900;
	TempTM.tm_mon = Month - 1;
    TempTM.tm_mday = Day;
	TempTM.tm_hour = Hour;
	TempTM.tm_min = Minute;
	TempTM.tm_sec = Second;
    TempTM.tm_wday = 0;
    TempTM.tm_yday = 0;
    TempTM.tm_isdst = 0;

	FromTM(TempTM);
	if (Value != 0)
		Value += Millisecond * 1000LL + Microsecond;
}

ZETimeStamp ZETimeStamp::operator+(ZETimeStamp Other) const
{
	return Value + Other.Value;
}

ZETimeStamp ZETimeStamp::operator-(ZETimeStamp Other) const
{
	return Value - Other.Value;
}

bool ZETimeStamp::operator==(ZETimeStamp Other) const
{
	return Value == Other.Value;
}

bool ZETimeStamp::operator!=(ZETimeStamp Other) const
{
	return Value != Other.Value;
}

bool ZETimeStamp::operator<(ZETimeStamp Other) const
{
	return Value < Other.Value;
}

bool ZETimeStamp::operator>(ZETimeStamp Other) const
{
	return Value > Other.Value;
}

bool ZETimeStamp::operator<=(ZETimeStamp Other) const
{
	return Value <= Other.Value;
}

bool ZETimeStamp::operator>=(ZETimeStamp Other) const
{
	return Value >= Other.Value;
}

ZETimeStamp ZETimeStamp::operator+=(ZETimeStamp Other)
{
	this->Value += Other.Value;
	return *this;
}
		
ZETimeStamp ZETimeStamp::operator-=(ZETimeStamp Other)
{
	this->Value -= Other.Value;
	return *this;
}

ZETimeStamp ZETimeStamp::operator=(ZETimeStamp Other)
{
	this->Value = Other.Value;
	return *this;
}

ZETimeStamp ZETimeStamp::operator=(const tm& Other)
{
	FromTM(Other);
	return *this;
}

ZETimeStamp ZETimeStamp::operator=(const time_t& Other)
{
	FromCTime(Other);
	return *this;
}

ZETimeStamp::operator tm() const
{
	return ToTM();
}

ZETimeStamp::operator time_t() const
{
	return ToCTime();
}

tm ZETimeStamp::ToTM() const
{
	time_t time = Value / (1000 * 1000);
	tm* Temp = localtime(&time);
	if (Temp == NULL)
	{
		tm Empty;
		Empty.tm_year = 0;
		Empty.tm_mon = 0;
		Empty.tm_mday = 0;
		Empty.tm_hour = 0;
		Empty.tm_min = 0;
		Empty.tm_sec = 0;
		Empty.tm_wday = 0;
		Empty.tm_yday = 0;
		Empty.tm_isdst = 0;
		return Empty;
	}
	return *Temp;
}

time_t ZETimeStamp::ToCTime() const
{
	return Value / 1000;
}

void ZETimeStamp::FromTM(const tm& TM)
{
	Value = (ZEInt64)mktime(const_cast<tm*>(&TM)) * 1000LL * 1000LL;
	if (Value == -1)
		Value = 0;	
}

void ZETimeStamp::FromCTime(time_t Time)
{
	Value = (ZEInt64)Time * 1000LL * 1000LL;
}

void ZETimeStamp::Clear()
{
	Value = 0;
}

ZETimeStamp::ZETimeStamp()
{
	Value = 0;
}

ZETimeStamp::ZETimeStamp(ZEInt64 Value)
{
	this->Value = Value;
}

ZETimeStamp::ZETimeStamp(ZEUInt Year, ZEUInt Month, ZEUInt Day, ZEUInt Hour, ZEUInt Minute, ZEUInt Second, ZEUInt Millisecond, ZEUInt Microsecond)
{
	SetDateTime(Year, Month, Day, Hour, Minute, Second, Millisecond);
}

ZETimeStamp ZETimeStamp::CreateTime(ZEUInt Hour, ZEUInt Minute, ZEUInt Second, ZEUInt Millisecond, ZEUInt Microsecond)
{
	ZETimeStamp Temp;
	Temp.SetTime(Hour, Minute, Second, Millisecond);
	return Temp;
}

ZETimeStamp ZETimeStamp::CreateDate(ZEUInt Year, ZEUInt Month, ZEUInt Day)
{
	ZETimeStamp Temp;
	Temp.SetDate(Year, Month, Day);
	return Temp;
}

ZETimeStamp ZETimeStamp::CreateDateTime(ZEUInt Year, ZEUInt Month, ZEUInt Day, ZEUInt Hour, ZEUInt Minute, ZEUInt Second, ZEUInt Millisecond, ZEUInt Microsecond)
{
	ZETimeStamp Temp;
	Temp.SetDateTime(Year, Month, Day, Hour, Minute, Second, Millisecond);
	return Temp;
}


#ifdef ZE_PLATFORM_WINDOWS
	#include <windows.h>

	#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
	  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
	#else
	  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
	#endif
	struct timezone 
	{
		int  tz_minuteswest;
		int  tz_dsttime;
	};
	int gettimeofday(struct timeval *tv, struct timezone *tz)
	{
		FILETIME ft;
		unsigned __int64 tmpres = 0;
		static int tzflag = 0;
		if (NULL != tv)
		{
			GetSystemTimeAsFileTime(&ft);
			tmpres |= ft.dwHighDateTime;
			tmpres <<= 32;
			tmpres |= ft.dwLowDateTime;
			tmpres /= 10; 
			tmpres -= DELTA_EPOCH_IN_MICROSECS; 
			tv->tv_sec = (long)(tmpres / 1000000UL);
			tv->tv_usec = (long)(tmpres % 1000000UL);
		}
		if (NULL != tz)
		{
			if (!tzflag)
			{
				_tzset();
				tzflag++;
			}
			tz->tz_minuteswest = _timezone / 60;
			tz->tz_dsttime = _daylight;
		}
		return 0;
}
#endif

ZETimeStamp ZETimeStamp::Now()
{
	timeval TempTV;
	gettimeofday(&TempTV, NULL);
	
	ZETimeStamp Temp;
	Temp.FromCTime(TempTV.tv_sec);
	Temp.AddMicroseconds(TempTV.tv_usec);

	return Temp;
}
