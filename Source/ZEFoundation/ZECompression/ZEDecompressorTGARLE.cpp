//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDecompressorTGARLE.cpp
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

#include "ZEDecompressorTGARLE.h"

#include <memory.h>

enum ZEDecompressorTGARLEMode
{
	ZE_TGA_RLE_DM_NONE,
	ZE_TGA_RLE_DM_REPEAT_INIT,
	ZE_TGA_RLE_DM_REPEAT,
	ZE_TGA_RLE_DM_ABSOLUTE
};

void ZEDecompressorTGARLE::SetInput(void* Buffer)
{
	InputCursor = 0;
	ZEDecompressor::SetInput(Buffer);
}

void ZEDecompressorTGARLE::SetOutput(void* Buffer)
{
	OutputCursor = 0;
	ZEDecompressor::SetOutput(Buffer);
}

void ZEDecompressorTGARLE::SetWordSize(ZESize Size)
{
	WordSize = Size;
}

ZESize ZEDecompressorTGARLE::GetWordSize()
{
	return WordSize;
}

void ZEDecompressorTGARLE::Decompress()
{
	ZESize Remaining = 0;
	while(true)
	{
		if (this->InputCursor >= this->InputSize)
		{
			State = ZE_DS_INPUT_PROCESSED;
			return;
		}

		if (this->OutputCursor >= this->OutputSize)
		{
			State = ZE_DS_OUTPUT_FULL;
			return;
		}

		ZEUInt8* Input = (ZEUInt8*)this->Input + this->InputCursor;
		ZEUInt8* Output = (ZEUInt8*)this->Output + this->OutputCursor;

		switch(this->Mode)
		{
			case ZE_TGA_RLE_DM_NONE: // None
				if ((*Input & 0x80) == 0x80)
				{
					this->Mode = ZE_TGA_RLE_DM_REPEAT_INIT;
					this->Count = ((*Input & 0x7F) + 1) * this->WordSize;
					this->Count2 = this->WordSize;
				}
				else
				{
					this->Mode = ZE_TGA_RLE_DM_ABSOLUTE;
					this->Count = ((*Input & 0x7F) + 1) * this->WordSize;
				}
				this->InputCursor++;
				break;

			case ZE_TGA_RLE_DM_REPEAT_INIT:
				this->Value[this->WordSize - this->Count2] = *Input;
				this->Count2--;
				this->InputCursor++;
				if (this->Count2 == 0)
					this->Mode = ZE_TGA_RLE_DM_REPEAT;
				break;

			case ZE_TGA_RLE_DM_REPEAT:
				if (this->OutputCursor + this->Count <= this->OutputSize * this->WordSize)
					Remaining = this->Count;
				else
					Remaining = this->OutputSize - this->OutputCursor;

				for (ZESize I = 0; I < Remaining; I++)
				{
					*Output++ = this->Value[this->Count2 % this->WordSize];
					this->Count2++;
				}

				this->Count -= Remaining;
				this->OutputCursor += Remaining;

				if (this->Count == 0)
					this->Mode = ZE_TGA_RLE_DM_NONE;
				break;

			case ZE_TGA_RLE_DM_ABSOLUTE:
				if (this->OutputCursor + this->Count <= this->OutputSize)
					Remaining = this->Count;
				else
					Remaining = this->OutputSize - this->OutputCursor;

				if (this->InputCursor + Remaining > this->InputSize)
					Remaining = this->InputSize - this->InputCursor;

				memcpy(Output, Input, Remaining);

				this->OutputCursor += Remaining;
				this->InputCursor += Remaining;
				this->Count -= Remaining;

				if (this->Count == 0)
					this->Mode = ZE_TGA_RLE_DM_NONE;
				break;
		}
	}
}

void ZEDecompressorTGARLE::Reset()
{
	Mode = ZE_TGA_RLE_DM_NONE;
	Count = 0;
	Count2 = 0;
	memset(Value, 0, sizeof(Value));

	Output = NULL;
	OutputCursor = 0;
	OutputSize = 0;

	Input = NULL;
	InputCursor = 0;
	InputSize = 0;
}

ZEDecompressorTGARLE::ZEDecompressorTGARLE()
{
	Reset();
}
