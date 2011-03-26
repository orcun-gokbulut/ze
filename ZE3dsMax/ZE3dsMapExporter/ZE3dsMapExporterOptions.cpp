//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMapExporterOptions.cpp
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

#include "ZE3dsMapExporterOptions.h"
#include "ZE3dsMapExporter.h"
#include "ZE3dsMapExporterResources.h"

#include <tchar.h>
#include <shlobj.h>
#include <stdio.h>
#include <Commctrl.h>
#include <windowsx.h>


INT_PTR CALLBACK ZEMapExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static ZE3dsMapExporter *imp = NULL;

	switch(message) 
	{
		case WM_INITDIALOG:
			imp = (ZE3dsMapExporter *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}

void ZE3dsMapExporterOptions::Save(char* Filename)
{
	FILE* OptionFile = fopen(Filename , "wb");
	if (OptionFile != NULL)
	{
		fwrite(this, sizeof(ZE3dsMapExporterOptions), 1, OptionFile);
		fclose(OptionFile);
	}
}

void ZE3dsMapExporterOptions::Load(char* Filename)
{
	FILE* OptionFile = fopen(Filename, "rb");
	if (OptionFile != NULL)
	{
		fread(this, sizeof(ZE3dsMapExporterOptions), 1, OptionFile);
		fclose(OptionFile);
	}
	else
	{
		strcpy(ZinekDir, "C:\\");
		Logging_Level = 1;
		Logging_OutputToFile = false;
		Logging_OutputFile[0] = '\0';
	}
}


bool Canceled;
void ZEMapExporterOptionsDialog::DialogDataExchange()
{
	Options.Logging_Level =			ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_LOGLEVEL)) + 1;
	Options.Logging_OutputToFile =	Button_GetCheck(GetDlgItem(hWnd, IDC_OUTPUTTOFILE)) == BST_CHECKED;

	GetWindowText(GetDlgItem(hWnd, IDC_LOGFILE), Options.Logging_OutputFile, 256);
	GetWindowText(GetDlgItem(hWnd, IDC_ZINEKDIR), Options.ZinekDir, 256);
}

void ZEMapExporterOptionsDialog::ManageDialog()
{
	if (SendDlgItemMessage(hWnd, IDC_OUTPUTTOFILE, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		EnableWindow(GetDlgItem(hWnd, IDC_LOGFILE), true);
		EnableWindow(GetDlgItem(hWnd, IDC_BROWSEFILE), true);				
	}
	else
	{
		EnableWindow(GetDlgItem(hWnd, IDC_LOGFILE), false);
		EnableWindow(GetDlgItem(hWnd, IDC_BROWSEFILE), false);
	}
}

INT_PTR CALLBACK OptionsDialogCallback(HWND hWnd,UINT message,WPARAM wParam, LPARAM lParam) 
{
	static ZEMapExporterOptionsDialog* Dlg;
	static TCHAR	PathBuffer[MAX_PATH];
	LPITEMIDLIST Path = NULL;
	switch(message) 
	{
		case WM_INITDIALOG:
			Dlg = (ZEMapExporterOptionsDialog*)lParam;
			Dlg->hWnd = hWnd;
			
			ComboBox_AddString(GetDlgItem(hWnd, IDC_LOGLEVEL), "1 (Default)");
			ComboBox_AddString(GetDlgItem(hWnd, IDC_LOGLEVEL), "2 (Extra Information)");
			ComboBox_AddString(GetDlgItem(hWnd, IDC_LOGLEVEL), "3 (Debugging Information)");
//			ComboBox_SetMinVisible(GetDlgItem(hWnd, IDC_LOGLEVEL), 3);
			ComboBox_SetCurSel(GetDlgItem(hWnd, IDC_LOGLEVEL), Dlg->Options.Logging_Level - 1);

			Button_SetCheck(GetDlgItem(hWnd, IDC_OUTPUTTOFILE), Dlg->Options.Logging_OutputToFile);
			SetWindowText(GetDlgItem(hWnd, IDC_ZINEKDIR), Dlg->Options.ZinekDir);
			SetWindowText(GetDlgItem(hWnd, IDC_LOGFILE), Dlg->Options.Logging_OutputFile);
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_OUTPUTTOFILE:
					Dlg->ManageDialog();
					break;
				case IDC_BROWSE:
					BROWSEINFO bi;
					ZeroMemory(&bi, sizeof(BROWSEINFO));
					bi.hwndOwner = hWnd;
					bi.lpszTitle = _T("Choose Zinek Engine directory.");

					Path = SHBrowseForFolder(&bi);
					if (Path != NULL)
					{
						if (SHGetPathFromIDList (Path, PathBuffer))
							SendDlgItemMessage(hWnd, IDC_ZINEKDIR, WM_SETTEXT, NULL, (LPARAM)PathBuffer);
						else
							MessageBox(hWnd, "Not valid path !", "Zinek Engine Map Exporter", MB_ICONERROR | MB_OK);

						IMalloc * imalloc = 0;
						if (SUCCEEDED(SHGetMalloc(&imalloc)))
						{
							imalloc->Free(Path);
							imalloc->Release();
						}
					}
					return TRUE;
					break;

				case IDC_BROWSEFILE:
					ZeroMemory(&bi, sizeof(BROWSEINFO));
					bi.hwndOwner = hWnd;
					bi.lpszTitle = _T("Choose Zinek Engine directory.");

					Path = SHBrowseForFolder(&bi);
					if (Path != NULL)
					{
						if (SHGetPathFromIDList (Path, PathBuffer))
							SendDlgItemMessage(hWnd, IDC_ZINEKDIR, WM_SETTEXT, NULL, (LPARAM)PathBuffer);
						else
							MessageBox(hWnd, "Not valid path !", "Zinek Engine Map Exporter", MB_ICONERROR | MB_OK);

						IMalloc * imalloc = 0;
						if (SUCCEEDED(SHGetMalloc(&imalloc)))
						{
							imalloc->Free(Path);
							imalloc->Release();
						}
					}
					return TRUE;
					break;

				case IDC_CANCEL:
					Canceled = true;
					EndDialog(hWnd, 0);
					return TRUE;
				case IDC_EXPORT:
					Canceled = false;
					Dlg->DialogDataExchange();
					EndDialog(hWnd, 0);
					return TRUE;
			}
			break;
	}	
	return 0;
}

bool ZEMapExporterOptionsDialog::ShowDialog(HINSTANCE Instance, ZE3dsMapExporterOptions& Options)
{
	this->Options = Options;
	Canceled = true;
	DialogBoxParam((HINSTANCE)Instance, MAKEINTRESOURCE(IDD_OPTIONS), GetActiveWindow(), OptionsDialogCallback, (LPARAM)this);
	if (Canceled == true)
		return false;
	else
	{
		Options = this->Options;
		return true;
	}
}
