//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureTools.h
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 YiÄŸit OrÃ§un GÃ–KBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "YiÄŸit  OrÃ§un  GÃ–KBULUT") and
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
  Name: YiÄŸit OrÃ§un GÃ–KBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#ifndef __ZETEXTURETOOLS_H__
#define __ZETEXTURETOOLS_H__
#pragma once


#include "ZETextureOptions.h"
#include "ZEGraphicsModule.h"
#include "ZETextureResource.h"


class ZETexture2D;
class ZEResourceFile;

/* fonksyon parametre yazýmý deiþecek!! */
class ZETextureTools
{
	protected:
		/* Empty */

	private:
		static bool			CheckInFileCache(const char			*FileName);

		static unsigned int	GetMaxMipmapCount(unsigned int		Width, 
											  unsigned int		Height);



		/* For now only works for texture2d resources */
		/* ilgili texture resource un içine taþýnacak */
		bool			CreateMipmaps(ZETexture2DResource*		TextureResource,
									  unsigned char*			Image, 
									  unsigned int				Width, 
									  unsigned int				Height, 
									  unsigned int				BPP, 
									  unsigned int				Pitch, 
									  bool						IsResizeable, 
									  const ZETextureOptions*	MipmapOptions = NULL);

		static bool		CompressTexture(void*					DestinationAdress, 
										unsigned int			DestinationPitch, 
										void*					SourceData, 
										unsigned int			SourcePitch, 
										unsigned int			SourceWidth, 
										unsigned int			SourceHeight, 
										const ZETextureOptions* CompressionOptions = NULL);

		static bool		DownSample2x(void*					DestinationData,
								     unsigned int			DestinationPitch,
								     void*					SourceData,
								     unsigned int			SourcePitch,
								     unsigned int			SourceWidth,
								     unsigned int			SourceHeight);

	public:
						ZETextureTools(void);
		virtual			~ZETextureTools(void);
		
		/* For now only works for texture2d resources */
		ZETexture2DResource*		LoadFromFileCache(const char	*FileName);

		ZETexture2DResource*		LoadFromOriginalFile(ZEResourceFile*			ResourceFile, 
														 const ZETextureOptions*	UserOptions = NULL);

		bool						SaveToFileCache(const char*			DestinationFile,
													const ZEBYTE*		SourceData, 
													unsigned int		SourceWidth, 
													unsigned int		SourceHeight, 
													unsigned int		SourceBPP, 
													unsigned int		SourcePitch);
		

};/* class ZETextureTools */

#endif /* __ZETEXTURETOOLS_H__ */
