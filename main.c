#include <stdio.h>

#define CLAY_IMPLEMENTATION
#include "./clay.h"
#include "./clay_renderer_raylib.h"
#include "./raylib/include/raylib.h"
#include "./tinyfiledialogs/tinyfiledialogs.h"

#define INITIAL_SCREEN_WIDTH  1600
#define INITIAL_SCREEN_HEIGHT 900

const uint32_t FONT_ID_BODY_24 = 0;
const uint32_t FONT_ID_BODY_16 = 1;

#define COLOR_ORANGE               (Clay_Color) { 225, 138,  50, 255 }
#define COLOR_BLUE                 (Clay_Color) { 111, 173, 162, 255 }
#define COLOR_LIGHT                (Clay_Color) { 230, 230, 230, 255 }
#define COLOR_RED                  (Clay_Color) { 230,  50,  50, 255 }
#define FONT_COLOR_LIGHT           (Clay_Color) { 255, 255, 255, 255 }
#define COLOR_BG1                  (Clay_Color) {  42,  37,  36, 255 }
#define COLOR_BG2                  (Clay_Color) {  62,  57,  56, 255 }
#define COLOR_BASIC_BUTTON         (Clay_Color) {  70,  70,  70, 255 }
#define COLOR_BASIC_BUTTON_HOVERED (Clay_Color) { 100, 100, 100, 255 }

Image img = {0};
Texture2D img_texture = {0};

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

typedef void (*ButtonCallback)(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);

void handle_blur_button(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData) {
    if (pointerData.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME) return;
    if (img.data == NULL) return;

    ImageBlurGaussian(&img, 4);
    UnloadTexture(img_texture);
    img_texture = LoadTextureFromImage(img);
}

void handle_open_image_button(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData) {
    if (pointerData.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME) return;
	char const *acceptable_image_file_extensions[1] = {"*.bmp"};
    char *filepath = tinyfd_openFileDialog("Choose image", "./", 1, acceptable_image_file_extensions, NULL, 0);
    if (filepath == NULL) {
        return;
    }
    printf("%s\n", filepath);
    img = LoadImage(filepath);
    img_texture = LoadTextureFromImage(img);
}

void basic_button(Clay_ElementId id, Clay_String text, ButtonCallback callback) {
    CLAY(id, {
        .layout = {
            .padding = {8, 8, 2, 2},
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
            .childGap = 4,
        },
        .cornerRadius = CLAY_CORNER_RADIUS(4),
        .backgroundColor = Clay_Hovered() ? COLOR_BASIC_BUTTON_HOVERED : COLOR_BASIC_BUTTON,
    }) {
        Clay_OnHover(callback, 0);
        CLAY_TEXT(text, {
            .fontSize = 24,
            .textColor = FONT_COLOR_LIGHT
        });
    }
}

int main(void) {
    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
    Clay_Initialize(clayMemory, (Clay_Dimensions) { (float)GetScreenWidth(), (float)GetScreenHeight() }, (Clay_ErrorHandler) { HandleClayErrors, 0 });
    Clay_Raylib_Initialize(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "Clay - Raylib Renderer Example", FLAG_WINDOW_RESIZABLE);
    SetExitKey(KEY_NULL);
    SetTargetFPS(144);

    Font fonts[2];
    fonts[FONT_ID_BODY_24] = LoadFontEx("resources/Roboto-Regular.ttf", 48, 0, 400);
	SetTextureFilter(fonts[FONT_ID_BODY_24].texture, TEXTURE_FILTER_BILINEAR);
    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Roboto-Regular.ttf", 32, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    while (!WindowShouldClose()) {
        Clay_SetLayoutDimensions((Clay_Dimensions) { GetScreenWidth(), GetScreenHeight() });
        Vector2 mouse_position = GetMousePosition();
        bool is_mouse_0_down = IsMouseButtonDown(0);
        float dt = GetFrameTime();
        Vector2 mouse_wheel_move = GetMouseWheelMoveV();
        Clay_SetPointerState((Clay_Vector2) { mouse_position.x, mouse_position.y }, is_mouse_0_down);
        Clay_UpdateScrollContainers(true, (Clay_Vector2) { mouse_wheel_move.x, mouse_wheel_move.y }, dt);

        Clay_BeginLayout();

        CLAY(CLAY_ID("OuterContainer"), {
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16,
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
            .backgroundColor = COLOR_BG1,
        }) {
            CLAY(CLAY_ID("Navbar"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
                    .childGap = 16,
                }
            }) {
                basic_button(CLAY_ID("OpenImageButton"), CLAY_STRING("Open Image"), handle_open_image_button);
                if (img.data != NULL) {
                    basic_button(CLAY_ID("BlurButton"), CLAY_STRING("Blur Image"), handle_blur_button);
                }
            }
            CLAY(CLAY_ID("ImageSlot"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                    .childGap = 4,
                    .padding = CLAY_PADDING_ALL(10),
                    .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
                },
                .cornerRadius = 8,
                .backgroundColor = COLOR_BG2,
            }) {
                if (img.data != NULL) {
                    CLAY(CLAY_ID("OriginalImage"), {
                        .image = { .imageData = &img_texture },
                        .aspectRatio = (float)(img.width) / img.height,
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                        },
                    }) {}
                }
            }
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout(dt);
        Clay_Raylib_Render(renderCommands, fonts);
    }
}
