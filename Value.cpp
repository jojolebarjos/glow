
#include "Value.hpp"
#include "Function.hpp"

#include <cstdio>

Value::Value() : type(NONE) {}

Value::Value(Value const & value) : type(NONE) {
    *this = value;
}

Value & Value::operator=(Value const & value) {
    if (&value != this)
        switch (value.type) {
            case NONE:
                setNone();
                break;
            case FLOAT:
                setFloat(value.data.f);
                break;
            case STRING:
                setString(*value.data.s);
                break;
            case VECTOR:
                // TODO set vector
                break;
            case MAP:
                // TODO set map
                break;
            case FUNCTION:
                setFunction(*value.data.fn);
                break;
        }
    return *this;
}

Value::~Value() {
    setNone();
}

bool Value::isNone() const {
    return type == NONE;
}

void Value::setNone() {
    switch (type) {
        case NONE:
        case FLOAT:
            break;
        case STRING:
            delete data.s;
            break;
        case VECTOR:
            delete data.v;
            break;
        case MAP:
            delete data.m;
            break;
        case FUNCTION:
            delete data.fn;
            break;
    }
    type = NONE;
}

bool Value::getBool() const {
    return getFloat() != 0.0f;
}

void Value::setBool(bool value) {
    setFloat(value ? 1.0f : 0.0f);
}

bool Value::isFloat() const {
    return type == FLOAT;
}

float Value::getFloat() const {
    switch (type) {
        case NONE:
            return 0.0f;
        case FLOAT:
            return data.f;
        case STRING:
            return 0.0f; // TODO cast to float or give length?
        case VECTOR:
            return 0.0f; // TODO length? first element?
        case MAP:
            return 0.0f; // TODO length?
        case FUNCTION:
            return 0.0f; // TODO ?
    }
    return 0.0f;
}

void Value::setFloat(float value) {
    setNone();
    type = FLOAT;
    data.f = value;
}

bool Value::isString() const {
    return type == STRING;
}

std::string Value::getString() const {
    switch (type) {
        case NONE:
            return "<none>";
        case FLOAT:
            char buffer[16];
            sprintf(buffer, "%f", data.f);
            return buffer;
        case STRING:
            return *data.s;
        case VECTOR:
            return "<vector>"; // TODO return concat? first element?
        case MAP:
            return "<map>"; // TODO concat?
        case FUNCTION:
            return "<function>"; //function? print code?
    }
    return "";
}

void Value::setString(std::string const & value) {
    if (type == STRING)
        *data.s = value;
    else {
        setNone();
        type = STRING;
        data.s = new std::string(value);
    }
}

bool Value::isVector() const {
    return type == VECTOR;
}

bool Value::isMap() const {
    return type == MAP;
}

uint32_t Value::getSize() const {
    switch (type) {
        case NONE:
            return 0;
        case FLOAT:
        case STRING:
        case FUNCTION:
            return 1;
        case VECTOR:
            return data.v->size();
        case MAP:
            return data.m->size();
    }
    return 0;
}

Value Value::get(uint32_t index) const {
    switch (type) {
        case NONE:
            return Value();
        case FLOAT:
        case STRING:
        case FUNCTION:
            return index == 0 ? *this : Value();
        case VECTOR:
            return index < data.v->size() ? (*data.v)[index] : Value();
        case MAP:
            // TODO return map element by index
            return Value();
    }
    return Value();
}

Value Value::get(std::string const & key) const {
    if (type == MAP) {
        auto it = data.m->find(key);
        if (it != data.m->end())
            return it->second;
    }
    return Value();
}
    
bool Value::isFunction() const {
    return type == FUNCTION;
}

Function Value::getFunction() const {
    if (type == FUNCTION)
        return *data.fn;
    else {
        Function function;
        function.setValue(*this);
        return function;
    }
}

void Value::setFunction(Function const & value) {
    setNone();
    type = FUNCTION;
    data.fn = new Function(value);
}

Value Value::evaluate(Value const & value) {
    switch (type) {
        case NONE:
        case FLOAT:
        case STRING:
            return *this;
        case VECTOR:
            // TODO access element
            return Value();
        case MAP:
            // TODO access element
            return Value();
        case FUNCTION:
            return data.fn->evaluate(value);
    }
    return Value();
}
