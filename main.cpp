#define STRICT
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <string>


namespace wis
{
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    void drawImage(HWND hwnd, const uint8_t* image, int32_t width, int32_t height);

    void resizeClientRect(HWND hwnd, int32_t width, int32_t height)
    {
        RECT clientRect, windowRect;
        GetClientRect(hwnd, &clientRect);
        GetWindowRect(hwnd, &windowRect);
        int32_t diffX = (windowRect.right - windowRect.left) - clientRect.right;
        int32_t diffY = (windowRect.bottom - windowRect.top) - clientRect.bottom;
        MoveWindow(hwnd, windowRect.left, windowRect.top, width + diffX, height + diffY, TRUE);
    }

    HWND createWindow()
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
        if (hwnd == NULL) {
            std::cerr << "failed to create a window." << std::endl;
        }
        return hwnd;
    }

    void showImage(const uint8_t* image, int32_t width, int32_t height)
    {
        HWND hwnd = createWindow();
        resizeClientRect(hwnd, width, height);
        ShowWindow(hwnd, 1);
        drawImage(hwnd, image, width, height);
    }

    void waitClose()
    {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void drawImage(HWND hwnd, const uint8_t* image, int32_t width, int32_t height)
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
                uint8_t red = uint8_t(x / float(width) * 255);
                uint8_t green = uint8_t(y / float(height) * 255);
                SetPixel(hBuffer, x, y, RGB(red, green, 0));
            }
        }

        hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc, 0, 0, width, height, hBuffer, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
    }
}


int main()
{
    int32_t width = 400;
    int32_t height = 400;
    uint8_t* image = new uint8_t[width, height, 3];
    wis::showImage(image, width, height);
    wis::waitClose();
}
