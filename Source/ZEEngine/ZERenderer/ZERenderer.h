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
#include "ZEGame/ZEDrawParameters.h"

#define	ZE_COMMAND_BUFFER_SIZE		(1024 * 1024) / 4	// 256KB

typedef ZESmartArray<const ZERenderCommand*>	ZECommandList;

class ZECommandBuffer
{
	private:
		void*						Buffer;
		ZESize						EndOfBuffer;

		ZECommandList				CommandList;

	public:
		void						Clear();
		void						SortCommands();

		bool						IsEmpty() const;
		ZESize						GetCount() const;

		bool						AddCommand(const ZERenderCommand* Entry);
		const ZERenderCommand*		GetCommand(ZESize Index) const;
	
									ZECommandBuffer();
									~ZECommandBuffer();
};

class ZECommandBucket : public ZEListItem
{
	friend class ZERenderer;

	protected:
		ZERenderStageType			TargetStages;
		ZERenderStageData*			StageData;
		ZECommandBuffer				CommandBuffer;

	public:
		void						Clear();

		bool						AddRenderCommand(const ZERenderCommand* Command);

									ZECommandBucket();
									~ZECommandBucket();
};

class ZECamera;
class ZERenderStage;
class ZEDrawParameters;
class ZERenderStageShadow;
class ZERenderStageGeometry;
class ZERenderStageLighting;
class ZERenderStageParticle;
class ZERenderStageAccumulation;

class ZERenderer
{
	protected:
		ZEList<ZECommandBucket>		BucketList;
		ZEList<ZECommandBucket>		EmptyBuckets;

		ZEConstantBuffer*			TimerConstants;

		ZERenderStageShadow*		ShadowStage;
		ZERenderStageGeometry*		GeometryStage;
		ZERenderStageLighting*		LightingStage;
		ZERenderStageParticle*		ParticleStage;
		ZERenderStageAccumulation*	AccumulationStage;

		void						ProcessStage(ZERenderStage* Stage);

									ZERenderer();
		virtual						~ZERenderer();

	public:
		virtual ZECommandBucket*	CreateCommandBucket(ZERenderStageType TargetStages, ZERenderStageData* StageData);

		virtual bool				PreRender();
		virtual bool				PostRender();

		virtual bool				Render(float ElaspedTime);

		virtual bool				Initialize();
		virtual void				Deinitialize();

		virtual void				Destroy();

		static ZERenderer*			CreateInstance();
};

#endif
