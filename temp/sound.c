#define BUILD_DLL
#include "sound.h"

static SoundSystem g_soundSystem = {0};

// Structure to pass parameters to the sound thread
typedef struct {
    double frequency;
    int duration_ms;
} SoundThreadParams;

// Thread function for playing sound_play_tone asynchronously
DWORD WINAPI async_tone_thread(LPVOID lpParam) {
    SoundThreadParams* params = (SoundThreadParams*)lpParam;
    sound_play_tone(params->frequency, params->duration_ms);
    free(params); // Clean up allocated memory
    return 0;
}

// Initialize the sound system
DLL_EXPORT int sound_init() {
    WAVEFORMATEX waveFormat;
    MMRESULT result;
    
    if (g_soundSystem.isInitialized) {
        return 1; // Already initialized
    }
    
    // Set up wave format
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = CHANNELS;
    waveFormat.nSamplesPerSec = SAMPLE_RATE;
    waveFormat.wBitsPerSample = BITS_PER_SAMPLE;
    waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;
    
    // Open wave output device
    result = waveOutOpen(&g_soundSystem.waveOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL);
    if (result != MMSYSERR_NOERROR) {
        return 0;
    }
    
    // Allocate buffers
    for (int i = 0; i < NUM_BUFFERS; i++) {
        g_soundSystem.buffers[i] = (short*)malloc(BUFFER_SIZE * sizeof(short));
        if (!g_soundSystem.buffers[i]) {
            sound_cleanup();
            return 0;
        }
        
        // Prepare wave header
        ZeroMemory(&g_soundSystem.waveHeaders[i], sizeof(WAVEHDR));
        g_soundSystem.waveHeaders[i].lpData = (LPSTR)g_soundSystem.buffers[i];
        g_soundSystem.waveHeaders[i].dwBufferLength = BUFFER_SIZE * sizeof(short);
        
        result = waveOutPrepareHeader(g_soundSystem.waveOut, &g_soundSystem.waveHeaders[i], sizeof(WAVEHDR));
        if (result != MMSYSERR_NOERROR) {
            sound_cleanup();
            return 0;
        }
    }
    
    g_soundSystem.currentBuffer = 0;
    g_soundSystem.phase = 0.0;
    g_soundSystem.frequency = 440.0;
    g_soundSystem.amplitude = 0.3;
    g_soundSystem.isInitialized = 1;
    
    return 1;
}

// Clean up sound system
DLL_EXPORT void sound_cleanup() {
    if (!g_soundSystem.isInitialized) return;
    
    // Stop playback
    waveOutReset(g_soundSystem.waveOut);
    
    // Unprepare and free buffers
    for (int i = 0; i < NUM_BUFFERS; i++) {
        if (g_soundSystem.waveHeaders[i].dwFlags & WHDR_PREPARED) {
            waveOutUnprepareHeader(g_soundSystem.waveOut, &g_soundSystem.waveHeaders[i], sizeof(WAVEHDR));
        }
        if (g_soundSystem.buffers[i]) {
            free(g_soundSystem.buffers[i]);
            g_soundSystem.buffers[i] = NULL;
        }
    }
    
    // Close wave output device
    waveOutClose(g_soundSystem.waveOut);
    g_soundSystem.isInitialized = 0;
}

// Generate sine wave samples
static void generate_sine_wave(short* buffer, int samples, double frequency, double* phase) {
    double phaseIncrement = 2.0 * 3.14159265359 * frequency / SAMPLE_RATE;
    
    for (int i = 0; i < samples; i++) {
        double sample = sin(*phase) * g_soundSystem.amplitude * 32767.0;
        buffer[i] = (short)sample;
        *phase += phaseIncrement;
        
        // Keep phase in range to prevent overflow
        if (*phase >= 2.0 * 3.14159265359) {
            *phase -= 2.0 * 3.14159265359;
        }
    }
}

// Set frequency for continuous tone
DLL_EXPORT void sound_set_frequency(double freq) {
    if (freq < 20.0) freq = 20.0;
    if (freq > 20000.0) freq = 20000.0;
    g_soundSystem.frequency = freq;
}

// Set amplitude (0.0 to 1.0)
DLL_EXPORT void sound_set_amplitude(double amp) {
    if (amp < 0.0) amp = 0.0;
    if (amp > 1.0) amp = 1.0;
    g_soundSystem.amplitude = amp;
}

// Play a tone with specified frequency and duration
DLL_EXPORT void sound_play_tone(double frequency, int duration_ms) {
    if (!g_soundSystem.isInitialized) {
        if (!sound_init()) return;
    }
    
    sound_set_frequency(frequency);
    
    // Calculate number of samples needed
    int totalSamples = (SAMPLE_RATE * duration_ms) / 1000;
    int samplesGenerated = 0;
    
    while (samplesGenerated < totalSamples) {
        // Check if current buffer is done playing
        if (!(g_soundSystem.waveHeaders[g_soundSystem.currentBuffer].dwFlags & WHDR_INQUEUE)) {
            int samplesToGenerate = min(BUFFER_SIZE, totalSamples - samplesGenerated);
            
            // Generate samples
            generate_sine_wave(g_soundSystem.buffers[g_soundSystem.currentBuffer], 
                             samplesToGenerate, frequency, &g_soundSystem.phase);
            
            // Update buffer length for partial fills
            g_soundSystem.waveHeaders[g_soundSystem.currentBuffer].dwBufferLength = 
                samplesToGenerate * sizeof(short);
            
            // Queue the buffer
            waveOutWrite(g_soundSystem.waveOut, 
                        &g_soundSystem.waveHeaders[g_soundSystem.currentBuffer], 
                        sizeof(WAVEHDR));
            
            samplesGenerated += samplesToGenerate;
            g_soundSystem.currentBuffer = (g_soundSystem.currentBuffer + 1) % NUM_BUFFERS;
        }
        
        Sleep(1); // Small delay to prevent busy waiting
    }
}

// Stop all sound playback
DLL_EXPORT void sound_stop() {
    if (g_soundSystem.isInitialized) {
        waveOutReset(g_soundSystem.waveOut);
    }
}

// Check if sound is currently playing
DLL_EXPORT int sound_is_playing() {
    if (!g_soundSystem.isInitialized) return 0;
    
    for (int i = 0; i < NUM_BUFFERS; i++) {
        if (g_soundSystem.waveHeaders[i].dwFlags & WHDR_INQUEUE) {
            return 1;
        }
    }
    return 0;
}

// Play a tone asynchronously in a separate thread
DLL_EXPORT void sound_play_tone_async(double frequency, int duration_ms) {
    // Check if sound is already playing to prevent conflicts
    if (sound_is_playing()) {
        return; // Skip if already playing
    }
    
    // Allocate memory for sound parameters
    SoundThreadParams* params = (SoundThreadParams*)malloc(sizeof(SoundThreadParams));
    if (params != NULL) {
        params->frequency = frequency;
        params->duration_ms = duration_ms;
        
        // Create a thread to play the sound asynchronously
        HANDLE soundThread = CreateThread(
            NULL,                   // Default security attributes
            0,                      // Default stack size
            async_tone_thread,      // Thread function
            params,                 // Parameters to pass to thread
            0,                      // Default creation flags
            NULL                    // Don't need thread ID
        );
        
        // Close the thread handle immediately (thread continues to run)
        if (soundThread != NULL) {
            CloseHandle(soundThread);
        } else {
            // If thread creation failed, clean up memory
            free(params);
        }
    }
}