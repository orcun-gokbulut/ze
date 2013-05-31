//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECompressorZLIB.cpp
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

#include "ZECompressorZLIB.h"

void ZECompressorZLIB::SetEos(bool Eos)
{
	this->Eos = Eos;
}

void ZECompressorZLIB::Initialize()
{
	Eos = false;
	ZLevel = Z_NO_FLUSH;
	Input = NULL;
	Output = NULL;
	InputSize = 0;
	OutputSize = 0;
	State = ZE_CS_NONE;
	ZStream.zalloc = Z_NULL;
	ZStream.zfree = Z_NULL;
	ZStream.opaque = Z_NULL;
}

void ZECompressorZLIB::Compress()
{
	if(State == ZE_CS_DONE)
		return;

	if(State == ZE_CS_ERROR)
		return;

	if(State == ZE_CS_OUTPUT_FULL)
	{
		ZStream.avail_out = OutputSize;
		ZStream.next_out = (unsigned char*)Output;
		if(Eos)
			ZLevel = Z_FINISH;

		if(deflate(&ZStream, ZLevel) != Z_STREAM_ERROR)
		{			
			if(ZStream.avail_out == 0)
			{
				ZStream.avail_out = OutputSize;
				ZStream.next_out = (unsigned char*)Output;
				State = ZE_CS_OUTPUT_FULL;
				return;
			}
			else if(Eos)
			{
				OutputSize = OutputSize - ZStream.avail_out;
				State = ZE_CS_DONE;
				(void)deflateEnd(&ZStream);
				return;
			}
			else
			{
				State = ZE_CS_INPUT_PROCESSED;
				return;
			}
		}
		else
			State = ZE_CS_ERROR;		
	}

	if(State == ZE_CS_NONE)
	{
		if(deflateInit(&ZStream, Z_DEFAULT_COMPRESSION) != Z_OK)
		 {
			 State = ZE_CS_ERROR;
			 return;
		 }

		ZStream.avail_in = InputSize;
		ZStream.next_in = (unsigned char*)Input;

		ZStream.avail_out = OutputSize;
		ZStream.next_out = (unsigned char*)Output;

		State = ZE_CS_INPUT_PROCESSED;
	}

	if(State == ZE_CS_INPUT_PROCESSED)
	{		
		ZStream.avail_in = InputSize;
		ZStream.next_in = (unsigned char*)Input;
		if(Eos)
			ZLevel = Z_FINISH;

		if(deflate(&ZStream, ZLevel) != Z_STREAM_ERROR)
		{
			if(ZStream.avail_out == 0)
			{
				State = ZE_CS_OUTPUT_FULL;
				return;
			}
			else if(Eos)
			{
				State = ZE_CS_DONE;
				OutputSize = OutputSize - ZStream.avail_out;
				(void)deflateEnd(&ZStream);
				return;
			}
			else
			{				
				State = ZE_CS_INPUT_PROCESSED;
			}
		}
		else
			State = ZE_CS_ERROR;
	}

}

void ZECompressorZLIB::Reset()
{
	Eos = false;
	ZLevel = Z_NO_FLUSH;
	Input = NULL;
	Output = NULL;
	InputSize = 0;
	OutputSize = 0;
	State = ZE_CS_NONE;
	ZStream.zalloc = Z_NULL;
	ZStream.zfree = Z_NULL;
	ZStream.opaque = Z_NULL;
}
