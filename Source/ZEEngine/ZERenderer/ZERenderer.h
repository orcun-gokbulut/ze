//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderer.h
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
#ifndef __ZE_RENDERER_H__
#define __ZE_RENDERER_H__

#include "ZETypes.h"
#include "ZEDS/ZEList.h"
#include "ZEDS/ZEArray.h"
#include "ZERenderStage.h"
#include "ZEMath/ZEVector.h"

#define ZE_MAX_STAGE_COUNT			32
#define	ZE_COMMAND_BUFFER_SIZE		1024 * 1024	* 1	// 1 MB

typedef ZESmartArray<const ZERenderCommand*>	ZECommandList;

class ZECommandBuffer
{
	private:
		void*					Buffer;
		ZESize					EndOfBuffer;

		ZECommandList			CommandList;

		void					SortCommands(ZESize StartIndex, ZESize EndIndex);

	public:
		void					Clear();
		void					SortFrames();

		bool					IsEmpty() const;
		ZESize					GetCount() const;

		bool					AddCommand(const ZERenderCommand* Entry);
		const ZERenderCommand*	GetCommand(ZESize Index) const;
	
								ZECommandBuffer();
								~ZECommandBuffer();
};

class ZECommandBucket : public ZEListItem
{
	public:
		ZEDrawParameters		Parameters;
		ZECommandBuffer			Commands;

		void					Clear();

								ZECommandBucket();
								~ZECommandBucket();
};

class ZECamera;
class ZERenderStage;
class ZEDrawParameters;
class ZERenderStageShadow;
class ZERenderStageAccumulation;
class ZERenderStageGeometry;
class ZERenderStageLighting;
class ZERenderStageParticle;

class ZERenderer
{
	protected:
		ZEList<ZECommandBucket>		Buckets;

		ZERenderStageShadow*		ShadowStage;
		ZERenderStageGeometry*		GeometryStage;
		ZERenderStageLighting*		LightingStage;
		ZERenderStageParticle*		ParticleStage;
		ZERenderStageAccumulation*	AccumulationStage;

									ZERenderer();
		virtual						~ZERenderer();

		void						ProcessStage(ZERenderStage* Stage);

	public:
		virtual ZECommandBucket*	CreateCommandBucket(const ZEDrawParameters* Parameters);

		virtual bool				PreRender();
		virtual bool				PostRender();

		virtual bool				Render(float ElaspedTime);

		virtual bool				Initialize();
		virtual void				Deinitialize();

		virtual void				Destroy();

		static ZERenderer*			CreateInstance();
};

#endif
