//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - xsens_time.h
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

#ifndef XSENS_TIME_H
#define XSENS_TIME_H

#ifndef _PSTDINT_H_INCLUDED
#	include "pstdint.h"
#endif

#include <time.h>

namespace xsens {

//! The number of seconds in a normal day
#define XSENS_SEC_PER_DAY	(60*60*24)
//! The number of milliseconds in a normal day
#define XSENS_MS_PER_DAY	(XSENS_SEC_PER_DAY*1000)
//! The number of milliseconds in a normal day
#define XSENS_TIMESTAMP_MAX	(0x7FFFFFFFFFFFFFFFLL)

//! A real-time timestamp (ms)
typedef int64_t TimeStamp;

/*! \brief A platform-independent clock.

	The function returns the time of day in ms since midnight. If the \c date parameter is
	non-NULL, corresponding the date is placed in the variable it points to.
*/
uint32_t getTimeOfDay(tm* date_ = NULL, time_t* secs_ = NULL);

/*! \brief Retrieves the date and time (platform-independent)	
	\param date : if non-zero the local (!) date and time is stored in the tm struct this parameter points to
	\return The UTC date and time as seconds since 1970
*/
int64_t getDateTime(tm * date = 0);

/*! \brief Retrieves the date as string representation
	The format is YYYYMMDD
	so 25 dec 2010 is stored as an array dest[8] = {'2', '0', '1', '0', '1', '2', '2', '5' }
	\param dest : A pointer to an array of at least (!) 8 bytes
	\param date : If date is non-zero this date is converted, otherwise the current date is retrieved and used)
*/
void getDateAsString(uint8_t* dest, tm const* date = 0);

/*! \brief Retrieves the time as binary 
	The format is HHMMSShh (where H is hour and 'h' is hundredths)
	so 14:25:01.23 is stored as an array dest[8] = { '1', '4', '2', '5', '0',  '1', '2', '3'}
	\param dest : A pointer to an array of at least (!) 8 bytes
	\param date : If date is non-zero this date is converted, otherwise the current date is retrieved and used)
	\note (for now hundreths are set to 0)
*/
void getTimeAsString(uint8_t* dest, tm const* date = 0);

/*! \brief A platform-independent sleep routine.

	Time is measured in ms. The function will not return until the specified
	number of ms have passed.
*/
void msleep(uint32_t ms);

TimeStamp timeStampNow();
void initializeTime();


class TimeSyncPrivate;
class TimeSync {
private:
	TimeSyncPrivate* d;

public:
	TimeSync(double skew, int historySize);
	~TimeSync();

	int isInitialized() const;
	void reset();
	void update(TimeStamp local, TimeStamp external);
	void setInitialSkew(double skew);

	TimeStamp localTime(TimeStamp external);
	TimeStamp externalTime(TimeStamp local);
};


class MillisecondTimer
{
public:
	MillisecondTimer();
	void restart();
	uint32_t millisecondsElapsed();
private:
	uint32_t m_tstart;
};


}	// end of xsens namespace




#endif	// XSENS_TIME_H
