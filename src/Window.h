#pragma once
#include "Graphics.h"
#include "Keyboard.h"
#include "LampreyException.h"
#include "LampreyWin.h"
#include "Mouse.h"
#include "Timer.h"
#include <memory>
#include <optional>

class Window
{
public:
    class Exception : public LampreyException
    {
        using LampreyException::LampreyException;

    public:
        static std::string TranslateErrorCode(
            HRESULT hr) noexcept;
    };
    class HrException : public Exception
    {
    public:
        HrException(int line, const char* file,
                    HRESULT hr) noexcept;
        const char* what() const noexcept override;
        const char* GetType() const noexcept override;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorDescription() const noexcept;

    private:
        HRESULT hr;
    };
    class NoGfxException : public Exception
    {
    public:
        using Exception::Exception;
        const char* GetType() const noexcept override;
    };

private:
    // singleton manages registration/cleanup of window
    // class
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
        static constexpr const char* wndClassName =
            "Lamprey Direct3D Engine Window";
        static WindowClass wndClass;
        HINSTANCE hInst;
    };

public:
    Window(int width, int height, const char* name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Graphics& Gfx();
    void SetTitle(const std::string& title);
    static std::optional<int> ProcessMessages();

public:
    Keyboard keyboard;
    Mouse mouse;
    Timer timer;

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd,
                                           UINT msg,
                                           WPARAM wParam,
                                           LPARAM lParam);
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd,
                                           UINT msg,
                                           WPARAM wParam,
                                           LPARAM lParam);
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam,
                      LPARAM lParam);

private:
    unsigned int width;
    unsigned int height;
    HWND hWnd;
    std::unique_ptr<Graphics> pGfx;
};
