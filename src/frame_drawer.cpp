#include "frame_drawer.h"
#include <iostream>

FrameDrawer::FrameDrawer(Display* display, Window root)
    : display_(display), root_(root) {
#ifdef DEBUG
    std::cout << "FrameDrawer initialized." << std::endl;
#endif
}

FrameDrawer::~FrameDrawer() {
    for (auto& pair : frames_) {
        Window frame = pair.second;
        XUnmapWindow(display_, frame);
        XDestroyWindow(display_, frame);
    }
}

void FrameDrawer::createFrame(Window window) {
#ifdef DEBUG
    std::cout << "Creating frame for window: " << window << std::endl;
#endif
    XWindowAttributes attr;
    XGetWindowAttributes(display_, window, &attr);

    // Create a frame window
    Window frame = XCreateSimpleWindow(display_, root_, attr.x, attr.y, attr.width, attr.height, 2, BlackPixel(display_, DefaultScreen(display_)), WhitePixel(display_, DefaultScreen(display_)));

    // Reparent the window inside the frame
    XReparentWindow(display_, window, frame, 0, 0);

    // Map the frame window
    XMapWindow(display_, frame);

    // Store the frame in the map
    frames_[window] = frame;

    // Draw the initial frame
    drawFrame(frame);
}

void FrameDrawer::removeFrame(Window window) {
#ifdef DEBUG
    std::cout << "Removing frame for window: " << window << std::endl;
#endif
    auto it = frames_.find(window);
    if (it != frames_.end()) {
        Window frame = it->second;
        XUnmapWindow(display_, frame);
        XDestroyWindow(display_, frame);
        frames_.erase(it);
    }
}

void FrameDrawer::drawFrame(Window frame) {
#ifdef DEBUG
    std::cout << "Drawing frame for window: " << frame << std::endl;
#endif
    // For now, this is a no-op since we use XCreateSimpleWindow to set border colors.
    // In a more advanced version, this method could handle custom drawing.
}
