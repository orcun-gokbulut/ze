//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRDevice.cpp
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

#include "ZEGRShader.h"
#include "ZEGRDevice.h"

ZEGRDevice::ZEGRDevice()
{
	ActiveAF = 0;
	ActiveMSAA = 0;
}

ZEGRDevice::~ZEGRDevice()
{
	MSAAModeList.Clear();
	AFModeList.Clear();
}

const ZEArray<ZEUInt>& ZEGRDevice::GetModesAF() const
{
	return AFModeList;
}

const ZEArray<ZEMSAAMode>& ZEGRDevice::GetModesMSAA() const
{
	return MSAAModeList;
}

bool ZEGRDevice::SetSampleCountAF(ZEUInt SampleCount)
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

ZEUInt ZEGRDevice::GetSampleCountAF() const
{
	return AFModeList[ActiveAF];
}

bool ZEGRDevice::SetSampleCountMSAA(ZEUInt SampleCount)
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

ZEUInt ZEGRDevice::GetSampleCountMSAA() const
{
	return MSAAModeList[ActiveMSAA].SampleCount;
}

bool ZEGRDevice::CheckSupportAF(ZEUInt SampleCount)
{
	ZESize Count = AFModeList.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		if (AFModeList[I] == SampleCount)
			return true;
	}
	return false;
}

bool ZEGRDevice::CheckSupportMSAA(ZEUInt SampleCount)
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
void ZEGRDevice::SetVertexLayout(ZEGRVertexLayout* Layout)
{
	CurrentState.VertexLayout = Layout;
}

ZEGRVertexLayout* ZEGRDevice::GetVertexLayout()
{
	return CurrentState.VertexLayout;
}

void ZEGRDevice::SetVertexBuffer(ZEUInt Index, ZEVertexBuffer* Buffer)
{
	zeDebugCheck(Index >= ZEGR_MAX_VERTEX_BUFFER_SLOT, "Index out of range.");

	CurrentState.VertexBuffers[(ZESize)Index] = Buffer;
}

const ZEVertexBuffer* ZEGRDevice::GetVertexBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_VERTEX_BUFFER_SLOT, "Index out of range.");

	return CurrentState.VertexBuffers[(ZESize)Index];
}

void ZEGRDevice::SetVertexBufferArray(ZEVertexBuffer* const Buffers[ZEGR_MAX_VERTEX_BUFFER_SLOT])
{
	zeDebugCheck(Buffers == NULL, "NULL Pointer.");

	memcpy(CurrentState.VertexBuffers, Buffers, sizeof(ZEVertexBuffer*) * ZEGR_MAX_VERTEX_BUFFER_SLOT);
}

void ZEGRDevice::SetIndexBuffer(ZEGRIndexBuffer* Buffer)
{
	CurrentState.IndexBuffer = Buffer;
}

const ZEGRIndexBuffer* ZEGRDevice::GetIndexBuffer() const
{
	return CurrentState.IndexBuffer;
}

void ZEGRDevice::SetVertexShader(ZEGRShader* Shader)
{
	zeDebugCheck(Shader != NULL && Shader->GetShaderType() != ZE_ST_VERTEX, "Wrong shader type");

	CurrentState.VertexShader = Shader;
}

const ZEGRShader* ZEGRDevice::GetVertexShader() const
{
	return CurrentState.VertexShader;
}

void ZEGRDevice::SetVertexShaderBuffer(ZEUInt Index, ZEGRConstantBuffer* Buffer)
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	CurrentState.VertexShaderBuffers[(ZESize)Index] = Buffer;
}

const ZEGRConstantBuffer* ZEGRDevice::GetVertexShaderBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	return CurrentState.VertexShaderBuffers[(ZESize)Index];
}

void ZEGRDevice::SetVertexShaderTexture(ZEUInt Index, const ZEGRTexture* Texture)
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	CurrentState.VertexShaderTextures[(ZESize)Index] = Texture;
}

const ZEGRTexture* ZEGRDevice::GetVertexShaderTexture(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	return CurrentState.VertexShaderTextures[(ZESize)Index];
}

void ZEGRDevice::SetVertexShaderSampler(ZEUInt Index, ZEGRSamplerState& Sampler)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	if (CurrentState.VertexShaderSamplers[(ZESize)Index].GetHash() != Sampler.GetHash())
	{
		CurrentState.VertexShaderSamplers[(ZESize)Index] = Sampler;
	}
}

ZEGRSamplerState& ZEGRDevice::GetVertexShaderSampler(ZEUInt Index)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	return CurrentState.VertexShaderSamplers[(ZESize)Index];
}

