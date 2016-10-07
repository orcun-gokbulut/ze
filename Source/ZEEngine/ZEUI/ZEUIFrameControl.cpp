//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIFrameControl.cpp
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

#include "ZEUIFrameControl.h"
#include "ZEUIRenderer.h"
#include "ZEUICheckBoxControl.h"
#include "ZEUIHorizontalSliderControl.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZETexture/ZETexture2DResource.h"

void ZEUIFrameControl::Draw(ZEUIRenderer* Renderer)
{
	Frame.Color = GetBackgroundColor();
	ZEUIRectangle TempRect = Frame;
	TempRect.Positions.SetPosition(GetScreenPosition());
	TempRect.ZOrder = GetZOrder();

	ZEUIRectangle Output;

	if(!ZEUIRectangle::Clip(Output, TempRect, GetVisibleRectangle()))
		Renderer->AddRectangle(&Output);

	ZEUIControl::Draw(Renderer);
}

void ZEUIFrameControl::SetSize(const ZEVector2& Size)
{
	SetWidth(Size.x);
	SetHeight(Size.y);
}

void ZEUIFrameControl::SetWidth(float Width)
{
	ZEUIControl::SetWidth(Width);
	Frame.Positions.SetWidth(Width);
}

void ZEUIFrameControl::SetHeight(float Height)
{
	ZEUIControl::SetHeight(Height);
	Frame.Positions.SetHeight(Height);
}

void ZEUIFrameControl::SetTexture(const ZEGRTexture* Texture)
{
	if (Frame.Texture == Texture)
		return;

	if (TextureResource != NULL)
	{
		Frame.Texture.Release();
		TextureResource->Release();
		TextureResource = NULL;	
	}

	Frame.Texture = Texture;
}

const ZEGRTexture* ZEUIFrameControl::GetTexture()
{
	return Frame.Texture;
}

void ZEUIFrameControl::SetTexturePath(const ZEString& Path)
{
	if (TexturePath == Path)
		return;

	if (TextureResource != NULL)
	{
		Frame.Texture.Release();
		TextureResource->Release();
		TextureResource = NULL;
	}

	if (Path.IsEmpty())
		return;

	TextureResource = ZETexture2DResource::LoadSharedResource(Path);
	if (TextureResource == NULL)
		return;

	Frame.Texture = TextureResource->GetTexture2D();
}

const ZEString& ZEUIFrameControl::GetTexturePath()
{
	return TexturePath;
}

void ZEUIFrameControl::SetPosition(const ZEVector2& Position)
{
	ZEUIControl::SetPosition(Position);
	Frame.Positions.SetPosition(GetScreenPosition());
}

ZEUIFrameControl::ZEUIFrameControl()
{
	TextureResource = NULL;

	Frame.Color = ZEUIManager::GetDefaultBackgroundColor();
	Frame.Texcoords.LeftUp = ZEVector2::Zero;
	Frame.Texcoords.RightDown = ZEVector2::One;

	SetPosition(ZEVector2::Zero);
	SetWidth(200);
	SetHeight(200);
}
