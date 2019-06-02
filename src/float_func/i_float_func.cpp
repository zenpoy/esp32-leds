#include <float_func/i_float_func.h>

#include <float_func/const_float_func.h>
#include <float_func/linear_float_func.h>

IFloatFunc *FloatAnimationFactory(const JsonObject &animation_params) {

    const char *func_type = "linear"; //animation_params["type"];
    if(func_type == NULL) {
        return NULL;
    }

    IFloatFunc *func = NULL;
    if(strcmp(func_type, "const") == 0) {
        func = new ConstFloatFunc();
    } else if(strcmp(func_type, "linear") == 0) {
        func = new LinearFloatFunc();
    }

    if(func != NULL) {
        const JsonObject &params = animation_params["params"];
        func->InitFromJson(params);
    }

    return func;
}


