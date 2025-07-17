#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <math.h>

// Global variables for 3D demo
static int g_running = 1;
static float g_rotationX = 0.0f;
static float g_rotationY = 0.0f;
static float g_rotationZ = 0.0f;

// Window procedure for the 3D renderer window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            g_running = 0;
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                g_running = 0;
                PostQuitMessage(0);
            }
            return 0;
        case WM_PAINT:
            ValidateRect(hwnd, NULL);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
// Function pointer typedefs
typedef int  (*Add)(int, int);
typedef int  (*Sub)(int, int);
typedef int  (*Mul)(int, int);
typedef void (*Print_E)(const char*);
typedef void (*Print_T)(const char*);
typedef int  (*Count)();
typedef void (*Matrix_Draw)();
typedef void (*Dot_Position)();
typedef void (*Bot_Navigate)(int, int);
typedef int  (*Rand_Bot)(int);

typedef int  (*Sound_Init)();
typedef void (*Sound_Cleanup)();
typedef void (*Sound_Play_Tone)(double, int);
typedef void (*Sound_Play_Tone_Async)(double, int);
typedef void (*Sound_Set_Frequency)(double);
typedef int  (*Sound_Is_Playing)();

typedef int  (*Render_Init)(HWND);
typedef void (*Render_Cleanup)();
typedef void (*Render_Clear)(unsigned char);
typedef void (*Render_Pixel)(int, int, unsigned char);
typedef void (*Render_Line)(int, int, int, int, unsigned char);
typedef void (*Render_Cube)(float, float, float);
typedef void (*Render_Present)();

typedef void (*Cmd_Drawer_ANSI)(unsigned int);

// DLL management structure
typedef struct {
    HMODULE handle;
    const char* name;
    int loaded;
} DllInfo;

// Function information structure
typedef struct {
    void** funcPtr;
    const char* funcName;
    int dllIndex;
    const char* description;
} FunctionInfo;

// Cleanup function
void cleanup_dlls(DllInfo* dlls, int count) {
    for (int i = 0; i < count; i++) {
        if (dlls[i].handle) {
            printf("Unloading %s...\n", dlls[i].name);
            FreeLibrary(dlls[i].handle);
            dlls[i].handle = NULL;
            dlls[i].loaded = 0;
        }
    }
}

