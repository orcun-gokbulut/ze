//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageDebug.cpp
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

#include "ZERNStageDebug.h"

#include "ZERNStageID.h"
#include "ZEDS/ZEString.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZEModel/ZEMDResource.h"
#include "ZERNCommand.h"
#include "ZEGame/ZEEntity.h"
#include "ZEModel/ZEModel.h"
#include "ZERNRenderParameters.h"
#include "ZEMath/ZEAngle.h"

#define ZERN_SDDF_SHADERS			1
#define ZERN_SDDF_RENDER_STATES		2
#define ZERN_SDDF_CONSTANT_BUFFERS	4
#define ZERN_SDDF_TEXTURE			8

bool ZERNStageDebug::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_SDDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNDebug.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNDebug_VertexShader_Main";
	VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_GEOMETRY;
	Options.EntryPoint = "ZERNDebug_GeometryShader_Main";
	GeometryShader = ZEGRShader::Compile(Options);
	zeCheckError(GeometryShader == NULL, false, "Cannot set geometry shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNDebug_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Cannot set pixel shader.");

	Options.Definitions.Add(ZEGRShaderDefinition("ZERN_DEBUG_SKIN_TRANSFORM"));
	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNDebug_VertexShader_Main";
	SkinnedVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(SkinnedVertexShader == NULL, false, "Cannot set skinned vertex shader.");
	Options.Definitions.Clear();

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNDebug_Canvas_VertexShader_Main";
	CanvasVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(CanvasVertexShader == NULL, false, "Cannot set vertex shader.");

	DirtyFlags.UnraiseFlags(ZERN_SDDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SDDF_RENDER_STATES);

	return true;
}

bool ZERNStageDebug::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_SDDF_RENDER_STATES))
		return true;

	ZEGRRenderState RenderState;
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetVertexLayout(ZEMDVertex::GetVertexLayout());
	
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_GEOMETRY, GeometryShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);
	
	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set debug render state.");
	
	RenderState.SetVertexLayout(ZEMDVertexSkin::GetVertexLayout());
	RenderState.SetShader(ZEGR_ST_VERTEX, SkinnedVertexShader);
	SkinnedRenderStateData = RenderState.Compile();
	zeCheckError(SkinnedRenderStateData == NULL, false, "Cannot set skinned debug render state.");

	RenderState.SetPrimitiveType(ZEGR_PT_LINE_LIST);
	RenderState.SetVertexLayout(*ZECanvasVertex::GetVertexLayout());

	RenderState.SetShader(ZEGR_ST_VERTEX, CanvasVertexShader);
	RenderState.SetShader(ZEGR_ST_GEOMETRY, NULL);

	LineRenderStateData = RenderState.Compile();
	zeCheckError(LineRenderStateData == NULL, false, "Cannot set line debug render state.");

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);

	ZEGRRasterizerState RasterizerStateNoCull;
	RasterizerStateNoCull.SetCullMode(ZEGR_CMD_NONE);
	RenderState.SetRasterizerState(RasterizerStateNoCull);

	TriangleRenderStateData = RenderState.Compile();
	zeCheckError(TriangleRenderStateData == NULL, false, "Cannot set triangle debug render state.");

	DirtyFlags.UnraiseFlags(ZERN_SDDF_RENDER_STATES);

	return true;
}

bool ZERNStageDebug::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_SDDF_CONSTANT_BUFFERS))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);

	return true;
}

bool ZERNStageDebug::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

