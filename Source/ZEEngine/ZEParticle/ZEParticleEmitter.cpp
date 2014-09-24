//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleEmitter.cpp
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

#include "ZEParticleEffect.h"
#include "ZEParticleEmitter.h"
#include "ZEParticleModifier.h"
#include "ZEMath/ZEAngle.h"
#include "ZERandom.h"

#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZEVertexTypes.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGame/ZEScene.h"

void ZEParticleEmitter::Tick(float TimeElapsed)
{
	AliveParticleCount = 0;

	if(IsContinuous == false)
	{
		if(EmittedParticleCount >= MaxParticleCount)
		{
			for(ZESize I = 0; I < ParticlePool.GetCount(); I++)
			{
				if(ParticlePool[I].State != ZE_PAS_DEAD)
				{
					continue;
				}
				else
				{
					ParticlesPerSecond = 0;
				}
			}
		}
	}

	ParticlesRemaining += ParticlesPerSecond * TimeElapsed;
	ZEUInt ParticeCountForTimeElapsed = (ZEUInt)ParticlesRemaining;
	ParticlesRemaining -= ParticeCountForTimeElapsed;

	ZEUInt ParticlesEmmitedThisFrame = 0;

	for (ZESize I = 0; I < ParticlePool.GetCount(); I++)
	{
		if(ParticlePool[I].State == ZE_PAS_NEW)
		{
			ParticlePool[I].State = ZE_PAS_ALIVE;
		}
	}

	if(ParticeCountForTimeElapsed != 0)
	{
		for (ZESize I = 0; I < ParticlePool.GetCount(); I++)
		{
			if(ParticlePool[I].State == ZE_PAS_DEAD)
			{
				GenerateParticle(ParticlePool[I]);
				EmittedParticleCount++;
				ParticlesEmmitedThisFrame++;
			}

			if(ParticlesEmmitedThisFrame == ParticeCountForTimeElapsed)
				break;
		}
	}

	for(ZESize I = 0; I < ParticlePool.GetCount(); I++)
	{
		if(ParticlePool[I].State == ZE_PAS_ALIVE)
		{
			ParticlePool[I].Life -= TimeElapsed;
			AliveParticleCount++;
		}

		if(ParticlePool[I].State != ZE_PAS_DEAD  && ParticlePool[I].Life < 0.0f)
		{
			ParticlePool[I].State = ZE_PAS_DEAD;
		}
	}

	for (ZESize I = 0; I < Modifiers.GetCount(); I++)
	{
		Modifiers[I]->Tick(TimeElapsed);
	}

	if(IsSortingEnabled)
	{
		SortParticles();
	}
	
	//zeLog("APC : %u", AliveParticleCount);
}

void ZEParticleEmitter::InitializeParticlePool()
{
	ParticlePool.SetCount((ZESize)MaxParticleCount);

	for(ZESize I = 0; I < (ZESize)MaxParticleCount; I++)
	{
		GenerateParticle(ParticlePool[I]);
	}
}

