#pragma once

#define LWND_EXCEPT(hr)                                    \
    Window::HrException(__LINE__, __FILE__, (hr))
#define LWND_LAST_EXCEPT()                                 \
    Window::HrException(__LINE__, __FILE__, GetLastError())
#define LWND_NOGFX_EXCEPT()                                \
    Window::NoGfxException(__LINE__, __FILE__)