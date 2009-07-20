//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SoundSource.cpp
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

#include "SoundSource.h"
#include "SoundModule.h"

bool ZESoundSource::Is3D()
{
	return false;
}

ZESoundState	ZESoundSource::GetSoundState()				
{
	return SoundState;
}

int ZESoundSource::GetBufferId()
{ 
	return BufferId;
}

unsigned int ZESoundSource::GetCursor()
{
	return Cursor;
}

void ZESoundSource::SetCursor(unsigned int NewCursor)	
{
	Cursor = NewCursor;
	Changed = true;
}	

unsigned int ZESoundSource ::GetPan()
{
	return Pan;
}

void ZESoundSource::SetPan(unsigned int NewPan)
{
	Pan = NewPan;
	Changed = true;
}	

unsigned int ZESoundSource ::GetFrequency()			
{
	return Frequency;
}

void ZESoundSource ::SetFrequency(unsigned int NewFrequency)
{
	Frequency = NewFrequency;
	Changed = true;
}

unsigned int ZESoundSource::GetVolume()
{
	return Volume;
}

void ZESoundSource::SetVolume(unsigned int NewVolume)
{
	Volume = NewVolume;
	Changed = true;
}

void ZESoundSource ::Play()
{
	if (SoundState != SOUNDSTATE_NOBUFFER)
	{
		SoundState = SOUNDSTATE_PLAYING;
		zeSound->UpdateSource(this);
		zeSound->Play(this);
	}
}

void ZESoundSource::Pause()
{	
	if (SoundState != SOUNDSTATE_NOBUFFER)
	{
		zeSound->Pause(this);
		SoundState = SOUNDSTATE_PAUSED;
	}
}

void ZESoundSource ::Stop() 
{
	if (SoundState != SOUNDSTATE_NOBUFFER)
	{
		zeSound->Stop(this);
		SoundState = SOUNDSTATE_STOPPED;
	}
}

void ZESoundSource ::PlayLoop()
{
	if (SoundState != SOUNDSTATE_NOBUFFER)
	{
		if (Changed)
			zeSound->UpdateSource(this);
		zeSound->PlayLoop(this);
		SoundState = SOUNDSTATE_LOOPING;
	}
}

void ZESoundSource::LoadSoundResource(ZESoundResource* Resource)
{
	if (SoundResource != NULL)
	{
		SoundResource->Release();
		zeSound->ReleaseBuffer(this);
	}
	
	if (Resource == NULL)
	{
		SoundState = SOUNDSTATE_NOBUFFER;
		BufferId = -1;
	}
	else
	{
		SoundResource = Resource;
		SoundState = SOUNDSTATE_STOPPED;	
		BufferId = zeSound->CreateBuffer(this, SoundResource);
		if (BufferId == -1)
			SoundState = SOUNDSTATE_NOBUFFER;
	}
}

void ZESoundSource::LoadSoundFile(const char* Filename)
{
	if (SoundResource != NULL)
	{
		SoundResource->Release();
		zeSound->ReleaseBuffer(this);
	}
	
	SoundResource = ZESoundResource::LoadResource(Filename);
	if (SoundResource == NULL)
	{
		SoundState = SOUNDSTATE_NOBUFFER;
		BufferId = -1;
	}
	else
	{
		SoundState = SOUNDSTATE_STOPPED;	
		BufferId = zeSound->CreateBuffer(this, SoundResource);
		if (BufferId == -1)
			SoundState = SOUNDSTATE_NOBUFFER;
	}
}

void ZESoundSource::Update()
{	
	if (SoundState != SOUNDSTATE_NOBUFFER && Changed)
	{
		zeSound->UpdateSource(this);
		Changed = false;
	}
}	

ZESoundSource::ZESoundSource()
{
	Changed = false;
	SoundResource = NULL;
	SoundState = SOUNDSTATE_NOBUFFER;
	BufferId = -1;
}

ZESoundSource::~ZESoundSource()
{
	if (SoundResource != NULL)
	{
		zeSound->ReleaseBuffer(this);
		SoundResource->Release();
	}
}
