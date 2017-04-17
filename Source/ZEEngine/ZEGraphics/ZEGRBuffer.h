//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRBuffer.h
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

#include "ZEGRResource.h"

#include "ZEPointer/ZEHolder.h"
#include "ZEExport.ZEEngine.h"

ZE_EXPORT_ZEENGINE ZE_ENUM_TYPED(ZEGRBufferType, ZEUInt8)
{
	ZEGR_BT_NONE,
	ZEGR_BT_VERTEX_BUFFER,
	ZEGR_BT_INDEX_BUFFER,
	ZEGR_BT_CONSTANT_BUFFER,
	ZEGR_BT_STRUCTURED_BUFFER,
	ZEGR_BT_RAW_BUFFER,
	ZEGR_BT_BUFFER
};

class ZE_EXPORT_ZEENGINE ZEGRBuffer : public ZEGRResource
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEGRBuffer)
	protected:
		ZESize								SizeInBytes;
		ZESize								StrideInBytes;
		ZEUInt								ElementCount;
		ZEGRBufferType						Type;

		virtual bool						Initialize(ZEGRBufferType BufferType, ZESize SizeInBytes, ZESize StrideInBytes, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEGRFormat Format, const void* Data);
		virtual void						Deinitialize();

											ZEGRBuffer();
		virtual								~ZEGRBuffer();

	public:
		ZESize								GetSizeInBytes() const;
		ZESize								GetStrideInBytes() const;
		ZEUInt								GetElementCount() const;

		ZEGRBufferType						GetBufferType() const;
		virtual ZEGRResourceType			GetResourceType() const;

		void								SetData(const void* Data, ZESize DataSize = 0);

		virtual bool						Map(ZEGRResourceMapType MapType, void** Buffer, ZESize* SizeInBytes = NULL) = 0;
		virtual void						Unmap() = 0;
		
		virtual void						Update(const void* SrcData, ZESize SrcSizeInBytes, ZEUInt DestOffsetInBytes = 0, ZESize DestSizeInBytes = 0) = 0;

		static ZEHolder<ZEGRBuffer>			CreateResource(ZEGRBufferType BufferType, ZESize SizeInBytes, ZESize StrideInBytes, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEGRFormat Format = ZEGR_TF_NONE, const void* Data = NULL);
		static ZEHolder<const ZEGRBuffer>	CreateResourceShared(const ZEGUID& GUID, ZEGRBufferType BufferType, ZESize SizeInBytes, ZESize StrideInBytes, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEGRFormat Format = ZEGR_TF_NONE, const void* Data = NULL, ZEGRBuffer** StagingResource = NULL);
};
