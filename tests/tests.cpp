#include <chrono>
#include <thread>

#include "gtest/gtest.h"
#include "rtaudio/RtAudio.h"

#include "engine.h"
#include "ieffect_spatializer.h"
#include "idatanode_oscillators.h"

using namespace Loom;

void SleepMs(unsigned int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

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
    ASSERT_EQ(engine.CreateEmitter(triangleEmitter, Transform(-10)), OK);
    triangleEmitter->AddEffect(new Spatializer());

    Emitter* sineEmitter = nullptr;
    ASSERT_EQ(engine.CreateEmitter(sineEmitter, Transform(10)), OK);
    sineEmitter->AddEffect(new Spatializer());

    Sound* triangleSound = nullptr;
    TriangleSource triangleSource(440, 1.0);
    ASSERT_EQ(engine.CreateSound(triangleSound, &triangleSource, triangleEmitter), OK);
    triangleSound->Play();

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

    SleepMs(1000);

    Sound* sineSound = nullptr;
    SineSource sineSource(660, 1.0);
    ASSERT_EQ(engine.CreateSound(sineSound, &sineSource, sineEmitter), OK);

    sineSound->Play();

    SleepMs(1000);

    triangleSound->Stop();

    SleepMs(1000);

    EXPECT_TRUE(sineSound->IsPlaying());
    EXPECT_FALSE(triangleSound->IsPlaying());

    if (audio.stopStream())
        FAIL();

    if (audio.isStreamOpen())
        audio.closeStream();
}
