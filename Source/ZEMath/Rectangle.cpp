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

bool ZERectangle::IsEmpty()
{
	return (LeftUp == ZEVector2::Zero && RightDown == ZEVector2::Zero);
}

ZEVector2 ZERectangle::GetCorner(ZERectangleCorner Corner) const
{
	switch(Corner)
	{
		case ZE_RC_LEFTDOWN:
			return ZEVector2(LeftUp.x, RightDown.y);
		case ZE_RC_RIGHTDOWN:
			return ZEVector2(RightDown.x, RightDown.y);
		case ZE_RC_LEFTUP:
			return ZEVector2(LeftUp.x, LeftUp.y);
		case ZE_RC_RIGHTUP:
			return ZEVector2(RightDown.x, LeftUp.y);
		/*default:
			zeAssert(true, "Wrong enum value ZERectangleCorner. Value : %d", Corner);*/
	}
}

void ZERectangle::SetPosition(const ZEVector2& Position)
{
	ZEVector2 Temp;
	ZEVector2::Substution(Temp, Position, LeftUp);	
	ZEVector2::Substution(RightDown, RightDown, Temp);
}

const ZEVector2& ZERectangle::GetPosition() const
{
	return LeftUp;
}

void ZERectangle::SetWidth(float Width)
{
	RightDown.x = LeftUp.x + Width;
}

float ZERectangle::GetWidth() const
{
	return LeftUp.x - RightDown.x;
}

void ZERectangle::SetHeight(float Height)
{
	RightDown.y = LeftUp.y + Height;
}

float ZERectangle::GetHeight() const
{
	return LeftUp.y - RightDown.y;
}

bool ZERectangle::IntersectionTest(const ZERectangle& RectangleA, const ZERectangle& RectangleB)
{
	if ((RectangleA.LeftUp.x > RectangleB.RightDown.x) || 
		(RectangleA.RightDown.x < RectangleB.LeftUp.x) ||
		(RectangleA.LeftUp.y > RectangleB.RightDown.y) ||
		(RectangleA.RightDown.y <RectangleB.LeftUp.y))
		return false;
	
	return true;
}

bool ZERectangle::Intersection(ZERectangle& Intersection, const ZERectangle& RectangleA, const ZERectangle& RectangleB)
{
	if (!ZERectangle::IntersectionTest(RectangleA, RectangleB))
		return false;

	Intersection.LeftUp.x = (RectangleA.LeftUp.x > RectangleB.LeftUp.x ? RectangleA.LeftUp.x : RectangleB.LeftUp.x);
	Intersection.LeftUp.y = (RectangleA.LeftUp.y > RectangleB.LeftUp.y ? RectangleA.LeftUp.y : RectangleB.LeftUp.y);
	Intersection.RightDown.x = (RectangleA.RightDown.x < RectangleB.RightDown.x ? RectangleA.RightDown.x : RectangleB.RightDown.x);
	Intersection.RightDown.y = (RectangleA.RightDown.y < RectangleB.RightDown.y ? RectangleA.RightDown.y : RectangleB.RightDown.y);

	return true;
}

bool ZERectangle::BoundingTest(const ZERectangle& Rectangle, const ZEVector2& Point)
{
	if ((Point.x >= Rectangle.LeftUp.x && Point.x <= Rectangle.RightDown.x) && 
		(Point.y >= Rectangle.LeftUp.y && Point.y <= Rectangle.RightDown.y))
		return true;
	else
		return false;
}

ZERectangle::ZERectangle()
{
}

ZERectangle::ZERectangle(const ZEVector2& UpLeft, const ZEVector2& DownRight)
{
	this->LeftUp = UpLeft;
	this->RightDown = DownRight;
}

ZERectangle::ZERectangle(const ZEVector2& UpLeft, float Width, float Height)
{
	this->LeftUp = UpLeft;
	this->RightDown.x = UpLeft.x + Width;
	this->RightDown.y = UpLeft.y + Height;
}
