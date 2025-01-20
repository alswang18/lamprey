#include "Windows.h"
#include <GLFW/glfw3.h>

int main()
{
    if (!glfwInit())
    {
        return -1;
    }

    // Create window
    glfwCreateWindow(800, 600, "Lamprey", nullptr, nullptr);

    return 0;
}