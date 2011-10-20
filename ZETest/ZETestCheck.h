//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETestCheck.h
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
#ifndef __ZE_TEST_CHECK_H__
#define __ZE_TEST_CHECK_H__

#define ZETestCheck(Condition)\
	do\
	{\
		try\
		{\
			if (!(Condition))\
			{\
				this->ReportProblem(("Check condition \"" #Condition "\" has failed."), __FILE__, __LINE__);\
			} \
		} \
		catch (...)\
		{\
			this->ReportProblem(("Exception occured at check condition. Condition : \"" #Condition "\"."), __FILE__, __LINE__);\
		} } \
	while(0)

#define ZETestCheckEqual(Actual, Expected)\
	do\
	{\
		try\
		{\
			if ((Actual) != (Expected))\
			{\
				this->ReportProblem(("Actual \"" #Actual " is not equal to expected \"" #Expected "\"."), __FILE__, __LINE__);\
			} \
		} \
		catch (...)\
		{\
			this->ReportProblem(("Exception occured at equality check. Actual value \"" #Actual "\", Expected value : \"" #Expected "\"."), __FILE__, __LINE__);\
		} } \
	while(0)


#define ZETestCheckClose(Actual, Expected, Tolerance)\
	do\
	{\
		try\
		{\
			if (((Actual) >= ((Expected) - (Tolerance))) && ((Actual) <= ((Expected) + (Tolerance))))\
			{\
				this->ReportProblem(("Actual value \"" #Actual " is not close (with tolerance \"" #Tolerance "\") to expected value \"" #Expected "\"."), __FILE__, __LINE__);\
			} \
		} \
		catch (...)\
		{\
			this->ReportProblem(("Exception occured at close check. Actual value \"" #Actual "\", Expected value : \"" #Expected "\", Tolerance : \"" #Tolerance "\"."), __FILE__, __LINE__);\
		} } \
	while(0)
#endif
