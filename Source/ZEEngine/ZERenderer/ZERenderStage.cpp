//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStage.cpp
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

#include "ZEError.h"
#include "ZEMaterial.h"
#include "ZERenderStage.h"
#include "ZERenderCommand.h"
#include "ZEGraphics/ZEIndexBuffer.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEgraphics/ZEGraphicsDevice.h"

static ZEUInt GetVertexCount(ZEPrimitiveType Type, ZEUInt32 Count)
{
	ZEUInt VertexCount = 0;
	switch (Type)
	{
		default:
		case ZE_PT_NONE:
			zeWarning("Unknown primitive type in render command.");
			break;
		case ZE_PT_POINT_LIST:
			VertexCount = Count;
			break;
		case ZE_PT_LINE_LIST:
			VertexCount = Count * 2;
			break;
		case ZE_PT_TRIANGLE_LIST:
			VertexCount = Count * 3;
			break;
		case ZE_PT_TRIANGLE_STRIPT:
			VertexCount = Count + 2;
			break;
	}
	return VertexCount;
}

void ZERenderStage::PumpStreams(const ZERenderCommand* RenderCommand)
{
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	switch (RenderCommand->Type)
	{
		default:
		case ZE_RCT_NONE:
		{
			zeWarning("Unknown render command type, possibly corrupted.");
			break;
		}
		case ZE_RCT_DEFAULT:
		{
			ZERenderCommandDefault* Command = (ZERenderCommandDefault*)RenderCommand;
			Device->SetVertexBufferArray(Command->VertexBuffers);
			Device->SetVertexLayout(Command->VertexLayout);

			Device->Draw(Command->PrimitiveType,
						GetVertexCount(Command->PrimitiveType, Command->PrimitiveCount),
						Command->FirstVertex);

			break;
		}
		case ZE_RCT_INDEXED:
		{
			ZERenderCommandIndexed* Command = (ZERenderCommandIndexed*)RenderCommand;
			Device->SetVertexBufferArray(Command->VertexBuffers);
			Device->SetVertexLayout(Command->VertexLayout);
			Device->SetIndexBuffer(Command->IndexBuffer);

			Device->DrawIndexed(Command->PrimitiveType, 
								Command->IndexCount, 
								Command->FirstIndex, 
								Command->BaseVertex);

			break;
		}	
		case ZE_RCT_INSTANCED:
		{
			ZERenderCommandInstanced* Command = (ZERenderCommandInstanced*)RenderCommand;
			Device->SetVertexBufferArray(Command->VertexBuffers);
			Device->SetVertexLayout(Command->VertexLayout);

			Device->DrawInstanced(	Command->PrimitiveType,
									GetVertexCount(Command->PrimitiveType, Command->PrimitiveCount),
									Command->FirstVertex,
									Command->InstanceCount, 
									Command->FirstInstance);

			break;
		}	
		case ZE_RCT_INDEXED_INSTANCED:
		{
			ZERenderCommandIndexedInstanced* Command = (ZERenderCommandIndexedInstanced*)RenderCommand;
			Device->SetVertexBufferArray(Command->VertexBuffers);
			Device->SetVertexLayout(Command->VertexLayout);
			Device->SetIndexBuffer(Command->IndexBuffer);

			Device->DrawIndexedInstanced(Command->PrimitiveType, 
										Command->IndexCount, 
										Command->InstanceCount, 
										Command->FirstIndex, 
										Command->BaseVertex, 
										Command->FirstInstance);

			break;
		}	
	}
}

void ZERenderStage::ResetStates()
{
	DefaultStates.SetToDefault();
}

void ZERenderStage::CommitStates()
{

}

const ZEGraphicsDeviceState* ZERenderStage::GetDefaultStates() const
{
	return &DefaultStates;
}

void ZERenderStage::Destroy()
{
	delete this;
}

ZERenderStage::ZERenderStage()
{

}

ZERenderStage::~ZERenderStage()
{

}
