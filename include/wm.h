#ifndef WM_H
#define WM_H

#include <X11/Xlib.h>
#include <vector>
#include "background_drawer.h"
#include "cursor_drawer.h"
#include "frame_drawer.h"

class WindowManager {
public:
    WindowManager();
    ~WindowManager();
    void run();

private:
    Display* display_;
    Window root_;
    std::vector<Window> windows_;
    BackgroundDrawer* backgroundDrawer_;
    CursorDrawer* cursorDrawer_;
    FrameDrawer* frameDrawer_;

    void onMapRequest(const XMapRequestEvent& e);
    void onUnmapNotify(const XUnmapEvent& e);
    void onDestroyNotify(const XDestroyWindowEvent& e);
    void onConfigureRequest(const XConfigureRequestEvent& e);
    void onKeyPress(const XKeyEvent& e);
    void tileWindows();
};

#endif // WM_H
