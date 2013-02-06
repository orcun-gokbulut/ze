//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsDevice.cpp
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

#include "ZEGraphicsDevice.h"

ZEGraphicsDevice::ZEGraphicsDevice()
{
	
}

ZEGraphicsDevice::~ZEGraphicsDevice()
{
	
}

void ZEGraphicsDevice::SetVertexLayout(ZEVertexLayout& Layout)
{
	Layout.UpdateHash();

	if (CurrentState.VertexLayout.Hash != Layout.Hash)
	{
		CurrentState.VertexLayout = Layout;
	}
}

ZEVertexLayout& ZEGraphicsDevice::GetVertexLayout()
{
	return CurrentState.VertexLayout;
}

void ZEGraphicsDevice::SetVertexBuffer(ZEUInt Index, const ZEVertexBuffer* Buffer)
{
	zeDebugCheck(Index >= ZE_MAX_VERTEX_BUFFER_SLOT, "Index out of range.");

	CurrentState.VertexBuffers[Index] = Buffer;
}

const ZEVertexBuffer* ZEGraphicsDevice::GetVertexBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZE_MAX_VERTEX_BUFFER_SLOT, "Index out of range.");

	return CurrentState.VertexBuffers[Index];
}

void ZEGraphicsDevice::SetVertexBufferArray(const ZEVertexBuffer* const Buffers[ZE_MAX_VERTEX_BUFFER_SLOT])
{
	zeDebugCheck(Buffers == NULL, "NULL Pointer.");
	
	memcpy(CurrentState.VertexBuffers, Buffers, sizeof(ZEVertexBuffer*) * ZE_MAX_VERTEX_BUFFER_SLOT);
}

void ZEGraphicsDevice::SetIndexBuffer(const ZEIndexBuffer* Buffer)
{
	CurrentState.IndexBuffer = Buffer;
}

const ZEIndexBuffer* ZEGraphicsDevice::GetIndexBuffer() const
{
	return CurrentState.IndexBuffer;
}

void ZEGraphicsDevice::SetVertexShader(ZEShader* Shader)
{
	CurrentState.VertexShader = Shader;
}

const ZEShader* ZEGraphicsDevice::GetVertexShader() const
{
	return CurrentState.VertexShader;
}

void ZEGraphicsDevice::SetVertexShaderBuffer(ZEUInt Index, ZEConstantBuffer* Buffer)
{
	zeDebugCheck(Index >= ZE_MAX_BUFFER_SLOT, "Index out of range.");

	CurrentState.VertexShaderBuffers[Index] = Buffer;
}

const ZEConstantBuffer* ZEGraphicsDevice::GetVertexShaderBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZE_MAX_BUFFER_SLOT, "Index out of range.");

	return CurrentState.VertexShaderBuffers[Index];
}

void ZEGraphicsDevice::SetVertexShaderTexture(ZEUInt Index, const ZETexture* Texture)
{
	zeDebugCheck(Index >= ZE_MAX_TEXTURE_SLOT, "Index out of range.");

	CurrentState.VertexShaderTextures[Index] = Texture;
}

const ZETexture* ZEGraphicsDevice::GetVertexShaderTexture(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZE_MAX_TEXTURE_SLOT, "Index out of range.");

	return CurrentState.VertexShaderTextures[Index];
}

void ZEGraphicsDevice::SetVertexShaderSampler(ZEUInt Index, ZEDeviceStateSampler& Sampler)
{
	zeDebugCheck(Index >= ZE_MAX_SAMPLER_SLOT, "Index out of range.");
	
	Sampler.UpdateHash();

	if (CurrentState.VertexShaderSamplers[Index].Hash != Sampler.Hash)
	{
		CurrentState.VertexShaderSamplers[Index] = Sampler;
	}
}

ZEDeviceStateSampler& ZEGraphicsDevice::GetVertexShaderSampler(ZEUInt Index)
{
	zeDebugCheck(Index >= ZE_MAX_SAMPLER_SLOT, "Index out of range.");

	return CurrentState.VertexShaderSamplers[Index];
}

void ZEGraphicsDevice::SetGeometryShader(ZEShader* Shader)
{
	CurrentState.GeometryShader = Shader;
}

const ZEShader* ZEGraphicsDevice::GetGeometryShader() const
{
	return CurrentState.GeometryShader;
}

void ZEGraphicsDevice::SetGeometryShaderBuffer(ZEUInt Index, ZEConstantBuffer* Buffer)
{
	zeDebugCheck(Index >= ZE_MAX_BUFFER_SLOT, "Index out of range.");

	CurrentState.GeometryShaderBuffers[Index] = Buffer;
}

