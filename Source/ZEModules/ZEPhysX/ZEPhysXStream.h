//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXStream.h
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
#ifndef __ZE_PHYSX_STREAM_H__
#define __ZE_PHYSX_STREAM_H__

#include <NxSimpleTypes.h>
#include <NxStream.h>
#include <Nxf.h>

#include "ZETypes.h"

class ZEPhysXFileStream : public NxStream
{
	private:
		void*					File;

	public:
								ZEPhysXFileStream();
		virtual					~ZEPhysXFileStream();
		
		bool					OpenFile(const char* FileName, bool ReadMode);
		void					CloseFile();

		virtual NxU8			readByte() const;
		virtual NxU16			readWord() const;
		virtual NxU32			readDword() const;
		virtual float			readFloat() const;
		virtual double			readDouble() const;
		virtual void			readBuffer(void* buffer, NxU32 size) const;

		virtual NxStream&		storeByte(NxU8 b);
		virtual NxStream&		storeWord(NxU16 w);
		virtual NxStream&		storeDword(NxU32 d);
		virtual NxStream&		storeFloat(NxReal f);
		virtual NxStream&		storeDouble(NxF64 f);
		virtual NxStream&		storeBuffer(const void* buffer, NxU32 size);


};

class ZEPhysXMemoryWriteStream : public NxStream
{
	private:
		NxU32					CurrentSize;
		NxU32					MaxSize;
		NxU8*					Data;

	public:
								ZEPhysXMemoryWriteStream();
		virtual					~ZEPhysXMemoryWriteStream();

		const NxU8*				GetData() const;
		const ZESize			GetDataSize() const;

		void					Clear();

		virtual NxU8			readByte() const;
		virtual NxU16			readWord() const;
		virtual NxU32			readDword() const;
		virtual float			readFloat() const;
		virtual double			readDouble() const;
		virtual void			readBuffer(void* buffer, NxU32 size) const;

		virtual NxStream&		storeByte(NxU8 b);
		virtual NxStream&		storeWord(NxU16 w);
		virtual NxStream&		storeDword(NxU32 d);
		virtual NxStream&		storeFloat(NxReal f);
		virtual NxStream&		storeDouble(NxF64 f);
		virtual NxStream&		storeBuffer(const void* buffer, NxU32 size);

};

class ZEPhysXMemoryReadStream : public NxStream
{
	private:
		mutable	const NxU8*		Buffer;

	public:
								ZEPhysXMemoryReadStream();
		virtual					~ZEPhysXMemoryReadStream();

		void					SetData(const NxU8* Data);
		const NxU8*				GetData() const;

		virtual NxU8			readByte() const;
		virtual NxU16			readWord() const;
		virtual NxU32			readDword() const;
		virtual float			readFloat() const;
		virtual double			readDouble() const;
		virtual void			readBuffer(void* buffer, NxU32 size)	const;

		virtual NxStream&		storeByte(NxU8 b);
		virtual NxStream&		storeWord(NxU16 w);
		virtual NxStream&		storeDword(NxU32 d);
		virtual NxStream&		storeFloat(NxReal f);
		virtual NxStream&		storeDouble(NxF64 f);
		virtual NxStream&		storeBuffer(const void* buffer, NxU32 size);
};

#endif
