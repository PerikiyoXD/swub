import os

project_structure = {
    "CMakeLists.txt": """cmake_minimum_required(VERSION 3.10)
project(TilingWM)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add source files
set(SOURCES
    src/main.cpp
    src/wm.cpp
)

# Add executable
add_executable(tiling-wm ${SOURCES})

# Find X11 library
find_package(X11 REQUIRED)

# Link libraries
target_link_libraries(tiling-wm X11)
""",
    "src/main.cpp": """#include "wm.h"

int main() {
    WindowManager wm;
    wm.run();
    return 0;
}
""",
    "src/wm.cpp": """#include "wm.h"
#include <iostream>

WindowManager::WindowManager() {
    display_ = XOpenDisplay(nullptr);
    if (!display_) {
        std::cerr << "Failed to open X display\\n";
        exit(1);
    }
    root_ = DefaultRootWindow(display_);
}

WindowManager::~WindowManager() {
    XCloseDisplay(display_);
}

void WindowManager::run() {
    XSelectInput(display_, root_, SubstructureRedirectMask | SubstructureNotifyMask);

    while (true) {
        XEvent e;
        XNextEvent(display_, &e);
        
        switch (e.type) {
            case MapRequest:
                XMapWindow(display_, e.xmaprequest.window);
                break;
            // Handle other events like ConfigureRequest, KeyPress, etc.
        }
    }
}
""",
    "include/wm.h": """#ifndef WM_H
#define WM_H

#include <X11/Xlib.h>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();
    void run();
    
private:
    Display* display_;
    Window root_;
};

#endif // WM_H
"""
}

def create_project_structure(base_dir):
    for filepath, content in project_structure.items():
        full_path = os.path.join(base_dir, filepath)
        os.makedirs(os.path.dirname(full_path), exist_ok=True)
        with open(full_path, 'w') as f:
            f.write(content)
    print("Project structure created successfully.")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Generate basic project structure for a tiling window manager.')
    parser.add_argument('project_dir', type=str, help='The root directory for the project.')

    args = parser.parse_args()
    create_project_structure(args.project_dir)
