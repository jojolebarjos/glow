
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
                // TODO set function
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
            return "none";
        case FLOAT:
            char buffer[16];
            sprintf(buffer, "%f", data.f);
            return buffer;
        case STRING:
            return *data.s;
        case VECTOR:
            return ""; // TODO return concat? first element?
        case MAP:
            return ""; // TODO concat?
        case FUNCTION:
            return ""; //function? print code?
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
