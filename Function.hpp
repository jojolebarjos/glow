
#ifndef GLOW_FUNCTION_HPP
#define GLOW_FUNCTION_HPP

#include "Common.hpp"
#include "Value.hpp"

class Function {
public:
    
    typedef Value (*Pointer)(Value);
    
    Function();
    Function(Function const & function);
    Function & operator=(Function const & function);
    ~Function();
    
    // TODO parent
    
    // TODO variables
    
    // TODO get/set pointer
    
    void setValue(Value const & value);
    
    bool setCode(std::string const & code);
    bool setCodeFile(std::string const & path);
    // TODO get code?
    
    Value evaluate(Value const & value = Value());
    
private:

    struct Impl;
    Impl * impl;
    
};

#endif
