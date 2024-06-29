#include "background_drawer.h"

BackgroundDrawer::BackgroundDrawer(Display* display, Window root)
    : display_(display), root_(root) {}

void BackgroundDrawer::drawBackground() {
    GC gc = XCreateGC(display_, root_, 0, nullptr);

    // Set background color (RGB: 25, 25, 25)
    XSetForeground(display_, gc, 0x191919);

    // Get root window dimensions
    XWindowAttributes rootAttributes;
    XGetWindowAttributes(display_, root_, &rootAttributes);

    // Fill the root window with the background color
    XFillRectangle(display_, root_, gc, 0, 0, rootAttributes.width, rootAttributes.height);

    // Clean up
    XFreeGC(display_, gc);
    XFlush(display_);
}
