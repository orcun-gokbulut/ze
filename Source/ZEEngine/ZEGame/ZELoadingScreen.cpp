//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELoadingScreen.cpp
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

#include "ZELoadingScreen.h"

#include "ZEScene.h"
#include "ZEUI/ZEUIFontTrueType.h"
#include "ZEUI/ZEUIManager.h"
#include "ZEUI/ZEUILabel.h"
#include "ZERenderer/ZERNView.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZECore/ZEConsole.h"
#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZECore/ZETimeParameters.h"

ZEEntityResult ZELoadingScreen::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	if (Manager == NULL)
		return ZE_ER_FAILED;

	FadeValue = 0.0f;

	ImageControl = new ZEUIFrameControl();
	ImageControl->SetTextureFileName(ImageFileName);
	Manager->AddControl(ImageControl);

	BackgroundControlA = new ZEUIFrameControl();
	BackgroundControlA->SetBackgroundColor(BackgroundColor);
	Manager->AddControl(BackgroundControlA);

	BackgroundControlB = new ZEUIFrameControl();
	BackgroundControlB->SetBackgroundColor(BackgroundColor);
	Manager->AddControl(BackgroundControlB);

	return ZE_ER_DONE;
}

ZEEntityResult ZELoadingScreen::UnloadInternal()
{
	Manager->RemoveControl(ImageControl);
	ImageControl->Destroy();
	ImageControl = NULL;

	Manager->RemoveControl(BackgroundControlA);
	BackgroundControlA->Destroy();
	BackgroundControlA = NULL;

	Manager->RemoveControl(BackgroundControlB);
	BackgroundControlB->Destroy();
	BackgroundControlB = NULL;

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);

	return ZE_ER_DONE;
}

ZELoadingScreen::ZELoadingScreen()
{
	SetEntityFlags(ZE_EF_RENDERABLE | ZE_EF_TICKABLE);
	State = ZE_LSS_NONE;
	FadeValue = 0.0f;
	ImageFileName = "#R:/ZEEngine/ZELoadingScreen/LoadingScreen-Development.png";
	BackgroundColor = ZEVector4(1.0f, 1.0f, 1.0f, 1.0f);
	FadeInFactor = 0.0f;
	FadeOutFactor = 1.0f;
}

ZELoadingScreen::~ZELoadingScreen()
{
	
}

void ZELoadingScreen::SetManager(ZEUIManager* Manager)
{
	if (this->Manager == Manager)
		return;

	this->Manager = Manager;

	if (IsLoaded())
		Reload();
}

ZEUIManager* ZELoadingScreen::GetManager()
{
	return Manager;
}

void ZELoadingScreen::SetImageFileName(const ZEString& FileName)
{
	bool Diffrent = ZEPathInfo::Compare(ImageFileName, FileName);
	ImageFileName = FileName;
	if (Diffrent && IsLoaded())
		ImageControl->SetTextureFileName(FileName);
}

const ZEString& ZELoadingScreen::GetImageFileName()
{
	return ImageFileName;
}


void ZELoadingScreen::SetBackgroundColor(const ZEVector4& Color)
{
	BackgroundColor = Color;
}

const ZEVector4& ZELoadingScreen::GetBackgroundColor()
{
	return BackgroundColor;
}

void ZELoadingScreen::SetFadeInFactor(float Factor)
{
	FadeInFactor = Factor;
}

float ZELoadingScreen::GetFadeInFacotr()
{
	return FadeInFactor;
}

void ZELoadingScreen::SetFadeOutFactor(float Factor)
{
	FadeOutFactor = Factor;
}

float ZELoadingScreen::GetFadeOutFactor()
{
	return FadeOutFactor;
}

void ZELoadingScreen::Tick(float ElapsedTime)
{
	if (State == ZE_LSS_NONE)
		return;

	if (State == ZE_LSS_FADING_IN)
	{
		if (FadeInFactor == 0.0f)
			FadeValue = 1.0f;
		else
			FadeValue += ElapsedTime * FadeInFactor;

		if (FadeValue >= 1.0f)
		{
			FadeValue = 1.0f;
			State = ZE_LSS_SCREEN;
		}
	}
	else if (State == ZE_LSS_FADING_OUT)
	{
		if (FadeOutFactor == 0.0f)
			FadeValue = 0.0f;
		else
			FadeValue -= ElapsedTime * FadeOutFactor;

		if (FadeValue <= 0.0f)
		{
			FadeValue = 0.0f;
			State = ZE_LSS_NONE;
		}
	}

	if (Counter.GetTimeMilliseconds() <= 0.5)
		return;

	Counter.Reset();

	if (GetScene()->GetEntityStateStats().LoadingCount == 0)
		State = ZE_LSS_FADING_OUT;
	
}

