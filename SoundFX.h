//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundFX.h
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

enum ZESoundFXType
{
	ZE_SFXT_CHORUS,
	ZE_SFXT_ENVIRONMENTAL_REVERB,
	ZE_SFXT_DISTORTION,
	ZE_SFXT_ECHO,
	ZE_SFXT_FLANGER,
	ZE_SFXT_GARGLE,
	ZE_SFXT_PARAMETRIC_EQ
	ZE_SFXT_NOISY_SUPRESSOR
};

class ZESoundFX
{
	private:
		bool						Enabled;

	public:
		virtual ZESoundFXType		GetType();

		void						SetEnabled(bool Enabled);
		bool						GetEnabled();

		void						Update();
};

class ZEChorusFX : public ZESoundFX
{
	private:
		float		WetDryMix;
		float		Depth;
		float		Feedback;
		float		Frequency;
		int			Waveform;
		float		Delay;
		int			Phase;

	public:
		void		SetWetDryMix();
		float		GetWetDryMix();
};

class ZEEnvironmentalReverbFX : public ZESoundFX 
{
	private:
		int			Room;
		int			RoomHF;
		float		RoomRolloffFactor;
		float		DecayTime;
		float		DecayHFRatio;
		int			Reflections;
		float		ReflectionsDelay;
		int			Reverb;
		float		ReverbDelay;
		float		Diffusion;
		float		Density;
		float		HFReference;
};

class DSFXDistortion : public ZESoundFX
{
    unsigned int	Gain;
    unsigned int	Edge;
    unsigned int	PostEQCenterFrequency;
    unsigned int	PostEQBandwidth;
    unsigned int	PreLowpassCutoff;
} ;

class DSFXEcho : public ZESoundFX
{
    float WetDryMix;
    float Feedback;
    float LeftDelay;
    float RightDelay;
    int PanDelay;
} ;

class DSFXFlanger : public ZESoundFX
{
    float WetDryMix;
    float Depth;
    float Feedback;
    float Frequency;
    int Weveform;
    float Delay;
    int Phase;
};

class DSFXGargle : public ZESoundFX
{
    unsigned int dwRateHz;
    unsigned int dwWaveShape;
};

class DSFXReverb : public ZESoundFX 
{
    float fInGain;
    float fReverbMix;
    float fReverbTime;
    float fHighFreqRTRatio;
};

class DSFXParamEq : public ZESoundFX
{
    float fCenter;
    float fBandwidth;
    float fGain;
};

class ZENoiseSupressorFX : public ZESoundFX
{
};
