//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNRenderer.h
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
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEGame/ZEDrawStatistics.h"
#include "ZEInitializable.h"
#include "ZEGraphics/ZEGRHolder.h"
#include "ZEGraphics/ZEGRRenderState.h"

struct ZEViewPoint
{
	float								FOV;
	float								NearZ;
	float								FarZ;
	ZEVector3							ViewPosition;
	ZEMatrix4x4							ViewMatrix;
	ZEMatrix4x4							ProjMatrix;
	ZEMatrix4x4							ViewProjMatrix;
};

class ZECamera;

enum ZERendererType
{
	ZE_RT_FRAME,
	ZE_RT_SHADOW
};

#define ZERN_CONSTANTS_PIPELINE				0
#define ZERN_CONSTANTS_RENDERER				1
#define ZERN_CONSTANTS_SCENE				2
#define ZERN_CONSTANTS_MATERIAL_GLOBAL		3
#define ZERN_CONSTANTS_MATERIAL				4
#define ZERN_CONSTANTS_INSTANCE				5

class ZEGRContext;
class ZERNStage;
class ZELight;
class ZERNCommand;
class ZEGRRenderState;
struct ZEDrawParameters;

class ZERNRenderer : public ZEInitializable
{
	private:
		ZEGRContext*					Device;
		
		ZESmartArray<ZERNCommand*>		Commands;
		ZEArray<ZERNStage*>				Stages;
		ZEArray<ZELight*>				Lights;
		ZERendererStatistics			Statistics;
		ZEGRRenderState					RenderState;

		virtual bool					InitializeSelf();
		virtual void					DeinitializeSelf();

	public:
		ZERendererType					GetRendererType();
		const ZERendererStatistics&		GetStatistics() const;

		void							SetDevice(ZEGRContext* Device);
		ZEGRContext*					GetDevice();

		void							SetDrawParameters(ZEDrawParameters* DrawParameters);
		ZEDrawParameters*				GetDrawParameters();

		void							AddLight(ZELight* Light);
		void							ClearLights();

		void							AddCommand(ZERNCommand* Command);
		void							ClearCommands();

		void							Render(float ElaspedTime = 0);
		void							Clear();

										ZERNRenderer();
										~ZERNRenderer();
};
