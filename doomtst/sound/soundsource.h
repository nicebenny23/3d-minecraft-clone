
#include <include/al.h>
#ifndef soundsource_HPP
#define soundsource_HPP

struct soundproperties {
    bool organic;
    float pitch;                 // Sound pitch (default: 1.0)
    float volume;
    bool loop;
    bool permanant;
    soundproperties() {

        organic = false;
        pitch = 1;
        volume = 1;
        loop = false;
    }
    // Add other properties as needed
};

struct soundsource {
    ALuint id;  // OpenAL source ID

    soundsource();


    void setpitchproperty(float pitch);
    void setloopproperty(bool shouldloop);
    void setvolumeproperty(float volume);
    void setattenuationproperty(float attenuation);
    void play();
    void stop();
    void release();
    void setproperties(soundproperties properties);
};

#endif // !soundsource_HPP