void ZEGRDevice::SetGeometryShader(ZEGRShader* Shader)
{
	zeDebugCheck(Shader != NULL && Shader->GetShaderType() != ZE_ST_GEOMETRY, "Wrong shader type");

	CurrentState.GeometryShader = Shader;
}

const ZEGRShader* ZEGRDevice::GetGeometryShader() const
{
	return CurrentState.GeometryShader;
}

void ZEGRDevice::SetGeometryShaderBuffer(ZEUInt Index, ZEGRConstantBuffer* Buffer)
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	CurrentState.GeometryShaderBuffers[(ZESize)Index] = Buffer;
}

const ZEGRConstantBuffer* ZEGRDevice::GetGeometryShaderBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	return CurrentState.GeometryShaderBuffers[(ZESize)Index];
}

void ZEGRDevice::SetGeometryShaderTexture(ZEUInt Index, const ZEGRTexture* Texture)
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	CurrentState.GeometryShaderTextures[(ZESize)Index] = Texture;
}

const ZEGRTexture* ZEGRDevice::GetGeometryShaderTexture(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	return CurrentState.GeometryShaderTextures[(ZESize)Index];
}

void ZEGRDevice::SetGeometryShaderSampler(ZEUInt Index, ZEGRSamplerState& Sampler)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	if (CurrentState.GeometryShaderSamplers[(ZESize)Index].GetHash() != Sampler.GetHash())
	{
		CurrentState.GeometryShaderSamplers[(ZESize)Index] = Sampler;
	}
}

ZEGRSamplerState& ZEGRDevice::GetGeometryShaderSampler(ZEUInt Index)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	return CurrentState.GeometryShaderSamplers[Index];
}

void ZEGRDevice::SetRasterizerState(ZEGRRasterizerState& State)
{
	if (CurrentState.RasterizerState.GetHash() != State.GetHash())
	{
		CurrentState.RasterizerState = State;
	}
}

ZEGRRasterizerState& ZEGRDevice::GetRasterizerState()
{
	return CurrentState.RasterizerState;
}

void ZEGRDevice::SetViewport(ZEUInt Index, ZEViewport& ViewPort)
{
	zeDebugCheck(Index >= ZEGR_MAX_VIEWPORT_SLOT, "Index out of range.");

	if (CurrentState.ViewPorts[(ZESize)Index] != ViewPort)
	{
		CurrentState.ViewPorts[(ZESize)Index] = ViewPort;
	}
}

ZEViewport& ZEGRDevice::GetViewport(ZEUInt Index)
{
	return CurrentState.ViewPorts[(ZESize)Index];
}

void ZEGRDevice::SetScissorRectangle(ZEUInt Index, ZEGRScissorRectangle& Rectangle)
{
	zeDebugCheck(Index >= ZEGR_MAX_SCISSOR_SLOT, "Index out of range.");

	if (CurrentState.ScissorRects[(ZESize)Index] != Rectangle)
	{
		CurrentState.ScissorRects[(ZESize)Index] = Rectangle;
	}
}

ZEGRScissorRectangle& ZEGRDevice::GetScissorRectangle(ZEUInt Index)
{
	zeDebugCheck(Index >= ZEGR_MAX_SCISSOR_SLOT, "Index out of range.");

	return CurrentState.ScissorRects[(ZESize)Index];
}

void ZEGRDevice::SetPixelShader(ZEGRShader* Shader)
{
	zeDebugCheck(Shader != NULL && Shader->GetShaderType() != ZE_ST_PIXEL, "Wrong shader type");

	CurrentState.PixelShader = Shader;
}

const ZEGRShader* ZEGRDevice::GetPixelShader() const
{
	return CurrentState.PixelShader;
}

void ZEGRDevice::SetPixelShaderBuffer(ZEUInt Index, ZEGRConstantBuffer* Buffer)
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	CurrentState.PixelShaderBuffers[(ZESize)Index] = Buffer;
}

const ZEGRConstantBuffer* ZEGRDevice::GetPixelShaderBuffer(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Index out of range.");

	return CurrentState.PixelShaderBuffers[Index];
}

void ZEGRDevice::SetPixelShaderTexture(ZEUInt Index, const ZEGRTexture* Texture)
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	CurrentState.PixelShaderTextures[(ZESize)Index] = Texture;
}

const ZEGRTexture* ZEGRDevice::GetPixelShaderTexture(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Index out of range.");

	return CurrentState.PixelShaderTextures[(ZESize)Index];
}

void ZEGRDevice::SetPixelShaderSampler(ZEUInt Index, ZEGRSamplerState& Sampler)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	if (CurrentState.PixelShaderSamplers[(ZESize)Index].GetHash() != Sampler.GetHash())
	{
		CurrentState.PixelShaderSamplers[(ZESize)Index] = Sampler;
	}
}