void ZEParticleEmitter::GenerateParticle(ZEParticle &Particle)
{
	if(IsEmmiterLocal)
	{
		switch(Type)
		{
			case ZE_PET_POINT:
			{
				Particle.Position = Position;
				break;
			}
			case ZE_PET_PLANE:
			{
				Particle.Position.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-PlaneSize.x / 2.0f, PlaneSize.x / 2.0f);
				Particle.Position.y = Position.y;
				Particle.Position.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-PlaneSize.y / 2.0f, PlaneSize.y / 2.0f);
				break;
			}
			case ZE_PET_BOX:
			{
				Particle.Position.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-BoxSize.x / 2.0f, BoxSize.x / 2.0f);
				Particle.Position.y = Position.y + RAND_BETWEEN_TWO_FLOAT(-BoxSize.y / 2.0f, BoxSize.y / 2.0f);
				Particle.Position.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-BoxSize.z / 2.0f, BoxSize.z / 2.0f);
				break;
			}
			case ZE_PET_TORUS:
			{
				float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float TubeRadius = RAND_BETWEEN_TWO_FLOAT(0.0f, TorusSize.y);
				Particle.Position.x = Position.x + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Cos(Theta);
				Particle.Position.y = Position.y + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Sin(Theta);
				Particle.Position.z = Position.z + TubeRadius * ZEAngle::Sin(Phi);
				break;
			}
			case ZE_PET_SPHERE:
			{
				float Radius = RAND_BETWEEN_TWO_FLOAT(0.0f, SphereRadius);
				float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, ZE_PI);
				Particle.Position.x = Position.x + Radius * ZEAngle::Cos(Theta) * ZEAngle::Sin(Phi);
				Particle.Position.y = Position.y + Radius * ZEAngle::Sin(Theta) * ZEAngle::Sin(Phi);
				Particle.Position.z = Position.z + Radius * ZEAngle::Cos(Phi);
				break;
			}
		}
	}
	else
	{
		ZEVector3 TempPosition;

		switch(Type)
		{
			case ZE_PET_POINT:
			{
				TempPosition = Position;
				break;
			}
			case ZE_PET_PLANE:
			{		
				TempPosition.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-PlaneSize.x / 2, PlaneSize.x / 2);
				TempPosition.y = Position.y;
				TempPosition.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-PlaneSize.y / 2, PlaneSize.y / 2);
				break;
			}
			case ZE_PET_BOX:
			{
				TempPosition.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-BoxSize.x / 2, BoxSize.x / 2);
				TempPosition.y = Position.y + RAND_BETWEEN_TWO_FLOAT(-BoxSize.y / 2, BoxSize.y / 2);
				TempPosition.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-BoxSize.z / 2, BoxSize.z / 2);
				break;
			}
			case ZE_PET_TORUS:
			{
				float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float TubeRadius = RAND_BETWEEN_TWO_FLOAT(0.0f, TorusSize.y);
				TempPosition.x = Position.x + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Cos(Theta);
				TempPosition.y = Position.y + (TorusSize.x + TubeRadius * ZEAngle::Cos(Phi)) * ZEAngle::Sin(Theta);
				TempPosition.z = Position.z + TubeRadius * ZEAngle::Sin(Phi);		
				break;
			}
			case ZE_PET_SPHERE:
			{
				float Radius = RAND_BETWEEN_TWO_FLOAT(0.0f, SphereRadius);
				float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, ZE_PI);
				TempPosition.x = Position.x + Radius * ZEAngle::Cos(Theta) * ZEAngle::Sin(Phi);
				TempPosition.y = Position.y + Radius * ZEAngle::Sin(Theta) * ZEAngle::Sin(Phi);
				TempPosition.z = Position.z + Radius * ZEAngle::Cos(Phi);		
				break;
			}
		}

		Particle.Position = GetOwner()->GetWorldTransform() * TempPosition;
	}

	Particle.TotalLife					= RAND_BETWEEN_TWO_FLOAT(MinLife, MaxLife);
	Particle.Life						= Particle.TotalLife;

	if(IsParticleFixedAspectRatio)
	{
		float ParticleAspectRatio		= MaxSize.x / MaxSize.y;
		Particle.Size2D.x				= RAND_BETWEEN_TWO_FLOAT(MinSize.x, MaxSize.x);
		Particle.Size2D.y				= Particle.Size2D.x  / ParticleAspectRatio;
	}				
	else
	{
		Particle.Size2D.x				= RAND_BETWEEN_TWO_FLOAT(MinSize.x, MaxSize.x);
		Particle.Size2D.y				= RAND_BETWEEN_TWO_FLOAT(MinSize.y, MaxSize.y);
	}

	Particle.InitialColor.x				= RAND_BETWEEN_TWO_FLOAT(MinColor.x, MaxColor.x);
	Particle.InitialColor.y				= RAND_BETWEEN_TWO_FLOAT(MinColor.y, MaxColor.y);
	Particle.InitialColor.z				= RAND_BETWEEN_TWO_FLOAT(MinColor.z, MaxColor.z);
	Particle.InitialColor.w				= RAND_BETWEEN_TWO_FLOAT(MinColor.w, MaxColor.w);
	Particle.Color						= Particle.InitialColor;
	Particle.Rotation					= 0.0f;
	Particle.Cos_NegSin					= ZEVector2::UnitY;
	Particle.MinTexCoord				= ZEVector2::Zero;
	Particle.MaxTexCoord				= ZEVector2::One;
	Particle.State						= ZE_PAS_NEW;

}

