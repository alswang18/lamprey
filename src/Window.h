#pragma once
#include "LampreyException.h"
#include "LampreyWin.h"

class Window
{
    class Exception : public LampreyException
    {
    public:
        Exception(int line, const char* file, HRESULT hr) noexcept;
        const char* what() const noexcept override;
        virtual const char* GetType() const noexcept;
        static std::string TranslateErrorCode(HRESULT hr) noexcept;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;

    private:
        HRESULT hr;
    };

private:
    // singleton manages registration/cleanup of window class
    class WindowClass
    {
    public:
        static const char* GetName();
        static HINSTANCE GetInstance();

    private:
        WindowClass();
        ~WindowClass();
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=(const WindowClass&) = delete;
        static constexpr const char* wndClassName = "Lamprey Direct3D Engine Window";
        static WindowClass wndClass;
        HINSTANCE hInst;
    };

public:
    Window(int width, int height, const char* name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    unsigned int width;
    unsigned int height;
    HWND hWnd;
};

// error exception helper macro
#define LWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr)
#define LWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())