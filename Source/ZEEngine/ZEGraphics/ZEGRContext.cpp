//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRContext.cpp
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

#include "ZEGRContext.h"

#include "ZEError.h"
#include "ZEGRBuffer.h"
#include "ZEGRTexture.h"
#include "ZEGRRenderTarget.h"
#include "ZEGRDepthStencilBuffer.h"
#include "ZEGRGraphicsModule.h"

bool ZEGRContext::CheckVertexBuffers(ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers)
{
	zeDebugCheck(Index >= ZEGR_MAX_VERTEX_BUFFER_SLOT, "Vertex buffer index exceeds the max slot count.");
	zeDebugCheck(Count > (ZEGR_MAX_VERTEX_BUFFER_SLOT - Index), "Vertex buffer count is too much.");

	bool Dirty = false;

	if (Buffers != NULL)
	{
		for (ZEUInt I = 0; I < Count; I++)
		{
			ZEUInt Slot = Index + I;
			const ZEGRResource* SrcResource = static_cast<const ZEGRResource*>(Buffers[I]);
			const ZEGRResource*& DestResource = VertexBuffers[Slot];

			if (SrcResource == NULL && DestResource == NULL)
				continue;

			if (SrcResource != NULL && DestResource != SrcResource)
			{
				Dirty = true;
				DestResource = SrcResource;
			}
		}
	}
	else
	{
		for (ZEUInt I = 0; I < Count; I++)
		{
			ZEUInt Slot = Index + I;
			if (VertexBuffers[Slot] != NULL)
			{
				Dirty = true;
				VertexBuffers[Slot] = NULL;
			}
		}
	}

	return Dirty;
}

bool ZEGRContext::CheckIndexBuffer(const ZEGRBuffer* Buffer)
{
	bool Dirty = false;

	if (Buffer != NULL)
	{
		if (IndexBuffer != Buffer)
		{
			Dirty = true;
			IndexBuffer = Buffer;
		}
	}
	else
	{
		if (IndexBuffer != NULL)
		{
			Dirty = true;
			IndexBuffer = NULL;
		}
	}

	return Dirty;
}

bool ZEGRContext::CheckConstantBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRBuffer*const* Buffers)
{
	zeDebugCheck(Index >= ZEGR_MAX_CONSTANT_BUFFER_SLOT, "Constant buffer index exceeds the max slot count.");
	zeDebugCheck(Count > (ZEGR_MAX_CONSTANT_BUFFER_SLOT - Index), "Constant buffer count is too much.");

	bool Dirty = false;

	if (Buffers != NULL)
	{
		ZEUInt ShaderStart = 0;
		ZEUInt ShaderEnd = 0;
		if (Shader == ZEGR_ST_ALL)
		{
			ShaderStart = 0;
			ShaderEnd = ZEGR_SHADER_TYPE_COUNT;
		}
		else
		{
			ShaderStart = Shader;
			ShaderEnd = ShaderStart + 1;
		}

		for (ZEUInt J = ShaderStart; J < ShaderEnd; J++)
		{
			for (ZEUInt I = 0; I < Count; I++)
			{
				ZEUInt Slot = Index + I;
				const ZEGRResource* SrcResource = Buffers[I];
				const ZEGRResource*& DestResource = ConstantBuffers[J][Slot];

				if (SrcResource == NULL && DestResource == NULL)
					continue;

				if (SrcResource != NULL && DestResource != SrcResource)
				{
					Dirty = true;
					DestResource = SrcResource;
				}
			}
		}
	}

	return Dirty;
}

