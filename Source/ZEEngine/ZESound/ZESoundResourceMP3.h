//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundResourceMP3.h
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

#include "ZETypes.h"
#include "ZESoundResource.h"

struct mpg123_handle_struct;

class ZESoundResourceMP3 : public ZESoundResource
{
	friend class ZESoundResource;
	private:
		unsigned char*								Data;
		ZESize										DataSize;


		int											Index;
		mpg123_handle_struct*						mpg123;
		ZESize										MemoryCursor;
		mutable ZELock								DecodeLock;

		ZETaskResult								LoadInternal();
		ZETaskResult								UnloadInternal();

													ZESoundResourceMP3();
		virtual										~ZESoundResourceMP3();

		static volatile int							LastResourceIndex;
		static ZESmartArray<ZESoundResourceMP3*>	IndexedResources;

		static int									CreateResourceIndex(ZESoundResourceMP3* Resource);
		static void									RemoveResourceIndex(ZESoundResourceMP3* Resource);
		static ZESoundResourceMP3*					IndexResource(int Index);

		static long									MP3Read(ZEInt fd, void *buffer, ZESize nbytes);
		static long									MP3Seek(ZEInt fd, long offset, ZEInt whence);

	public:
		virtual ZESize								GetDataSize() const;		
		virtual const void*							GetData() const;

		virtual bool								Decode(void* DestinationBuffer, ZESize SampleIndex, ZESize SampleCount) const;
};
