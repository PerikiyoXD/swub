#ifndef CURSOR_DRAWER_H
#define CURSOR_DRAWER_H

#include <X11/Xlib.h>

class CursorDrawer {
public:
    CursorDrawer(Display* display, Window root);
    void setDefaultCursor();

private:
    Display* display_;
    Window root_;
    Cursor cursor_;
};

#endif // CURSOR_DRAWER_H
