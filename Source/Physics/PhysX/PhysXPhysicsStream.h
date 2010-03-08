//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicsStream.h
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
#ifndef	__ZE_PHYSX_PHYSICS_STREAM_H__
#define __ZE_PHYSX_PHYSICS_STREAM_H__

#include "NxStream.h"

class ZEPhysXPhysicsMemoryWriteBuffer : public NxStream
{
public:
						ZEPhysXPhysicsMemoryWriteBuffer();
	virtual				~ZEPhysXPhysicsMemoryWriteBuffer();
	void				clear();

	virtual NxU8		readByte()   const	{ NX_ASSERT(0);	return 0;	}
	virtual NxU16		readWord()   const	{ NX_ASSERT(0);	return 0;	}
	virtual NxU32		readDword()  const	{ NX_ASSERT(0);	return 0;	}
	virtual float		readFloat()  const	{ NX_ASSERT(0);	return 0.0f;}
	virtual double		readDouble() const	{ NX_ASSERT(0);	return 0.0;	}
	virtual void		readBuffer(void* buffer, NxU32 size) const{ NX_ASSERT(0);}

	virtual NxStream&	storeByte(NxU8 b);
	virtual NxStream&	storeWord(NxU16 w);
	virtual NxStream&	storeDword(NxU32 d);
	virtual NxStream&	storeFloat(NxReal f);
	virtual NxStream&	storeDouble(NxF64 f);
	virtual NxStream&	storeBuffer(const void* buffer, NxU32 size);

	NxU32				currentSize;
	NxU32				maxSize;
	NxU8*				data;
};

class ZEPhysXPhysicsMemoryReadBuffer : public NxStream
{
public:
								ZEPhysXPhysicsMemoryReadBuffer(const NxU8* data);
	virtual						~ZEPhysXPhysicsMemoryReadBuffer();

	virtual NxU8				readByte() const;
	virtual NxU16				readWord() const;
	virtual NxU32				readDword() const;
	virtual float				readFloat() const;
	virtual double				readDouble() const;
	virtual void				readBuffer(void* buffer, NxU32 size) const;

	virtual NxStream&			storeByte(NxU8 b) { NX_ASSERT(0);	 return *this; }
	virtual NxStream&			storeWord(NxU16 w) { NX_ASSERT(0);	 return *this; }
	virtual NxStream&			storeDword(NxU32 d) { NX_ASSERT(0);	 return *this; }
	virtual NxStream&			storeFloat(NxReal f) { NX_ASSERT(0); return *this; }
	virtual NxStream&			storeDouble(NxF64 f) { NX_ASSERT(0); return *this; }
	virtual NxStream&			storeBuffer(const void* buffer, NxU32 size) { NX_ASSERT(0);	return *this; }

	mutable const NxU8*			buffer;
};

#endif
