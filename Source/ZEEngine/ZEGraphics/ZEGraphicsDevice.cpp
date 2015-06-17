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

#include "ZEShader.h"
#include "ZEGraphicsDevice.h"

ZEGraphicsDevice::ZEGraphicsDevice()
{
	ActiveAF = 0;
	ActiveMSAA = 0;
}

ZEGraphicsDevice::~ZEGraphicsDevice()
{
	MSAAModeList.Clear();
	AFModeList.Clear();
}

const ZEArray<ZEUInt>& ZEGraphicsDevice::GetModesAF() const
{
	return AFModeList;
}

const ZEArray<ZEMSAAMode>& ZEGraphicsDevice::GetModesMSAA() const
{
	return MSAAModeList;
}

bool ZEGraphicsDevice::SetSampleCountAF(ZEUInt SampleCount)
{
	ZESize Count = AFModeList.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		if (AFModeList[I] == SampleCount)
		{
			ActiveAF = I;
			return true;
		}
	}
	return false;
}

ZEUInt ZEGraphicsDevice::GetSampleCountAF() const
{
	return AFModeList[ActiveAF];
}

bool ZEGraphicsDevice::SetSampleCountMSAA(ZEUInt SampleCount)
{
	ZESize Count = MSAAModeList.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		if (MSAAModeList[I].SampleCount == SampleCount)
		{
			ActiveMSAA = I;
			return true;
		}
	}
	return false;
}

ZEUInt ZEGraphicsDevice::GetSampleCountMSAA() const
{
	return MSAAModeList[ActiveMSAA].SampleCount;
}

bool ZEGraphicsDevice::CheckSupportAF(ZEUInt SampleCount)
{
	ZESize Count = AFModeList.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		if (AFModeList[I] == SampleCount)
			return true;
	}
	return false;
}

bool ZEGraphicsDevice::CheckSupportMSAA(ZEUInt SampleCount)
{
	ZESize Count = MSAAModeList.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		if (MSAAModeList[I].SampleCount == SampleCount)
			return true;
	}
	return false;
}

// Null means auto generate
void ZEGraphicsDevice::SetVertexLayout(ZEGRVertexLayout* Layout)
{
	CurrentState.VertexLayout = Layout;
}

ZEGRVertexLayout* ZEGraphicsDevice::GetVertexLayout()
{
	return CurrentState.VertexLayout;
}

void ZEGraphicsDevice::SetVertexBuffer(ZEUInt Index, ZEVertexBuffer* Buffer)
{
	zeDebugCheck(Index >= ZEGR_MAX_VERTEX_BUFFER_SLOT, "Index out of range.");

	CurrentState.VertexBuffers[(ZESize)Index] = Buffer;
}

const ZEVertexBuffer* ZEGraphicsDevice::GetVertexBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_VERTEX_BUFFER_SLOT, "Index out of range.");

	return CurrentState.VertexBuffers[(ZESize)Index];
}

void ZEGraphicsDevice::SetVertexBufferArray(ZEVertexBuffer* const Buffers[ZEGR_MAX_VERTEX_BUFFER_SLOT])
{
	zeDebugCheck(Buffers == NULL, "NULL Pointer.");

	memcpy(CurrentState.VertexBuffers, Buffers, sizeof(ZEVertexBuffer*) * ZEGR_MAX_VERTEX_BUFFER_SLOT);
}

void ZEGraphicsDevice::SetIndexBuffer(ZEGRIndexBuffer* Buffer)
{
	CurrentState.IndexBuffer = Buffer;
}

const ZEGRIndexBuffer* ZEGraphicsDevice::GetIndexBuffer() const
{
	return CurrentState.IndexBuffer;
}

void ZEGraphicsDevice::SetVertexShader(ZEGRShader* Shader)
{
	zeDebugCheck(Shader != NULL && Shader->GetShaderType() != ZE_ST_VERTEX, "Wrong shader type");

	CurrentState.VertexShader = Shader;
}

const ZEGRShader* ZEGraphicsDevice::GetVertexShader() const
{
	return CurrentState.VertexShader;
}

void ZEGraphicsDevice::SetVertexShaderBuffer(ZEUInt Index, ZEGRConstantBuffer* Buffer)
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	CurrentState.VertexShaderBuffers[(ZESize)Index] = Buffer;
}

const ZEGRConstantBuffer* ZEGraphicsDevice::GetVertexShaderBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	return CurrentState.VertexShaderBuffers[(ZESize)Index];
}

void ZEGraphicsDevice::SetVertexShaderTexture(ZEUInt Index, const ZEGRTexture* Texture)
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	CurrentState.VertexShaderTextures[(ZESize)Index] = Texture;
}

const ZEGRTexture* ZEGraphicsDevice::GetVertexShaderTexture(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	return CurrentState.VertexShaderTextures[(ZESize)Index];
}

void ZEGraphicsDevice::SetVertexShaderSampler(ZEUInt Index, ZEGRSamplerState& Sampler)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	if (CurrentState.VertexShaderSamplers[(ZESize)Index].GetHash() != Sampler.GetHash())
	{
		CurrentState.VertexShaderSamplers[(ZESize)Index] = Sampler;
	}
}

