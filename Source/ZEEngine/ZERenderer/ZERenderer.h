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
#include "ZEDS/ZEArray.h"
#include "ZEGame/ZEDrawParameters.h"

class ZELight;
class ZECamera;
class ZERenderCommand;
struct ZEDrawParameters;
class ZERenderStageShadow;
class ZERenderStageForward;
class ZERenderStageParticle;
class ZERenderStageGeometry;
class ZERenderStageLighting;
class ZERenderStageTransparent;
class ZERenderStagePostProcess;
class ZERenderStageUserInterface;

#define	ZE_COMMAND_BUFFER_SIZE					1024 * 1024	// 1 MB

#define ZE_CMB_COMMAND							0xFFFFFFFF
#define ZE_CBM_RENDER_START						0x01
#define ZE_CBM_RENDER_END						0x02
#define ZE_CBM_FRAME_START						0x03
#define ZE_CBM_FRAME_END						0x04
#define ZE_CBM_FRAME_RENDER_COMMAND_START		0x05
#define ZE_CBM_FRAME_RENDER_COMMAND_END			0x06
#define ZE_CBM_FRAME_RENDER_COMMAND_TYPE		0x07
#define ZE_CBM_FRAME_RENDER_COMMAND_DATA		0x08

/*
class ZECommandBuffer
{
	private:
		ZESize					BufferPointer;
		void*					CommandBuffer;
		ZESize					RenderEndMarker;
		ZESize					RenderStartMarker;
	
	public:
		bool					CheckBufferFull(ZESize Size) const;
		
		bool					ReadCommand(ZEUInt32* Marker, void* Data, ZESize* Size);
		bool					WriteCommand(ZEUInt32 Marker, const void* Data, ZESize Size);
		
								ZECommandBuffer();
								~ZECommandBuffer();
};
*/

class ZERenderer
{
	protected:
		/*ZECommandBuffer					CommandBuffer;*/

		ZERenderStageGeometry*				GeometryStage;
		ZERenderStageShadow*				ShadowStage;
		ZERenderStageLighting*				LightingStage;
		ZERenderStageForward*				ForwardStage;
		ZERenderStageTransparent*			TransparentStage;
		ZERenderStagePostProcess*			PostProcessStage;
		ZERenderStageUserInterface*			UserInterfaceStage;
		ZERenderStageParticle*				ParticleStage;

		ZEDrawParameters					DrawParameters;
		ZESmartArray<ZERenderCommand*>		CommandList;

		ZEVector2							ShadowMapDimesion;

											ZERenderer();
		virtual								~ZERenderer();

		void								DoShadowPass();
		void								DoGeomtryPass();
		void								DoForwardPass();
		void								DoLightingPass();
		void								DoParticlePass();
		void								DoTransparentPass();
		void								DoPostProcessPass();
		void								DoUserInterfacePass();

	public:
		virtual void						AddRenderCommand(ZERenderCommand* RenderCommand);

		void								SetDrawParameters(const ZEDrawParameters& Parameters);
		const ZEDrawParameters&				GetDrawParameters() const;

		void								SetShadowMapDimension(ZEVector2 Value);
		ZEVector2							GetShadowMapDimension() const;

		virtual void						Render(float ElaspedTime);

		virtual bool						Initialize();
		virtual void						Deinitialize();

		virtual void						Destroy();
		static ZERenderer*					CreateInstance();
};

#endif
