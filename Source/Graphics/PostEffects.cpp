//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PostEffects.cpp
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

#include "PostEffects.h"
#include "Core/Error.h"

ZEPostEffect::ZEPostEffect()
{

}

ZEPostEffect::~ZEPostEffect()
{

}

void ZEPostEffect::Destroy()
{
	Deinitialize();
	delete this;
}

void ZEBlurPostEffect::UpdateKernel()
{
	if (KernelDirtyFlag)
	{
		Kernel.SetCount(KernelSize);
		for (size_t I = 0; I < KernelSize; I++)
			Kernel[I] = (1.0f / (sqrtf(2.0f * ZE_PI * StandartDeviation))) * powf(ZE_E, -((I * I) / (2 * StandartDeviation * StandartDeviation)));
	}
}

ZEBlurPostEffect::ZEBlurPostEffect()
{
	KernelDirtyFlag = true;
	Input = NULL;
	Output = NULL;

	HorizontalPass = true;
	VerticalPass = true;
	KernelSize = 5;
	StandartDeviation = 0.84089642f;		
	KernelDirtyFlag = true;
}

ZEBlurPostEffect::~ZEBlurPostEffect()
{
}

size_t ZEBlurPostEffect::GetDependencyCount()
{
	return 1;
}

ZEPostEffect** ZEBlurPostEffect::GetDependencies()
{
	return &Input;
}

void ZEBlurPostEffect::SetInput(ZETexture2D* Texture)
{
	InputTexture = Texture;
}

void ZEBlurPostEffect::SetInput(ZEPostEffect* Input)
{
	this->Input = Input;
}

ZEPostEffect* ZEBlurPostEffect::GetInput()
{
	return Input;
}

ZETexture2D* ZEBlurPostEffect::GetOutput()
{
	return Output;
}

void ZEBlurPostEffect::SetOutputToFrameBuffer(bool Enabled)
{
	OutputToFrameBuffer = Enabled;
}

bool ZEBlurPostEffect::GetOutputToFrameBuffer()
{
	return OutputToFrameBuffer;	
}

void ZEBlurPostEffect::SetHorizontalPass(bool Enable)
{
	HorizontalPass = Enable;
}

bool ZEBlurPostEffect::GetHorizontalPass()
{
	return HorizontalPass;
}

void ZEBlurPostEffect::SetVerticalPass(bool Enable)
{
	VerticalPass = Enable;
}

bool ZEBlurPostEffect::GetVerticalPass()
{
	return VerticalPass;
}

void ZEBlurPostEffect::SetStandartDeviation(float Ro)
{
	StandartDeviation = Ro;
	KernelDirtyFlag = true;
}

float ZEBlurPostEffect::GetStandartDeviation()
{
	return StandartDeviation;
}

void ZEBlurPostEffect::SetKernelSize(unsigned int Size)
{
	if (Size != KernelSize)
	{
		if (KernelSize > 16)
		{
			zeWarning("Blur Post Effect", "Kernel size is too big.");
			KernelSize = 16;
		}
		else
			KernelSize = Size;

		KernelDirtyFlag = true;
	}
}

unsigned int ZEBlurPostEffect::GetKernelSize()
{
	return KernelSize;
}
