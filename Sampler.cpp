
#include "Sampler.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>

Sampler::Sampler() : reader(nullptr) {}

Sampler::~Sampler() {
    delete reader;
}

bool Sampler::load(std::string const & path) {
    // TODO improve this based on extension
    return loadWav(path) || loadOgg(path);
}

bool Sampler::loadWav(std::string const & path) {
    delete reader;
    reader = createWav(path);
    return reader;
}

bool Sampler::loadOgg(std::string const & path) {
    delete reader;
    reader = createOgg(path);
    return reader;
}

ALenum Sampler::getFormat() const {
    return reader ? reader->format : AL_NONE;
}

uint32_t Sampler::getChannels() const {
    return reader ? reader->channels : 0;
}

uint32_t Sampler::getBits() const {
    return reader ? reader->bits : 0;
}

uint32_t Sampler::getBytesPerSample() const {
    return reader ? reader->channels * reader->bits / 8 : 0;
}

uint32_t Sampler::getFrequency() const {
    return reader ? reader->frequency : 0;
}

uint32_t Sampler::getSize() const {
    return reader ? reader->size : 0;
}

uint32_t Sampler::getBytes() const {
    return reader ? reader->size * reader->channels * reader->bits / 8 : 0;
}

float Sampler::getDuration() const {
    return reader ? (float)reader->size / (float)(reader->frequency) : 0.0f;
}

uint32_t Sampler::getOffset() const {
    return reader ? reader->offset : 0;
}

void Sampler::rewind() {
    if (reader)
        reader->rewind();
}

uint32_t Sampler::read(void * buffer, uint32_t samples) {
    return reader ? reader->read(buffer, samples) : 0;
}

Sampler::Reader * Sampler::createWav(std::string const & path) {
    struct WavReader : Reader {
        FILE * file;
        uint32_t start;
        
        WavReader(std::string const & path) {
            
            // Open file
            file = fopen(path.c_str(), "rb");
            if (!file)
                return;
            
            // Read header
            char header[36];
            if (fread(header, sizeof(header), 1, file) != 1) {
                fclose(file);
                file = nullptr;
                return;
            }
            
            // Check PCM header
            if (header[0] != 'R' || header[1] != 'I' || header[2] != 'F' || header[3] != 'F' ||
                    header[8] != 'W' || header[9] != 'A' || header[10] != 'V' || header[11] != 'E' ||
                    header[12] != 'f' || header[13] != 'm' || header[14] != 't' || header[15] != ' ' ||
                    *(uint32_t *)(header + 16) != 16 || *(uint16_t *)(header + 20) != 1) {
                std::cout << "not a valid RIFF WAVE PCM" << std::endl;
                fclose(file);
                file = nullptr;
                return;
            }
            
            // Check format
            bits = *(uint16_t *)(header + 34);
            channels = *(uint16_t *)(header + 22);
            if ((bits != 8 && bits != 16) || (channels != 1 && channels != 2)) {
                std::cout << "invalid WAV format, only mono/stereo 8/16 bits is supported" << std::endl;
                fclose(file);
                file = nullptr;
                return;
            }
            if (channels == 2)
                format = bits == 16 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
            else
                format = bits == 16 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
            frequency = *(uint32_t *)(header + 24);
            
            // Search data chunk
            start = 0;
            char chunk[8];
            while (start == 0) {
                if (fread(chunk, sizeof(chunk), 1, file) != 1)
                    break;
                size = *(uint32_t*)(chunk + 4);
                if (chunk[0] == 'd' && chunk[1] == 'a' && chunk[2] == 't' && chunk[3] == 'a') {
                    start = ftell(file);
                    break;
                } else
                    fseek(file, size, SEEK_CUR);
            }
            if (start == 0) {
                std::cout << "wav data chunk not found" << std::endl;
                fclose(file);
                file = nullptr;
                return;
            }
            offset = 0;
            size /= channels * bits / 8;
        }
        
        ~WavReader() {
            if (file)
                fclose(file);
        }
        
        void rewind() {
            fseek(file, start, SEEK_SET);
            offset = 0;
        }
        
        uint32_t read(void * buffer, uint32_t samples) {
            uint32_t sample_size = channels * bits / 8;
            uint32_t max_samples = (size - offset) / sample_size;
            uint32_t samples_read = fread(buffer, glm::min(samples, max_samples), sample_size, file);
            offset += samples_read;
            return samples_read;
        }
        
    };
    WavReader * reader = new WavReader(path);
    if (reader->file)
        return reader;
    delete reader;
    return nullptr;
}

Sampler::Reader * Sampler::createOgg(std::string const & path) {
    // TODO use ogg vorbis
    return nullptr;
}
