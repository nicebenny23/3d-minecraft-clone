#include "../util/fileloader.h"
#include <include/al.h>
#include <include/alc.h>
#include "../util/dynamicarray.h"

using namespace dynamicarray;

#ifndef SOUNDFILE_HPP
#define SOUNDFILE_HPP

struct wavheader {
    int sampleRate;
    int dataSize;
    ALenum format;  // Use ALenum to directly represent OpenAL format
    float duration;
    wavheader(const char* filename) {
        safefile file(filename, fileread);
        char* header;  // Buffer to hold the WAV header
      header=   file.read<char>(44);

        short bitsPerSample = *reinterpret_cast<short*>(&header[34]);
        short channels = *reinterpret_cast<short*>(&header[22]);

        // Determine the format based on channels and bits per sample
        if (channels == 1) {
            format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
        }
        else if (channels == 2) {
            format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
        }
        else {
            format = AL_FORMAT_MONO16;  // Default format in case of unsupported channels
            std::cerr << "Unsupported number of channels: " << channels << std::endl;
        }

        // Extract sample rate and data size
        sampleRate = *reinterpret_cast<int*>(&header[24]);
        dataSize = *reinterpret_cast<int*>(&header[40]);
            duration = static_cast<float>(dataSize) / (sampleRate * channels * (bitsPerSample / 8));
    }
};

struct wavfile {
    wavheader header;
    array< char> data;

    wavfile(const char* name) : header(name) {
        safefile file(name, fileread);
        file.go(44);  // Skip header to start of data
        data = array<char>(file.read<char>(header.dataSize), header.dataSize);
    }
};

#endif // !SOUNDFILE_HPP