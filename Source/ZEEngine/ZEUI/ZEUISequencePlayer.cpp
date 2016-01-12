//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUISequencePlayer.cpp
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

#include "ZEUISequencePlayer.h"
#include "ZETexture\ZETexture2DResource.h"
#include "ZEGame\ZEScene.h"

ZEUISequencePlayer::ZEUISequencePlayer()
{
	Frames.Clear();
	SoundPlayer = ZESoundSource::CreateInstance();
	Time = 0.0f;
	TimeRemainder = 0.0f;
	FrameCounter = 0.0f;
	FramesPerSecond = 25.0f;
	State = ZEUI_SPS_STOPPED;
	zeScene->AddEntity(SoundPlayer);
}

ZEUISequencePlayer::~ZEUISequencePlayer()
{
	Frames.Clear();
	SoundPlayer->Stop();
	SoundPlayer->Destroy();
}

void ZEUISequencePlayer::Tick(float ElapsedTime)
{
	if(State == ZEUI_SPS_PLAYING)
	{
		if(SoundPlayer->GetSoundSourceState() != ZE_SSS_PLAYING)
			SoundPlayer->Play();

		Time += ElapsedTime + TimeRemainder;

		if(Time >= (1.0f / FramesPerSecond))
		{
			TimeRemainder = Time - (1.0f / FramesPerSecond);
			Time = 0.0f;
			FrameCounter++;

			if(FrameCounter > Frames.GetCount() - 1)
			{
				FrameCounter = 0;
				Stop();
			}

			((ZEUIMaterial*)GetMaterial())->SetTexture(Frames[FrameCounter]);
		}
	}
}

void ZEUISequencePlayer::SetFramesPerSecond(float FPS)
{
	FramesPerSecond = FPS;
}

float ZEUISequencePlayer::GetFramesPerSecond() const
{
	return FramesPerSecond;
}

void ZEUISequencePlayer::SetFramesSequence(const ZEString& FolderPath, ZEUInt FrameCount)
{
	for(ZESize I = 1; I <= FrameCount; I++)
	{
		if(I < 10)
			Frames.Add(ZETexture2DResource::LoadSharedResource(FolderPath + ZEString("/") + ZEString("VideoSequance000") + I + ".jpg")->GetTexture2D());
		else if(I < 100)
			Frames.Add(ZETexture2DResource::LoadSharedResource(FolderPath + ZEString("/") + ZEString("VideoSequance00") + I + ".jpg")->GetTexture2D());
		else if(I < 1000)
			Frames.Add(ZETexture2DResource::LoadSharedResource(FolderPath + ZEString("/") + ZEString("VideoSequance0") + I + ".jpg")->GetTexture2D());
		else
			Frames.Add(ZETexture2DResource::LoadSharedResource(FolderPath + ZEString("/") + ZEString("VideoSequance") + I + ".jpg")->GetTexture2D());
	}
}

void ZEUISequencePlayer::SetAudioFile(const ZEString& FileName)
{
	SoundPlayer->SetSoundResource(ZESoundResource::LoadSharedResource(FileName));
	SoundPlayer->Stop();
}

void ZEUISequencePlayer::Play()
{
	State = ZEUI_SPS_PLAYING;
	((ZEUIMaterial*)GetMaterial())->SetTexture(Frames[0]);
}

void ZEUISequencePlayer::Stop()
{
	Time = 0.0f;
	TimeRemainder = 0.0f;
	FrameCounter = 0.0f;
	SoundPlayer->Stop();
	State = ZEUI_SPS_STOPPED;
}

void ZEUISequencePlayer::SetVolume(ZEUInt Volume)
{
	SoundPlayer->SetVolume(Volume);
}