ZEGRSamplerState& ZEGRDevice::GetPixelShaderSampler(ZEUInt Index)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	return CurrentState.PixelShaderSamplers[(ZESize)Index];
}

void ZEGRDevice::SetBlendState(ZEGRBlendState& State, ZEVector4& ComponentBlendFactors, ZEGRColorMask ComponentBlendMask)
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

void ZEGRDevice::SetBlendState(ZEGRBlendState& State)
{
	if (CurrentState.BlendState.GetHash() != State.GetHash())
	{
		CurrentState.BlendState = State;
	}
}

ZEGRBlendState& ZEGRDevice::GetBlendState()
{
	return CurrentState.BlendState;
}
		
void ZEGRDevice::SetComponentBlendMask(ZEGRColorMask ComponentBlendMask)
{
	CurrentState.ComponentBlendMask = ComponentBlendMask;
}

ZEGRColorMask& ZEGRDevice::GetComponentBlendMask()
{
	return CurrentState.ComponentBlendMask;
}

void ZEGRDevice::SetComponentBlendFactors(ZEVector4* ComponentBlendFactors)
{
	CurrentState.ComponentBlendFactors = ComponentBlendFactors == NULL ? ZEVector4::One : *ComponentBlendFactors;
}

ZEVector4& ZEGRDevice::GetComponentBlendFactors()
{
	return CurrentState.ComponentBlendFactors;
}

void ZEGRDevice::SetDepthStencilState(ZEGRDepthStencilState& State, ZEUInt32 StencilReferance)
{
	if (CurrentState.DepthStencilState.GetHash() != State.GetHash() || 
		CurrentState.StencilReferance != StencilReferance)
	{
		CurrentState.DepthStencilState = State;
		CurrentState.StencilReferance = StencilReferance;
	}
}

void ZEGRDevice::SetDepthStencilState(ZEGRDepthStencilState& State)
{
	if (CurrentState.DepthStencilState.GetHash() != State.GetHash())
	{
		CurrentState.DepthStencilState = State;
	}
}

ZEGRDepthStencilState& ZEGRDevice::GetDepthStencilState()
{
	return CurrentState.DepthStencilState;
}

void ZEGRDevice::SetStencilReferance(ZEUInt32 StencilReferance)
{
	CurrentState.StencilReferance = StencilReferance;
}

ZEUInt32& ZEGRDevice::GetStencilReferance()
{
	return CurrentState.StencilReferance;
}

void ZEGRDevice::SetRenderTargetScreen(const ZEGRRenderTarget* FrameBuffer)
{
	zeDebugCheck(FrameBuffer == NULL, "NULL pointer.");

	memset(CurrentState.RenderTargets, NULL, sizeof(ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);

	CurrentState.RenderTargets[0] = FrameBuffer;
	CurrentState.ScreenWriteEnable = true;
}

bool ZEGRDevice::GetScreenWriteEnable() const
{
	return CurrentState.ScreenWriteEnable;
}

void ZEGRDevice::SetRenderTarget(ZEUInt Index, const ZEGRRenderTarget* Target)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");

	CurrentState.RenderTargets[(ZESize)Index] = Target;
	CurrentState.ScreenWriteEnable = false;
}

void ZEGRDevice::SetRenderTargetArray(const ZEGRRenderTarget* const Targets[ZEGR_MAX_RENDER_TARGET_SLOT])
{
	zeDebugCheck(Targets == NULL, "NULL Pointer.");

	memcpy(CurrentState.RenderTargets, Targets, sizeof(ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);
	CurrentState.ScreenWriteEnable = false;
}

const ZEGRRenderTarget* ZEGRDevice::GetRenderTarget(ZEUInt Index) const
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Index out of range.");
	
	return CurrentState.RenderTargets[(ZESize)Index];
}

void ZEGRDevice::SetDepthStencilBuffer(const ZEGRDepthStencilBuffer* Buffer)
{
	CurrentState.DepthStencilBuffer = Buffer;
}

const ZEGRDepthStencilBuffer* ZEGRDevice::GetDepthStencilBuffer() const
{
	return CurrentState.DepthStencilBuffer;
}

void ZEGRDevice::SetDeviceState(ZEGRDeviceState& State)
{
	memcpy(&CurrentState, &State, sizeof(ZEGRDeviceState));
}

ZEGRDeviceState& ZEGRDevice::GetDeviceState()
{
	return CurrentState;
}

void ZEGRDevice::ResetStates()
{
	CurrentState.SetToDefault();
	OldState.SetToDefault();
}
