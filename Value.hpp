
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
    bool isMap() const;
    uint32_t getSize() const;
    Value get(uint32_t index) const;
    Value get(std::string const & key) const;
    // TODO set for vector and map
    
    bool isFunction() const;
    Function getFunction() const;
    void setFunction(Function const & value);
    Value evaluate(Value const & value);
    
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
