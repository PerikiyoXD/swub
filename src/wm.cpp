#include "wm.h"
#include <iostream>
#include <algorithm>

int handleXError(Display* display, XErrorEvent* error) {
    char errorMessage[256];
    XGetErrorText(display, error->error_code, errorMessage, sizeof(errorMessage));
    std::cerr << "X Error: " << errorMessage << std::endl;
    return 0; // Return an int instead of void
}

WindowManager::WindowManager() {
#ifdef DEBUG
    std::cout << "Initializing WindowManager." << std::endl;
#endif
    display_ = XOpenDisplay(nullptr);
    if (!display_) {
        std::cerr << "Failed to open X display\n";
        exit(1);
    }
    root_ = DefaultRootWindow(display_);

    // Set the error handler
    XSetErrorHandler(handleXError);

    // Select input events on the root window
    XSelectInput(display_, root_, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask);

    // Initialize keybindings
    XGrabKey(display_, XKeysymToKeycode(display_, XStringToKeysym("J")), Mod1Mask, root_, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display_, XKeysymToKeycode(display_, XStringToKeysym("K")), Mod1Mask, root_, True, GrabModeAsync, GrabModeAsync);

    // Initialize and draw the background
    backgroundDrawer_ = new BackgroundDrawer(display_, root_);
    backgroundDrawer_->drawBackground();
}

WindowManager::~WindowManager() {
#ifdef DEBUG
    std::cout << "Destroying WindowManager." << std::endl;
#endif
    delete backgroundDrawer_;  // Clean up the BackgroundDrawer
    XCloseDisplay(display_);
}

void WindowManager::run() {
#ifdef DEBUG
    std::cout << "Entering main event loop." << std::endl;
#endif
    while (true) {
        XEvent e;
        XNextEvent(display_, &e);

#ifdef DEBUG
        std::cout << "Event received: " << e.type << std::endl;
#endif

        switch (e.type) {
            case MapRequest:
                onMapRequest(e.xmaprequest);
                break;
            case ConfigureRequest:
                onConfigureRequest(e.xconfigurerequest);
                break;
            case KeyPress:
                onKeyPress(e.xkey);
                break;
            // Handle other events like DestroyNotify, etc.
        }
    }
}

void WindowManager::onMapRequest(const XMapRequestEvent& e) {
#ifdef DEBUG
    std::cout << "Handling MapRequest event." << std::endl;
#endif
    XMapWindow(display_, e.window);
    windows_.push_back(e.window);
    tileWindows();
}

void WindowManager::onConfigureRequest(const XConfigureRequestEvent& e) {
#ifdef DEBUG
    std::cout << "Handling ConfigureRequest event." << std::endl;
#endif
    XWindowChanges changes;
    changes.x = e.x;
    changes.y = e.y;
    changes.width = e.width;
    changes.height = e.height;
    changes.border_width = e.border_width;
    changes.sibling = e.above;
    changes.stack_mode = e.detail;
    XConfigureWindow(display_, e.window, e.value_mask, &changes);
}

void WindowManager::onKeyPress(const XKeyEvent& e) {
#ifdef DEBUG
    std::cout << "Handling KeyPress event." << std::endl;
#endif
    if (e.keycode == XKeysymToKeycode(display_, XStringToKeysym("J"))) {
#ifdef DEBUG
        std::cout << "Focusing next window." << std::endl;
#endif
        // Focus next window
        if (!windows_.empty()) {
            Window focused;
            int revert;
            XGetInputFocus(display_, &focused, &revert);
            auto it = std::find(windows_.begin(), windows_.end(), focused);
            if (it != windows_.end()) {
                ++it;
                if (it == windows_.end()) it = windows_.begin();
                XSetInputFocus(display_, *it, RevertToPointerRoot, CurrentTime);
            }
        }
    } else if (e.keycode == XKeysymToKeycode(display_, XStringToKeysym("K"))) {
#ifdef DEBUG
        std::cout << "Focusing previous window." << std::endl;
#endif
        // Focus previous window
        if (!windows_.empty()) {
            Window focused;
            int revert;
            XGetInputFocus(display_, &focused, &revert);
            auto it = std::find(windows_.begin(), windows_.end(), focused);
            if (it != windows_.begin()) {
                --it;
                XSetInputFocus(display_, *it, RevertToPointerRoot, CurrentTime);
            } else {
                XSetInputFocus(display_, windows_.back(), RevertToPointerRoot, CurrentTime);
            }
        }
    }
}

void WindowManager::tileWindows() {
#ifdef DEBUG
    std::cout << "Tiling windows." << std::endl;
#endif
    unsigned int numWindows = windows_.size();
    if (numWindows == 0) return;

    unsigned int width = DisplayWidth(display_, DefaultScreen(display_)) / numWindows;
    unsigned int height = DisplayHeight(display_, DefaultScreen(display_));

    for (unsigned int i = 0; i < numWindows; ++i) {
        XMoveResizeWindow(display_, windows_[i], i * width, 0, width, height);
    }
}
