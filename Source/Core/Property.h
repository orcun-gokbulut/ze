//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Property.h
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

ZEPropertContainer

ZEProperty(Type, GetFunction, SetFunction) Plus;

ZE_META()
	ZE_PROPERTIES(ZEClass) 
		ZE_PROPERTY()
		ZE_ENUM_PROPERTY_START("Name", GetFunction, SetFunction, Description)
			ZE_ENUM_ELEMENT("Name", Value);
		ZE_ENUM_PROPERTY
ZE_META_PROPERTIES()

enum ZEPropertySemantic
{
	ZE_PS_POSITION,
	ZE_PS_ROTATION,
	ZE_PS_DIRECTION,
	ZE_PS_SCALE,
	ZE_PS_FILENAME,
	ZE_PS_COLOR,
};

enum ZEPropertyAccess
{
	ZE_PA_READWRITE		= 0,
	ZE_PA_READONLY		= 1,
	ZE_PA_WRITEONLY		= 2,
}

class ZEPropertyDescription
{
	const char*									Name;
	ZEVariantType								Type;
	ZEPropertyAccess							Access;
	const char*									Description;
	ZEPropertySemantic							Semantic;
	void*										SemanticProperties;
	bool										Hidden;
	size_t										EnumuratorCount;
	ZEPropertyEnumurator*						Enumurators;
};

class ZEMethodDescription
{
	const char*									Name;
	size_t										ParameterCount;
	const ZEVariantType* const					Parameters;
	ZEVariantType								ReturnType;
};

class ZEModel

class ZEPropertyContainer
{
	public:
		virtual ZEMethod*						GetMethods(size_t& Count);
		virtual size_t							GetPropertyIndex(const char* PropertyName);
		virtual bool							CallMethod(const char* Name, ZEArray<ZEVariant&> Parameters, ZEVariant& ReturnValue);

		virtual const ZEPropertyDescription*	GetProperties(size_t& Count);

		virtual size_t							GetPropertyId(const char* PropertyName);
		
		virtual bool							GetProperty(size_t PropertyId);
		virtual bool							SetProperty(const char* Name, const ZEVariant& Value);
		virtual bool							SetPropertyController(void* Controller);
		
		virtual bool							GetProperty(size_t PropertyId);
		virtual bool							GetProperty(const char* Name, ZEVariant& Value);
		virtual bool							GetPropertyController(void* Controller);
};

class ZEPropertyKeyFrame
{
	public:
		float									Time;
		size_t									ElementIndex;
		size_t									PropertyIndex;
		ZEVariant								Value;
};

class ZEPropertyAnimations
{
	public:
		size_t									FrameCount;
		ZEArray<ZEPropertyKeyFrame>				KeyFrames;
};

class ZEPropertyContainerAnimation
{
	public:
		ZEPropertyAnimation*					Animation;
		float									BlendFactor;
		int										Priority;
		float									AnimationSpeed;
		ZEArray<size_t>							AnimationMask;
		bool 									Looping;
};

class ZEAnimatedPropertyContainer : public ZEPropertyContainer
{
	public:
		ZEArray<ZEPropertyAnimation>			Animations;
		ZEArray<ZEPropertyAnimationTrack>		Tracks;

		void									SetTrackLoop(bool Enabled);
		bool									GetTrackLoop();

		void									AdvanceAnimation(float ElapsedTime);
};
