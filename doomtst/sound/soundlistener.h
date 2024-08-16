#ifndef listener_HPP
#define listener_HPP

#include <include/al.h>

struct listener {
    ALfloat* position;    // Listener position in 3D space

    listener();

    // Set the listener's position
    void setposition(ALfloat x, ALfloat y, ALfloat z);

    // Update OpenAL listener properties
    void update();
};

#endif // !listener_HPP