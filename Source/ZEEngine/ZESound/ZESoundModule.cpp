//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundModule.cpp
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

#include "ZESoundModule.h"
#include "ZESoundResourceMP3.h"
#include "ZESoundSource.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEOptionManager.h"

ZEOptionSection  ZESoundModule::SoundOptions;

ZE_MODULE_DESCRIPTION_ABSTRACT(ZESoundModule, ZEModule, &ZESoundModule::SoundOptions)

static void OnOptionsChanged(ZEOption* Option)
{
	if (zeSound != NULL)
		zeSound->OptionsChanged();
}

void ZESoundModule::BaseInitialize()
{
	SoundOptions.SetName("Sound");
	SoundOptions.SetOnChanged(&OnOptionsChanged);

	SoundOptions.AddOption(new ZEOption("DeviceId", 0, ZE_OA_NORMAL));
	SoundOptions.AddOption(new ZEOption("StreamingDisabled", false, ZE_OA_NORMAL));
	SoundOptions.AddOption(new ZEOption("MaxBufferSize", 2000, ZE_OA_NORMAL));
	SoundOptions.AddOption(new ZEOption("MasterVolume", 100, ZE_OA_NORMAL));
	SoundOptions.AddOption(new ZEOption("SpeakerLayout", 1, ZE_OA_NORMAL));
	SoundOptions.AddOption(new ZEOption("EffectVolume", 100, ZE_OA_NORMAL));
	SoundOptions.AddOption(new ZEOption("DialogVolume", 100, ZE_OA_NORMAL));
	SoundOptions.AddOption(new ZEOption("MusicVolume", 100, ZE_OA_NORMAL));
	SoundOptions.AddOption(new ZEOption("VideoVolume", 100, ZE_OA_NORMAL));
	SoundOptions.AddOption(new ZEOption("PlayerCommVolume", 100, ZE_OA_NORMAL));
	ZEOptionManager::GetInstance()->RegisterSection(&SoundOptions);

	ZESoundResourceMP3::BaseInitialize();
}

void ZESoundModule::BaseDeinitialize()
{
	ZESoundResourceMP3::BaseDeinitialize();
	ZEOptionManager::GetInstance()->UnregisterSection(&SoundOptions);
}

void ZESoundModule::OptionsChanged()
{
	ZEOption* Current = SoundOptions.GetOption("StreamingDisabled");
	if (Current->IsChanged())
	{
		SetStreamingDisabled(Current->GetValue().GetBoolean());
		Current->SetValue(GetStreamingDisabled());
	}

	Current = SoundOptions.GetOption("MaxBufferSize");
	if (Current->IsChanged())
	{
		SetMaxBufferSize((ZESize)Current->GetValue().GetInt32());
		Current->SetValue((ZEInt)GetMaxBufferSize());
	}

	Current = SoundOptions.GetOption("MasterVolume");
	if (Current->IsChanged())
	{
		SetMasterVolume(Current->GetValue().GetInt32());
		Current->SetValue((ZEInt)GetMasterVolume());
	}

	Current = SoundOptions.GetOption("SpeakerLayout");
	if (Current->IsChanged())
	{
		SetSpeakerLayout((ZESpeakerLayout)Current->GetValue().GetInt32());
		Current->SetValue((ZEInt)GetSpeakerLayout());
	}

	Current = SoundOptions.GetOption("EffectVolume");
	if (Current->IsChanged())
	{
		SetTypeVolume(ZE_SST_EFFECT, Current->GetValue().GetInt32());
		Current->SetValue((ZEInt)GetTypeVolume(ZE_SST_EFFECT));
	}

	Current = SoundOptions.GetOption("DialogVolume");
	if (Current->IsChanged())
	{
		SetTypeVolume(ZE_SST_DIALOG, Current->GetValue().GetInt32());
		Current->SetValue((ZEInt)GetTypeVolume(ZE_SST_DIALOG));
	}

	Current = SoundOptions.GetOption("MusicVolume");
	if (Current->IsChanged())
	{
		SetTypeVolume(ZE_SST_MUSIC, Current->GetValue().GetInt32());
		Current->SetValue((ZEInt)GetTypeVolume(ZE_SST_MUSIC));
	}

	Current = SoundOptions.GetOption("VideoVolume");
	if (Current->IsChanged())
	{
		SetTypeVolume(ZE_SST_VIDEO, Current->GetValue().GetInt32());
		Current->SetValue((ZEInt)GetTypeVolume(ZE_SST_VIDEO));
	}

	Current = SoundOptions.GetOption("PlayerCommVolume");
	if (Current->IsChanged())
	{
		SetTypeVolume(ZE_SST_PLAYER_COMM, Current->GetValue().GetInt32());
		Current->SetValue((ZEInt)GetTypeVolume(ZE_SST_PLAYER_COMM));
	}
}

ZESoundModule* ZESoundModule::GetInstance()
{
	return ZECore::GetInstance()->GetSoundModule();
}
