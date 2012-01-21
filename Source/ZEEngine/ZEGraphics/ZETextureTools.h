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

#include "ZETextureOptions.h"
#include "ZEGraphicsModule.h"
#include "ZETextureResource.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZEGraphics/ZETextureLoader.h"

class ZETexture2D;

class ZETextureTools
{
	private:
									ZETextureTools();
	virtual							~ZETextureTools();

	public:
		static bool					IsResizeable(ZEUInt Width, ZEUInt Height);
		static bool					IsCompressible(ZEUInt Width, ZEUInt Height);
		static ZEUInt			GetMaxMipmapCount(ZEUInt Width, ZEUInt Height);

		static void					CopyTextureRegion(void* DestData, ZEUInt DestPitch,
													  ZEUInt DestX, ZEUInt DestY,
													  void* SourceData, ZEUInt SourcePitch, ZEUInt SourceBitsPP,
													  ZEUInt SourceX, ZEUInt SourceY, 
													  ZEUInt CopyWidth, ZEUInt CopyHeight);

		static void					CopyTexture(void* DestData, ZEUInt DestPitch,
												void* SourceData, ZEUInt SourcePitch,
												ZEUInt RowSize, ZEUInt RowCount);

		static void					CompressTexture(void* DestinationData, ZEUInt DestinationPitch, 
													void* SourceData, ZEUInt SourcePitch, 
													ZEUInt SourceWidth, ZEUInt SourceHeight, 
													const ZETextureOptions* CompressionOptions = NULL);

		static void					DownSample2x(void* DestinationData, ZEUInt DestinationPitch, 
												 void* SourceData, ZEUInt SourcePitch,
												 ZEUInt SourceWidth, ZEUInt SourceHeight, bool UseGpu = true);
		
		

};/* class ZETextureTools */

#endif /* __ZETEXTURETOOLS_H__ */
