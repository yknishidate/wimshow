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

    void drawImage(HWND hwnd, const char* image, int width, int height);

    void resizeClientRect(HWND hwnd, int width, int height)
    {
        RECT clientRect, windowRect;
        GetClientRect(hwnd, &clientRect);
        GetWindowRect(hwnd, &windowRect);
        int diffX = (windowRect.right - windowRect.left) - clientRect.right;
        int diffY = (windowRect.bottom - windowRect.top) - clientRect.bottom;
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

    void showImage(const char* image, int width, int height)
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

    void drawImage(HWND hwnd, const char* image, int width, int height)
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

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int red = int(x / float(width) * 255);
                int green = int(y / float(height) * 255);
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
    int width = 400;
    int height = 400;
    char* image = new char[width, height, 3];
    wis::showImage(image, width, height);
    wis::waitClose();
}