const ZEConstantBuffer* ZEGraphicsDevice::GetGeometryShaderBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZE_MAX_BUFFER_SLOT, "Index out of range.");

	return CurrentState.GeometryShaderBuffers[Index];
}

void ZEGraphicsDevice::SetGeometryShaderTexture(ZEUInt Index, const ZETexture* Texture)
{
	zeDebugCheck(Index >= ZE_MAX_TEXTURE_SLOT, "Index out of range.");

	CurrentState.GeometryShaderTextures[Index] = Texture;
}

const ZETexture* ZEGraphicsDevice::GetGeometryShaderTexture(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZE_MAX_TEXTURE_SLOT, "Index out of range.");

	return CurrentState.GeometryShaderTextures[Index];
}

void ZEGraphicsDevice::SetGeometryShaderSampler(ZEUInt Index, ZEDeviceStateSampler& Sampler)
{
	zeDebugCheck(Index >= ZE_MAX_SAMPLER_SLOT, "Index out of range.");
	
	Sampler.UpdateHash();

	if (CurrentState.GeometryShaderSamplers[Index].Hash != Sampler.Hash)
	{
		CurrentState.GeometryShaderSamplers[Index] = Sampler;
	}
}

ZEDeviceStateSampler& ZEGraphicsDevice::GetGeometryShaderSampler(ZEUInt Index)
{
	zeDebugCheck(Index >= ZE_MAX_SAMPLER_SLOT, "Index out of range.");

	return CurrentState.GeometryShaderSamplers[Index];
}

void ZEGraphicsDevice::SetRasterizerState(ZEDeviceStateRasterizer& State)
{
	State.UpdateHash();

	if (CurrentState.RasterizerState.Hash != State.Hash)
	{
		CurrentState.RasterizerState = State;
	}
}

ZEDeviceStateRasterizer& ZEGraphicsDevice::GetRasterizerState()
{
	return CurrentState.RasterizerState;
}

void ZEGraphicsDevice::SetViewport(ZESize Index, ZEViewport& ViewPort)
{
	zeDebugCheck(Index >= ZE_MAX_VIEWPORT_SLOT, "Index out of range.");

	if (CurrentState.ViewPorts[Index] != ViewPort)
	{
		CurrentState.ViewPorts[Index] = ViewPort;
	}
}

ZEViewport& ZEGraphicsDevice::GetViewport(ZESize Index)
{
	return CurrentState.ViewPorts[Index];
}

void ZEGraphicsDevice::SetScissorRectangle(ZESize Index, ZEScissorRectangle& Rectangle)
{
	zeDebugCheck(Index >= ZE_MAX_SCISSOR_SLOT, "Index out of range.");

	if (CurrentState.ScissorRectangles[Index] != Rectangle)
	{
		CurrentState.ScissorRectangles[Index] = Rectangle;
	}
}

ZEScissorRectangle& ZEGraphicsDevice::GetScissorRectangle(ZESize Index)
{
	zeDebugCheck(Index >= ZE_MAX_SCISSOR_SLOT, "Index out of range.");

	return CurrentState.ScissorRectangles[Index];
}

void ZEGraphicsDevice::SetPixelShader(ZEShader* Shader)
{
	CurrentState.PixelShader = Shader;
}

const ZEShader* ZEGraphicsDevice::GetPixelShader() const
{
	return CurrentState.PixelShader;
}

void ZEGraphicsDevice::SetPixelShaderBuffer(ZEUInt Index, ZEConstantBuffer* Buffer)
{
	zeDebugCheck(Index >= ZE_MAX_BUFFER_SLOT, "Index out of range.");

	CurrentState.PixelShaderBuffers[Index] = Buffer;
}

const ZEConstantBuffer* ZEGraphicsDevice::GetPixelShaderBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZE_MAX_BUFFER_SLOT, "Index out of range.");

	return CurrentState.PixelShaderBuffers[Index];
}

void ZEGraphicsDevice::SetPixelShaderTexture(ZEUInt Index, const ZETexture* Texture)
{
	zeDebugCheck(Index >= ZE_MAX_TEXTURE_SLOT, "Index out of range.");

	CurrentState.PixelShaderTextures[Index] = Texture;
}

const ZETexture* ZEGraphicsDevice::GetPixelShaderTexture(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZE_MAX_TEXTURE_SLOT, "Index out of range.");

	return CurrentState.PixelShaderTextures[Index];
}