bool ZEGRContext::CheckSamplers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRSampler*const* Samplers)
{
	zeDebugCheck(Index >= ZEGR_MAX_SAMPLER_SLOT, "Sampler index exceeds the max slot count.");
	zeDebugCheck(Count > (ZEGR_MAX_SAMPLER_SLOT - Index), "Sampler count is too much.");

	bool Dirty = false;

	if (Samplers != NULL)
	{
		ZEUInt ShaderStart = 0;
		ZEUInt ShaderEnd = 0;
		if (Shader == ZEGR_ST_ALL)
		{
			ShaderStart = 0;
			ShaderEnd = ZEGR_SHADER_TYPE_COUNT;
		}
		else
		{
			ShaderStart = Shader;
			ShaderEnd = ShaderStart + 1;
		}

		for (ZEUInt J = ShaderStart; J < ShaderEnd; J++)
		{
			for (ZEUInt I = 0; I < Count; I++)
			{
				ZEUInt Slot = Index + I;
				const ZEGRSampler* SrcSampler = Samplers[I];
				const ZEGRSampler*& DestSampler = this->Samplers[J][Slot];

				if (SrcSampler == NULL && DestSampler == NULL)
					break;

				if (SrcSampler != NULL && DestSampler != SrcSampler)
				{
					Dirty = true;
					DestSampler = SrcSampler;
				}
			}
		}
	}

	return Dirty;
}

bool ZEGRContext::CheckShaderResources(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, const ZEGRResource*const* Resources)
{
	zeDebugCheck(Index >= ZEGR_MAX_TEXTURE_SLOT, "Texture index exceeds the max slot count.");
	zeDebugCheck(Count > (ZEGR_MAX_TEXTURE_SLOT - Index), "Texture count is too much.");

	bool Dirty = false;
	ZEUInt ShaderStart = 0;
	ZEUInt ShaderEnd = 0;
	if (Shader == ZEGR_ST_ALL)
	{
		ShaderStart = 0;
		ShaderEnd = ZEGR_SHADER_TYPE_COUNT;
	}
	else
	{
		ShaderStart = Shader;
		ShaderEnd = ShaderStart + 1;
	}

	if (Resources != NULL)
	{
		for (ZEUInt J = ShaderStart; J < ShaderEnd; J++)
		{
			for (ZEUInt I = 0; I < Count; I++)
			{
				ZEUInt Slot = Index + I;
				const ZEGRResource* SrcResource = Resources[I];
				const ZEGRResource*& DestResource = ShaderResources[J][Slot];

				if (SrcResource == NULL || DestResource == SrcResource)
					continue;

				Dirty = true;

				if (DestResource != NULL)
					const_cast<ZEGRResource*>(DestResource)->SetBoundStage((ZEGRShaderType)J, Slot, false);

				DestResource = SrcResource;

				if (DestResource->GetResourceType() == ZEGR_RT_TEXTURE)
				{
					ze_for_each(Stage, DestResource->GetBoundStages())
					{
						if (Stage->BoundAsShaderResource && Stage.GetIndex() == J)
							SetTextures((ZEGRShaderType)J, Stage->Slot, 1, NULL);

						else if (Stage->BoundAsUnorderedAccess)
							SetRWTextures(Stage->Slot, 1, NULL);
					}

					const ZEGRTexture* DestTexture = static_cast<const ZEGRTexture*>(DestResource);

					ze_for_each(RenderTarget, DestTexture->GetRenderTargets())
					{
						ZEGRRenderTarget* Item = RenderTarget.GetItem();
						if (Item != NULL && Item->GetBound())
							SetRenderTargets(0, NULL, this->DepthStencilBuffer);
					}
					
					ze_for_each(DepthStencilBuffer, DestTexture->GetDepthStencilBuffers())
					{
						ZEGRDepthStencilBuffer* Item = DepthStencilBuffer.GetItem();
						if (Item != NULL && Item->GetBound() && !Item->GetReadOnly())
							SetRenderTargets(ZEGR_MAX_RENDER_TARGET_SLOT, this->RenderTargets, NULL);
					}
				}
				else if (DestResource->GetResourceType() == ZEGR_RT_BUFFER)
				{
					ze_for_each(Stage, DestResource->GetBoundStages())
					{
						if (Stage->BoundAsShaderResource && Stage.GetIndex() == J)
							SetBuffers((ZEGRShaderType)J, Stage->Slot, 1, NULL);

						else if (Stage->BoundAsUnorderedAccess)
							SetRWBuffers(Stage->Slot, 1, NULL);
					}
				}

				const_cast<ZEGRResource*>(DestResource)->SetBoundStage((ZEGRShaderType)J, Slot);
			}
		}
	}
	else
	{
		for (ZEUInt J = ShaderStart; J < ShaderEnd; J++)
		{
			for (ZEUInt I = 0; I < Count; I++)
			{
				ZEUInt Slot = Index + I;
				const ZEGRResource*& Resource = ShaderResources[J][Slot];
				if (Resource != NULL)
				{
					Dirty = true;
					const_cast<ZEGRResource*>(Resource)->SetBoundStage((ZEGRShaderType)J, Slot, false);
					Resource = NULL;
				}
			}
		}
	}

	return Dirty;
}