void ZERNStageDebug::ProcessCommands(ZEGRContext* Context)
{
	ZERNRenderParameters RenderParameters;
	RenderParameters.Context = Context;
	RenderParameters.Renderer = GetRenderer();
	RenderParameters.Stage = this;

	ze_for_each(Command, GetCommands())
	{
		ZEEntity* Entity = Command->Entity;
		if (Entity != NULL)
		{
			if (Entity->GetClass() == ZEModel::Class() && !Constants.ShowBoundingBox)
			{
				ZEModel* Model = static_cast<ZEModel*>(Entity);
				bool Skinned = Model->GetBones().GetCount() > 0;

				Context->SetRenderState(Skinned ? SkinnedRenderStateData : RenderStateData);

				RenderParameters.Command = Command.GetPointer();
				Command->Execute(&RenderParameters);
			}

			GatherVertices(Entity);
		}
	}

	if (LineCanvas.GetVertexCount() > 0)
	{
		if (LineVertexBuffer == NULL || 
			LineVertexBuffer->GetElementCount() < LineCanvas.GetVertexCount())
			LineVertexBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_VERTEX_BUFFER, LineCanvas.GetBufferSize(), sizeof(ZECanvasVertex), ZEGR_RU_DYNAMIC, ZEGR_RBF_VERTEX_BUFFER, ZEGR_TF_NONE, LineCanvas.GetBuffer());
		else
		{
			void* Buffer;
			LineVertexBuffer->Map(ZEGR_RMT_WRITE_DISCARD, &Buffer);
			memcpy(Buffer, LineCanvas.GetBuffer(), LineCanvas.GetBufferSize());
			LineVertexBuffer->Unmap();
			//LineVertexBuffer->Update(LineCanvas.GetBuffer(), LineCanvas.GetBufferSize(), 0, LineCanvas.GetBufferSize());
		}

		Context->SetRenderState(LineRenderStateData);

		Context->SetVertexBuffer(0, LineVertexBuffer);
		Context->Draw(LineCanvas.GetVertexCount(), 0);
	}

	if (TriangleCanvas.GetVertexCount() > 0)
	{
		if (TriangleVertexBuffer == NULL || 
			TriangleVertexBuffer->GetElementCount() < TriangleCanvas.GetVertexCount())
			TriangleVertexBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_VERTEX_BUFFER, TriangleCanvas.GetBufferSize(), sizeof(ZECanvasVertex), ZEGR_RU_STATIC, ZEGR_RBF_VERTEX_BUFFER, ZEGR_TF_NONE, TriangleCanvas.GetBuffer());
		else
			TriangleVertexBuffer->Update(TriangleCanvas.GetBuffer(), TriangleCanvas.GetBufferSize(), 0, TriangleCanvas.GetBufferSize());
	
		Context->SetRenderState(TriangleRenderStateData);

		Context->SetVertexBuffer(0, TriangleVertexBuffer);
		Context->Draw(TriangleCanvas.GetVertexCount(), 0);
	}

	LineCanvas.Clean();
	TriangleCanvas.Clean();
}

void ZERNStageDebug::GatherVertices(ZEEntity* Entity)
{
	if (Constants.ShowBoundingBox)
	{
		ZEAABBox EntityAABB = Entity->GetWorldBoundingBox();

		LineCanvas.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		LineCanvas.SetTranslation(EntityAABB.GetCenter());
		ZEVector3 WidthHeightLength = EntityAABB.Max - EntityAABB.Min;
		LineCanvas.AddWireframeBox(WidthHeightLength.x, WidthHeightLength.y, WidthHeightLength.z);

		if (Entity->GetClass() == ZEModel::Class())
		{
			ZEModel* Model = static_cast<ZEModel*>(Entity);

			LineCanvas.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
			ze_for_each(Mesh, Model->GetMeshes())
			{
				ZEAABBox MeshAABB = Mesh->GetWorldBoundingBox();
				LineCanvas.SetTranslation(MeshAABB.GetCenter());
				WidthHeightLength = MeshAABB.Max - MeshAABB.Min;
				LineCanvas.AddWireframeBox(WidthHeightLength.x, WidthHeightLength.y, WidthHeightLength.z);
			}

			LineCanvas.SetColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
			ze_for_each(Bone, Model->GetBones())
			{
				ZEAABBox BoneAABB = Bone->GetWorldBoundingBox();
				LineCanvas.SetTranslation(BoneAABB.GetCenter());
				WidthHeightLength = BoneAABB.Max - BoneAABB.Min;
				LineCanvas.AddWireframeBox(WidthHeightLength.x, WidthHeightLength.y, WidthHeightLength.z);
			}
		}
	}
	else
	{
		if (Entity->GetClass() == ZEModel::Class())
		{
			LineCanvas.ResetTransforms();
			TriangleCanvas.ResetTransforms();

			ZEModel* Model = static_cast<ZEModel*>(Entity);

			ze_for_each(Bone, Model->GetBones())
			{
				ZEVector3 LineStart = Bone->GetWorldPosition();

				TriangleCanvas.SetColor(ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));
				TriangleCanvas.SetTranslation(LineStart);
				TriangleCanvas.AddSphere(0.05f, 4, 1);

				LineCanvas.SetColor(ZEVector4(1.0f, 0.0f, 1.0f, 1.0f));
				ze_for_each(ChildBone, Bone->GetChildBones())
				{
					ZEVector3 LineEnd = ChildBone->GetWorldPosition();

					LineCanvas.AddPoint(LineStart);
					LineCanvas.AddPoint(LineEnd);
				}
			}

			TriangleCanvas.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
			ze_for_each(Mesh, Model->GetMeshes())
			{
				for (ZESize I = 0; I < Mesh->GetClippingPlaneCount(); I++)
				{
					ZEVector3 PositionWorld;
					ZEVector3 NormalWorld;
					ZEMatrix4x4::Transform(PositionWorld, Mesh->GetWorldTransform(), Mesh->GetClippingPlane(I).p);
					ZEMatrix4x4::Transform3x3(NormalWorld, Mesh->GetWorldTransform(), Mesh->GetClippingPlane(I).n);
					ZEVector3::Normalize(NormalWorld, NormalWorld);

					ZEQuaternion RotationWorld;
					ZEQuaternion::CreateFromDirection(RotationWorld, NormalWorld);
					TriangleCanvas.SetRotation(RotationWorld * ZEQuaternion(ZE_PI_2, ZEVector3::UnitX));
					TriangleCanvas.SetTranslation(PositionWorld);
					TriangleCanvas.AddPlane(10.0f, 10.0f);

					LineCanvas.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
					LineCanvas.AddPoint(PositionWorld);
					LineCanvas.AddPoint(PositionWorld + NormalWorld * 1.0f);
				}
			}
		}
	}
}

