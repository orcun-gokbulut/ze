//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PostEffects.h
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
#ifndef __ZE_POST_EFFECTS_H__
#define __ZE_POST_EFFECTS_H__

#include "ZEDS/Array.h"

class ZETexture2D;

class ZEPostEffect
{
	protected:
										ZEPostEffect();
		virtual							~ZEPostEffect();

	public:
		virtual size_t					GetDependencyCount() = 0;
		virtual ZEPostEffect*			GetDependencies() = 0;


		virtual bool					SetInput(ZETexture2D* Texture) = 0;
		virtual bool					SetInput(ZEPostEffect* Input) = 0;
		virtual ZEPostEffect*			GetInput() = 0;

		virtual void					SetOutputToFrameBuffer(bool Enabled) = 0;
		virtual bool					GetOutputToFrameBuffer() = 0;

		virtual ZETexture2D*			GetOutput() = 0;

		virtual bool					Initialize() = 0;
		virtual void					Deinitialize() = 0;

		virtual void					Destroy();

		virtual bool					Process() = 0;
};

class ZEBlurPostEffect
{
	protected:		
		ZEPostEffect*					Input;
		ZETexture2D*					InputTexture;

		ZETexture2D*					Output;
		bool							OutputToFrameBuffer;

		bool							HorizontalPass;
		bool							VerticalPass;
		float							StandartDeviation;	

		ZEArray<float>					Kernel;
		int								KernelSize;
		bool							KernelDirtyFlag;

		void							UpdateKernel();

										ZEBlurPostEffect();
		virtual							~ZEBlurPostEffect();

	public:	
		virtual size_t					GetDependencyCount();
		virtual ZEPostEffect**			GetDependencies();

		virtual void					SetInput(ZETexture2D* Texture);
		virtual void					SetInput(ZEPostEffect* Input);
		virtual ZEPostEffect*			GetInput();

		virtual ZETexture2D*			GetOutput();
		
		void							SetOutputToFrameBuffer(bool Enabled);
		bool							GetOutputToFrameBuffer();

		void							SetHorizontalPass(bool Enable);
		bool							GetHorizontalPass();

		void							SetVerticalPass(bool Enable);
		bool							GetVerticalPass();

		void							SetStandartDeviation(float Ro);
		float							GetStandartDeviation();

		void							SetKernelSize(unsigned int Size);
		unsigned int					GetKernelSize();
};

#endif
