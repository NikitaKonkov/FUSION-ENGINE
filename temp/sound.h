#ifndef SOUND_H
#define SOUND_H

#include <windows.h>
#include <mmsystem.h>
#include <math.h>

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

// Sound system constants
#define SAMPLE_RATE 44100
#define CHANNELS 1
#define BITS_PER_SAMPLE 16
#define BUFFER_SIZE 4096
#define NUM_BUFFERS 3

// Sound system structure
typedef struct {
    HWAVEOUT waveOut;
    WAVEHDR waveHeaders[NUM_BUFFERS];
    short* buffers[NUM_BUFFERS];
    int currentBuffer;
    int isInitialized;
    double phase;
    double frequency;
    double amplitude;
} SoundSystem;

// Function exports
DLL_EXPORT int sound_init();
DLL_EXPORT void sound_cleanup();
DLL_EXPORT void sound_set_frequency(double freq);
DLL_EXPORT void sound_set_amplitude(double amp);
DLL_EXPORT void sound_play_tone(double frequency, int duration_ms);
DLL_EXPORT void sound_play_tone_async(double frequency, int duration_ms);
DLL_EXPORT void sound_stop();
DLL_EXPORT int sound_is_playing();

#endif // SOUND_H