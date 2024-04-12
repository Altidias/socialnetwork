// Application.hpp
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "SocialNetwork.hpp"
#include "NetworkManager.hpp"
#include "imgui.h"
#include "imguial_term.h"

// Data stored per platform window
struct WGL_WindowData {
    HDC hDC;
};

class Application {
public:
    Application();
    virtual ~Application();
    void Run();

private:
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data);
    void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data);
    void ResetDeviceWGL();
    void MainLoop();

    SocialNetwork socialNetwork; 
    NetworkManager networkManager; 
    HGLRC g_hRC;
    WGL_WindowData g_MainWindow;
    int g_Width;
    int g_Height;

    bool show_network_manager = false;
    bool show_logger = false;
    ImVec4 clear_color;
    
    
};