bool ZELoadingScreen::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (Parameters->Type != ZERN_RT_COLOR)
		return false;

	if (State == ZE_LSS_NONE)
	{
		BackgroundControlA->SetVisiblity(false);
		BackgroundControlB->SetVisiblity(false);
		ImageControl->SetVisiblity(false);
	}
	else
	{
		ZEVector2 ScreenSize = Parameters->View->Viewport.GetSize();
		const ZEGRTexture* ImageTexture = ImageControl->GetTexture();
		if (ImageTexture != NULL && ImageTexture->IsLoaded())
		{
			ZEVector2 Position;
			ZEUInt ImageWidth = ImageTexture->GetWidth();
			ZEUInt ImageHeight = ImageTexture->GetHeight();
			float ImageAR = (float)ImageWidth / (float)ImageHeight;

			if (ImageWidth > ScreenSize.x && ImageHeight > ScreenSize.y)
			{
				float ScreenAR = ScreenSize.x / ScreenSize.y;
				if (ScreenAR > ImageAR)
				{
					ImageHeight = ScreenSize.y; 
					ImageWidth = ImageHeight * ImageAR;
				}
				else
				{
					ImageWidth = ScreenSize.x; 
					ImageHeight = ImageWidth / ImageAR;
				}
			}
			else if (ImageWidth > ScreenSize.x)
			{
				ImageWidth = ScreenSize.x; 
				ImageHeight = ImageWidth / ImageAR;
			}
			else if (ImageHeight > ScreenSize.y)
			{
				ImageHeight = ScreenSize.y; 
				ImageWidth *= ImageHeight * ImageAR;
			}

			Position.x = (ZEUInt)(ScreenSize.x - ImageWidth) / 2;
			Position.y = (ZEUInt)(ScreenSize.y - ImageHeight) / 2;

			ImageControl->SetPosition(Position);
			ImageControl->SetSize(ZEVector2(ImageWidth, ImageHeight));
			ImageControl->SetBackgroundColor(ZEVector4(ZEVector3::One, BackgroundColor.w * FadeValue));
			ImageControl->SetVisiblity(true);

			if (Position.x == 0.0f && Position.y == 0.0f)
			{
				// Fully Covers
				BackgroundControlA->SetVisiblity(false);
				BackgroundControlB->SetVisiblity(false);
			}
			else if (Position.x == 0.0f)
			{
				// Horizontal
				BackgroundControlA->SetPosition(ZEVector2(0.0f, 0.0f));
				BackgroundControlA->SetSize(ZEVector2(ScreenSize.x, Position.y));
				BackgroundControlA->SetBackgroundColor(ZEVector4(BackgroundColor.xyz(), BackgroundColor.w * FadeValue));
				BackgroundControlA->SetVisiblity(true);

				BackgroundControlB->SetPosition(ZEVector2(0.0f, Position.y + ImageHeight));
				BackgroundControlB->SetSize(ZEVector2(ScreenSize.x, Position.y));
				BackgroundControlB->SetBackgroundColor(ZEVector4(BackgroundColor.xyz(), BackgroundColor.w * FadeValue));
				BackgroundControlB->SetVisiblity(true);
			}
			else if (Position.y == 0.0f)
			{
				// Vertical
				BackgroundControlA->SetPosition(ZEVector2(0.0f, 0.0f));
				BackgroundControlA->SetSize(ZEVector2(Position.x, ScreenSize.y));
				BackgroundControlA->SetBackgroundColor(ZEVector4(BackgroundColor.xyz(), BackgroundColor.w * FadeValue));
				BackgroundControlA->SetVisiblity(true);

				BackgroundControlB->SetPosition(ZEVector2(Position.x + ImageWidth, 0.0f));
				BackgroundControlB->SetSize(ZEVector2(Position.x, ScreenSize.y));
				BackgroundControlB->SetBackgroundColor(ZEVector4(BackgroundColor.xyz(), BackgroundColor.w * FadeValue));
				BackgroundControlB->SetVisiblity(true);
			}
		}
		else
		{
			BackgroundControlA->SetPosition(ZEVector2(0.0f, 0.0f));
			BackgroundControlA->SetSize(ScreenSize);
			BackgroundControlA->SetBackgroundColor(ZEVector4(BackgroundColor.xyz(), BackgroundColor.w * FadeValue));
			BackgroundControlA->SetVisiblity(true);
			BackgroundControlB->SetVisiblity(false);			
		}
	}

	return false;
}

void ZELoadingScreen::Show()
{
	State = ZE_LSS_FADING_IN;
	Counter.Start();
}

void ZELoadingScreen::Hide()
{
	State = ZE_LSS_FADING_OUT;
}

ZELoadingScreen* ZELoadingScreen::CreateInstance()
{
	return new ZELoadingScreen;
}