void ZEGraphicsDevice::SetPixelShaderSampler(ZEUInt Index, ZEDeviceStateSampler& Sampler)
{
	zeDebugCheck(Index >= ZE_MAX_SAMPLER_SLOT, "Index out of range.");
	
	Sampler.UpdateHash();

	if (CurrentState.PixelShaderSamplers[Index].Hash != Sampler.Hash)
	{
		CurrentState.PixelShaderSamplers[Index] = Sampler;
	}
}

ZEDeviceStateSampler& ZEGraphicsDevice::GetPixelShaderSampler(ZEUInt Index)
{
	zeDebugCheck(Index >= ZE_MAX_SAMPLER_SLOT, "Index out of range.");

	return CurrentState.PixelShaderSamplers[Index];
}

void ZEGraphicsDevice::SetBlendState(ZEDeviceStateBlend& State)
{
	State.UpdateHash();

	if (CurrentState.BlendState.Hash != State.Hash)
	{
		CurrentState.BlendState = State;
	}	
}

ZEDeviceStateBlend& ZEGraphicsDevice::GetBlendState()
{
	return CurrentState.BlendState;
}

void ZEGraphicsDevice::SetDepthStencilState(ZEDeviceStateDepthStencil& State)
{
	State.UpdateHash();

	if (CurrentState.DepthStencilState.Hash != State.Hash)
	{
		CurrentState.DepthStencilState = State;
	}
}

ZEDeviceStateDepthStencil& ZEGraphicsDevice::GetDepthStencilState()
{
	return CurrentState.DepthStencilState;
}

void ZEGraphicsDevice::SetRenderTargetScreen(const ZERenderTarget* FrameBuffer)
{
	zeDebugCheck(FrameBuffer == NULL, "NULL pointer.");

	CurrentState.RenderTargets[0] = FrameBuffer;
	CurrentState.RenderTargets[1] = NULL;
	CurrentState.RenderTargets[2] = NULL;
	CurrentState.RenderTargets[3] = NULL;
	CurrentState.RenderTargets[4] = NULL;
	CurrentState.RenderTargets[5] = NULL;
	CurrentState.RenderTargets[6] = NULL;
	CurrentState.RenderTargets[7] = NULL;
	CurrentState.ScreenWriteEnable = true;
}

bool ZEGraphicsDevice::GetScreenWriteEnable() const
{
	return CurrentState.ScreenWriteEnable;
}

void ZEGraphicsDevice::SetRenderTarget(ZEUInt Index, const ZERenderTarget* Target)
{
	zeDebugCheck(Index >= ZE_MAX_SAMPLER_SLOT, "Index out of range.");

	CurrentState.RenderTargets[Index] = Target;
	CurrentState.ScreenWriteEnable = false;
}

void ZEGraphicsDevice::SetRenderTargetArray(const ZERenderTarget* const Targets[ZE_MAX_RENDER_TARGET_SLOT])
{
	zeDebugCheck(Targets == NULL, "NULL Pointer.");

	CurrentState.RenderTargets[0] = Targets[0];
	CurrentState.RenderTargets[1] = Targets[1];
	CurrentState.RenderTargets[2] = Targets[2];
	CurrentState.RenderTargets[3] = Targets[3];
	CurrentState.RenderTargets[4] = Targets[4];
	CurrentState.RenderTargets[5] = Targets[5];
	CurrentState.RenderTargets[6] = Targets[6];
	CurrentState.RenderTargets[7] = Targets[7];
	CurrentState.ScreenWriteEnable = false;
}

const ZERenderTarget* ZEGraphicsDevice::GetRenderTarget(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZE_MAX_SAMPLER_SLOT, "Index out of range.");
	
	return CurrentState.RenderTargets[Index];
}

void ZEGraphicsDevice::SetDepthStencilBuffer(const ZEDepthStencilBuffer* Buffer)
{
	CurrentState.DepthStencilBuffer = Buffer;
}

const ZEDepthStencilBuffer* ZEGraphicsDevice::GetDepthStencilBuffer() const
{
	return CurrentState.DepthStencilBuffer;
}

void ZEGraphicsDevice::SetDeviceState(ZEDeviceState& State)
{
	memcpy(&CurrentState, &State, sizeof(ZEDeviceState));
}

ZEDeviceState& ZEGraphicsDevice::GetDeviceState()
{
	return CurrentState;
}

void ZEGraphicsDevice::ResetStates()
{
	CurrentState.SetToDefault();
	OldState.SetToDefault();
}
