#include <stdio.h>

#define CLAY_IMPLEMENTATION
#include "./clay.h"
#include "./clay_renderer_raylib.h"
#include "./raylib/include/raylib.h"

#define INITIAL_SCREEN_WIDTH  1600
#define INITIAL_SCREEN_HEIGHT 900

const uint32_t FONT_ID_BODY_24 = 0;
const uint32_t FONT_ID_BODY_16 = 1;
#define COLOR_ORANGE (Clay_Color) {225, 138, 50, 255}
#define COLOR_BLUE (Clay_Color) {111, 173, 162, 255}
#define COLOR_LIGHT (Clay_Color) { .r = 230, .g = 230, .b = 230, .a = 255 }
#define COLOR_RED (Clay_Color) { .r = 230, .g = 50, .b = 50, .a = 255 }

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int main(void) {
    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
    Clay_Initialize(clayMemory, (Clay_Dimensions) { (float)GetScreenWidth(), (float)GetScreenHeight() }, (Clay_ErrorHandler) { HandleClayErrors, 0 });
    Clay_Raylib_Initialize(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "Clay - Raylib Renderer Example", FLAG_WINDOW_RESIZABLE);

    Font fonts[2];
    fonts[FONT_ID_BODY_24] = LoadFontEx("resources/Roboto-Regular.ttf", 48, 0, 400);
	SetTextureFilter(fonts[FONT_ID_BODY_24].texture, TEXTURE_FILTER_BILINEAR);
    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Roboto-Regular.ttf", 32, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    while (!WindowShouldClose()) { // Will be different for each renderer / environment
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
                .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                .padding = CLAY_PADDING_ALL(16), .childGap = 16
            },
            .backgroundColor = {250,250,255,255}
        }) {
            CLAY(CLAY_ID("SideBar"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(16), .childGap = 16
                },
                .backgroundColor = COLOR_LIGHT
            }) {
                CLAY(CLAY_ID("ProfilePictureOuter"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .padding = CLAY_PADDING_ALL(16),
                        .childGap = 16,
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_RED
                }) {
                    CLAY(CLAY_ID("ProfilePicture"), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_FIXED(60) }
                        }
                    }) {}
                    CLAY_TEXT(CLAY_STRING("Clay - UI Library"), { .fontSize = 24, .textColor = {255, 255, 255, 255} });
                }

                CLAY(CLAY_ID("MainContent"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
                    },
                    .backgroundColor = COLOR_LIGHT
                }) {}
            }
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout(dt);
        Clay_Raylib_Render(renderCommands, fonts);
    }
}
