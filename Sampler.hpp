
#ifndef GLOW_SAMPLER_HPP
#define GLOW_SAMPLER_HPP

#include "Common.hpp"

class Sampler {
public:
    
    Sampler();
    ~Sampler();
    
    Sampler(Sampler const &) = delete;
    Sampler & operator=(Sampler const &) = delete;
    
    bool load(std::string const & path);
    bool loadWav(std::string const & path);
    bool loadOgg(std::string const & path);
    // TODO add an option to force mono
    
    ALenum getFormat() const;
    uint32_t getChannels() const;
    uint32_t getBits() const;
    uint32_t getBytesPerSample() const;
    uint32_t getFrequency() const;
    uint32_t getSize() const;
    uint32_t getBytes() const;
    float getDuration() const;
    uint32_t getOffset() const;
    
    void rewind();
    uint32_t read(void * buffer, uint32_t samples);
    
private:

    struct Reader {
        ALenum format;
        uint32_t channels;
        uint32_t bits;
        uint32_t frequency;
        uint32_t size;
        uint32_t offset;
        virtual ~Reader() {}
        virtual void rewind() = 0;
        virtual uint32_t read(void * buffer, uint32_t samples) = 0;
    };
    Reader * reader;
    
    Reader * createWav(std::string const & path);
    Reader * createOgg(std::string const & path);
    
};

#endif
