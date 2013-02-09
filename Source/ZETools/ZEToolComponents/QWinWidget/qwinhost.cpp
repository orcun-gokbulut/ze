//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - qwinhost.cpp
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

/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

// Implementation of the QWinHost classes

#ifdef QT3_SUPPORT
#undef QT3_SUPPORT
#endif

#include "qwinhost.h"

#include <QtCore/QEvent>
#include <QtCore/qt_windows.h>

/*!
    \class QWinHost qwinhost.h
    \brief The QWinHost class provides an API to use native Win32
    windows in Qt applications.

    QWinHost exists to provide a QWidget that can act as a parent for
    any native Win32 control. Since QWinHost is a proper QWidget, it
    can be used as a toplevel widget (e.g. 0 parent) or as a child of
    any other QWidget.

    QWinHost integrates the native control into the Qt user interface,
    e.g. handles focus switches and laying out.

    Applications moving to Qt may have custom Win32 controls that will
    take time to rewrite with Qt. Such applications can use these
    custom controls as children of QWinHost widgets. This allows the
    application's user interface to be replaced gradually.

    When the QWinHost is destroyed, and the Win32 window hasn't been
    set with setWindow(), the window will also be destroyed.
*/

/*!
    Creates an instance of QWinHost. \a parent and \a f are
    passed on to the QWidget constructor. The widget has by default
    no background.

    \warning You cannot change the parent widget of the QWinHost instance 
    after the native window has been created, i.e. do not call 
    QWidget::setParent or move the QWinHost into a different layout.
*/
QWinHost::QWinHost(QWidget *parent, Qt::WFlags f)
: QWidget(parent, f), wndproc(0),own_hwnd(false), hwnd(0)
{
    setAttribute(Qt::WA_NoBackground);
    setAttribute(Qt::WA_NoSystemBackground);
}

/*!
    Destroys the QWinHost object. If the hosted Win32 window has not
    been set explicitly using setWindow() the window will be
    destroyed.
*/
QWinHost::~QWinHost()
{
    if (wndproc) {
#if defined(GWLP_WNDPROC)
	QT_WA({
	    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)wndproc);
	},{
	    SetWindowLongPtrA(hwnd, GWLP_WNDPROC, (LONG_PTR)wndproc);
	})
#else
	QT_WA({
	    SetWindowLong(hwnd, GWL_WNDPROC, (LONG)wndproc);
	},{
	    SetWindowLongA(hwnd, GWL_WNDPROC, (LONG)wndproc);
	})
#endif
    }

    if (hwnd && own_hwnd)
	DestroyWindow(hwnd);
}

/*!
    Reimplement this virtual function to create and return the native
    Win32 window. \a parent is the handle to this widget, and \a
    instance is the handle to the application instance. The returned HWND
    must be a child of the \a parent HWND.

    The default implementation returns null. The window returned by a
    reimplementation of this function is owned by this QWinHost
    instance and will be destroyed in the destructor.

    This function is called by the implementation of polish() if no
    window has been set explicitly using setWindow(). Call polish() to
    force this function to be called.

    \sa setWindow()
*/
HWND QWinHost::createWindow(HWND parent, HINSTANCE instance)
{
    Q_UNUSED(parent);
    Q_UNUSED(instance);
    return 0;
}

/*!
    Ensures that the window provided a child of this widget, unless
    it is a WS_OVERLAPPED window.
*/
void QWinHost::fixParent()
{
    if (!hwnd)
        return;
    if (!::IsWindow(hwnd)) {
        hwnd = 0;
        return;
    }
    if (::GetParent(hwnd) == winId())
        return;
    long style = GetWindowLong(hwnd, GWL_STYLE);
    if (style & WS_OVERLAPPED)
        return;
    ::SetParent(hwnd, winId());
}

/*!
    Sets the native Win32 window to \a window. If \a window is not a child 
    window of this widget, then it is reparented to become one. If \a window 
    is not a child window (i.e. WS_OVERLAPPED is set), then this function does nothing.

    The lifetime of the window handle will be managed by Windows, QWinHost does not
    call DestroyWindow. To verify that the handle is destroyed when expected, handle
    WM_DESTROY in the window procedure.

    \sa window(), createWindow()
*/
void QWinHost::setWindow(HWND window)
{
    if (hwnd && own_hwnd)
	DestroyWindow(hwnd);

    hwnd = window;
    fixParent();

    own_hwnd = false;
}

