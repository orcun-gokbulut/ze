//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDGrid.h
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

#include "ZEGame/ZEEntity.h"

#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNCommand.h"

class ZEGRVertexBuffer;
class ZEGRConstantBuffer;
class ZERNSimpleMaterial;

class ZEDGrid : public ZEEntity
{
	ZE_OBJECT
	private:
		ZERNCommand						RenderCommand;

		ZEHolder<ZEGRVertexBuffer>		VertexBuffer;
		ZEHolder<ZEGRConstantBuffer>	ConstantBufferAxisTransform;
		ZEHolder<ZEGRConstantBuffer>	ConstantBufferMinorGridTransform;
		ZEHolder<ZEGRConstantBuffer>	ConstantBufferMajorGridTransform;
		ZEHolder<ZERNSimpleMaterial>	Material;

		ZESize							MinorGridOffset;
		ZESize							MinorGridCount;

		ZESize							MajorGridOffset;
		ZESize							MajorGridCount;

		ZEVector2						GridSize;
		bool							FollowerGrid;

		bool							MinorGridEnabled;
		ZEVector2						MinorGridUnitSize;
		ZEVector3						MinorGridColor;

		bool							MajorGridEnabled;
		ZEVector2						MajorGridUnitSize;
		ZEVector3						MajorGridColor;

		bool							AxisEnabled;
		ZEVector3						AxisColor;

		struct
		{
			ZEMatrix4x4					AxisTransform;
			ZEMatrix4x4					MinorGridTransform;
			ZEMatrix4x4					MajorGridTransform;
		} Constants;

		void							GenerateGrid();

		virtual ZEEntityResult			LoadInternal();
		virtual ZEEntityResult			UnloadInternal();

										ZEDGrid();

	public:
		void							SetGridSize(const ZEVector2& Size);
		const ZEVector2&				GetGridSize();

		void							SetMinorGridEnabled(bool Enable);
		bool							GetMinorGridEnabled();
		void							SetMinorGridUnitSize(const ZEVector2& Size);
		const ZEVector2&				GetMinorGridUnitSize();
		void							SetMinorGridColor(const ZEVector3& Color);
		const ZEVector3&				GetMinorGridColor();

		void							SetMajorGridEnabled(bool Enabled);
		bool							GetMajorGridEnabled();
		void							SetMajorGridUnitSize(const ZEVector2& Size);
		const ZEVector2&				GetMajorGridUnitSize();
		void							SetMajorGridColor(const ZEVector3& Color);
		const ZEVector3&				GetMajorGridColor();

		void							SetAxisEnabled(bool Enabled);
		bool							GetAxisEnabled();
		void							SetAxisColor(const ZEVector3& Color);
		const ZEVector3&				GetAxisColor();
		
		virtual	bool					PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void					Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);
		
		static ZEDGrid*					CreateInstance();
};
