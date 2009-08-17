//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PostProcessor.h
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
#ifndef __ZE_POSTPROCESSOR_H__
#define __ZE_POSTPROCESSOR_H__

#include "Graphics/TextureResource.h"
#include "ZEMath/Matrix.h"

enum ZEPostProcessorDestination
{
	ZE_PPD_INTERNAL			= 0,
	ZE_PPD_OUTPUT			= 1,
	ZE_PPD_FRAMEBUFFER		= 2
};

enum ZEPostProcessorSource
{
	ZE_PPS_INTERNAL			= 0,
	ZE_PPS_INPUT			= 1,
};

class ZEPostProcessor
{
	public:
		virtual void					SetInput(ZETexture2D* Texture) = 0;
		virtual void					SetOutput(ZETexture2D* Texture) = 0;

		virtual void					DirectOutput(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;

		virtual void					ApplyUpSample4x(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
		virtual void					ApplyDownSample4x(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;

		virtual void					ApplyColorTransform(ZEPostProcessorSource Source, ZEMatrix3x3 Matrix, ZEPostProcessorDestination Destination) = 0;
		virtual void					ApplyInverse(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
		virtual void					ApplyGrayscale(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;

		virtual void					ApplyBlurH(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
		virtual void					ApplyBlurV(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;

		virtual void					ApplySharpen(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;

		virtual void					ApplyEdgeDetectionH(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
		virtual void					ApplyEdgeDetectionV(ZEPostProcessorSource Source, ZEPostProcessorDestination Destination) = 0;
};

#endif
