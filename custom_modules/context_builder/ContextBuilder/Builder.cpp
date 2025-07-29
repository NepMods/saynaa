#include "Builder.h"

void* ContextBuilder::buildContext(Bytecode ctx) {
    bytecode = &ctx;

    run();
    return static_cast<void*>(bytecode);
}

void ContextBuilder::compileTimeError(const std::string &message) {
    throw std::runtime_error("Compile-time error: " + message); 
}

void ContextBuilder::buildNext(uint32_t opc) {

    switch(opc) {
        case OP_CONSTANT: {
            uint32_t constantIndex = next_op();
            auto constantValue = bytecode->value[constantIndex];
            constantStack.push(constantValue);
        }
        break;

        case OP_DEFINE_LOCAL:
        {
            auto name = bytecode->name[next_op()];
            auto value = constantStack.pop();

            stackVariableType variable;
            variable.name = name;
            if(context.stack_level == 0) {
                variable.scope = GLOBAL;
            } else {
                variable.scope = LOCAL;
            }
            variable.value = value;
        }
        break;

    }
 
}

void ContextBuilder::run() {
    
    opcode = bytecode->opcode;

    while (next_op() != OP_NONE) {
        uint32_t op = get_op();
        if (op == OP_NONE) {
            compileTimeError("Unexpected end of bytecode");
        }
        buildNext(op);
    }

    
}


uint32_t ContextBuilder::get_op() {
    if (index < opcode.size()) {
      return opcode[index];
    }
    return OP_NONE;
  }
  
  uint32_t ContextBuilder::next_op() {
    index++;
    return get_op();
  }
  
  uint32_t ContextBuilder::peek_op(int offset) {
    if (index + offset < opcode.size()) {
      return opcode[index + offset];
    }
    return OP_NONE;
  }
  
  void ContextBuilder::ignore_to(uint32_t opfcode) {
    while (peek_op() != opfcode) {
      next_op();
      if (get_op() == OP_NONE) {
        compileTimeError("Unexpected end");
      }
    }
  }