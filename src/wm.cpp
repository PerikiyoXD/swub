#include "wm.h"
#include <iostream>
#include <algorithm>

int handleXError(Display* display, XErrorEvent* error) {
    char errorMessage[256];
    XGetErrorText(display, error->error_code, errorMessage, sizeof(errorMessage));
    std::cerr << "X Error: " << errorMessage << " (Error code: " << error->error_code << ")" << std::endl;
    return 0;
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

    // Set the cursor
    cursorDrawer_ = new CursorDrawer(display_, root_);
    cursorDrawer_->setDefaultCursor();

    // Select input events on the root window
    XSelectInput(display_, root_, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask);

    // Initialize keybindings
    XGrabKey(display_, XKeysymToKeycode(display_, XStringToKeysym("J")), Mod1Mask, root_, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display_, XKeysymToKeycode(display_, XStringToKeysym("K")), Mod1Mask, root_, True, GrabModeAsync, GrabModeAsync);

    // Initialize and draw the background
    backgroundDrawer_ = new BackgroundDrawer(display_, root_);
    backgroundDrawer_->drawBackground();

    // Initialize the frame drawer
    frameDrawer_ = new FrameDrawer(display_, root_);
}

WindowManager::~WindowManager() {
#ifdef DEBUG
    std::cout << "Destroying WindowManager." << std::endl;
#endif
    delete backgroundDrawer_;  // Clean up the BackgroundDrawer
    delete cursorDrawer_;  // Clean up the CursorDrawer
    delete frameDrawer_;  // Clean up the FrameDrawer
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
            case UnmapNotify:
                onUnmapNotify(e.xunmap);
                break;
            case DestroyNotify:
                onDestroyNotify(e.xdestroywindow);
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
    frameDrawer_->createFrame(e.window);
    XMapWindow(display_, e.window);
    windows_.push_back(e.window);
    tileWindows();
}

void WindowManager::onUnmapNotify(const XUnmapEvent& e) {
#ifdef DEBUG
    std::cout << "Handling UnmapNotify event." << std::endl;
#endif
    frameDrawer_->removeFrame(e.window);
    windows_.erase(std::remove(windows_.begin(), windows_.end(), e.window), windows_.end());
    tileWindows();
}

void WindowManager::onDestroyNotify(const XDestroyWindowEvent& e) {
#ifdef DEBUG
    std::cout << "Handling DestroyNotify event." << std::endl;
#endif
    frameDrawer_->removeFrame(e.window);
    windows_.erase(std::remove(windows_.begin(), windows_.end(), e.window), windows_.end());
    tileWindows();
}

void WindowManager::onConfigureRequest(const XConfigureRequestEvent& e) {
#ifdef DEBUG
    std::cout << "Handling ConfigureRequest event." << std::endl;
#endif
    Window frame = frameDrawer_->getFrame(e.window);
    if (frame != None) {
        // Resize and move the frame window
        XMoveResizeWindow(display_, frame, e.x, e.y, e.width, e.height);
    }
    XWindowChanges changes;
    changes.x = 0;
    changes.y = 0;
    changes.width = e.width;
    changes.height = e.height;
    changes.border_width = e.border_width;
    changes.sibling = e.above;
    changes.stack_mode = e.detail;
    XConfigureWindow(display_, e.window, e.value_mask & ~(CWX | CWY), &changes);
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
        Window frame = frameDrawer_->getFrame(windows_[i]);
        if (frame != None) {
            XMoveResizeWindow(display_, frame, i * width, 0, width, height);
            XMoveResizeWindow(display_, windows_[i], 0, 0, width, height);
        }
    }
}
