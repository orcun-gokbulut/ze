//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRStructuredBuffer.h
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

#include "ZEGRDefinitions.h"
#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"

class ZEGRStructuredBuffer : public ZEGRResource
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEGRStructuredBuffer)
	friend class ZEGRContext;
	private:
		struct BoundStage
		{
			bool								BoundAsShaderResource;
			bool								BoundAsUnorderedAccess;
			ZEInt								Slot;
		};

		ZEArray<BoundStage>						BoundStages;

		void									SetBoundStage(ZEGRShaderType Shader, ZEInt Slot, bool BoundAsShaderResource = true, bool BoundAsUnorderedAccess = false);
		const ZEArray<BoundStage>&				GetBoundStages() const;

	protected:
		virtual bool							Initialize(ZESize ElementCount, ZESize ElementSize, ZEGRResourceUsage Usage, ZEFlags BindFlags);
		virtual void							Deinitialize();

												ZEGRStructuredBuffer();
		virtual									~ZEGRStructuredBuffer();

	public:
		ZEGRResourceType						GetResourceType() const;

		void									SetData(void* ConstantData);

		virtual bool							Lock(void** Buffer) = 0;
		virtual void							Unlock() = 0;

		static ZEHolder<ZEGRStructuredBuffer>	Create(ZESize ElementCount, ZESize ElementSize, ZEGRResourceUsage Usage = ZEGR_RU_GPU_READ_CPU_WRITE, ZEFlags BindFlags = ZEGR_RBF_SHADER_RESOURCE);
};
