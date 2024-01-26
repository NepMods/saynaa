#include "public.h"

Value::Value() {

}

void Value::write(uint8_t opcode, int line) {
    this->lines.push_back(line);
    this->opcode.push_back(opcode);
}

int Value::addValue(int value) {
    this->value.push_back(value);
    return this->value.size() -1;
}

void Value::free() {
    lines.clear();
    opcode.clear();
    value.clear();
}