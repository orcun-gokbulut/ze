//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - UIRectangle.cpp
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

#include "UIRectangle.h"
#include "Graphics/VertexTypes.h"

void ZEUIRectangle::ConvertToVertices(ZEUIVertex* Buffer) const
{
	Buffer[0].Position = Positions.LeftUp;
	Buffer[0].Texcoord = Texcoords.LeftUp;
	Buffer[0].Color = Color;
	Buffer[1].Position = ZEVector2(Positions.RightDown.x, Positions.LeftUp.y);
	Buffer[1].Texcoord = ZEVector2(Texcoords.RightDown.x, Texcoords.LeftUp.y);
	Buffer[1].Color = Color;
	Buffer[2].Position = Positions.RightDown;
	Buffer[2].Texcoord = Texcoords.RightDown;
	Buffer[2].Color = Color;
	Buffer[3] = Buffer[2];
	Buffer[4].Position = ZEVector2(Positions.LeftUp.x, Positions.RightDown.y);
	Buffer[4].Texcoord = ZEVector2(Texcoords.LeftUp.x, Texcoords.RightDown.y);
	Buffer[4].Color = Color;
	Buffer[5] = Buffer[0];
}

bool ZEUIRectangle::Clip(ZEUIRectangle& Output, const ZEUIRectangle& Rectangle, const ZERectangle& ClipRectangle)
{
	bool Test1 = Rectangle.Positions.LeftUp.x > ClipRectangle.RightDown.x;
	bool Test2 = Rectangle.Positions.RightDown.x < ClipRectangle.LeftUp.x;
    bool Test3 = Rectangle.Positions.LeftUp.y > ClipRectangle.RightDown.y;
    bool Test4 = Rectangle.Positions.RightDown.y <ClipRectangle.LeftUp.y;

	if (!Test1 && !Test2 && !Test3 && !Test4)
		return false;
	else if (Test1 && Test2 && Test3 && Test4)
	{
		Output = Rectangle;
		return true;
	}
	else
	{
		Output.Material = Rectangle.Material;
		Output.Color = Rectangle.Color;

		if (ClipRectangle.LeftUp.x > Rectangle.Positions.LeftUp.x)
		{
			Output.Positions.LeftUp.x = ClipRectangle.LeftUp.x;
			Output.Texcoords.LeftUp.x = Rectangle.Texcoords.LeftUp.x +
									   ((ClipRectangle.LeftUp.x - Rectangle.Positions.LeftUp.x) / 
									   (Rectangle.Positions.RightDown.x - Rectangle.Positions.LeftUp.x)) * 
									   (Rectangle.Texcoords.RightDown.x - Rectangle.Texcoords.LeftUp.x);
		}
		else
		{
			Output.Positions.LeftUp.x = Rectangle.Positions.LeftUp.x;
			Output.Texcoords.LeftUp.x = Rectangle.Positions.LeftUp.x;
		}


		if (ClipRectangle.LeftUp.y > Rectangle.Positions.LeftUp.y)
		{
			Output.Positions.LeftUp.y = ClipRectangle.LeftUp.y;
			Output.Texcoords.LeftUp.y = Rectangle.Texcoords.LeftUp.y + 
									   ((ClipRectangle.LeftUp.y - Rectangle.Positions.LeftUp.y) / 
									   (Rectangle.Positions.RightDown.y - Rectangle.Positions.LeftUp.y)) * 
									   (Rectangle.Texcoords.RightDown.y - Rectangle.Texcoords.LeftUp.y);
		}
		else
		{
			Output.Positions.LeftUp.y = Rectangle.Positions.LeftUp.y;
			Output.Texcoords.LeftUp.y = Rectangle.Positions.LeftUp.y;
		}


		if (ClipRectangle.RightDown.x < Rectangle.Positions.LeftUp.x)
		{
			Output.Positions.RightDown.x = ClipRectangle.RightDown.x;
			Output.Texcoords.RightDown.x = Rectangle.Texcoords.RightDown.x - 
										  ((ClipRectangle.RightDown.x - Rectangle.Positions.RightDown.x) / 
										  (Rectangle.Positions.RightDown.x - Rectangle.Positions.LeftUp.x)) * 
										  (Rectangle.Texcoords.RightDown.x - Rectangle.Texcoords.LeftUp.x);
		}
		else
		{
			Output.Positions.RightDown.x = Rectangle.Positions.RightDown.x;
			Output.Texcoords.RightDown.x = Rectangle.Positions.RightDown.x;
		}


		if (ClipRectangle.RightDown.y < Rectangle.Positions.LeftUp.y)
		{
			Output.Positions.RightDown.y = ClipRectangle.RightDown.y;
			Output.Texcoords.RightDown.y = Rectangle.Texcoords.RightDown.y - 
										  ((ClipRectangle.RightDown.y - Rectangle.Positions.RightDown.y) / 
										  (Rectangle.Positions.RightDown.y - Rectangle.Positions.LeftUp.y)) * 
										  (Rectangle.Texcoords.RightDown.y - Rectangle.Texcoords.LeftUp.y);
		}
		else
		{
			Output.Positions.RightDown.y = Rectangle.Positions.RightDown.y;
			Output.Texcoords.RightDown.y = Rectangle.Positions.RightDown.y;
		}
		
		return true;
	}
}
