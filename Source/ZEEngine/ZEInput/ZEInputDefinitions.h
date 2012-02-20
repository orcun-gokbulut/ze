//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputDefinitions.h
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
#ifndef __ZE_INPUT_KEYBOARD_H__
#define __ZE_INPUT_KEYBOARD_H__

#define ZE_IDK_DEFAULT_KEYBOARD		0x00
#define ZE_IDK_DEFAULT_MOUSE		0x00
#define ZE_IDK_DEFAULT_JOYSTICK		0x00

// Keyboard Button Definitions
#define ZE_IKB_ESCAPE				0x01
#define ZE_IKB_1					0x02
#define ZE_IKB_2					0x03
#define ZE_IKB_3					0x04
#define ZE_IKB_4					0x05
#define ZE_IKB_5					0x06
#define ZE_IKB_6					0x07
#define ZE_IKB_7					0x08
#define ZE_IKB_8					0x09
#define ZE_IKB_9					0x0A
#define ZE_IKB_0					0x0B
#define ZE_IKB_MINUS				0x0C
#define ZE_IKB_EQUALS				0x0D
#define ZE_IKB_BACK					0x0E 
#define ZE_IKB_TAB					0x0F
#define ZE_IKB_Q					0x10
#define ZE_IKB_W					0x11
#define ZE_IKB_E					0x12
#define ZE_IKB_R					0x13
#define ZE_IKB_T					0x14
#define ZE_IKB_Y					0x15
#define ZE_IKB_U					0x16
#define ZE_IKB_I					0x17
#define ZE_IKB_O					0x18
#define ZE_IKB_P					0x19
#define ZE_IKB_LBRACKET				0x1A
#define ZE_IKB_RBRACKET				0x1B
#define ZE_IKB_RETURN				0x1C
#define ZE_IKB_LCONTROL				0x1D
#define ZE_IKB_A					0x1E
#define ZE_IKB_S					0x1F
#define ZE_IKB_D					0x20
#define ZE_IKB_F					0x21
#define ZE_IKB_G					0x22
#define ZE_IKB_H					0x23
#define ZE_IKB_J					0x24
#define ZE_IKB_K					0x25
#define ZE_IKB_L					0x26
#define ZE_IKB_SEMICOLON			0x27
#define ZE_IKB_APOSTROPHE			0x28
#define ZE_IKB_GRAVE				0x29
#define ZE_IKB_LSHIFT				0x2A
#define ZE_IKB_BACKSLASH			0x2B
#define ZE_IKB_Z					0x2C
#define ZE_IKB_X					0x2D
#define ZE_IKB_C					0x2E
#define ZE_IKB_V					0x2F
#define ZE_IKB_B					0x30
#define ZE_IKB_N					0x31
#define ZE_IKB_M					0x32
#define ZE_IKB_COMMA				0x33
#define ZE_IKB_PERIOD				0x34
#define ZE_IKB_SLASH				0x35
#define ZE_IKB_RSHIFT				0x36
#define ZE_IKB_MULTIPLY				0x37
#define ZE_IKB_LMENU				0x38 
#define ZE_IKB_SPACE				0x39
#define ZE_IKB_CAPITAL				0x3A
#define ZE_IKB_F1					0x3B
#define ZE_IKB_F2					0x3C
#define ZE_IKB_F3					0x3D
#define ZE_IKB_F4					0x3E
#define ZE_IKB_F5					0x3F
#define ZE_IKB_F6					0x40
#define ZE_IKB_F7					0x41
#define ZE_IKB_F8					0x42
#define ZE_IKB_F9					0x43
#define ZE_IKB_F10					0x44
#define ZE_IKB_NUMLOCK				0x45
#define ZE_IKB_SCROLL				0x46
#define ZE_IKB_NUMPAD7				0x47
#define ZE_IKB_NUMPAD8				0x48
#define ZE_IKB_NUMPAD9				0x49
#define ZE_IKB_SUBTRACT				0x4A
#define ZE_IKB_NUMPAD4				0x4B
#define ZE_IKB_NUMPAD5				0x4C
#define ZE_IKB_NUMPAD6				0x4D
#define ZE_IKB_ADD					0x4E
#define ZE_IKB_NUMPAD1				0x4F
#define ZE_IKB_NUMPAD2				0x50
#define ZE_IKB_NUMPAD3				0x51
#define ZE_IKB_NUMPAD0				0x52
#define ZE_IKB_DECIMAL				0x53 
#define ZE_IKB_OEM_102				0x56
#define ZE_IKB_F11					0x57
#define ZE_IKB_F12					0x58
#define ZE_IKB_F13					0x64
#define ZE_IKB_F14					0x65
#define ZE_IKB_F15					0x66
#define ZE_IKB_KANA					0x70
#define ZE_IKB_ABNT_C1				0x73
#define ZE_IKB_CONVERT				0x79 
#define ZE_IKB_NOCONVERT			0x7B
#define ZE_IKB_YEN					0x7D
#define ZE_IKB_ABNT_C2				0x7E
#define ZE_IKB_NUMPADEQUALS			0x8D
#define ZE_IKB_PREVTRACK			0x90
#define ZE_IKB_AT					0x91
#define ZE_IKB_COLON				0x92
#define ZE_IKB_UNDERLINE			0x93
#define ZE_IKB_KANJI				0x94
#define ZE_IKB_STOP					0x95
#define ZE_IKB_AX					0x96 
#define ZE_IKB_UNLABELED			0x97
#define ZE_IKB_NEXTTRACK			0x99 
#define ZE_IKB_NUMPADENTER			0x9C
#define ZE_IKB_RCONTROL				0x9D
#define ZE_IKB_MUTE					0xA0
#define ZE_IKB_CALCULATOR			0xA1
#define ZE_IKB_PLAYPAUSE			0xA2
#define ZE_IKB_MEDIASTOP			0xA4
#define ZE_IKB_VOLUMEDOWN			0xAE
#define ZE_IKB_VOLUMEUP				0xB0
#define ZE_IKB_WEBHOME				0xB2
#define ZE_IKB_NUMPADCOMMA			0xB3
#define ZE_IKB_DIVIDE				0xB5
#define ZE_IKB_SYSRQ				0xB7
#define ZE_IKB_RMENU				0xB8
#define ZE_IKB_PAUSE				0xC5 
#define ZE_IKB_HOME					0xC7
#define ZE_IKB_UP					0xC8 
#define ZE_IKB_PRIOR				0xC9
#define ZE_IKB_LEFT					0xCB 
#define ZE_IKB_RIGHT				0xCD 
#define ZE_IKB_END					0xCF
#define ZE_IKB_DOWN					0xD0
#define ZE_IKB_NEXT					0xD1
#define ZE_IKB_INSERT				0xD2
#define ZE_IKB_DELETE				0xD3
#define ZE_IKB_LWIN					0xDB
#define ZE_IKB_RWIN					0xDC 
#define ZE_IKB_APPS					0xDD
#define ZE_IKB_POWER				0xDE
#define ZE_IKB_SLEEP				0xDF
#define ZE_IKB_WAKE					0xE3
#define ZE_IKB_WEBSEARCH			0xE5
#define ZE_IKB_WEBFAVORITES			0xE6
#define ZE_IKB_WEBREFRESH			0xE7
#define ZE_IKB_WEBSTOP				0xE8
#define ZE_IKB_WEBFORWARD			0xE9
#define ZE_IKB_WEBBACK				0xEA 
#define ZE_IKB_MYCOMPUTER			0xEB
#define ZE_IKB_MAIL					0xEC 
#define ZE_IKB_MEDIASELECT			0xED
#define ZE_IKB_BACKSPACE			ZE_IKB_BACK
#define ZE_IKB_NUMPADSTAR			ZE_IKB_MULTIPLY
#define ZE_IKB_LALT					ZE_IKB_LMENU
#define ZE_IKB_CAPSLOCK				ZE_IKB_CAPITAL
#define ZE_IKB_NUMPADMINUS			ZE_IKB_SUBTRACT 
#define ZE_IKB_NUMPADPLUS			ZE_IKB_ADD
#define ZE_IKB_NUMPADPERIOD			ZE_IKB_DECIMAL
#define ZE_IKB_NUMPADSLASH			ZE_IKB_DIVIDE
#define ZE_IKB_RALT					ZE_IKB_RMENU
#define ZE_IKB_UPARROW				ZE_IKB_UP
#define ZE_IKB_PGUP					ZE_IKB_PRIOR
#define ZE_IKB_LEFTARROW			ZE_IKB_LEFT
#define ZE_IKB_RIGHTARROW			ZE_IKB_RIGHT
#define ZE_IKB_DOWNARROW			ZE_IKB_DOWN
#define ZE_IKB_PGDN					ZE_IKB_NEXT