ZEGRSamplerState& ZEGraphicsDevice::GetVertexShaderSampler(ZEUInt Index)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	return CurrentState.VertexShaderSamplers[(ZESize)Index];
}

void ZEGraphicsDevice::SetGeometryShader(ZEGRShader* Shader)
{
	zeDebugCheck(Shader != NULL && Shader->GetShaderType() != ZE_ST_GEOMETRY, "Wrong shader type");

	CurrentState.GeometryShader = Shader;
}

const ZEGRShader* ZEGraphicsDevice::GetGeometryShader() const
{
	return CurrentState.GeometryShader;
}

void ZEGraphicsDevice::SetGeometryShaderBuffer(ZEUInt Index, ZEGRConstantBuffer* Buffer)
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	CurrentState.GeometryShaderBuffers[(ZESize)Index] = Buffer;
}

const ZEGRConstantBuffer* ZEGraphicsDevice::GetGeometryShaderBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	return CurrentState.GeometryShaderBuffers[(ZESize)Index];
}

void ZEGraphicsDevice::SetGeometryShaderTexture(ZEUInt Index, const ZEGRTexture* Texture)
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	CurrentState.GeometryShaderTextures[(ZESize)Index] = Texture;
}

const ZEGRTexture* ZEGraphicsDevice::GetGeometryShaderTexture(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	return CurrentState.GeometryShaderTextures[(ZESize)Index];
}

void ZEGraphicsDevice::SetGeometryShaderSampler(ZEUInt Index, ZEGRSamplerState& Sampler)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	if (CurrentState.GeometryShaderSamplers[(ZESize)Index].GetHash() != Sampler.GetHash())
	{
		CurrentState.GeometryShaderSamplers[(ZESize)Index] = Sampler;
	}
}

ZEGRSamplerState& ZEGraphicsDevice::GetGeometryShaderSampler(ZEUInt Index)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	return CurrentState.GeometryShaderSamplers[Index];
}

void ZEGraphicsDevice::SetRasterizerState(ZEGRRasterizerState& State)
{
	if (CurrentState.RasterizerState.GetHash() != State.GetHash())
	{
		CurrentState.RasterizerState = State;
	}
}

ZEGRRasterizerState& ZEGraphicsDevice::GetRasterizerState()
{
	return CurrentState.RasterizerState;
}

void ZEGraphicsDevice::SetViewport(ZEUInt Index, ZEViewport& ViewPort)
{
	zeDebugCheck(Index >= ZEGR_MAX_VIEWPORT_SLOT, "Index out of range.");

	if (CurrentState.ViewPorts[(ZESize)Index] != ViewPort)
	{
		CurrentState.ViewPorts[(ZESize)Index] = ViewPort;
	}
}

ZEViewport& ZEGraphicsDevice::GetViewport(ZEUInt Index)
{
	return CurrentState.ViewPorts[(ZESize)Index];
}

void ZEGraphicsDevice::SetScissorRectangle(ZEUInt Index, ZEGRScissorRectangle& Rectangle)
{
	zeDebugCheck(Index >= ZEGR_MAX_SCISSOR_SLOT, "Index out of range.");

	if (CurrentState.ScissorRects[(ZESize)Index] != Rectangle)
	{
		CurrentState.ScissorRects[(ZESize)Index] = Rectangle;
	}
}

ZEGRScissorRectangle& ZEGraphicsDevice::GetScissorRectangle(ZEUInt Index)
{
	zeDebugCheck(Index >= ZEGR_MAX_SCISSOR_SLOT, "Index out of range.");

	return CurrentState.ScissorRects[(ZESize)Index];
}

void ZEGraphicsDevice::SetPixelShader(ZEGRShader* Shader)
{
	zeDebugCheck(Shader != NULL && Shader->GetShaderType() != ZE_ST_PIXEL, "Wrong shader type");

	CurrentState.PixelShader = Shader;
}

const ZEGRShader* ZEGraphicsDevice::GetPixelShader() const
{
	return CurrentState.PixelShader;
}

void ZEGraphicsDevice::SetPixelShaderBuffer(ZEUInt Index, ZEGRConstantBuffer* Buffer)
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	CurrentState.PixelShaderBuffers[(ZESize)Index] = Buffer;
}

const ZEGRConstantBuffer* ZEGraphicsDevice::GetPixelShaderBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	return CurrentState.PixelShaderBuffers[Index];
}

void ZEGraphicsDevice::SetPixelShaderTexture(ZEUInt Index, const ZEGRTexture* Texture)
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	CurrentState.PixelShaderTextures[(ZESize)Index] = Texture;
}

const ZEGRTexture* ZEGraphicsDevice::GetPixelShaderTexture(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	return CurrentState.PixelShaderTextures[(ZESize)Index];
}

void ZEGraphicsDevice::SetPixelShaderSampler(ZEUInt Index, ZEGRSamplerState& Sampler)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	if (CurrentState.PixelShaderSamplers[(ZESize)Index].GetHash() != Sampler.GetHash())
	{
		CurrentState.PixelShaderSamplers[(ZESize)Index] = Sampler;
	}
}

