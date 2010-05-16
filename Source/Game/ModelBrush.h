//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ModelBrush.h
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
#ifndef __ZE_MODELBRUSH_H__
#define __ZE_MODELBRUSH_H__

#include "Game/Entity.h"
#include "Model/Model.h"

ZE_META_CLASS_DESCRIPTION(ZEModelBrush);

class ZEModel;
class ZEModelResource;

class ZEModelBrush : public ZEEntity
{
	ZE_META_CLASS()
	private:
		ZEModel*							Model;
		ZEModelResource*					ModelResource;

	public:
		virtual ZEDWORD						GetDrawFlags() const;

		ZEModel*							GetModel();

		virtual const ZEAABoundingBox&		GetLocalBoundingBox();
		virtual const ZEAABoundingBox&		GetWorldBoundingBox();

		void								SetModelFile(const char* FileName);
		const char*							GetModelFile() const;

		void								SetEnablePhysics(bool Enable);
		bool 								GetEnablePhysics() const;

		void 								SetAnimationState(ZEModelAnimationState AnimationState);
		ZEModelAnimationState 				GetAnimationState() const;

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType() const;

		void								SetAnimationName(const char* AnimationName);
		const char*							GetAnimationName() const;

		void								SetAnimationLooping(bool AnimationLooping);
		bool								GetAnimationLooping() const;

		void 								SetAnimationFrame(int AnimationFrame);
		int									GetAnimationFrame() const;

		void								SetAnimationSpeed(float AnimationSpeed);
		float								GetAnimationSpeed() const;

		virtual bool						Initialize();
		virtual void						Deinitialize();
		virtual void						Reset();

		virtual void						Update();
		virtual void						Draw(ZERenderer* Renderer, const ZESmartArray<ZELight*>& Lights);

											ZEModelBrush();
		virtual								~ZEModelBrush();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta>
	 	<class name="ZEModelBrush" parent="ZEEntity" icon="" description="This entity used for displaying animated visual models in the game environments like statues, mechineries, furnitures and etc.">
			<property name="ModelFile" type="string" autogetset="true" semantic="ZE_PS_FILENAME" fileextension="ZEMODEL" description="FileName of the model that will be used"/>
			<property name="EnablePhysics" type="boolean" autogetset="true" description="Will model brush will have physical interaction"/>
			<property name="AnimationState" type="integer" autogetset="true" description="Animation state of the model brush.">
				<enumurator name="ZEModelAnimationState">
					<item name="Static Frame" value="ZE_MAS_STATICFRAME"/>
					<item name="Playing" value="ZE_MAS_PLAYING"/>
					<item name="Paused" value="ZE_MAS_PAUSED"/>
					<item name="Stopped" value="ZE_MAS_STOPPED"/>
				</enumurator>
			</property>
			<property name="AnimationType" type="integer" autogetset="true" description="Animation type of the model brush.">
				<enumurator name="ZEModelAnimationType">
					<item name="No Animation" value="ZE_MAT_NOANIMATION"/>
					<item name="Predifined" value="ZE_MAT_PREDEFINED"/>
					<item name="Physical" value="ZE_MAT_PHYSICAL"/>
				</enumurator>
			</property>
			<property name="AnimationName" type="string" autogetset="true" description="Animation name of the model brush"/>
			<property name="AnimationLooping" type="boolean" autogetset="true" description="Animation loooping"/>
			<property name="AnimationFrame" type="integer" autogetset="true" description="Animation Frame"/>
			<property name="AnimationSpeed" type="float" autogetset="true" description="Speed of the animation being played."/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif 