void ZEParticleEmitter::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEParticleEmitter::GetName() const
{
	return Name;
}

void ZEParticleEmitter::SetOwner(ZEParticleEffect *Owner)
{
	this->Owner = Owner;
}

ZEParticleEffect* ZEParticleEmitter::GetOwner() const
{
	return Owner;
}

void ZEParticleEmitter::SetEmmiterLocal(bool IsLocal)
{
	IsEmmiterLocal = IsLocal;
}

bool ZEParticleEmitter::GetIsEmmiterLocal() const
{
	return IsEmmiterLocal;
}

void ZEParticleEmitter::SetSortingEnabled(bool IsSorted)
{
	IsSortingEnabled = IsSorted;
}

bool ZEParticleEmitter::GetSortingEnabled() const
{
	return IsSortingEnabled;
}

void ZEParticleEmitter::SetParticleFixedAspectRatio(bool IsParticleFixedAspectRatio)
{
	this->IsParticleFixedAspectRatio = IsParticleFixedAspectRatio;
}

bool ZEParticleEmitter::GetParticleFixedAspectRatio() const
{
	return IsParticleFixedAspectRatio;
}

ZEArray<ZEParticle>& ZEParticleEmitter::GetParticlePool()
{
	return ParticlePool;
}

void ZEParticleEmitter::SetParticlesPerSecond(ZEUInt Value)
{
	ParticlesPerSecond = Value;
}

ZEUInt ZEParticleEmitter::GetParticlesPerSecond() const
{
	return ParticlesPerSecond;
}

ZEUInt ZEParticleEmitter::GetAliveParticleCount() const
{
	return AliveParticleCount;
}

void ZEParticleEmitter::SetPosition(const ZEVector3& EmitterPosition)
{
	Position = EmitterPosition;
}

const ZEVector3& ZEParticleEmitter::GetPosition() const
{
	return Position;
}

const ZEAABBox&	ZEParticleEmitter::GetBoundingBox()
{
	return BoundingBox;
}

const ZEArray<ZEParticleModifier*>& ZEParticleEmitter::GetModifiers()
{
	return Modifiers;
}

void ZEParticleEmitter::AddModifier(ZEParticleModifier* Modifier)
{
	if(Modifier != NULL)
		Modifier->SetOwner(this);

	if(!Modifiers.Exists(Modifier))
		Modifiers.Add(Modifier);
}

void ZEParticleEmitter::RemoveModifier(ZEParticleModifier* Modifier)
{
	if (Modifiers.Exists(Modifier))
		Modifiers.RemoveValue(Modifier);
}

void ZEParticleEmitter::SetSphereRadius(float SphereRadius)
{
	this->SphereRadius = SphereRadius;
}

float ZEParticleEmitter::GetSphereRadius() const
{
	return SphereRadius;
}

void ZEParticleEmitter::SetTorusSize(const ZEVector2& TorusRadius)
{
	this->TorusSize = TorusRadius;
}

const ZEVector2& ZEParticleEmitter::GetTorusSize() const
{
	return TorusSize;
}

void ZEParticleEmitter::SetPlaneSize(const ZEVector2& PlaneSize)
{
	this->PlaneSize = PlaneSize;
}

