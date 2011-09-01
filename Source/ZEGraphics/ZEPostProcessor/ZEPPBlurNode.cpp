//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPPBlurNode.cpp
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

#include "ZEPPBlurNode.h"
#include "ZECore/ZEError.h"
#include "ZEMath/ZEMathDefinitions.h"

void ZEPPBlurNode::UpdateKernel()
{
	if (KernelDirtyFlag)
	{
		for (size_t I = 0; I <= 7; I++)
			Kernel[I] = (1.0f / (sqrtf(2.0f * ZE_PI * StandartDeviation))) * powf(ZE_E, -((((float)I - 3) * ((float)I - 3)) / (2 * StandartDeviation * StandartDeviation)));

		KernelDirtyFlag = false;
	}
}

ZEPPBlurNode::ZEPPBlurNode()
{
	KernelDirtyFlag = true;
	Input = NULL;

	HorizontalPass = true;
	VerticalPass = true;
	StandartDeviation = 0.84089642f;		
	KernelDirtyFlag = true;
	PassCount = 1;
	DownSample = 1.0f;
}

ZEPPBlurNode::~ZEPPBlurNode()
{
}

ZEPostProcessorNodeType ZEPPBlurNode::GetNodeType()
{
	return ZE_PPNT_PROCESSOR_NODE;
}

size_t ZEPPBlurNode::GetDependencyCount()
{
	return 1;
}

ZEPostProcessorNode** ZEPPBlurNode::GetDependencies()
{
	return &Input;
}

void ZEPPBlurNode::SetInput(ZEPostProcessorNode* Input)
{
	this->Input = Input;
}

ZEPostProcessorNode* ZEPPBlurNode::GetInput()
{
	return Input;
}

void ZEPPBlurNode::SetDownSample(unsigned int Factor)
{
	DownSample = Factor;
}

unsigned int ZEPPBlurNode::GetDownSample()
{
	return DownSample;
}

void ZEPPBlurNode::SetPassCount(unsigned int PassCount)
{
	this->PassCount = PassCount;
}

unsigned int ZEPPBlurNode::GetPassCount()
{
	return PassCount;
}

void ZEPPBlurNode::SetHorizontalPass(bool Enable)
{
	HorizontalPass = Enable;
}

bool ZEPPBlurNode::GetHorizontalPass()
{
	return HorizontalPass;
}

void ZEPPBlurNode::SetVerticalPass(bool Enable)
{
	VerticalPass = Enable;
}

bool ZEPPBlurNode::GetVerticalPass()
{
	return VerticalPass;
}

void ZEPPBlurNode::SetStandartDeviation(float Ro)
{
	StandartDeviation = Ro;
	KernelDirtyFlag = true;
}

float ZEPPBlurNode::GetStandartDeviation()
{
	return StandartDeviation;
}




