//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETimeCounterTests.cpp
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

#include "ZEFoundation/ZETimeCounter.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"

// Unit Tests
	// Unit Tests
	// Function Test
		// Cases

ZE_TEST(ZETimeCounter)
{
	ZE_TEST_ITEM("ZETimeCounter::ZETimeCounter()")
	{
		ZETimeCounter TimeCounter;
		//TimeCounter.Started false
		//TimeCounter.StartTime = 0
		//TimeCounter.EndTime = 0
		//TimeCounter.Frequency = 1000000
	}

	ZE_TEST_ITEM("ZEUInt64 ZETimeCounter::GetTime()")
	{
		ZETimeCounter TimeCounter;

		ZEUInt64 Time = TimeCounter.GetTimeMicroseconds();
		ZE_TEST_CHECK_EQUAL(Time, 0);

		ZE_TEST_CASE("start time counter")
		{
			TimeCounter.Start();
			//TimeCounter.Started true
			//TimeCounter.StartTime != 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency 2766640

			Time = TimeCounter.GetTimeMicroseconds();
			//ZE_TEST_CHECK_EQUAL(Time, 44094391);
		}

		ZE_TEST_CASE("reset time counter")
		{
			TimeCounter.Reset();
			//TimeCounter.Started true

			Time = TimeCounter.GetTimeMicroseconds();
			//Time = TimeCounter.EndTime
		}

		ZE_TEST_CASE("stop time counter")
		{
			TimeCounter.Stop();
			//TimeCounter.Started false

			Time = TimeCounter.GetTimeMicroseconds();
			//Time = TimeCounter.EndTime
		}

		ZE_TEST_CASE("set time")
		{
			TimeCounter.SetTime(44094391);

			Time = TimeCounter.GetTimeMicroseconds();
		}

		ZE_TEST_CASE("reset and set time")
		{
			TimeCounter.Reset();
			TimeCounter.SetTime(44094391);

			Time = TimeCounter.GetTimeMicroseconds();
			ZE_TEST_CHECK_EQUAL(Time, ZEUInt64(-44094391));
		}
	}

	ZE_TEST_ITEM("void ZETimeCounter::SetTime(ZEUInt64 Microseconds)")
	{
		ZETimeCounter TimeCounter;
		//TimeCounter.Started false

		TimeCounter.SetTime(0);
		ZE_TEST_CHECK_EQUAL(TimeCounter.GetTimeMicroseconds(), 0);

		ZE_TEST_CASE("microseconds different from zero")
		{
			ZEUInt64 Microseconds = 44094391;

			TimeCounter.SetTime(Microseconds);
			//TimeCounter.StartTime = Microseconds
			ZE_TEST_CHECK_EQUAL(TimeCounter.GetTimeMicroseconds(), ZEUInt64(-Microseconds));
		}

		ZE_TEST_CASE("start time counter")
		{
			TimeCounter.Start();
			ZEUInt64 Microseconds = 44094391;

			TimeCounter.SetTime(Microseconds);
			//TimeCounter.GetTime() = TimeCounter.EndTime
		}
	}

	ZE_TEST_ITEM("void ZETimeCounter::Reset()")
	{
		ZETimeCounter TimeCounter;

		TimeCounter.Reset();
		//TimeCounter.Started false

		ZE_TEST_CASE("start time counter")
		{
			TimeCounter.Start();
			//TimeCounter.Started true
			//TimeCounter.StartTime != 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency = 2766640

			TimeCounter.Reset();
			//TimeCounter.Started true
			//TimeCounter.StartTime = 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency = 1000000
		}

		ZE_TEST_CASE("stop time counter")
		{
			TimeCounter.Stop();
			//TimeCounter.Started false
			//TimeCounter.StartTime = 0
			//TimeCounter.EndTime != 0
			//TimeCounter.Frequency = 1000000

			TimeCounter.Reset();
			//TimeCounter.Started false
			//TimeCounter.StartTime = 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency = 1000000
		}
	}

	ZE_TEST_ITEM("void ZETimeCounter::Start()")
	{
		ZETimeCounter TimeCounter;
		//TimeCounter.Started false

		TimeCounter.Start();
		//TimeCounter.Started true
		//TimeCounter.StartTime != 0
		//TimeCounter.EndTime = 0
		//TimeCounter.Frequency = 2766640

		ZE_TEST_CASE("stop time counter")
		{
			TimeCounter.Stop();
			//TimeCounter.Started false
			//TimeCounter.StartTime != 0
			//TimeCounter.EndTime != 0
			//TimeCounter.Frequency = 2766640

			TimeCounter.Start();
			//TimeCounter.Started true
			//TimeCounter.StartTime != 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency = 2766640
		}

		ZE_TEST_CASE("reset time counter")
		{
			TimeCounter.Reset();
			//TimeCounter.Started true
			//TimeCounter.StartTime = 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency = 1000000

			TimeCounter.Start();
			//TimeCounter.Started true
			//TimeCounter.StartTime != 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency = 2766640
		}
	}

	ZE_TEST_ITEM("void ZETimeCounter::Stop()")
	{
		ZETimeCounter TimeCounter;

		TimeCounter.Stop();
		//TimeCounter.Started false
		//TimeCounter.StartTime = 0
		//TimeCounter.EndTime != 0
		//TimeCounter.Frequency = 1000000

		ZE_TEST_CASE("start time counter")
		{
			TimeCounter.Start();
			//TimeCounter.Started true
			//TimeCounter.StartTime ! 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency = 2766640

			TimeCounter.Stop();
			//TimeCounter.Started false
			//TimeCounter.StartTime != 0
			//TimeCounter.EndTime != 0
			//TimeCounter.Frequency = 2766640
		}

		ZE_TEST_CASE("reset time counter")
		{
			TimeCounter.Reset();
			//TimeCounter.Started false
			//TimeCounter.StartTime = 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency = 1000000

			TimeCounter.Stop();
			//TimeCounter.Started false
			//TimeCounter.StartTime = 0
			//TimeCounter.EndTime != 0
			//TimeCounter.Frequency = 1000000
		}
	}
}
