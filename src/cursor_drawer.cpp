#include "cursor_drawer.h"
#include <X11/cursorfont.h>
#include <iostream>


CursorDrawer::CursorDrawer(Display* display, Window root)
    : display_(display), root_(root) {
#ifdef DEBUG
    std::cout << "CursorDrawer initialized." << std::endl;
#endif
    // Create the default cursor
    cursor_ = XCreateFontCursor(display_, XC_left_ptr);
}

void CursorDrawer::setDefaultCursor() {
#ifdef DEBUG
    std::cout << "Setting default cursor." << std::endl;
#endif
    // Set the cursor for the root window
    XDefineCursor(display_, root_, cursor_);
    XFlush(display_);
}
