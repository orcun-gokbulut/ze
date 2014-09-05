//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDecompressorBMPRLE.cpp
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

#include "ZEDecompressorBMPRLE.h"

#include <memory.h>

enum ZEBMPRLEDecompressorMode
{
	ZE_BMP_RLE_DM_NONE				= 0,
	ZE_BMP_RLE_DM_MARKER			= 1,
	ZE_BMP_RLE_DM_DELTA_INIT		= 2,
	ZE_BMP_RLE_DM_DELTA				= 3,
	ZE_BMP_RLE_DM_REPEAT_INIT		= 4,
	ZE_BMP_RLE_DM_REPEAT			= 5,
	ZE_BMP_RLE_DM_ABSOLUTE_PADDING	= 6,
	ZE_BMP_RLE_DM_ABSOLUTE			= 7,
	ZE_BMP_RLE_DM_ABSOLUTE_END		= 8,
	ZE_BMP_RLE_DM_FILL_BLACK		= 9
};

void ZEDecompressorBMPRLE::SetInput(void* Buffer)
{
	InputPosition = 0;
	ZEDecompressor::SetInput(Buffer);
}

void ZEDecompressorBMPRLE::SetOutput(void* Buffer)
{
	OutputPosition = 0;
	ZEDecompressor::SetOutput(Buffer);
}

void ZEDecompressorBMPRLE::Decompress()
{
	ZESize Remaining = 0;
	while(true)
	{
		if (this->InputPosition >= this->InputSize && this->Mode != ZE_BMP_RLE_DM_FILL_BLACK)
		{
			this->InputPosition = 0;
			State = ZE_DS_INPUT_PROCESSED;
			return;
		}

		if (this->OutputPosition >= this->OutputSize)
		{
			this->Wrapped = true;
			this->OutputPosition = 0;
			State = ZE_DS_OUTPUT_FULL;
			return;
		}

		ZEUInt8* Input = (ZEUInt8*)this->Input + this->InputPosition;
		ZEUInt16* Output = (ZEUInt16*)this->Output + this->OutputPosition;

		switch(this->Mode)
		{
			case ZE_BMP_RLE_DM_NONE: // None
				if (*Input == 0)
					this->Mode = ZE_BMP_RLE_DM_MARKER;
				else
				{
					this->Mode = ZE_BMP_RLE_DM_REPEAT_INIT;
					this->Count = *Input;
				}
				this->InputPosition++;
				break;

			case ZE_BMP_RLE_DM_MARKER:
				if (*Input == 0) // End of Row
				{
					if (!this->Wrapped)
					{
						this->Count = this->OutputSize - this->OutputPosition;
						this->Wrapped = false;
						this->Mode = ZE_BMP_RLE_DM_FILL_BLACK;
					}
					else
						this->Mode = ZE_BMP_RLE_DM_NONE;
				}
				else if (*Input == 1) // End of Bitmap
				{
					this->Count = (ZESize)-1;
					this->Mode = ZE_BMP_RLE_DM_FILL_BLACK;
				}
				else if (*Input == 2) // Delta
				{
					this->Mode = ZE_BMP_RLE_DM_DELTA_INIT;
				}
				else // Absolute Mode
				{
					this->Count = *Input;
					/*if (this->Count % 2 == 1)
						this->Mode = ZE_BMP_RLE_DM_ABSOLUTE_PADDING;
					else*/
						this->Mode = ZE_BMP_RLE_DM_ABSOLUTE;
				}
				this->InputPosition++;
				break;

			case ZE_BMP_RLE_DM_DELTA_INIT:
				this->DeltaX = *Input;
				this->Mode = ZE_BMP_RLE_DM_DELTA;
				this->InputPosition++;
				break;

			case ZE_BMP_RLE_DM_DELTA:
				if (*Input > 0)
					this->Count = this->OutputSize * (ZESize)*Input - this->OutputPosition;
				this->Count += this->DeltaX;
				if (this->Count == 0)
					this->Mode = ZE_BMP_RLE_DM_NONE;
				this->Mode = ZE_BMP_RLE_DM_FILL_BLACK;
				this->InputPosition++;
				break;

			case ZE_BMP_RLE_DM_REPEAT_INIT:
				this->Value = *Input;
				this->Mode = ZE_BMP_RLE_DM_REPEAT;
				this->InputPosition++;
				break;

			case ZE_BMP_RLE_DM_REPEAT:
				if (this->OutputPosition + this->Count <= this->OutputSize)
					Remaining = this->Count;
				else
					Remaining = this->OutputSize - this->OutputPosition;

				for (ZESize I = 0; I < Remaining; I++)
					*Output++ = this->Value;
				
				this->Count -= Remaining;
				this->OutputPosition += Remaining;
				this->Wrapped = false;

				if (this->Count == 0)
					this->Mode = ZE_BMP_RLE_DM_NONE;
				break;

			case ZE_BMP_RLE_DM_ABSOLUTE_PADDING:
				if (*Input != 0)
				{
					State = ZE_DS_ERROR;
					return;
				}
				this->InputPosition++;
				this->Mode = ZE_BMP_RLE_DM_ABSOLUTE;
				break;

			case ZE_BMP_RLE_DM_ABSOLUTE:
				if (this->OutputPosition + this->Count <= this->OutputSize)
					Remaining = this->Count;
				else
					Remaining = this->OutputSize - this->OutputPosition;

				if (this->InputPosition + Remaining > this->InputSize)
					Remaining = this->InputSize - this->InputPosition;
								
				for (ZESize I = 0; I < Remaining; I++)
					*Output++ = *Input++;

				this->OutputPosition += Remaining;
				this->InputPosition += Remaining;
				this->Count -= Remaining;
				this->Wrapped = false;
				
				if (this->Count == 0)
					this->Mode = ZE_BMP_RLE_DM_ABSOLUTE_END;
				break;

			case ZE_BMP_RLE_DM_ABSOLUTE_END:
				if (*Input != 0)
				{
					State = ZE_DS_ERROR;
					return;
				}
				this->InputPosition++;
				this->Mode = ZE_BMP_RLE_DM_NONE;
				break;

			case ZE_BMP_RLE_DM_FILL_BLACK:
				if (this->OutputPosition + this->Count <= this->OutputSize)
					Remaining = this->Count;
				else
					Remaining = this->OutputSize - this->OutputPosition;

				memset(Output, 0xFFFF, Remaining * sizeof(ZEUInt16));

				this->OutputPosition += Remaining;
				this->Count -= Remaining;
				this->Wrapped = false;

				if (this->Count == 0)
					this->Mode = ZE_BMP_RLE_DM_NONE;

				break;
		}
	}
}

void ZEDecompressorBMPRLE::Reset()
{
	Mode = ZE_BMP_RLE_DM_NONE;
	Count = 0;
	Value = 0;
	DeltaX = 0;
	Wrapped = false;

	Output = NULL;
	OutputPosition = 0;
	OutputSize = 0;

	Input = NULL;
	InputPosition = 0;
	InputSize = 0;
}

ZEDecompressorBMPRLE::ZEDecompressorBMPRLE()
{
	Reset();
}