// Mouse Axises
#define ZE_IMA_AXIS0				0x00
#define ZE_IMA_AXIS1				0x01
#define ZE_IMA_AXIS2				0x02
#define ZE_IMA_AXIS3				0x03
#define ZE_IMA_AXIS4				0x04
#define ZE_IMA_AXIS5				0x05

#define ZE_IMA_XAXIS				ZE_IMA_AXIS0
#define ZE_IMA_YAXIS				ZE_IMA_AXIS1
#define ZE_IMA_ZAXIS				ZE_IMA_AXIS2
#define ZE_IMA_WAXIS				ZE_IMA_AXIS3

#define ZE_IMA_LEFTRIGHT_AXIS		ZE_IMA_AXIS0
#define ZE_IMA_HORIZANTAL_AXIS		ZE_IMA_AXIS0

#define ZE_IMA_UPDOWN_AXIS			ZE_IMA_AXIS1
#define ZE_IMA_VERTICAL_AXIS		ZE_IMA_AXIS1

#define ZE_IMA_WHEEL_AXIS			ZE_IMA_AXIS2

// Mouse Buttons
#define ZE_IMB_BUTTON0				0x00
#define ZE_IMB_BUTTON1				0x01
#define ZE_IMB_BUTTON2				0x02
#define ZE_IMB_BUTTON3				0x03
#define ZE_IMB_BUTTON4				0x04
#define ZE_IMB_BUTTON5				0x05
#define ZE_IMB_BUTTON6				0x06
#define ZE_IMB_BUTTON7				0x07
#define ZE_IMB_BUTTON8				0x08
#define ZE_IMB_BUTTON9				0x09
#define ZE_IMB_BUTTON10				0x0A

