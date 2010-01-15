//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Rectangle.cpp
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

#include "Rectangle.h"
#include "Core/Error.h"

bool ZERectangle::BoundingTest(const ZEPoint2& Point) const
{
	if ((Point.x >= LeftUp.x && Point.x <= RightDown.x) && (Point.y >= LeftUp.y &&  Point.y <= RightDown.y))
		return true;
	else
		return false;
}
ZEPoint2 ZERectangle::GetCorner(ZERectangleCorner Corner) const
{
	switch(Corner)
	{
		case ZERECTANGLECORNER_LEFTDOWN:
			return ZEPoint2(LeftUp.x, RightDown.y);
		case ZERECTANGLECORNER_RIGHTDOWN:
			return ZEPoint2(RightDown.x, RightDown.y);
		case ZERECTANGLECORNER_LEFTUP:
			return ZEPoint2(LeftUp.x, LeftUp.y);
		case ZERECTANGLECORNER_RIGHTUP:
			return ZEPoint2(RightDown.x, LeftUp.y);
		/*default:
			ZEASSERT(true, "Wrong enum value ZERectangleCorner. Value : %d", Corner);*/
	}
}

float ZERectangle::GetWidth() const
{
	return LeftUp.x - RightDown.x;
}

float ZERectangle::GetHeight() const
{
	return LeftUp.y - RightDown.y;
}

ZERectangle::ZERectangle()
{
}

ZERectangle::ZERectangle(const ZEPoint2& UpLeft, const ZEPoint2& DownRight)
{
	this->LeftUp = UpLeft;
	this->RightDown = DownRight;
}

ZERectangle::ZERectangle(const ZEPoint2& UpLeft, float Width, float Height)
{
	this->LeftUp = UpLeft;
	this->RightDown.x = UpLeft.x + Width;
	this->RightDown.y = UpLeft.y + Height;
}
