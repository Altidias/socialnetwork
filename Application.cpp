// Application.cpp
#include "Application.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#include "NetworkManager.hpp"
#include "SocialNetwork.hpp"
#include "Logger.hpp"
#include "IconsFontAwesome5.h"

#include <stdexcept>
#include <GL/GL.h>

Application::Application()
    : g_hRC(nullptr), g_Width(1280), g_Height(800),
      show_network_manager(false), show_logger(false),
    clear_color(ImVec4(0.75f, 0.75f, 0.75f, 1.00f)) {
    // Initialize the application window and Dear ImGui
    WNDCLASSEXW wc = { sizeof(WNDCLASSEX), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ApplicationClass", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Social Network", WS_OVERLAPPEDWINDOW, 100, 100, g_Width, g_Height, nullptr, nullptr, wc.hInstance, nullptr);

    // Store the window handle in a way that the static WndProc can retrieve it,
    // for example by using SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    if (!CreateDeviceWGL(hwnd, &g_MainWindow)) {
        CleanupDeviceWGL(hwnd, &g_MainWindow);
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        throw std::runtime_error("Failed to create OpenGL context.");
    }
    wglMakeCurrent(g_MainWindow.hDC, g_hRC);

     IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.Fonts->AddFontDefault();
    float baseFontSize = 13.0f;
    float iconFontSize = baseFontSize * 2.0f / 3.0f;

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;

    io.Fonts->AddFontFromFileTTF( FONT_ICON_FILE_NAME_FAS, iconFontSize, &icons_config, icons_ranges );

    // Setup ImGui bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplOpenGL3_Init("#version 130");

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (g_hRC) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(g_hRC);
    }
    if (g_MainWindow.hDC) {
        ::ReleaseDC(FindWindowW(L"ApplicationClass", nullptr), g_MainWindow.hDC);
    }
    UnregisterClassW(L"ApplicationClass", GetModuleHandle(nullptr));
}

void Application::Run() {
    MainLoop();
}

void Application::MainLoop() {
    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
        if (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Full-screen window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Main DockSpace", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
        
        

        // DockSpace
        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        
        // Menu Bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Network Manager")) {
                if (ImGui::MenuItem(ICON_FA_EYE " Show", NULL, &show_network_manager)) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Logger")) {
                if (ImGui::MenuItem(ICON_FA_EYE " Show", NULL, &show_logger)) {}
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        
        // End of the full-screen window
        ImGui::End();

        // Conditional rendering based on menu selection
        if (show_network_manager) {
            ImGui::Begin("Network Manager", &show_network_manager);
            networkManager.renderSocialNetworkUI();
            ImGui::End();
        }

        if (show_logger) {
            ImGui::Begin("Logger", &show_logger);
            Logger::getInstance().draw(ImVec2(600, 600)); // Adjust size as needed
            ImGui::End();
        }



        ImGui::Render();
        RECT rect;
        ::GetClientRect(FindWindowW(L"ApplicationClass", nullptr), &rect);
        int display_w = rect.right - rect.left;
        int display_h = rect.bottom - rect.top;
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ::SwapBuffers(g_MainWindow.hDC);
    }
}

// Implement other Application methods (WndProc, CreateDeviceWGL, CleanupDeviceWGL...)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Application* app = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED && app) {
            app->g_Width = LOWORD(lParam);  // Access through app instance
            app->g_Height = HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool Application::CreateDeviceWGL(HWND hWnd, WGL_WindowData* data) {
    HDC hDc = ::GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    const int pf = ::ChoosePixelFormat(hDc, &pfd);
    if (pf == 0)
        return false;
    if (::SetPixelFormat(hDc, pf, &pfd) == FALSE)
        return false;
    ::ReleaseDC(hWnd, hDc);

    data->hDC = ::GetDC(hWnd);
    if (!g_hRC)
        g_hRC = wglCreateContext(data->hDC);
    return true;
}

void Application::CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data) {
    wglMakeCurrent(nullptr, nullptr);
    ::ReleaseDC(hWnd, data->hDC);
}