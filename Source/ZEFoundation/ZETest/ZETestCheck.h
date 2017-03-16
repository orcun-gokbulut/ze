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

#include "ZEMath/ZEMath.h"
#include <stdio.h>

#define ZE_TEST_CLOSE_THRESHOLD 0.00001f

static bool ZETestInternalCheckClose(const float& Actual, const float& Expected, const float& Threshold = ZE_TEST_CLOSE_THRESHOLD)
{
	return (ZEMath::Abs(Actual - Expected) <= Threshold);
}

#define ZE_TEST_CHECK(Condition, ProblemType, ...) \
	do { \
	try { if (!(Condition)) {PauseTimeCounter(); ReportProblem(ProblemType, ZEFormat::Format("Check for condition \"" #Condition "\" has failed. " __VA_ARGS__), __FILE__, __LINE__); ResumeTimeCounter(); if (ProblemType == ZE_TPT_CRITICAL_ERROR) return;}} \
		catch(...) {PauseTimeCounter(); ReportProblem(ProblemType, "Exception occurred while checking \"" #Condition "\" condition.", __FILE__, __LINE__); ResumeTimeCounter(); if (ProblemType == ZE_TPT_CRITICAL_ERROR) return;} \
	} while(false)
#define ZE_TEST_CHECK_CRITICAL_ERROR(Condition, ...) ZE_TEST_CHECK(Condition, ZE_TPT_CRITICAL_ERROR, __VA_ARGS__)
#define ZE_TEST_CHECK_ERROR(Condition, ...) ZE_TEST_CHECK(Condition, ZE_TPT_ERROR, __VA_ARGS__)
#define ZE_TEST_CHECK_WARNING(Condition, ...) ZE_TEST_CHECK(Condition, ZE_TPT_WARNING,  __VA_ARGS__)

#define ZE_TEST_CHECK_EQUAL(Result, Expected) \
	ZE_TEST_CHECK((Result) == (Expected), ZE_TPT_ERROR, \
		"Result and expected value is not equal. \n" \
		" Result: {0}\n" \
		" Expected: {1}", Result, Expected)

#define ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, Expected) \
	ZE_TEST_CHECK(ZETestInternalCheckClose(Result, Expected), ZE_TPT_ERROR, \
		"Result and expected are not equal or close. \n" \
		" Result: {0}\n" \
		" Expected: {1}\n" \
		" Treshold: {2}", \
		Result, Expected, ZE_TEST_CLOSE_THRESHOLD)

#define ZE_TEST_CHECK_STRING_EQUAL(Result, Expected) \
	ZE_TEST_CHECK(strcmp(Result, Expected) == 0, ZE_TPT_ERROR, \
		"Result and expected are not equal. \n" \
		" Result: {0}\n" \
		" Expected: {1}", Result, Expected)
