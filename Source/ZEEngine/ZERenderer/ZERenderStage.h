//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStage.h
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

#ifndef __ZE_RENDER_STAGE_H__
#define __ZE_RENDER_STAGE_H__

#include "ZEDS/ZEArray.h"
#include "ZEGraphics/ZEGraphicsDeviceState.h"

#define ZE_RENDER_STAGE_GEOMETRY		1
#define ZE_RENDER_STAGE_SHADOW			2
#define ZE_RENDER_STAGE_LIGHTING		3
#define ZE_RENDER_STAGE_FORWARD			4
#define ZE_RENDER_STAGE_TRANSPARENT		5
#define ZE_RENDER_STAGE_UI				6

enum ZERenderStageConstant
{
	// Time
	ZE_PC_TIME_0_1,
	ZE_PC_SIN_TIME_0_1,
	ZE_PC_COS_TIME_0_1,
	ZE_PC_TAN_TIME_0_1,
	ZE_PC_TIME_0_N,
	ZE_PC_SIN_TIME_0_N,
	ZE_PC_COS_TIME_0_N,
	ZE_PC_TAN_TIME_0_N,

	// Camera View-port
	ZE_PC_FOV,
	ZE_PC_NEAR_FAR_Z,
	ZE_PC_ACPECT_RATIO,
	ZE_PC_CAMERA_POS,
	ZE_PC_CAMERA_WORLD_UP,
	ZE_PC_CAMERA_WORLD_FRONT,
	ZE_PC_CAMERA_WORLD_RIGHT,
	ZE_PC_VIEWPORT_WIDTH_HEIGHT,
	ZE_PC_INV_VIEWPORT_WIDTH_HEIGHT,

	// Transformation
	ZE_PC_VIEW_MATRIX,
	ZE_PC_PROJ_MATRIX,
	ZE_PC_INV_VIEW_MATRIX,
	ZE_PC_INV_PROJ_MATRIX,
	ZE_PC_VIEW_PROJ_MATRIX,
	ZE_PC_INV_VIEW_PROJ_MATRIX,
};

class ZECamera;
class ZEMaterial;
class ZERenderCommand;
class ZEConstantBuffer;

class ZERenderStage
{
	protected:
		ZEDeviceState					DefaultStates;
		ZEConstantBuffer*				StageBuffer;
		ZEArray<ZERenderStageConstant>	StageConstants;

		void							DestroyBuffers();
		virtual void					UpdateBuffers();
				
		virtual void					ResetStageDefaults();
		virtual void					CommitStageDefaults();

		void							PumpStreams(ZERenderCommand* RenderCommand);

	public:
		const ZEDeviceState*			GetDefaultStates() const;

		virtual void					Destroy();

		virtual void					Setup() = 0;
		virtual void					Process(ZERenderCommand* RenderCommand) = 0;

		virtual ZEUInt32				GetStageFlags() const = 0;

										ZERenderStage();
		virtual							~ZERenderStage();
};

#endif
