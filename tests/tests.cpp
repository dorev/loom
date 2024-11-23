#include <cmath>
#include <vector>
#include <queue>
#include <memory>
#include <algorithm>

#include "gtest/gtest.h"
#include "external/rtaudio/RtAudio.h"

#include "loom/src/engine.h"
#include "loom/src/ieffect_spatializer.h"
#include "loom/src/idatanode_oscillators.h"

using namespace Loom;

int audioCallback(
    void* outputBuffer,
    void* /*inputBuffer*/,
    unsigned int frameCount,
    double /*streamTime*/,
    RtAudioStreamStatus /*status*/,
    void* userData
)
{
    Engine* engine = static_cast<Engine*>(userData);
    Buffer* audioBuffer = nullptr;

    Error error = engine->outputBus.Read(audioBuffer, frameCount);
    if (error)
        return 1;

    for (unsigned int i = 0; i < frameCount * 2; ++i)
        static_cast<float*>(outputBuffer)[i] = audioBuffer->data[i];

    engine->virtualizer.ReturnBuffer(audioBuffer);

    return 0;
}

TEST(Loom, SmokeTest)
{
    Engine engine(10, 512, Format(2, 44100));

    Emitter* triangleEmitter = nullptr;
    engine.CreateEmitter(triangleEmitter, Transform(-10));
    triangleEmitter->AddEffect(new Spatializer());

    Emitter* sineEmitter = nullptr;
    engine.CreateEmitter(sineEmitter, Transform(10));
    sineEmitter->AddEffect(new Spatializer());

    Sound* triangleSound = nullptr;
    TriangleSource triangleSource(440, 1.0);
    engine.CreateSound(triangleSound, &triangleSource, triangleEmitter);
    triangleSound->play = true;

    RtAudio audio;
    if (audio.getDeviceCount() < 1)
        FAIL();

    RtAudio::StreamParameters parameters;
    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;

    unsigned int bufferFrames = 512;

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, 44100, &bufferFrames, &audioCallback, &engine))
        FAIL();

    if (audio.startStream())
        FAIL();
    
    Sleep(1000);

    Sound* sineSound = nullptr;
    SineSource sineSource(660, 1.0);
    engine.CreateSound(sineSound, &sineSource, sineEmitter);
    sineSound->emitter = sineEmitter;
    sineSound->play = true;

    Sleep(1000);

    triangleSound->play = false;

    Sleep(1000);

    if (audio.stopStream())
        FAIL();

    if (audio.isStreamOpen())
        audio.closeStream();

}
