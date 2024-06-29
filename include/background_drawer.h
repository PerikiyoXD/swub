#ifndef BACKGROUND_DRAWER_H
#define BACKGROUND_DRAWER_H

#include <X11/Xlib.h>

class BackgroundDrawer {
public:
    BackgroundDrawer(Display* display, Window root);
    void drawBackground();

private:
    Display* display_;
    Window root_;
};

#endif // BACKGROUND_DRAWER_H