bool ZEGRContext::CheckUnorderedAccesses(ZEUInt Index, ZEUInt Count, const ZEGRResource*const* Resources)
{
	zeDebugCheck(Index >= ZEGR_MAX_RWTEXTURE_SLOT, "RW Structured buffer index exceeds the max slot count.");
	zeDebugCheck(Count > (ZEGR_MAX_RWTEXTURE_SLOT - Index), "RW Structured buffer count is too much.");

	bool Dirty = false;

	if (Resources != NULL)
	{
		for (ZEUInt I = 0; I < Count; I++)
		{
			ZEUInt Slot = Index + I;
			const ZEGRResource* SrcResource = Resources[I];
			const ZEGRResource*& DestResource = UnorderedAccesses[Slot];

			if (SrcResource == NULL || DestResource == SrcResource)
				continue;

			Dirty = true;

			if (DestResource != NULL)
				const_cast<ZEGRResource*>(DestResource)->SetBoundStage(ZEGR_ST_COMPUTE, Slot, false, false);

			DestResource = SrcResource;

			if (DestResource->GetResourceType() == ZEGR_RT_TEXTURE)
			{
				ze_for_each(Stage, DestResource->GetBoundStages())
				{
					if (Stage->BoundAsShaderResource)
						SetTextures((ZEGRShaderType)Stage.GetIndex(), Stage->Slot, 1, NULL);

					else if (Stage->BoundAsUnorderedAccess)
						SetRWTextures(Stage->Slot, 1, NULL);
				}

				const ZEGRTexture* DestTexture = static_cast<const ZEGRTexture*>(DestResource);

				ze_for_each(RenderTarget, DestTexture->GetRenderTargets())
				{
					ZEGRRenderTarget* Item = RenderTarget.GetItem();
					if (Item != NULL && Item->GetBound())
						SetRenderTargets(0, NULL, this->DepthStencilBuffer);
				}

				ze_for_each(DepthStencilBuffer, DestTexture->GetDepthStencilBuffers())
				{
					ZEGRDepthStencilBuffer* Item = DepthStencilBuffer.GetItem();
					if (Item != NULL && Item->GetBound() && !Item->GetReadOnly())
						SetRenderTargets(ZEGR_MAX_RENDER_TARGET_SLOT, this->RenderTargets, NULL);
				}
			}
			else if (DestResource->GetResourceType() == ZEGR_RT_BUFFER)
			{
				ze_for_each(Stage, DestResource->GetBoundStages())
				{
					if (Stage->BoundAsShaderResource)
						SetBuffers((ZEGRShaderType)Stage.GetIndex(), Stage->Slot, 1, NULL);

					else if (Stage->BoundAsUnorderedAccess)
						SetRWBuffers(Stage->Slot, 1, NULL);
				}
			}

			const_cast<ZEGRResource*>(DestResource)->SetBoundStage(ZEGR_ST_COMPUTE, Slot, false, true);
		}
	}
	else
	{
		for (ZEUInt I = 0; I < Count; I++)
		{
			ZEUInt Slot = Index + I;

			const ZEGRResource*& Resource = UnorderedAccesses[Slot];
			if (Resource != NULL)
			{
				Dirty = true;
				const_cast<ZEGRResource*>(Resource)->SetBoundStage(ZEGR_ST_COMPUTE, Slot, false, false);
				Resource = NULL;
			}
		}
	}

	return Dirty;
}

