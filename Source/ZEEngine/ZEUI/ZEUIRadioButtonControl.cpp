//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIRadioButtonControl.cpp
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

// #include "ZEUIRadioButtonControl.h"
// #include "ZEFontResourceBitmap.h"
// #include "ZEGraphics/ZEFixedMaterial.h"
// #include "ZETexture//ZETexture2DResource.h"
// #include "ZEGraphics/ZEUIMaterial.h"
// 
// void ZEUIRadioButtonControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
// {
// 	ZEUIControl::MouseButtonPressed(Button, MousePosition);
// 
// 	if (State == ZE_UI_RBS_CHECKED)
// 		return;
// 
// 	if(Button == ZE_UI_MOUSE_BUTTON_LEFT)
// 	{
// 		SetState(ZE_UI_RBS_CHECKED);
// 
// 		for (size_t I = 0; I < InteractingRadioButtons.GetCount(); I++)
// 			InteractingRadioButtons[I]->SetState(ZE_UI_RBS_UNCHECKED);
// 	}
// }
// 
// void ZEUIRadioButtonControl::AddInteractingRadioButton(ZEUIRadioButtonControl* RadioButton)
// {
// 	InteractingRadioButtons.Add(RadioButton);
// }
// 
// void ZEUIRadioButtonControl::RemoveInteractingRadioButton(ZEUIRadioButtonControl* RadioButton)
// {
// 	InteractingRadioButtons.DeleteValue(RadioButton);
// }
// 
// void ZEUIRadioButtonControl::SetState(ZEUIRadioButtonState State)
// {
// 	this->State = State;
// 
// 	if (State == ZE_UI_RBS_CHECKED)
// 		((ZEUIMaterial*)Box.GetMaterial())->SetTexture(ZETexture2DResource::LoadResource("SemiChecked.png")->GetTexture());
// 
// 	else if (State == ZE_UI_RBS_UNCHECKED)
// 		((ZEUIMaterial*)Box.GetMaterial())->SetTexture(ZETexture2DResource::LoadResource("UnChecked.png")->GetTexture());
// }
// 
// ZEUIRadioButtonState ZEUIRadioButtonControl::GetState() const
// {
// 	return State;
// }
// 
// void ZEUIRadioButtonControl::SetText(ZEString Text)
// {
// 	Label.SetText(Text);
// }
// 
// ZEString ZEUIRadioButtonControl::GetText()
// {
// 	return Label.GetText();
// }
// 
// ZEMaterial*	ZEUIRadioButtonControl::GetMaterial() const
// {
// 	return NULL;
// }
// 
// void ZEUIRadioButtonControl::SetMaterial(ZEMaterial* Material)
// {
// 	//Box.Button.Material = Material;
// }
// 
// ZEUIRadioButtonControl::ZEUIRadioButtonControl()
// {
// 	State = ZE_UI_RBS_UNCHECKED;
// 	Label.SetFont(ZEFontResourceBitmap::LoadResource("Courier New.zeFont"));
// 	Label.SetFontSize(ZEVector2::One);
// 	Label.SetText(ZEString("Test"));
// 
// 	Box.SetWidth(24);
// 	Box.SetHeight(24);
// 	Label.SetWidth(120);
// 	Label.SetHeight(Label.GetTextControlCharacters()[0].RenderableCharacter.Positions.GetHeight());
// 
// 	AddChildControl(&Box);
// 	AddChildControl(&Label);
// 	Box.SetPosition(ZEVector2::Zero);
// 	Label.SetPosition(ZEVector2(Box.GetPosition().x + Box.GetWidth() + 1, Box.GetPosition().y + (Box.GetHeight() - Label.GetHeight()) / 2));
// 
// 	SetHeight(Box.GetHeight());
// 	SetWidth(Box.GetWidth() + Label.GetWidth());
// 
// 	Box.SetMouseButtonPressedEvent(BindDelegate(this, &ZEUIRadioButtonControl::MouseButtonPressed));
// 
// 	((ZEUIMaterial*)Box.GetMaterial())->SetTexture(ZETexture2DResource::LoadResource("UnChecked.png")->GetTexture());
// }
// 
// ZEUIRadioButtonControl::~ZEUIRadioButtonControl()
// {
// 
// }