const ZEVector2& ZEParticleEmitter::GetPlaneSize() const
{
	return PlaneSize;
}

void ZEParticleEmitter::SetBoxSize(const ZEVector3& BoxSize)
{
	this->BoxSize = BoxSize;
}

const ZEVector3& ZEParticleEmitter::GetBoxSize() const
{
	return BoxSize;
}

void ZEParticleEmitter::SetType(ZEParticleEmitterType EmitterType)
{
	Type = EmitterType;
}

ZEParticleEmitterType ZEParticleEmitter::GetType() const
{
	return Type;
}

void ZEParticleEmitter::SetMinColor(const ZEVector4& Color)
{
	MinColor = Color;
}

const ZEVector4& ZEParticleEmitter::GetMinColor() const
{
	return MinColor;
}

void ZEParticleEmitter::SetMaxColor(const ZEVector4& Color)
{
	MaxColor = Color;
}

const ZEVector4& ZEParticleEmitter::GetMaxColor() const
{
	return MaxColor;
}

void ZEParticleEmitter::Reset()
{
	if (IsContinuous == false)
		EmittedParticleCount = 0;
}

void ZEParticleEmitter::SetContinuity(bool Value)
{
	IsContinuous = Value;
}

bool ZEParticleEmitter::GetContinuity() const
{
	return IsContinuous;
}

void ZEParticleEmitter::SetMinLife(float Life)
{
	MinLife = Life;
}

float ZEParticleEmitter::GetMinLife() const
{
	return MinLife;
}

void ZEParticleEmitter::SetMaxLife(float Life)
{
	MaxLife = Life;
}

float ZEParticleEmitter::GetMaxLife() const
{
	return MaxLife;
}

void ZEParticleEmitter::SetMaxParticleCount(ZEUInt Value)
{
	MaxParticleCount = Value;

	ZESize OldSize = ParticlePool.GetCount();
	ParticlePool.Resize((ZESize)Value);
	SortArray.Resize((ZESize)Value);
	for (ZESize I = OldSize; I < (ZESize)MaxParticleCount; I++)
	{
		GenerateParticle(ParticlePool[I]);
	}

}

ZEUInt ZEParticleEmitter::GetMaxParticleCount() const
{
	return MaxParticleCount;
}



void ZEParticleEmitter::SetMinSize(const ZEVector2& Size)
{
	MinSize = Size;
}

const ZEVector2& ZEParticleEmitter::GetMinSize() const
{
	return MinSize;
}

void ZEParticleEmitter::SetMaxSize(const ZEVector2& Size)
{
	MaxSize = Size;
}

const ZEVector2& ZEParticleEmitter::GetMaxSize() const
{
	return MaxSize;
}

void ZEParticleEmitter::SetBillboardType(ZEParticleBillboardType Type)
{
	BillboardType = Type;
}

ZEParticleBillboardType ZEParticleEmitter::GetBillboardType() const
{
	return BillboardType;
}

void ZEParticleEmitter::SetAxisOrientation(const ZEVector3& Orientation)
{
	AxisOrientation = Orientation;
}

const ZEVector3& ZEParticleEmitter::GetAxisOrientation() const
{
	return AxisOrientation;
}

void ZEParticleEmitter::SetMaterial(ZEMaterial *Material)
{
	this->Material = Material;
}

ZEMaterial* ZEParticleEmitter::GetMaterial() const
{
	return Material;
}