/*!
    Returns the handle to the native Win32 window, or null if no
    window has been set or created yet.

    \sa setWindow(), createWindow()
*/
HWND QWinHost::window() const
{
    return hwnd;
}

void *getWindowProc(QWinHost *host)
{
    return host ? host->wndproc : 0;
}

LRESULT CALLBACK WinHostProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    QWinHost *widget = qobject_cast<QWinHost*>(QWidget::find(::GetParent(hwnd)));
    WNDPROC oldproc = (WNDPROC)getWindowProc(widget);
    if (widget) {
	switch(msg) {
	case WM_LBUTTONDOWN:
	    if (::GetFocus() != hwnd && (widget->focusPolicy() & Qt::ClickFocus)) {
		widget->setFocus(Qt::MouseFocusReason);
	    }
	    break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	    QT_WA({
		SendMessage(widget->winId(), msg, wParam, lParam);
	    }, {
		SendMessageA(widget->winId(), msg, wParam, lParam);
	    })
	    break;

	case WM_KEYDOWN:
	    if (wParam == VK_TAB) {
		QT_WA({
		    SendMessage(widget->winId(), msg, wParam, lParam);
		}, {
		    SendMessageA(widget->winId(), msg, wParam, lParam);
		})
	    }
	    break;

	default:
	    break;
	}
    }

    QT_WA({
	if (oldproc)
	    return CallWindowProc(oldproc, hwnd, msg, wParam, lParam);
	return DefWindowProc(hwnd,msg,wParam,lParam);
    }, {
	if (oldproc)
	    return CallWindowProcA(oldproc, hwnd, msg, wParam, lParam);
	return DefWindowProcA(hwnd,msg,wParam,lParam);
    })
}

/*!
    \reimp
*/
bool QWinHost::event(QEvent *e)
{
    switch(e->type()) {
    case QEvent::Polish:
        if (!hwnd) {
            hwnd = createWindow(winId(), qWinAppInst());
            fixParent();
            own_hwnd = hwnd != 0;
        }
        if (hwnd && !wndproc && GetParent(hwnd) == winId()) {
#if defined(GWLP_WNDPROC)
            QT_WA({
                wndproc = (void*)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
                SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WinHostProc);
            }, {
                wndproc = (void*)GetWindowLongPtrA(hwnd, GWLP_WNDPROC);
                SetWindowLongPtrA(hwnd, GWLP_WNDPROC, (LONG_PTR)WinHostProc);
            })
#else
                QT_WA({
                wndproc = (void*)GetWindowLong(hwnd, GWL_WNDPROC);
                SetWindowLong(hwnd, GWL_WNDPROC, (LONG)WinHostProc);
            }, {
                wndproc = (void*)GetWindowLongA(hwnd, GWL_WNDPROC);
                SetWindowLongA(hwnd, GWL_WNDPROC, (LONG)WinHostProc);
            })
#endif

            LONG style;
            QT_WA({
                style = GetWindowLong(hwnd, GWL_STYLE);
            }, {
                style = GetWindowLongA(hwnd, GWL_STYLE);
            })
            if (style & WS_TABSTOP)
                setFocusPolicy(Qt::FocusPolicy(focusPolicy() | Qt::StrongFocus));
        }
        break;
    case QEvent::WindowBlocked:
        if (hwnd)
            EnableWindow(hwnd, false);
        break;
    case QEvent::WindowUnblocked:
        if (hwnd)
            EnableWindow(hwnd, true);
        break;
    }
    return QWidget::event(e);
}

/*!
    \reimp
*/
void QWinHost::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);

    if (hwnd)
	SetWindowPos(hwnd, HWND_TOP, 0, 0, width(), height(), SWP_SHOWWINDOW);
}

/*!
    \reimp
*/
void QWinHost::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);

    if (hwnd)
	::SetFocus(hwnd);
}

/*!
    \reimp
*/
void QWinHost::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    if (hwnd)
	SetWindowPos(hwnd, HWND_TOP, 0, 0, width(), height(), 0);
}

/*!
    \reimp
*/
bool QWinHost::winEvent(MSG *msg, long *result)
{
    switch (msg->message)
    {
    case WM_SETFOCUS:
        if (hwnd) {
            ::SetFocus(hwnd);
            return true;
        }
    default:
        break;
    }

    return QWidget::winEvent(msg, result);
}
