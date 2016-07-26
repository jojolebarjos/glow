
#ifndef GLOW_VALUE_HPP
#define GLOW_VALUE_HPP

#include "Common.hpp"

class Function;

class Value {
public:
    
    Value();
    Value(Value const & value);
    Value & operator=(Value const & value);
    ~Value();
    
    bool isNone() const;
    void setNone();
    
    // Note: a boolean is represented as a float
    bool getBool() const;
    void setBool(bool value);
    
    bool isFloat() const;
    float getFloat() const;
    void setFloat(float value);
    
    bool isString() const;
    std::string getString() const;
    void setString(std::string const & value);
    
    bool isVector() const;
    // TODO array access (get, set, size, at)
    
private:
    
    // TODO glm objects?

    enum Type {
        NONE,
        FLOAT,
        STRING,
        VECTOR,
        MAP,
        FUNCTION
    };
    Type type;
    
    union {
        float f;
        std::string * s;
        std::vector<Value> * v;
        std::map<std::string, Value> * m;
        Function * fn;
    } data;
    
};

#endif
