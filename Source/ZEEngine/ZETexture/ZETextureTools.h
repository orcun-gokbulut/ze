//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureTools.h
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
#ifndef __ZE_TEXTURE_TOOLS_H__
#define __ZE_TEXTURE_TOOLS_H__

#include "ZETypes.h"
#include "ZETexture/ZETextureOptions.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZETexture/ZETextureResource.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureLoader.h"

class ZEGRTexture2D;

class ZETextureTools
{
	private:
									ZETextureTools();
		virtual						~ZETextureTools();

	public:
		static bool					IsCompressible(const ZEUInt Width, const ZEUInt Height, const ZEUInt HorizTileCount = 1, const ZEUInt VertTileCount = 1);
		static bool					IsResizeable(const ZEUInt Width, const ZEUInt Height, const ZEUInt HorizTileCount = 1, const ZEUInt VertTileCount = 1, const ZEGRTextureType TextureType = ZEGR_TT_2D);
		static ZEUInt				GetMaxMipmapCount(const ZEUInt Width, const ZEUInt Height, const ZEUInt HorizTileCount = 1, const ZEUInt VertTileCount = 1, const ZEGRTextureType TextureType = ZEGR_TT_2D);

		static void					CompressTexture(void* DestinationData, const ZESize DestinationPitch, const void* SourceData, const ZESize SourcePitch, 
													const ZEUInt SourceWidth, const ZEUInt SourceHeight, const ZETextureOptions* CompressionOptions = NULL);

		static void					DownSample2x(	void* DestinationData, const ZESize DestinationPitch, const void* SourceData, const ZESize SourcePitch,
													const ZEUInt SourceWidth, const ZEUInt SourceHeight, bool UseGpu = true);

		static void					Average(void* DestinationData, const ZESize DestinationPitch, const void* SourceData1, const ZESize SourcePitch1,
											const ZEUInt SourceWidth1, const ZEUInt SourceHeight1, const void* SourceData2, const ZESize SourcePitch2,
											const ZEUInt SourceWidth2, const ZEUInt SourceHeight2);
};

#endif
