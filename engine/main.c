#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <math.h>


// Function pointer typedefs

typedef void (*ANSI_DRAWER)();

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


    ANSI_DRAWER output_buffer = NULL;

    // DLL information array
    DllInfo dlls[] = {
        {NULL, "bin/ANSI_DRAWER.dll", 0}
    };
    const int dll_count = sizeof(dlls) / sizeof(dlls[0]);
    
    // Function information array
    FunctionInfo functions[] = {
        //
        {(void**)&output_buffer, "output_buffer", 0, "Outputs a buffer of ANSI colors"},
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
    
    DWORD lastTime = GetTickCount();
    DWORD frameCount = 0;
    float fps = 0.0f;
    int render_mode = 0; // 0 = ANSI pattern, 1 = 3D cube
    
    while (1)
    {
        DWORD currentTime = GetTickCount();
        frameCount++;
        
        // Calculate FPS every second
        if (currentTime - lastTime >= 250) {
            fps = frameCount * 250.0f / (currentTime - lastTime);
            frameCount = 0;
            lastTime = currentTime;
        }
        
        
        // Check for exit (Escape key)
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        output_buffer();
        // Display FPS and mode in top-right corner
        
        printf("\x1B[0;0H%.2f FPS", fps);
        
    }
    

    cleanup_dlls(dlls, dll_count);
    
    return 0;
}
