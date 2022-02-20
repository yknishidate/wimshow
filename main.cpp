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

    void drawImage(HWND hwnd);

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

    void imshow()
    {
        HWND hwnd = createWindow();
        resizeClientRect(hwnd, 400, 400);
        ShowWindow(hwnd, 1);
        drawImage(hwnd);
    }

    void waitClose()
    {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void drawImage(HWND hwnd)
    {
        HDC hdc;
        PAINTSTRUCT ps;
        static HBITMAP hBitmap;
        static HDC hBuffer;

        hdc = GetDC(hwnd);
        hBitmap = CreateCompatibleBitmap(hdc, 400, 400);
        hBuffer = CreateCompatibleDC(hdc);
        SelectObject(hBuffer, hBitmap);
        SelectObject(hBuffer, GetStockObject(NULL_PEN));
        PatBlt(hBuffer, 0, 0, 400, 400, BLACKNESS);
        ReleaseDC(hwnd, hdc);

        auto border = GetSystemMetrics(SM_CXSIZEFRAME);
        std::cout << "border: " << border << std::endl;

        RECT r;
        GetClientRect(hwnd, &r);

        if (r.bottom == 0) {
            return;
        }
        for (int y = 0; y < r.bottom; y++) {
            for (int x = 0; x < r.right; x++) {
                int red = int(x / float(r.right) * 255);
                int green = int(y / float(r.bottom) * 255);
                SetPixel(hBuffer, x, y, RGB(red, green, 0));
            }
        }

        hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc, 0, 0, 400, 400, hBuffer, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
    }
}


int main()
{
    wis::imshow();
    wis::waitClose();
}