ZEParticleEmitter::ZEParticleEmitter()
{
	EmittedParticleCount = 0;
	MaxParticleCount = 0;
	Owner = NULL;
	ParticlesRemaining = 0.0f;
	IsEmmiterLocal = false;
	IsSortingEnabled = false;
	IsParticleFixedAspectRatio = false;
	AliveParticleCount = 0;
			
	ParticlesPerSecond = 0;				
	Position = ZEVector3(0.0f, 0.0f, 0.0f);						
	Type = ZE_PET_POINT;				
	MinColor = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);					
	MaxColor = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);
	
	MinSize = ZEVector2::One;					
	MaxSize = ZEVector2::One;					
	MinLife = 0.0f;					
	MaxLife = 0.0f;							
	MaxParticleCount = 0;			
	IsContinuous = true;		

	BoxSize = ZEVector3::One;
	SphereRadius = 1.0f;
	TorusSize = ZEVector2::One;
	PlaneSize = ZEVector2::One;

	VertexBuffer = NULL;
	Material = NULL;

	RenderCommand.SetZero();
	RenderCommand.Priority = 4;
	RenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_NO_Z_WRITE;
	RenderCommand.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
	RenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE;
	
	BillboardType = ZE_PBT_SCREEN_ALIGNED;
	AxisOrientation = -ZEVector3::UnitZ;
}

ZEParticleEmitter::~ZEParticleEmitter()
{
}


static void DrawParticle(ZESimpleVertex* Buffer, const ZEParticle* Particle, const ZEVector3& Right, const ZEVector3& Up)
{
	ZEVector2 ParticleSize_2 = Particle->Size2D * 0.5f;

	ZEVector3 PU, NU, PV, NV;
	ZEVector3::Scale(PV, Right, ParticleSize_2.y);
	ZEVector3::Scale(NV, Right, -ParticleSize_2.y);
	ZEVector3::Scale(PU, Up, ParticleSize_2.x);
	ZEVector3::Scale(NU, Up, -ParticleSize_2.x);


	Buffer[0].Position = Particle->Position + NV + PU;
	Buffer[0].Texcoord = ZEVector2(Particle->MinTexCoord.x, Particle->MinTexCoord.y);
	Buffer[0].Color = Particle->Color;

	Buffer[1].Position = Particle->Position + PV + PU;
	Buffer[1].Texcoord = ZEVector2(Particle->MaxTexCoord.x, Particle->MinTexCoord.y);
	Buffer[1].Color = Particle->Color;

	Buffer[2].Position = Particle->Position + PV + NU;
	Buffer[2].Texcoord = ZEVector2(Particle->MaxTexCoord.x, Particle->MaxTexCoord.y);
	Buffer[2].Color = Particle->Color;

	Buffer[3].Position = Buffer[2].Position;
	Buffer[3].Texcoord = Buffer[2].Texcoord;
	Buffer[3].Color = Particle->Color;

	Buffer[4].Position = Particle->Position + NV + NU;
	Buffer[4].Texcoord = ZEVector2(Particle->MinTexCoord.x, Particle->MaxTexCoord.y);
	Buffer[4].Color = Particle->Color;

	Buffer[5].Position = Buffer[0].Position;
	Buffer[5].Texcoord = Buffer[0].Texcoord;
	Buffer[5].Color = Particle->Color;
}

