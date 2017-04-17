//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECompressor.h
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

#pragma once

#ifndef __ZE_COMPRESSOR_H__
#define __ZE_COMPRESSOR_H__
#include "ZETypes.h"
#include "ZEExport.ZEFoundation.h"

enum ZECompressorState
{
	ZE_CS_NONE,
	ZE_CS_OUTPUT_FULL,	
	ZE_CS_INPUT_PROCESSED,
	ZE_CS_ERROR,
	ZE_CS_DONE
};

class ZE_EXPORT_ZEFOUNDATION ZECompressor
{
	protected:		
		void*					Input;
		ZESize					InputSize;

		void *					Output;
		ZESize					OutputSize;

		ZECompressorState		State;
	
	public:		
		ZECompressorState		GetState();

		virtual void			SetInput(void* Buffer);
		void*					GetInput();

		void					SetInputSize(ZESize Size);
		ZESize					GetInputSize();

		virtual void			SetOutput(void* Buffer);
		void*					GetOutput();

		void					SetOutputSize(ZESize Size);
		ZESize					GetOutputSize();

		virtual void			Compress() = 0;
		virtual void			Reset() = 0;

								ZECompressor();
								~ZECompressor();
};

#endif