#define ZE_IMB_LEFTBUTTON			ZE_IMB_BUTTON0
#define ZE_IMB_RIGHTBUTTON			ZE_IMB_BUTTON1
#define ZE_IMB_MIDDLEBUTTON			ZE_IMB_BUTTON2
#define ZE_IMB_BACKBUTTON			ZE_IMB_BUTTON3
#define ZE_IMB_FORWARDBUTTON		ZE_IMB_BUTTON4

// Joystick Axises
#define ZE_IJA_AXIS0				0x00
#define ZE_IJA_AXIS1				0x01
#define ZE_IJA_AXIS2				0x02
#define ZE_IJA_AXIS3				0x03
#define ZE_IJA_AXIS4				0x04
#define ZE_IJA_AXIS5				0x05

#define ZE_IJA_XAXIS				ZE_IJA_AXIS0
#define ZE_IJA_YAXIS				ZE_IJA_AXIS1
#define ZE_IJA_ZAXIS				ZE_IJA_AXIS2
#define ZE_IJA_WAXIS				ZE_IJA_AXIS3

// Joystick Buttons
#define ZE_IJB_BUTTON0				0x00
#define ZE_IJB_BUTTON1				0x01
#define ZE_IJB_BUTTON2				0x02
#define ZE_IJB_BUTTON3				0x03
#define ZE_IJB_BUTTON4				0x04
#define ZE_IJB_BUTTON5				0x05
#define ZE_IJB_BUTTON6				0x06
#define ZE_IJB_BUTTON7				0x07
#define ZE_IJB_BUTTON8				0x08
#define ZE_IJB_BUTTON9				0x09
#define ZE_IJB_BUTTON10				0x0A
#define ZE_IJB_BUTTON11				0x0B
#define ZE_IJB_BUTTON12				0x0C 
#define ZE_IJB_BUTTON13				0x0D
#define ZE_IJB_BUTTON14				0x0E
#define ZE_IJB_BUTTON15				0x0F
#define ZE_IJB_BUTTON16				0x10
#define ZE_IJB_BUTTON17				0x11
#define ZE_IJB_BUTTON18				0x12
#define ZE_IJB_BUTTON19				0x13
#define ZE_IJB_BUTTON20				0x14

unsigned char zeGetKeyboardButtonId(char* KeyShortName);
const char* zeGetKeyboardButtonName(unsigned char Key);
const char* zeGetKeyboardButtonShortName(unsigned char Key);

#endif