bool ZEGRContext::CheckRenderTargetsAndDepthStencilBuffer(ZEUInt Count, const ZEGRRenderTarget*const* RenderTargets, const ZEGRDepthStencilBuffer* DepthStencilBuffer)
{
	zeDebugCheck(Count > ZEGR_MAX_RENDER_TARGET_SLOT, "RenderTarget count is too much.");

	bool DirtyRenderTarget = false;
	bool DirtyDepthStencilBuffer = false;

	if (RenderTargets != NULL)
	{
		for (ZEUInt I = 0; I < Count; I++)
		{
			const ZEGRRenderTarget* SrcRenderTarget = RenderTargets[I];
			const ZEGRRenderTarget*& DestRenderTarget = this->RenderTargets[I];

			if (SrcRenderTarget == NULL || DestRenderTarget == SrcRenderTarget)
				continue;

			DirtyRenderTarget = true;

			if (DestRenderTarget != NULL)
				const_cast<ZEGRRenderTarget*>(DestRenderTarget)->SetBound(false);

			DestRenderTarget = SrcRenderTarget;

			const ZEGRTexture* Texture = DestRenderTarget->GetOwner();
			if (Texture != NULL)//TODO: unnecessary anymore
			{
				ze_for_each(Stage, Texture->GetBoundStages())
				{
					if (Stage->BoundAsShaderResource)
						SetTextures((ZEGRShaderType)Stage.GetIndex(), Stage->Slot, 1, NULL);

					else if (Stage->BoundAsUnorderedAccess)
						SetRWTextures(Stage->Slot, 1, NULL);
				}
			}

			const_cast<ZEGRRenderTarget*>(DestRenderTarget)->SetBound(true);
		}

		if (DirtyRenderTarget)
		{
			for (ZEUInt I = Count; I < ZEGR_MAX_RENDER_TARGET_SLOT; I++)
			{
				if (this->RenderTargets[I] != NULL)
				{
					const_cast<ZEGRRenderTarget*>(this->RenderTargets[I])->SetBound(false);
					this->RenderTargets[I] = NULL;
				}
			}
		}
	}
	else
	{
		for (ZEUInt I = 0 ; I < ZEGR_MAX_RENDER_TARGET_SLOT; I++)
		{
			const ZEGRRenderTarget*& RenderTarget = this->RenderTargets[I];
			if (RenderTarget != NULL)
			{
				DirtyRenderTarget = true;
				const_cast<ZEGRRenderTarget*>(RenderTarget)->SetBound(false);
				RenderTarget = NULL;
			}
		}
	}

	if (DepthStencilBuffer != NULL)
	{
		const ZEGRDepthStencilBuffer* SrcDepthStencilBuffer = DepthStencilBuffer;
		const ZEGRDepthStencilBuffer*& DestDepthStencilBuffer = this->DepthStencilBuffer;

		if (DestDepthStencilBuffer != SrcDepthStencilBuffer)
		{
			DirtyDepthStencilBuffer = true;

			if (DestDepthStencilBuffer != NULL)
				const_cast<ZEGRDepthStencilBuffer*>(DestDepthStencilBuffer)->SetBound(false);

			DestDepthStencilBuffer = SrcDepthStencilBuffer;

			if (!DestDepthStencilBuffer->GetReadOnly())
			{
				const ZEGRTexture* Texture = DestDepthStencilBuffer->GetOwner();

				ze_for_each(Stage, Texture->GetBoundStages())
				{
					if (Stage->BoundAsShaderResource)
						SetTextures((ZEGRShaderType)Stage.GetIndex(), Stage->Slot, 1, NULL);

					else if (Stage->BoundAsUnorderedAccess)
						SetRWTextures(Stage->Slot, 1, NULL);
				}
			}

			const_cast<ZEGRDepthStencilBuffer*>(DestDepthStencilBuffer)->SetBound(true);
		}
	}
	else
	{
		if (this->DepthStencilBuffer != NULL)
		{
			DirtyDepthStencilBuffer = true;
			const_cast<ZEGRDepthStencilBuffer*>(this->DepthStencilBuffer)->SetBound(false);
			this->DepthStencilBuffer = NULL;
		}
	}

	return (DirtyRenderTarget | DirtyDepthStencilBuffer);
}

