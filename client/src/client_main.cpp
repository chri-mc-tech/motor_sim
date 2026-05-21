#include "client_main.h"
#include <raylib.h>

int main() {
    InitWindow(1280, 720, "test");

    while (!WindowShouldClose()) {
        ClearBackground({100, 150, 200});
    }
}