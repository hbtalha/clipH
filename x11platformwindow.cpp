/*
    Copyright (c) 2020, Lukas Holecek <hluk@email.cz>

    This file is part of CopyQ.

    CopyQ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CopyQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CopyQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sleeptimer.h"
#include "x11platformwindow.h"

#include <unistd.h>
#include <QX11Info>
#include <QTimer>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#ifdef HAS_X11TEST
#   include <X11/extensions/XTest.h>
#endif



void waitMs(int msec)
{
    if (msec <= 0)
        return;

    QEventLoop loop;
    QTimer t;
    QObject::connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
    t.start(msec);
    loop.exec();
}

void simulateKeyPress(Display *display, Window window, unsigned int modifiers, unsigned int key)
{
    XKeyEvent event;
    XEvent *xev = reinterpret_cast<XEvent *>(&event);
    event.display     = display;
    event.window      = window;
    event.root        = DefaultRootWindow(display);
    event.subwindow   = None;
    event.time        = CurrentTime;
    event.x           = 1;
    event.y           = 1;
    event.x_root      = 1;
    event.y_root      = 1;
    event.same_screen = True;
    event.keycode     = XKeysymToKeycode(display, key);
    event.state       = modifiers;

    event.type = KeyPress;
    XSendEvent(display, window, True, KeyPressMask, xev);
    XSync(display, False);

    event.type = KeyRelease;
    XSendEvent(display, window, True, KeyPressMask, xev);
    XSync(display, False);
}

class X11WindowProperty final
{
public:
    X11WindowProperty(Display *display, Window w, Atom property, long longOffset,
                      long longLength, Atom reqType)
    {
        if ( XGetWindowProperty(display, w, property, longOffset, longLength, false,
                                reqType, &type, &format, &len, &remain, &data) != Success )
        {
            data = nullptr;
        }
    }

    ~X11WindowProperty()
    {
        if (data != nullptr)
            XFree(data);
    }

    bool isValid() const
    {
        return data != nullptr;
    }

    X11WindowProperty(const X11WindowProperty &) = delete;
    X11WindowProperty &operator=(const X11WindowProperty &) = delete;

    Atom type{};
    int format{};
    unsigned long len{};
    unsigned long remain{};
    unsigned char *data;
};

Window getCurrentWindow()
{
    if (!QX11Info::isPlatformX11())
        return 0L;

    auto display = QX11Info::display();
    XSync(display, False);

    static Atom atomWindow = XInternAtom(display, "_NET_ACTIVE_WINDOW", true);

    X11WindowProperty property(display, DefaultRootWindow(display), atomWindow, 0l, 1l, XA_WINDOW);

    if ( property.isValid() && property.type == XA_WINDOW && property.format == 32 && property.len == 1)
        return *reinterpret_cast<Window *>(property.data);

    return 0L;
}



X11PlatformWindow::X11PlatformWindow(Window winId)
    : m_window(winId)
{
}


void X11PlatformWindow::raise()
{
    Q_ASSERT( isValid() );

    if (!QX11Info::isPlatformX11())
        return;


    auto display = QX11Info::display();

    XEvent e{};
    memset(&e, 0, sizeof(e));
    e.type = ClientMessage;
    e.xclient.display = display;
    e.xclient.window = m_window;
    e.xclient.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    e.xclient.format = 32;
    e.xclient.data.l[0] = 2;
    e.xclient.data.l[1] = CurrentTime;
    e.xclient.data.l[2] = 0;
    e.xclient.data.l[3] = 0;
    e.xclient.data.l[4] = 0;

    XWindowAttributes wattr{};
    XGetWindowAttributes(display, m_window, &wattr);

    if (wattr.map_state == IsViewable)
    {
        XSendEvent(display, wattr.screen->root, False,
                   SubstructureNotifyMask | SubstructureRedirectMask,
                   &e);
        XSync(display, False);
        XRaiseWindow(display, m_window);
        XSetInputFocus(display, m_window, RevertToPointerRoot, CurrentTime);
        XSync(display, False);
    }
}

void X11PlatformWindow::pasteClipboard()
{
    sendKeyPress(XK_Shift_L, XK_Insert);
//    sendKeyPress(XK_Shift_L, XK_Insert);
}

void X11PlatformWindow::copy()
{

}

bool X11PlatformWindow::isValid() const
{
    return m_window != 0L;
}

bool X11PlatformWindow::waitForFocus(int ms)
{
    Q_ASSERT( isValid() );

    if (ms >= 0)
    {
        SleepTimer t(ms);
        while (t.sleep())
        {
            const auto currentWindow = getCurrentWindow();
            if (currentWindow == m_window)
                return true;
        }
    }

    return m_window == getCurrentWindow();
}

void X11PlatformWindow::sendKeyPress(int modifier, int key)
{
    Q_ASSERT( isValid() );

    if ( !waitForFocus(50) )
    {
        raise();
        if ( !waitForFocus(150) )
        {
            return;
        }
    }

    waitMs(50);

    if (!QX11Info::isPlatformX11())
        return;

    auto display = QX11Info::display();

    const int modifierMask = (modifier == XK_Control_L) ? ControlMask : ShiftMask;
    simulateKeyPress(display, m_window, modifierMask, key);

}
