#include "soundlistener.h"
#include "../util/debug.h" // For Assert function

listener::listener() {
    // Initialize default position
    position = new float[3];
    position[0] = 0.0f;
    position[1] = 0.0f;
    position[2] = 0.0f;

}

void listener::setposition(ALfloat x, ALfloat y, ALfloat z) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
    update(); // Apply changes immediately
}

void listener::update() {
    alListener3f(AL_POSITION, position[0], position[1], position[2]);

    if (alGetError() != AL_NO_ERROR) {
        Assert("Failed to update listener position");
    }
}