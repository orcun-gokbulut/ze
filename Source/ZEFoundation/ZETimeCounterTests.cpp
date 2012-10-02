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

ZETestSuite(ZETimeCounter)
{
	ZETest("ZETimeCounter::ZETimeCounter()")
	{
		ZETimeCounter TimeCounter;
		//TimeCounter.Started false
		//TimeCounter.StartTime = 0
		//TimeCounter.EndTime = 0
		//TimeCounter.Frequency = 1000000
	}

	ZETest("ZEUInt64 ZETimeCounter::GetTime()")
	{
		ZETimeCounter TimeCounter;

		ZEUInt64 Time = TimeCounter.GetTime();
		ZETestCheckEqual(Time, 0);

		ZETestCase("start time counter")
		{
			TimeCounter.Start();
			//TimeCounter.Started true
			//TimeCounter.StartTime != 0
			//TimeCounter.EndTime = 0
			//TimeCounter.Frequency 2766640

			Time = TimeCounter.GetTime();
			//ZETestCheckEqual(Time, 44094391);
		}

		ZETestCase("reset time counter")
		{
			TimeCounter.Reset();
			//TimeCounter.Started true

			Time = TimeCounter.GetTime();
			//Time = TimeCounter.EndTime
		}

		ZETestCase("stop time counter")
		{
			TimeCounter.Stop();
			//TimeCounter.Started false

			Time = TimeCounter.GetTime();
			//Time = TimeCounter.EndTime
		}

		ZETestCase("set time")
		{
			TimeCounter.SetTime(44094391);

			Time = TimeCounter.GetTime();
		}

		ZETestCase("reset and set time")
		{
			TimeCounter.Reset();
			TimeCounter.SetTime(44094391);

			Time = TimeCounter.GetTime();
			ZETestCheckEqual(Time, ZEUInt64(-44094391));
		}
	}

	ZETest("void ZETimeCounter::SetTime(ZEUInt64 Microseconds)")
	{
		ZETimeCounter TimeCounter;
		//TimeCounter.Started false

		TimeCounter.SetTime(0);
		ZETestCheckEqual(TimeCounter.GetTime(), 0);

		ZETestCase("microseconds different from zero")
		{
			ZEUInt64 Microseconds = 44094391;

			TimeCounter.SetTime(Microseconds);
			//TimeCounter.StartTime = Microseconds
			ZETestCheckEqual(TimeCounter.GetTime(), ZEUInt64(-Microseconds));
		}

		ZETestCase("start time counter")
		{
			TimeCounter.Start();
			ZEUInt64 Microseconds = 44094391;

			TimeCounter.SetTime(Microseconds);
			//TimeCounter.GetTime() = TimeCounter.EndTime
		}
	}

	ZETest("void ZETimeCounter::Reset()")
	{
		ZETimeCounter TimeCounter;

		TimeCounter.Reset();
		//TimeCounter.Started false

		ZETestCase("start time counter")
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

		ZETestCase("stop time counter")
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

	ZETest("void ZETimeCounter::Start()")
	{
		ZETimeCounter TimeCounter;
		//TimeCounter.Started false

		TimeCounter.Start();
		//TimeCounter.Started true
		//TimeCounter.StartTime != 0
		//TimeCounter.EndTime = 0
		//TimeCounter.Frequency = 2766640

		ZETestCase("stop time counter")
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

		ZETestCase("reset time counter")
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

	ZETest("void ZETimeCounter::Stop()")
	{
		ZETimeCounter TimeCounter;

		TimeCounter.Stop();
		//TimeCounter.Started false
		//TimeCounter.StartTime = 0
		//TimeCounter.EndTime != 0
		//TimeCounter.Frequency = 1000000

		ZETestCase("start time counter")
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

		ZETestCase("reset time counter")
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
