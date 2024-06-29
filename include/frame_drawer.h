#ifndef FRAME_DRAWER_H
#define FRAME_DRAWER_H

#include <X11/Xlib.h>
#include <unordered_map>
#include <iostream>

class FrameDrawer {
public:
    FrameDrawer(Display* display, Window root);
    ~FrameDrawer();  // Destructor to clean up resources
    void createFrame(Window window);
    void removeFrame(Window window);

private:
    Display* display_;
    Window root_;
    std::unordered_map<Window, Window> frames_;

    void drawFrame(Window frame);
};

#endif // FRAME_DRAWER_H
