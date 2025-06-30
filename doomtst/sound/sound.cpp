#pragma once
#include "sound.h"
using namespace soundname;
soundname::alwrapper soundname::soundobj;
void soundname::alwrapper::updatesounds()
{
    for (int i = 0; i < soundlist.length; i++)
    {
        if (soundlist[i].ptr() != nullptr) {
         
            soundlist[i]->source.setproperties(soundlist[i]->properties);


            ALint state=0;
            alGetSourcei(soundlist[i]->source.id, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
            {
                if (!soundlist[i]->properties.permanant)
                {
                    soundlist[i]->destroy();
              
                }
                
            }
        }
    }    
    
         
}

void soundname::soundinit()
{
    soundobj = alwrapper();
}

cptr<sound> soundname::createsound(const char* name)
{

    for (int i = 0; i < soundamt; i++)
    {
        if (soundobj.soundlist[i] == nullptr)
        {
            wavfile* newfile = new wavfile(name);
            soundobj.soundlist[i] = cptr<soundname::sound>(new soundname::sound(*newfile,i));
            newfile->data.destroy();
            delete newfile;
           
            return soundobj.soundlist[i]; 
        }
    }

}

cptr<sound> soundname::createsound(wavfile& file)
{
    for (int i = 0; i < soundamt; i++)
    {
        if (soundobj.soundlist[i] == nullptr)
        {
            soundobj.soundlist[i] = cptr<sound>(new sound(file,i));
        
           
            return soundobj.soundlist[i];
        }
    }

}