void ZEParticleEmitter::UpdateVertexBuffer(ZEDrawParameters* DrawParameters)
{
	ZESize ParticleCount = ParticlePool.GetCount();

	if (VertexBuffer == NULL)
		VertexBuffer = ZEStaticVertexBuffer::CreateInstance();

	if (VertexBuffer->GetBufferSize() != ParticlePool.GetCount() * sizeof(ZESimpleVertex) * 6)
	{
		if (!VertexBuffer->Create(ParticlePool.GetCount() * sizeof(ZESimpleVertex) * 6))
		{
			zeError("Could not create particle vertex buffer.");
			return;
		}
	}

	RenderCommand.PrimitiveCount = 0;

	ZESimpleVertex* Buffer = (ZESimpleVertex*)VertexBuffer->Lock();
	if (Buffer == NULL)
	{
		zeError("Could not lock particle vertex buffer.");
		return;
	}

	ZEVector3 CameraRight = DrawParameters->View->Camera->GetWorldRight();
	ZEVector3 CameraUp = DrawParameters->View->Camera->GetWorldUp();
	ZEVector3 CameraLook = DrawParameters->View->Camera->GetWorldFront();

	if (BillboardType == ZE_PBT_AXIS_ORIENTED)
	{
		ZESize VertexIndex = 0;
		for (ZESize N = 0; N < ParticleCount; N++)
		{
			ZEParticle* Particle = &ParticlePool[ParticleCount - N - 1];
			if (Particle->State != ZE_PAS_DEAD)
			{
				ZEVector3 RightVec;
				ZEVector3::CrossProduct(RightVec, ZEVector3(Particle->Cos_NegSin, 0.0f), AxisOrientation);
				DrawParticle(Buffer + VertexIndex, Particle, RightVec, ZEVector3(Particle->Cos_NegSin, 0.0f));			
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}
	}
	else if (BillboardType == ZE_PBT_SCREEN_ALIGNED)
	{
		ZESize VertexIndex = 0;

		for (ZESize N = 0; N < ParticleCount; N++)
		{
			ZEParticle* Particle = &ParticlePool[ParticleCount - N - 1];

			if (Particle->State != ZE_PAS_DEAD)
			{
				ZEVector3 NegLook = -CameraLook;
				ZEVector3 Up,RightVec;


				float ZxZ = NegLook.z * NegLook.z;
				float YxY = NegLook.y * NegLook.y;
				float XxX = NegLook.x * NegLook.x;

				float XY = NegLook.x * NegLook.y;
				float XZ = NegLook.x * NegLook.z;
				float YZ = NegLook.y * NegLook.z;

				ZEMatrix3x3 L(	 0.0f,		 NegLook.z, -NegLook.y,
								-NegLook.z,  0.0f,		 NegLook.x,
								 NegLook.y,	-NegLook.x,  0.0f);
				
				ZEMatrix3x3 LL(	-ZxZ - YxY,  XY,			XZ,
								XY,			-ZxZ - XxX, YZ,
								XZ,			 YZ,		-YxY - XxX);

				Up = (ZEMatrix3x3::Identity + Particle->Cos_NegSin.y * L + ((1 - Particle->Cos_NegSin.x) * LL)) * CameraUp;
				
				ZEVector3::CrossProduct(RightVec, Up, CameraLook);
				DrawParticle(Buffer + VertexIndex, Particle, RightVec, Up);
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}
	}
	else if (BillboardType == ZE_PBT_VIEW_POINT_ORIENTED)
	{
		ZESize VertexIndex = 0;

		for (ZESize N = 0; N < ParticleCount; N++)
		{
			ZEParticle* Particle = &ParticlePool[ParticleCount - N - 1];

			ZEVector3 ParticleFaceDirection = DrawParameters->View->Camera->GetWorldPosition() - Particle->Position;
			ParticleFaceDirection.NormalizeSelf();

			if (Particle->State != ZE_PAS_DEAD)
			{
				ZEVector3 InitialUp, FinalUp, RightVec;

				float ZxZ = ParticleFaceDirection.z * ParticleFaceDirection.z;
				float YxY = ParticleFaceDirection.y * ParticleFaceDirection.y;
				float XxX = ParticleFaceDirection.x * ParticleFaceDirection.x;

				float XY = ParticleFaceDirection.x * ParticleFaceDirection.y;
				float XZ = ParticleFaceDirection.x * ParticleFaceDirection.z;
				float YZ = ParticleFaceDirection.y * ParticleFaceDirection.z;

				ZEMatrix3x3 L(	 0.0f,						 ParticleFaceDirection.z,	-ParticleFaceDirection.y,
								-ParticleFaceDirection.z,	 0.0f,						 ParticleFaceDirection.x,
								 ParticleFaceDirection.y,	-ParticleFaceDirection.x,	 0.0f);

				ZEMatrix3x3 LL(	-ZxZ - YxY,		 XY,			 XZ,
								 XY,			-ZxZ - XxX,		 YZ,
								 XZ,			 YZ,			-YxY - XxX);

	
				ZEVector3::CrossProduct(InitialUp, CameraRight, ParticleFaceDirection);
				FinalUp = (ZEMatrix3x3::Identity + Particle->Cos_NegSin.y * L + ((1.0f - Particle->Cos_NegSin.x) * LL)) * InitialUp; 

				ZEVector3::CrossProduct(RightVec, ParticleFaceDirection, FinalUp);
				DrawParticle(Buffer + VertexIndex, Particle, RightVec, FinalUp);
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}
	}

	VertexBuffer->Unlock();
}

void ZEParticleEmitter::Draw(ZEDrawParameters* DrawParameters)
{
	if (ParticlePool.GetCount() == 0)
		return;

	UpdateVertexBuffer(DrawParameters);

	if (DrawParameters->Pass == ZE_RP_COLOR)
		Owner->Statistics.TotalParticleCount += ParticlePool.GetCount();

	if (RenderCommand.PrimitiveCount == 0)
		return;

	if (VertexBuffer == NULL || Material == NULL)
		return;

	if (DrawParameters->Pass == ZE_RP_COLOR)
		Owner->Statistics.DrawedParticleCount += RenderCommand.PrimitiveCount / 2;

	if(IsEmmiterLocal)
		ZEMatrix4x4::CreateOrientation(RenderCommand.WorldMatrix, GetOwner()->GetWorldPosition(), ZEQuaternion::Identity, GetOwner()->GetWorldScale());
	else
		RenderCommand.WorldMatrix = ZEMatrix4x4::Identity;

	RenderCommand.VertexBuffer = VertexBuffer;
	RenderCommand.Material = Material;
	RenderCommand.Priority = 1000;
	RenderCommand.Order = 1001.0f;
	DrawParameters->Renderer->AddToRenderList(&RenderCommand);
}

void ZEParticleEmitter::SortParticles()
{
	ZEVector3 CamPos = zeScene->GetActiveCamera()->GetWorldPosition();
	float DistanceSqr;

	for (ZEInt I = 0; I < 32; I++)
	{
		ZEUInt Radix = (1 << I);

		ZESize ParticleCount = ParticlePool.GetCount();

		ZESize Count0 = 0;
		ZESize Count1 = 0;

		for (ZESize J = 0; J < ParticleCount; J++)
		{
			DistanceSqr =	(CamPos.x - ParticlePool[J].Position.x) * (CamPos.x - ParticlePool[J].Position.x) +
							(CamPos.y - ParticlePool[J].Position.y) * (CamPos.y - ParticlePool[J].Position.y) +
							(CamPos.z - ParticlePool[J].Position.z) * (CamPos.z - ParticlePool[J].Position.z);

			if (!((*(ZEUInt*)&(DistanceSqr)) & Radix))
				++Count1;
		}

		for (ZESize J = 0; J < ParticleCount; J++)
		{
			DistanceSqr =	(CamPos.x - ParticlePool[J].Position.x) * (CamPos.x - ParticlePool[J].Position.x) +
							(CamPos.y - ParticlePool[J].Position.y) * (CamPos.y - ParticlePool[J].Position.y) +
							(CamPos.z - ParticlePool[J].Position.z) * (CamPos.z - ParticlePool[J].Position.z);

			if ((*(ZEUInt*)&(DistanceSqr)) & Radix)
			{
				SortArray[Count1]=ParticlePool[J];
				++Count1;
			}
			else 
			{
				SortArray[Count0]=ParticlePool[J];
				++Count0;
			}
		}

		for (ZESize J = 0; J < ParticleCount; J++)
		{
			ParticlePool[J] = SortArray[J];
		}
	}
}

ZEParticleEmitter* ZEParticleEmitter::CreateInstance()
{
	return new ZEParticleEmitter();
}