int main() {

    // Function pointers
    Add add = NULL;
    Sub sub = NULL;
    Mul mul = NULL;
    Print_E print_e = NULL;
    Print_T print_t = NULL;
    Count count = NULL;
    Matrix_Draw matrix_draw = NULL;
    Dot_Position dot_position = NULL;
    Rand_Bot rand_bot = NULL;
    Bot_Navigate bot_navigate = NULL;

    // Add sound function pointers
    Sound_Init sound_init = NULL;
    Sound_Cleanup sound_cleanup = NULL;
    Sound_Play_Tone sound_play_tone = NULL;
    Sound_Play_Tone_Async sound_play_tone_async = NULL;
    Sound_Set_Frequency sound_set_frequency = NULL;
    Sound_Is_Playing sound_is_playing = NULL;

    // Add render function pointers
    Render_Init render_init = NULL;
    Render_Cleanup render_cleanup = NULL;
    Render_Clear render_clear = NULL;
    Render_Pixel render_pixel = NULL;
    Render_Line render_line = NULL;
    Render_Cube render_cube = NULL;
    Render_Present render_present = NULL;

    // Add cmd_drawer function pointer
    Cmd_Drawer_ANSI cmd_drawer_ANSI = NULL;

    // DLL information array
    DllInfo dlls[] = {
        {NULL, "math.dll", 0},
        {NULL, "printer.dll", 0},
        {NULL, "count.dll", 0},
        {NULL, "matrix.dll", 0},
        {NULL, "bot.dll", 0},
        {NULL, "sound.dll", 0},
        {NULL, "render.dll", 0}
    };
    const int dll_count = sizeof(dlls) / sizeof(dlls[0]);
    
    // Function information array
    FunctionInfo functions[] = {
        //
        {(void**)&add, "add", 0, "Addition function"},
        {(void**)&sub, "sub", 0, "Subtraction function"},
        {(void**)&mul, "mul", 0, "Multiplication function"},
        //
        {(void**)&print_e, "print_e", 1, "Error printing function"},
        {(void**)&print_t, "print_t", 1, "Text printing function"},
        //
        {(void**)&count, "count", 2, "Counter function"},
        //
        {(void**)&matrix_draw, "matrix_draw", 3, "Matrix drawing function"},
        {(void**)&dot_position, "dot_position", 3, "Dot position function"},
        {(void**)&bot_navigate, "bot_navigate", 3, "Bot navigation function"},
        //
        {(void**)&rand_bot, "rand_bot", 4, "Random bot function"},
        //
        {(void**)&sound_init, "sound_init", 5, "Initialize sound system"},
        {(void**)&sound_cleanup, "sound_cleanup", 5, "Cleanup sound system"},
        {(void**)&sound_play_tone, "sound_play_tone", 5, "Play tone"},
        {(void**)&sound_play_tone_async, "sound_play_tone_async", 5, "Play tone async"},
        {(void**)&sound_set_frequency, "sound_set_frequency", 5, "Set frequency"},
        {(void**)&sound_is_playing, "sound_is_playing", 5, "Check if playing"},
        //
        {(void**)&cmd_drawer_ANSI, "cmd_drawer_ANSI", 6, "ANSI drawer function"}
    };
    const int func_count = sizeof(functions) / sizeof(functions[0]);
    
    // Load DLLs
    for (int i = 0; i < dll_count; i++) {
        dlls[i].handle = LoadLibrary(dlls[i].name);
        printf("Loading %s...\n", dlls[i].name);
        Sleep(100); // Simulate loading time
        if (dlls[i].handle) {
            dlls[i].loaded = 1;
        } else {
            printf("FAILED (Error: %lu)\n", GetLastError());
            cleanup_dlls(dlls, dll_count);
            return 1;
        }
    }
    // Load function addresses
    for (int i = 0; i < func_count; i++) {
        *(functions[i].funcPtr) = GetProcAddress(dlls[functions[i].dllIndex].handle, functions[i].funcName);
        if (*(functions[i].funcPtr)) {
        } else {
            printf("FAILED (Error: %lu)\n", GetLastError());
            cleanup_dlls(dlls, dll_count);
            return 1;
        }
    }
    
    if (sound_init()) {
        printf("Sound system initialized successfully!\n");
    } else {
        printf("Failed to initialize sound system!\n");
    }

    // Test the functions
    // int a = 15, b = 7;
    // 
    // printf("a = %d, b = %d\n", a, b);
    // printf("add(a, b) = %d\n", add(a, b));
    // printf("sub(a, b) = %d\n", sub(a, b));
    // printf("mul(a, b) = %d\n", mul(a, b));
// 
    // print_t("This is a text message!");
    // print_e("This is an error message!");
// 
    // for (int i = 0; i < 20; ++i) {
    //     printf("\x1B[8Hcount() = %d", count());  // \x1B[K clears the line
    //     fflush(stdout);
    //     Sleep(100);
    // }

    system("cls");  // Clear the console

    unsigned int tick = 0;
    //while (1) {
    //    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
    //        break; // Exit the loop if Escape is pressed
    //    }
    //    matrix_draw();
//
    //    if ((GetAsyncKeyState(VK_UP) & 0x8000) ||
    //        (GetAsyncKeyState(VK_DOWN) & 0x8000) ||
    //        (GetAsyncKeyState(VK_LEFT) & 0x8000) ||
    //        (GetAsyncKeyState(VK_RIGHT) & 0x8000)) {
    //        dot_position();
    //        //sound_play_tone_async(880.0, 50); // Quick high beep for player movement
    //    } else {
    //        bot_navigate(rand_bot(rand()), rand_bot(rand()));
    //        // sound_play_tone_async(220.0, 50); // Lower beep for bot movement
    //    }
//
//
    //    if (tick % 60 == 0) { // Every second
    //        sound_play_tone_async(500.0, 200); // Short 200ms tone every second
    //    }
    //    
    //    Sleep(1000/60); // Sleep for 16.67 ms (approx. 60 FPS)
    //    tick++;
    //}
    
    //sound_cleanup();
    
    tick = 0;
    DWORD lastTime = GetTickCount();
    DWORD frameCount = 0;
    float fps = 0.0f;
    
    while (1)
    {
        DWORD currentTime = GetTickCount();
        frameCount++;
        
        // Calculate FPS every second
        if (currentTime - lastTime >= 1000) {
            fps = frameCount * 1000.0f / (currentTime - lastTime);
            frameCount = 0;
            lastTime = currentTime;
        }
        
        // Display FPS in top-right corner
        printf("\x1B[0m\x1B[0H%.2f", fps);
        // fflush(stdout);
        
        cmd_drawer_ANSI(tick);
        tick++;
    }
    

    cleanup_dlls(dlls, dll_count);
    
    return 0;
}