bool ZERNStageDebug::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	return Update();
}

bool ZERNStageDebug::DeinitializeInternal()
{
	VertexShader.Release();
	GeometryShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	SkinnedVertexShader.Release();
	SkinnedRenderStateData.Release();

	LineVertexBuffer.Release();
	TriangleVertexBuffer.Release();

	CanvasVertexShader.Release();
	LineRenderStateData.Release();
	TriangleRenderStateData.Release();

	DepthMap.Release();
	OutputTexture.Release();

	return ZERNStage::DeinitializeInternal();
}

ZEInt ZERNStageDebug::GetId() const
{
	return ZERN_STAGE_DEBUG;
}

const ZEString& ZERNStageDebug::GetName() const
{
	static const ZEString Name = "Stage Debug";
	return Name;
}

void ZERNStageDebug::SetShowNormalVectors(bool ShowNormalVectors)
{
	if (Constants.ShowNormalVectors == (ZEBool32)ShowNormalVectors)
		return;

	Constants.ShowNormalVectors = (ZEBool32)ShowNormalVectors;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);
}

bool ZERNStageDebug::GetShowNormalVectors() const
{
	return (bool)Constants.ShowNormalVectors;
}

void ZERNStageDebug::SetShowBoundingBox(bool ShowBoundingBox)
{
	if (Constants.ShowBoundingBox == (ZEBool32)ShowBoundingBox)
		return;

	Constants.ShowBoundingBox = (ZEBool32)ShowBoundingBox;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);
}

bool ZERNStageDebug::GetShowBoundingBox() const
{
	return (bool)Constants.ShowBoundingBox;
}

void ZERNStageDebug::SetShowWireframe(bool ShowWireframe)
{
	if (Constants.ShowWireframe == (ZEBool32)ShowWireframe)
		return;

	Constants.ShowWireframe = (ZEBool32)ShowWireframe;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);
}

bool ZERNStageDebug::GetShowWireframe() const
{
	return (bool)Constants.ShowWireframe;
}

void ZERNStageDebug::SetCullBackface(bool CullBackface)
{
	if (Constants.CullBackface == (ZEBool32)CullBackface)
		return;

	Constants.CullBackface = (ZEBool32)CullBackface;

	DirtyFlags.RaiseFlags(ZERN_SDDF_CONSTANT_BUFFERS);
}

bool ZERNStageDebug::GetCullBackface() const
{
	return (bool)Constants.CullBackface;
}

void ZERNStageDebug::Resized(ZEUInt Width, ZEUInt Height)
{
	DepthMap = ZEGRTexture::CreateResource(ZEGR_TT_2D, Width, Height, 1, ZEGR_TF_D24_UNORM_S8_UINT, ZEGR_RU_STATIC, ZEGR_RBF_DEPTH_STENCIL);
}

bool ZERNStageDebug::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (!Update())
		return false;

	const ZEGRDepthStencilBuffer* DepthStencilBuffer = DepthMap->GetDepthStencilBuffer();
	Context->ClearDepthStencilBuffer(DepthStencilBuffer, true, true, 0.0f, 0x00);

	const ZEGRRenderTarget* RenderTarget = OutputTexture->GetRenderTarget();

	Context->SetConstantBuffer(ZEGR_ST_GEOMETRY, 8, ConstantBuffer);
	Context->SetRenderTargets(1, &RenderTarget, DepthStencilBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, RenderTarget->GetWidth(), RenderTarget->GetHeight()));
	
	ProcessCommands(Context);

	return false;
}

void ZERNStageDebug::CleanUp(ZEGRContext* Context)
{
	ZERNStage::CleanUp(Context);
}

ZERNStageDebug::ZERNStageDebug()
{
	DirtyFlags.RaiseAll();

	Constants.ShowNormalVectors = false;
	Constants.ShowBoundingBox = false;
	Constants.ShowWireframe = true;
	Constants.CullBackface = false;

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&OutputTexture), "ColorTexture", ZERN_SRUT_WRITE, ZERN_SRCF_GET_OUTPUT);
}