ZEGRSamplerState& ZEGraphicsDevice::GetPixelShaderSampler(ZEUInt Index)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	return CurrentState.PixelShaderSamplers[(ZESize)Index];
}

void ZEGraphicsDevice::SetBlendState(ZEGRBlendState& State, ZEVector4& ComponentBlendFactors, ZEGRColorMask ComponentBlendMask)
{
	if (CurrentState.BlendState.GetHash() != State.GetHash() || 
		CurrentState.ComponentBlendFactors != ComponentBlendFactors || 
		CurrentState.ComponentBlendMask != ComponentBlendMask)
	{
		CurrentState.BlendState = State;
		CurrentState.ComponentBlendMask = ComponentBlendMask;
		CurrentState.ComponentBlendFactors = ComponentBlendFactors;
	}	
}

void ZEGraphicsDevice::SetBlendState(ZEGRBlendState& State)
{
	if (CurrentState.BlendState.GetHash() != State.GetHash())
	{
		CurrentState.BlendState = State;
	}
}

ZEGRBlendState& ZEGraphicsDevice::GetBlendState()
{
	return CurrentState.BlendState;
}
		
void ZEGraphicsDevice::SetComponentBlendMask(ZEGRColorMask ComponentBlendMask)
{
	CurrentState.ComponentBlendMask = ComponentBlendMask;
}

ZEGRColorMask& ZEGraphicsDevice::GetComponentBlendMask()
{
	return CurrentState.ComponentBlendMask;
}

void ZEGraphicsDevice::SetComponentBlendFactors(ZEVector4* ComponentBlendFactors)
{
	CurrentState.ComponentBlendFactors = ComponentBlendFactors == NULL ? ZEVector4::One : *ComponentBlendFactors;
}

ZEVector4& ZEGraphicsDevice::GetComponentBlendFactors()
{
	return CurrentState.ComponentBlendFactors;
}

void ZEGraphicsDevice::SetDepthStencilState(ZEGRDepthStencilState& State, ZEUInt32 StencilReferance)
{
	if (CurrentState.DepthStencilState.GetHash() != State.GetHash() || 
		CurrentState.StencilReferance != StencilReferance)
	{
		CurrentState.DepthStencilState = State;
		CurrentState.StencilReferance = StencilReferance;
	}
}

void ZEGraphicsDevice::SetDepthStencilState(ZEGRDepthStencilState& State)
{
	if (CurrentState.DepthStencilState.GetHash() != State.GetHash())
	{
		CurrentState.DepthStencilState = State;
	}
}

ZEGRDepthStencilState& ZEGraphicsDevice::GetDepthStencilState()
{
	return CurrentState.DepthStencilState;
}

void ZEGraphicsDevice::SetStencilReferance(ZEUInt32 StencilReferance)
{
	CurrentState.StencilReferance = StencilReferance;
}

ZEUInt32& ZEGraphicsDevice::GetStencilReferance()
{
	return CurrentState.StencilReferance;
}

void ZEGraphicsDevice::SetRenderTargetScreen(const ZEGRRenderTarget* FrameBuffer)
{
	zeDebugCheck(FrameBuffer == NULL, "NULL pointer.");

	memset(CurrentState.RenderTargets, NULL, sizeof(ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);

	CurrentState.RenderTargets[0] = FrameBuffer;
	CurrentState.ScreenWriteEnable = true;
}

bool ZEGraphicsDevice::GetScreenWriteEnable() const
{
	return CurrentState.ScreenWriteEnable;
}

void ZEGraphicsDevice::SetRenderTarget(ZEUInt Index, const ZEGRRenderTarget* Target)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	CurrentState.RenderTargets[(ZESize)Index] = Target;
	CurrentState.ScreenWriteEnable = false;
}

void ZEGraphicsDevice::SetRenderTargetArray(const ZEGRRenderTarget* const Targets[ZEGR_MAX_RENDER_TARGET_SLOT])
{
	zeDebugCheck(Targets == NULL, "NULL Pointer.");

	memcpy(CurrentState.RenderTargets, Targets, sizeof(ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);
	CurrentState.ScreenWriteEnable = false;
}

const ZEGRRenderTarget* ZEGraphicsDevice::GetRenderTarget(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");
	
	return CurrentState.RenderTargets[(ZESize)Index];
}

void ZEGraphicsDevice::SetDepthStencilBuffer(const ZEGRDepthStencilBuffer* Buffer)
{
	CurrentState.DepthStencilBuffer = Buffer;
}

const ZEGRDepthStencilBuffer* ZEGraphicsDevice::GetDepthStencilBuffer() const
{
	return CurrentState.DepthStencilBuffer;
}

void ZEGraphicsDevice::SetDeviceState(ZEGraphicsDeviceState& State)
{
	memcpy(&CurrentState, &State, sizeof(ZEGraphicsDeviceState));
}

ZEGraphicsDeviceState& ZEGraphicsDevice::GetDeviceState()
{
	return CurrentState;
}

void ZEGraphicsDevice::ResetStates()
{
	CurrentState.SetToDefault();
	OldState.SetToDefault();
}