bool ZEGRContext::CheckViewports(ZEUInt Count, const ZEGRViewport* Viewports)
{
	zeDebugCheck(Count > ZEGR_MAX_VIEWPORT_SLOT, "Viewport count is too much.");

	bool Dirty = false;

	for (ZEUInt I = 0; I < Count; I++)
	{
		if (memcmp(&this->Viewports[I].StateData, &Viewports[I].StateData, sizeof(ZEGRViewport::ZEViewportData)) != 0)
		{
			Dirty = true;
			this->Viewports[I] = Viewports[I];
		}
	}

	return Dirty;
}

bool ZEGRContext::CheckScissorRects(ZEUInt Count, const ZEGRScissorRect* ScissorRects)
{
	zeDebugCheck(Count > ZEGR_MAX_SCISSOR_SLOT, "Scissor rect count is too much.");

	bool Dirty = false;

	for (ZESize I = 0; I < Count; I++)
	{
		if (memcmp(&this->ScissorRects[I].StateData, &ScissorRects[I].StateData, sizeof(ZEGRScissorRect::ZEScissorRectangleData) != 0))
		{
			Dirty = true;
			this->ScissorRects[I] = ScissorRects[I];
		}
	}

	return Dirty;
}

ZEGRContext::ZEGRContext()
{
	memset(VertexBuffers, NULL, sizeof(ZEGRResource*) * ZEGR_MAX_VERTEX_BUFFER_SLOT);
	IndexBuffer = NULL;
	memset(ConstantBuffers, NULL, sizeof(ZEGRResource*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
	memset(Samplers, NULL, sizeof(ZEGRSampler*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_SAMPLER_SLOT);
	memset(ShaderResources, NULL, sizeof(ZEGRResource*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_TEXTURE_SLOT);
	memset(UnorderedAccesses, NULL, sizeof(ZEGRResource*) * ZEGR_MAX_RWTEXTURE_SLOT);
	memset(RenderTargets, NULL, sizeof(ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);
	DepthStencilBuffer = NULL;
}

ZEGRContext::~ZEGRContext()
{
	memset(VertexBuffers, NULL, sizeof(ZEGRResource*) * ZEGR_MAX_VERTEX_BUFFER_SLOT);
	IndexBuffer = NULL;
	memset(ConstantBuffers, NULL, sizeof(ZEGRResource*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
	memset(Samplers, NULL, sizeof(ZEGRSampler*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_SAMPLER_SLOT);
	memset(ShaderResources, NULL, sizeof(ZEGRResource*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_TEXTURE_SLOT);
	memset(UnorderedAccesses, NULL, sizeof(ZEGRResource*) * ZEGR_MAX_RWTEXTURE_SLOT);
	memset(RenderTargets, NULL, sizeof(ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);
	DepthStencilBuffer = NULL;
}

void ZEGRContext::SetVertexBuffer(ZEUInt Index, const ZEGRBuffer* Buffer, const ZEUInt Offset)
{
	SetVertexBuffers(Index, 1, &Buffer, &Offset);
}

void ZEGRContext::SetConstantBuffer(ZEGRShaderType Shader, ZEUInt Index, const ZEGRBuffer* Buffer)
{
	SetConstantBuffers(Shader, Index, 1, &Buffer);
}

void ZEGRContext::GetConstantBuffers(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, ZEGRBuffer** Buffers)
{
	zeCheckError(Buffers == NULL, ZE_VOID, "Buffers out parameter cannot be NULL.");
	zeCheckError(Shader == ZEGR_ST_ALL, ZE_VOID, "Shader type cannot be ZEGR_ST_ALL.");

	memcpy(Buffers, &ConstantBuffers[Shader][Index], sizeof(ZEGRBuffer*) * Count);
}

void ZEGRContext::SetBuffer(ZEGRShaderType Shader, ZEUInt Index, const ZEGRBuffer* Buffer)
{
	SetBuffers(Shader, Index, 1, &Buffer);
}

void ZEGRContext::SetTexture(ZEGRShaderType Shader, ZEUInt Index, const ZEGRTexture* Texture)
{
	SetTextures(Shader, Index, 1, &Texture);
}

void ZEGRContext::GetTextures(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count, ZEGRTexture** Textures)
{
	zeCheckError(Textures == NULL, ZE_VOID, "Textures out parameter cannot be NULL");
	zeCheckError(Shader == ZEGR_ST_ALL, ZE_VOID, "Shader type cannot be ZEGR_ST_ALL");

	memcpy(Textures, &ShaderResources[Shader][Index], sizeof(ZEGRTexture*) * Count);
}

void ZEGRContext::SetSampler(ZEGRShaderType Shader, ZEUInt Index, const ZEGRSampler* Sampler)
{
	SetSamplers(Shader, Index, 1, &Sampler);
}

void ZEGRContext::GetRenderTargets(ZEUInt Count, ZEGRRenderTarget** RenderTargets)
{
	zeDebugCheck(Count > ZEGR_MAX_RENDER_TARGET_SLOT, "RenderTarget count is too much.");
	zeDebugCheck(RenderTargets == NULL, "Both render target out parameter cannot be NULL.");

	memcpy(RenderTargets, this->RenderTargets, sizeof(ZEGRRenderTarget*) * Count);
}

void ZEGRContext::GetDepthStencilBuffer(ZEGRDepthStencilBuffer** DepthStencilBuffer)
{
	zeDebugCheck(DepthStencilBuffer == NULL, "Depth-stencil buffer out parameter cannot be NULL.");

	*DepthStencilBuffer = const_cast<ZEGRDepthStencilBuffer*>(this->DepthStencilBuffer);
}

void ZEGRContext::SetRWBuffer(ZEUInt Index, const ZEGRBuffer*Buffer)
{
	SetRWBuffers(Index, 1, &Buffer);
}

void ZEGRContext::SetRWTexture(ZEUInt Index, const ZEGRTexture* Texture)
{
	SetRWTextures(Index, 1, &Texture);
}

void ZEGRContext::ClearShaderResources(ZEGRShaderType Shader, ZEUInt Index, ZEUInt Count)
{
	ZEUInt ShaderStart = 0;
	ZEUInt ShaderEnd = 0;
	if (Shader == ZEGR_ST_ALL)
	{
		ShaderStart = 0;
		ShaderEnd = ZEGR_SHADER_TYPE_COUNT;
	}
	else
	{
		ShaderStart = Shader;
		ShaderEnd = ShaderStart + 1;
	}

	for (ZEUInt J = ShaderStart; J < ShaderEnd; J++)
	{
		for (ZEUInt I = 0; I < Count; I++)
		{
			ShaderResources[J][Index + I] = NULL;
		}
	}
}

void ZEGRContext::ClearUnorderedAccesses(ZEUInt Index, ZEUInt Count)
{
	for (ZEUInt I = 0; I < Count; I++)
		UnorderedAccesses[Index + I] = NULL;
}

void ZEGRContext::ClearState()
{
	memset(VertexBuffers, NULL, sizeof(ZEGRResource*) * ZEGR_MAX_VERTEX_BUFFER_SLOT);
	IndexBuffer = NULL;
	memset(ConstantBuffers, NULL, sizeof(ZEGRResource*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_CONSTANT_BUFFER_SLOT);
	memset(Samplers, NULL, sizeof(ZEGRSampler*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_SAMPLER_SLOT);
	memset(ShaderResources, NULL, sizeof(ZEGRResource*) * ZEGR_SHADER_TYPE_COUNT * ZEGR_MAX_TEXTURE_SLOT);
	memset(UnorderedAccesses, NULL, sizeof(ZEGRResource*) * ZEGR_MAX_RWTEXTURE_SLOT);
	memset(RenderTargets, NULL, sizeof(ZEGRRenderTarget*) * ZEGR_MAX_RENDER_TARGET_SLOT);
	DepthStencilBuffer = NULL;

	memset(&Viewports, 0, sizeof(ZEGRViewport) * ZEGR_MAX_VIEWPORT_SLOT);
	memset(&ScissorRects, 0, sizeof(ZEGRScissorRect) * ZEGR_MAX_SCISSOR_SLOT);
}
