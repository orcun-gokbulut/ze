//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGMCUController.h
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
#ifndef __ZE_TEXTURE_FILE_JPEG_MCU_CONTROLLER_H__
#define __ZE_TEXTURE_FILE_JPEG_MCU_CONTROLLER_H__

#include "ZETextureFileJPEGCommonDefinitions.h"


class ZETextureData;
class ZEJpegChromaUpsampler;
class ZEJpegIDCTProcessor;
class ZEJpegDequantizator;
class ZEJpegHuffmanDecoder;
class ZEJpegFileMarkerReader;
class ZEJpegColorSpaceConverter;
class ZEJpegFileMarkerBuffer;

struct ZEJpegDeCompressionInfo;

class ZEJpegMCUController
{
	private:
		ZEJpegDeCompressionInfo*		Info;

		ZEJpegHuffmanDecoder*			HuffmanDecoder;
		ZEJpegDequantizator*			Dequantizator;
		ZEJpegIDCTProcessor*			IDCTProcessor;
		ZEJpegChromaUpsampler*			ChromaUpsampler;

		ZEUInt							ProcessedMcuCount;

		ZEJpegMcu						McuBuffer;

										ZEJpegMCUController();
										~ZEJpegMCUController();

	public:
		bool							ProcessMCU(ZEUInt McuCoordX, ZEUInt McuCoordY);

		ZEUInt							GetProcessedMcuCount();

		void							Initialize(ZEJpegDeCompressionInfo* Info, ZEJpegFileMarkerReader* MarkerReader);
		void							Deinitialize();

		void							Destroy();
		static ZEJpegMCUController*		CreateInstance();
};


#endif // __ZE_TEXTURE_FILE_JPEG_MCU_CONTROLLER_H__
