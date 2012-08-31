//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleModifier.h
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
#ifndef __ZE_PARTICLE_CONTROLLER_H__
#define __ZE_PARTICLE_CONTROLLER_H__

#include "ZEMeta/ZEObject.h"
#include "ZEDS/ZEString.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZEParticle.h"

class ZEParticleEmitter;

class ZEParticleModifier
{
	private:

		ZEString							Name;
		ZEParticleEmitter*					Owner;

	protected: 

		ZEArray<ZEParticle>&				GetOwnerParticlePool();

	public:

		void								SetName(const ZEString& Name);
		const ZEString&						GetName();

		void								SetOwner(ZEParticleEmitter* Owner);
		ZEParticleEmitter*					GetOwner();
		
		virtual void						Tick(float ElapsedTime) = 0;

											ZEParticleModifier();
		virtual								~ZEParticleModifier();
};

class ZEParticlePhysicsModifier : public ZEParticleModifier
{
	private:

		bool							IsRadialMovement;
		float							MaxRadialSpeed;
		float							MinRadialSpeed;

		ZEVector3						MinAcceleration;
		ZEVector3						MaxAcceleration;
		ZEVector3						MinVelocity;
		ZEVector3						MaxVelocity;

	public:

		void							SetRadialMovement(bool Enabled);
		void							SetRadialSpeed(float Max, float Min);

		void							SetMinAcceleration(const ZEVector3& Acceleration);
		const ZEVector3&				GetMinAcceleration() const;
		void							SetMaxAcceleration(const ZEVector3& Acceleration);
		const ZEVector3&				GetMaxAcceleration() const;

		void							SetMinVelocity(const ZEVector3& Velocity);
		const ZEVector3&				GetMinVelocity() const;
		void							SetMaxVelocity(const ZEVector3& Velocity);
		const ZEVector3&				GetMaxVelocity() const;

		virtual void					Tick(float ElapsedTime);

										ZEParticlePhysicsModifier();
										~ZEParticlePhysicsModifier();
};

class ZEParticleRotationModifier : public ZEParticleModifier
{
	private:

		float							MaxRotation;
		float							MinRotation;

		float							MinAngularAcceleration;
		float							MaxAngularAcceleration;
		float							MinAngularVelocity;
		float							MaxAngularVelocity;

	public:

		void							SetMaxRotation(float MaxRotation);
		float							GetMaxRotation() const;
		void							SetMinRotation(float MinRotation);
		float							GetMinRotation() const;

		void 							SetMinAngularAcceleration(float AngularAcceleration);
		float			 				GetMinAngularAcceleration() const;
		void 							SetMaxAngularAcceleration(float AngularAcceleration);
		float 							GetMaxAngularAcceleration() const;

		void 							SetMinAngularVelocity(float AngularVelocity);
		float							GetMinAngularVelocity() const;
		void 							SetMaxAngularVelocity(float AngularVelocity);
		float							GetMaxAngularVelocity() const;

		virtual	void					Tick(float ElapsedTime);

										ZEParticleRotationModifier();
										~ZEParticleRotationModifier();
};

class ZEParticleGrowModifier : public ZEParticleModifier
{
	private:

		float	GrowFactor;

	public:

		void			SetGrowFactor(float Factor);
		float			GetGrowFactor() const;

		virtual	void	Tick(float ElapsedTime);

						ZEParticleGrowModifier();
						~ZEParticleGrowModifier();
};

class ZEParticleColorOverLifeModifier : public ZEParticleModifier
{
	private:

		ZEVector4			ToColor;

	public:

		void				SetToColor(const ZEVector4& Factor);
		const ZEVector4&	GetToColor() const;

		virtual	void		Tick(float ElapsedTime);

							ZEParticleColorOverLifeModifier();
							~ZEParticleColorOverLifeModifier();
};

class ZEParticleVelocityOverLifeModifier : public ZEParticleModifier
{
	private:

		ZEVector3			ToVelocity;

	public:

		void				SetToVelocity(const ZEVector3& Factor);
		const ZEVector3&	GetToVelocity() const;

		virtual	void		Tick(float ElapsedTime);

							ZEParticleVelocityOverLifeModifier();
							~ZEParticleVelocityOverLifeModifier();
};

class ZEParticleDiffuseMapChangerModifier : public ZEParticleModifier
{
	private:

		ZEUInt							CurrentTextureIndex;
		float							TotalTime;
	
		ZEArray<ZETexture2DResource*>	Textures;
		float							Interval;

	public:
	
		void							SetInterval(float NewInterval);
		float							GetInterval();

		void							AddTextureResource(ZETexture2DResource* Resource);

		virtual	void					Tick(float ElapsedTime);
	
										ZEParticleDiffuseMapChangerModifier();
										~ZEParticleDiffuseMapChangerModifier();
};

class ZEParticleDisplacementModifier : public ZEParticleModifier
{
	private:
	
		ZEVector3						Displacement;
	
	public:
	

		void							SetDisplacement(ZEVector3 NewDisplacement);
		ZEVector3						GetDisplacement();
	
		virtual	void					Tick(float ElapsedTime);
	
										ZEParticleDisplacementModifier();
										~ZEParticleDisplacementModifier();
};

class ZEParticleRandomAccelerationModifier : public ZEParticleModifier
{
	private:
	
		float							MaxStrength;
	
	public:
	
	
		void							SetMaxStrength(float NewStrength = 1000.0f);
		float							GetMaxStrength() const;
	
		virtual	void					Tick(float ElapsedTime);
	
										ZEParticleRandomAccelerationModifier();
										~ZEParticleRandomAccelerationModifier();
};

class ZEParticleUVModifier : public ZEParticleModifier
{
	private:

		bool							DoOnce;
		ZEVector2						UVFrameSize;
		ZEInt							CurrentUVFrame;

		ZEVector2						TextureSize;
		ZEInt							RowCount;
		ZEInt							ColumnCount;

	public:

		void							SetTextureSize(ZEVector2 TextureSize);
		void							SetRowCount(ZEInt RowCount);
		void							SetColumnCount(ZEInt ColumnCount);

		virtual void					Tick(float ElapsedTime);

										ZEParticleUVModifier();
										~ZEParticleUVModifier();
};

#endif
