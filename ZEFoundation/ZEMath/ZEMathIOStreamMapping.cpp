//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMathIOStreamMapping.cpp
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

#include "ZEMathIOStreamMapping.h"
#include <iostream>
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"

using namespace std;

ostream& operator<<(ostream& Out, const ZEString& Input)
{
	return Out;// << (char)Input;
}

ostream& operator<<(ostream& Out, const ZEVector2& Input)
{
	return Out << "[x:" << Input.x << ", y:" << Input.y << "]";
}

ostream& operator<<(ostream& Out, const ZEVector3& Input)
{
	return Out << "[x:" << Input.x << ", y:" << Input.y << ", z:" << Input.z << "]";
}

ostream& operator<<(ostream& Out, const ZEVector4& Input)
{
	return Out << "[x:" << Input.x << ", y:" << Input.y << ", z:" << Input.z << "w:" << Input.w;
}

ostream& operator<<(ostream& Out, const ZEQuaternion& Input)
{
	return Out << "[w:" << Input.w << ", x:" << Input.x << ", y:" << Input.y << ", z:" << Input.z << "]";
}

ostream& operator<<(ostream& Out, const ZEMatrix3x3& Input)
{
	 Out << "[";
	for (size_t I = 0; I < 9; I++)
		if (I != 8)
			Out << Input.M[I] << ", ";
		else
			Out << Input.M[I] << "]";

	return Out;
}

ostream& operator<<(ostream& Out, const ZEMatrix4x4& Input)
{
	Out << "[";
	for (size_t I = 0; I < 16; I++)
		if (I != 15)
			Out << Input.M[I] << ", ";
		else
			Out << Input.M[I] << "]";

	return Out;
}
