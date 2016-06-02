//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIRectangle.cpp
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

#include "ZEUIRectangle.h"
#include "ZEUIRenderer.h"

void ZEUIRectangle::ConvertToVertices(ZEUIVertex* Output) const
{
	Output[0].Position = Positions.LeftUp;
	Output[0].Texcoord = Texcoords.LeftUp;
	Output[0].Color = Color;
	Output[1].Position = ZEVector2(Positions.RightDown.x, Positions.LeftUp.y);
	Output[1].Texcoord = ZEVector2(Texcoords.RightDown.x, Texcoords.LeftUp.y);
	Output[1].Color = Color;
	Output[2].Position = Positions.RightDown;
	Output[2].Texcoord = Texcoords.RightDown;
	Output[2].Color = Color;
	Output[3] = Output[2];
	Output[4].Position = ZEVector2(Positions.LeftUp.x, Positions.RightDown.y);
	Output[4].Texcoord = ZEVector2(Texcoords.LeftUp.x, Texcoords.RightDown.y);
	Output[4].Color = Color;
	Output[5] = Output[0];
}

ZEUIRectangle::ZEUIRectangle() : RendererLink(this)
{

}

bool ZEUIRectangle::Clip(ZEUIRectangle& Output, const ZEUIRectangle& Rectangle, const ZERectangle& ClipRectangle)
{
	Output.ZOrder = Rectangle.ZOrder;

	if ((Rectangle.Positions.RightDown.x < ClipRectangle.LeftUp.x) ||
		(Rectangle.Positions.LeftUp.x > ClipRectangle.RightDown.x) ||
		(Rectangle.Positions.RightDown.y < ClipRectangle.LeftUp.y) ||
		(Rectangle.Positions.LeftUp.y > ClipRectangle.RightDown.y))
	{
		return true;
	}
	else
	{
		if (ClipRectangle.LeftUp.x > Rectangle.Positions.LeftUp.x)
		{
			Output.Positions.LeftUp.x = ClipRectangle.LeftUp.x;
			Output.Texcoords.LeftUp.x = Rectangle.Texcoords.LeftUp.x + ((Output.Positions.LeftUp.x - Rectangle.Positions.LeftUp.x) * (Rectangle.Texcoords.GetWidth()) / Rectangle.Positions.GetWidth());
		}
		else
		{
			Output.Positions.LeftUp.x = Rectangle.Positions.LeftUp.x;
			Output.Texcoords.LeftUp.x = Rectangle.Texcoords.LeftUp.x;
		}


		if (ClipRectangle.LeftUp.y > Rectangle.Positions.LeftUp.y)
		{
			Output.Positions.LeftUp.y = ClipRectangle.LeftUp.y;
			Output.Texcoords.LeftUp.y = Rectangle.Texcoords.LeftUp.y + ((Output.Positions.LeftUp.y - Rectangle.Positions.LeftUp.y) * (Rectangle.Texcoords.GetHeight()) / Rectangle.Positions.GetHeight());
		}
		else
		{
			Output.Positions.LeftUp.y = Rectangle.Positions.LeftUp.y;
			Output.Texcoords.LeftUp.y = Rectangle.Texcoords.LeftUp.y;
		}


		if (ClipRectangle.RightDown.x < Rectangle.Positions.RightDown.x)
		{
			Output.Positions.RightDown.x = ClipRectangle.RightDown.x;
			Output.Texcoords.RightDown.x = Rectangle.Texcoords.RightDown.x + ((Output.Positions.RightDown.x - Rectangle.Positions.RightDown.x) * (Rectangle.Texcoords.GetWidth()) / Rectangle.Positions.GetWidth());
		}
		else
		{
			Output.Positions.RightDown.x = Rectangle.Positions.RightDown.x;
			Output.Texcoords.RightDown.x = Rectangle.Texcoords.RightDown.x;
		}


		if (ClipRectangle.RightDown.y < Rectangle.Positions.RightDown.y)
		{
			Output.Positions.RightDown.y = ClipRectangle.RightDown.y;
			Output.Texcoords.RightDown.y = Rectangle.Texcoords.RightDown.y + ((Output.Positions.RightDown.y - Rectangle.Positions.RightDown.y) * (Rectangle.Texcoords.GetHeight()) / Rectangle.Positions.GetHeight());
		}
		else
		{
			Output.Positions.RightDown.y = Rectangle.Positions.RightDown.y;
			Output.Texcoords.RightDown.y = Rectangle.Texcoords.RightDown.y;
		}

		Output.Texture = Rectangle.Texture;
		Output.Color = Rectangle.Color;
		Output.ZOrder = Rectangle.ZOrder;
		
		return false;
	}
}
