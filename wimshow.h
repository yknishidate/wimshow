#pragma once
#define STRICT
#include <windows.h>
#include <iostream>


namespace wis
{
    inline LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    inline void drawImage(HWND hwnd, const uint8_t* image, int32_t width, int32_t height)
    {
        HDC hdc;
        PAINTSTRUCT ps;
        static HBITMAP hBitmap;
        static HDC hBuffer;

        hdc = GetDC(hwnd);
        hBitmap = CreateCompatibleBitmap(hdc, width, height);
        hBuffer = CreateCompatibleDC(hdc);
        SelectObject(hBuffer, hBitmap);
        SelectObject(hBuffer, GetStockObject(NULL_PEN));
        PatBlt(hBuffer, 0, 0, width, height, BLACKNESS);
        ReleaseDC(hwnd, hdc);

        for (int32_t y = 0; y < height; y++) {
            for (int32_t x = 0; x < width; x++) {
                int32_t offset = (y * width + x) * 3;
                uint8_t r = image[offset + 0];
                uint8_t g = image[offset + 1];
                uint8_t b = image[offset + 2];
                SetPixel(hBuffer, x, y, RGB(r, g, b));
            }
        }

        hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc, 0, 0, width, height, hBuffer, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
    }

    inline void resizeClientRect(HWND hwnd, int32_t width, int32_t height)
    {
        RECT clientRect, windowRect;
        GetClientRect(hwnd, &clientRect);
        GetWindowRect(hwnd, &windowRect);
        int32_t diffX = (windowRect.right - windowRect.left) - clientRect.right;
        int32_t diffY = (windowRect.bottom - windowRect.top) - clientRect.bottom;
        MoveWindow(hwnd, windowRect.left, windowRect.top, width + diffX, height + diffY, TRUE);
    }

    inline HWND createWindow()
    {
        const wchar_t CLASS_NAME[] = L"Window Class";
        HINSTANCE hInstance = GetModuleHandle(0);
        WNDCLASS wc{};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        RegisterClass(&wc);

        HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Window", WS_OVERLAPPEDWINDOW,
                                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                   NULL, NULL, hInstance, NULL);
        return hwnd;
    }

    inline void showImage(const uint8_t* image, int32_t width, int32_t height)
    {
        HWND hwnd = createWindow();
        resizeClientRect(hwnd, width, height);
        ShowWindow(hwnd, 1);
        drawImage(hwnd, image, width, height);
    }

    inline void waitClose()
    {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
