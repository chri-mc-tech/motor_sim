

int main() {
    using namespace graphics;
    InitWindow(window_width, window_height, "test");

    while (!WindowShouldClose()) {
        main_loop()
    }
}
