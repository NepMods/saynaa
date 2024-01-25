#ifndef lang_value_h
#define lang_value_h

#include "common.h"
/****************************************************************************/
/* Object                                                                   */
/****************************************************************************/

typedef enum {
  OBJ_STRING,
} ObjType;

typedef struct Obj {
  ObjType type;
};

typedef struct ObjString {
  Obj obj;
  int length;
  char* chars;
};


#define OBJ_TYPE(value)        (AS_OBJ(value)->type)

#define IS_STRING(value)       isObjType(value, OBJ_STRING)

#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)

#define isObjType(value, type) (IS_OBJ(value) && AS_OBJ(value)->type == type)


/****************************************************************************/
/* Value                                                                    */
/****************************************************************************/

typedef enum {
  VAL_BOOL,
  VAL_NIL, 
  VAL_NUMBER,
  VAL_OBJ
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
    Obj* obj;
  } as; 
} Value;

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
#define IS_OBJ(value)     ((value).type == VAL_OBJ)

#define AS_OBJ(value)     ((value).as.obj)
#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)

#define BOOL_VAL(value)   ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL           ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)   ((Value){VAL_OBJ, {.obj = (Obj*)object}})

class ValueArray {
private:
    
public:
    ValueArray();
    void write(Value value);
    void printValue(Value value);
    bool valuesEqual(Value a, Value b);
    void free();

    int capacity;
    int count;
    Value* values;
};

#endif