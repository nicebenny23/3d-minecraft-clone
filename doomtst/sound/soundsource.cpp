#include "soundsource.h"
#include <include/al.h>
#include "../debugger/debug.h"

soundsource::soundsource() {
    id = 0;
    alGenSources(1, &id);
    if (alGetError() != AL_NO_ERROR) {
        Assert("Failed to generate sound source");
    }
}


void soundsource::setpitchproperty(float pitch) {
    alSourcef(id, AL_PITCH,pitch);
    if (alGetError() != AL_NO_ERROR) {
        Assert("Failed to set pitch");
    }
}

void soundsource::setloopproperty(bool shouldloop) {
   alSourcei(id, AL_LOOPING, shouldloop );
    if (alGetError() != AL_NO_ERROR) {
        Assert("Failed to set looping");
    }
}

void soundsource::setvolumeproperty(float volume) {
    alSourcef(id, AL_GAIN, volume);
    if (alGetError() != AL_NO_ERROR) {
        Assert("Failed to set volume");
    }
}

void soundsource::setattenuationproperty(float attenuation)
{
    alSourcef(id, AL_ROLLOFF_FACTOR, attenuation);
    if (alGetError() != AL_NO_ERROR) {
        Assert("Failed to set rolloff factor");
    }
}

void soundsource::play() {
    alSourcePlay(id);
    if (alGetError() != AL_NO_ERROR) {
        Assert("Failed to play sound source");
    }
}

void soundsource::stop() {
    alSourceStop(id);
    if (alGetError() != AL_NO_ERROR) {
        Assert("Failed to stop sound source");
    }
}

void soundsource::release() {
    if (id != 0) {
        alDeleteSources(1, &id);
        id = 0;
    }
}

void soundsource::setproperties(soundproperties properties)
{
 
  
    alSource3f(id, AL_POSITION, 0.0f, 0.0f, 0.0f);
    setattenuationproperty(properties.organic);
    setloopproperty(properties.loop);
    setpitchproperty(properties.pitch);
    setvolumeproperty(properties.volume);
}
