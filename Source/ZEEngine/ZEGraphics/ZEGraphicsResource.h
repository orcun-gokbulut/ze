//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsResource.h
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

#ifndef __ZE_GRAPHICS_RESOURCE_H__
#define __ZE_GRAPHICS_RESOURCE_H__

#include "ZEDS/ZEString.h"
#include "ZEGraphicsDefinitions.h"

class ZETextureData;

class ZEShadowCopy
{
	friend class ZERenderer;

	protected:
		ZEUInt			RowCount;
		ZEUInt			SliceCount;
		ZESize			RowSize;
		ZESize			SliceSize;

		ZEUInt			CurrentIndex;
		bool			SynchEnabled;

		void*			Shadows[ZE_MAX_SHADOW_COPY_COUNT];
		bool			Changed[ZE_MAX_SHADOW_COPY_COUNT];

		void			MoveNext();

	public:
						ZEShadowCopy();
		virtual			~ZEShadowCopy();

		void			SetSynchEnabled(bool Enabled);
		bool			GetSynchEnabled() const;

		ZEUInt			GetRowCount() const;
		ZEUInt			GetSliceCount() const;
		ZESize			GetRowSize() const;
		ZESize			GetSliceSize() const;

		ZEUInt			GetCurrentIndex() const;

		void*			GetData();
		const void*		GetConstData() const;

		void			SetChanged(bool Value);
		bool			GetChanged() const;
		
		void*			GetData(ZEUInt Index);
		const void*		GetConstData(ZEUInt Index) const;
		
		void			SetChanged(ZEUInt Index, bool Value);
		bool			GetChanged(ZEUInt Index) const;
		

		bool			CreateBuffer(ZESize RowSize, const void* InitialData = NULL);
		bool			CreateTexture(ZEUInt RowCount, ZESize RowSize, ZEUInt SliceCount, ZESize SliceSize, ZETextureData* InitialData = NULL);
};

enum ZEGraphicsResourceType
{
	ZE_GRT_NONE			= 0,
	ZE_GRT_TEXTURE		= 1,
	ZE_GRT_BUFFER		= 2,
};

class ZEGraphicsResource
{
	friend class ZEGraphicsDevice;
	friend class ZEGraphicsModule;

	protected:
		struct 
		{
			bool						IsCreated;
			bool						IsStatic;
			bool						IsLocked;

			bool						IsOccupied;
			bool						IsDestroyed;
			
		} State;
		
#ifdef ZE_DEBUG_ENABLE
		ZEString						DebugName;
#endif

		virtual bool					UpdateWith(ZEUInt ShadowIndex) = 0;
		
										ZEGraphicsResource();
		virtual 						~ZEGraphicsResource();

	public:
		bool							GetIsCreated() const;
		bool							GetIsStatic() const;
		bool							GetIsLocked() const;
		bool							GetIsDeleted() const;
		
		void							SetDebugName(const char* Name);
		const char*						GetDebugName() const;

		virtual void					Destroy();

		virtual ZEGraphicsResourceType	GetResourceType() const = 0;

};

#endif
