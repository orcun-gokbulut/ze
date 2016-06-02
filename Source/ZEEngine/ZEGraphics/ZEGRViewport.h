//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRViewport.h
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

#include "ZEMeta\ZEObject.h"
#include "ZETypes.h"
#include "ZEMath\ZEVector.h"

class ZEGRViewport : public ZEObject
{
	ZE_OBJECT
	public:
		struct ZEViewportData
		{
			ZEVector2			Position;
			ZEVector2			Size;
			float				MinDepth;
			float				MaxDepth;

		} StateData;

	public:
		void					SetPosition(const ZEVector2& Position);
		const ZEVector2&		GetPosition();

		void					SetSize(const ZEVector2& Size);
		const ZEVector2&		GetSize();

		void					SetX(float x);
		float					GetX() const;

		void					SetY(float y);
		float					GetY() const;

		void					SetWidth(float Width);
		float					GetWidth() const;

		void					SetHeight(float Height);
		float					GetHeight() const;

		void					SetMinDepth(float Depth);
		float					GetMinDepth() const;

		void					SetMaxDepth(float Depth);
		float					GetMaxDepth() const;

								ZEGRViewport(const ZEVector2& Position, const ZEVector2& Size, float MinDepth = 0.0f, float MaxDepth = 1.0f);
								ZEGRViewport(float X, float Y, float Width, float Height, float MinDepth = 0.0f, float MaxDepth = 1.0f);
								ZEGRViewport();
};
