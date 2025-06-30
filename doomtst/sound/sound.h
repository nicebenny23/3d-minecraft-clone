#include <include/al.h>
#include <include/alc.h>
#include <iostream>
#include "../util/sharedptr.h"
#include "soundfile.h"
#include "soundsource.h"
#include <GLFW/glfw3.h>
#include "soundbuf.h"
#include "soundlistener.h"
#include "../util/sharedptr.h"
#pragma once 
using namespace Cptr;
constexpr auto soundamt = 400;
namespace soundname {
    struct sound;
    class alwrapper {
    public:
        listener listen;
        array<cptr<sound>,true> soundlist;
        alwrapper() {
            soundlist = array<cptr<sound>,true>(400);
            for (int i = 0; i < 400; i++)
            {
                soundlist[i] = cptr<sound>(nullptr);
            }
            device = alcOpenDevice(nullptr);
            if (!device) {
                std::cerr << "Failed to open OpenAL device." << std::endl;
                return;
            }
            
            context = alcCreateContext(device, nullptr);
            if (!context) {
                std::cerr << "Failed to create OpenAL context." << std::endl;
                alcCloseDevice(device);
                return;
            }

            alcMakeContextCurrent(context);
         
        }

        void destroy() {
            alcMakeContextCurrent(nullptr);
            if (context) {
                alcDestroyContext(context);
            }
            if (device) {
                alcCloseDevice(device);
            }
        }
       void updatesounds();

        


    private:
        ALCdevice* device = nullptr;
        ALCcontext* context = nullptr;
    };

    
    extern alwrapper soundobj;
    struct  sound
    {
        int id;
        void destroy() {
    
            buf.release();
            source.release();
            soundobj.soundlist[id] = cptr<sound>(nullptr);
        }
        sound(wavfile& file,int newid) {
            buf = soundbuf(file);
            bind();
            id = newid;
            source.play();
        }
        void bind() {
            alSourcei(source.id, AL_BUFFER, buf.id);
        }
        soundproperties properties;
         soundbuf buf;
        soundsource source;

    };
    void soundinit();
    cptr<sound> createsound(const char* name);
    cptr<sound> createsound(wavfile& file);
}
 // !sound_HPP
