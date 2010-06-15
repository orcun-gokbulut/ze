//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PPFilter1DNode.h
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
#ifndef __ZE_PP_LINEAR_KERNEL_FILTER_NODE_H__
#define __ZE_PP_LINEAR_KERNEL_FILTER_NODE_H__

#include "ZEDS/Array.h"
#include "PostProcessorNode.h"

class ZETexture2D;

struct ZEKernel1DElement
{
	float										SampleMultiplier;
	float										SampleLocation;
	float										Reserved0;
	float										Reserved1;
};

class ZEPPFilter1DNode : public ZEPostProcessorNode
{
	protected:		
		ZEPostProcessorNode*					Input;
		ZETexture2D*							Internal;
		ZETexture2D*							Output;

		bool									HorizontalPass;
		bool									VerticalPass;

		ZEArray<ZEKernel1DElement>				Kernel;

												ZEPPFilter1DNode();
		virtual									~ZEPPFilter1DNode();

	public:	
		virtual size_t							GetDependencyCount();
		virtual ZEPostProcessorNode**			GetDependencies();

		virtual void							SetInput(ZEPostProcessorNode* Node);
		virtual ZEPostProcessorNode*			GetInput();

		void									SetKernelElements(const ZEArray<ZEKernel1DElement>& Values);
		const ZEArray<ZEKernel1DElement>&		GetKernelElements();

		void									SetHorizontalPass(bool Enable);
		bool									GetHorizontalPass();

		void									SetVerticalPass(bool Enable);
		bool									GetVerticalPass();

		virtual ZETexture2D*					GetOutput();
};

#endif
