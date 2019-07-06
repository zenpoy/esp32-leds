#include <discrete_float_func/i_discrete_float_func.h>

IDiscreteFloatFunc *DiscreteFloatFuncFactory(const JsonObject &animation_params) {

    const char *func_type = animation_params["type"];
    if(func_type == NULL) {
        return NULL;
    }

    IDiscreteFloatFunc *func = NULL;
    if(strcmp(func_type, "const") == 0) {
        // func = new ConstFloatFunc();
    } else if(strcmp(func_type, "linear") == 0) {
        // func = new LinearFloatFunc();
    } else if(strcmp(func_type, "sin") == 0) {
        // func = new SinFloatFunc();
    } else if(strcmp(func_type, "segment") == 0) {
        // func = new SegmentFloatFunc();
    }

    if(func != NULL) {
        const JsonObject &params = animation_params["params"];
        func->InitFromJson(params);
    }

    return func;
}


