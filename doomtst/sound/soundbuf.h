#include <include/al.h>
#include "soundfile.h"
#include "../util/dynamicarray.h"

#ifndef soundbuffer_HPP
#define soundbuffer_HPP

struct soundbuf {
    ALuint id;  
    ALenum format;
    int frequency;
    Cont::array< char> data;

    soundbuf() : id(0), format(AL_FORMAT_MONO16), frequency(44100) {}
    void generatebuffer() {

        alGenBuffers(1, &id);
        if (alGetError() != AL_NO_ERROR) {
            Assert("albuferror");
        }


    }
    soundbuf(wavfile& file) {
    
        format = file.header.format;
        frequency = file.header.sampleRate;
        data =array<char>( file.data);
        std::cout << data.at(10000000);
        generatebuffer();
        fillbuffer();
    }
   
    void fillbuffer() {
       
        alBufferData(id, format, data.list, data.length, frequency);
        if (alGetError() != AL_NO_ERROR) {
            Assert("alBufferData error");
        }
    }
    // Release the buffer resources
    void release() {
        if (id != 0) {
            data.destroy();
            alDeleteBuffers(1, &id);
            id= 0;
        }
    }
  
    
}; 

#endif // !soundbuffer_HPP
