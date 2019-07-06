#include <boolean_func/i_boolean_func.h>

#include <boolean_func/const_boolean_func.h>
#include <boolean_func/equal_spreads_boolean_func.h>

IBooleanFunc *BooleanFuncFactory(const JsonObject &animation_params) {

    const char *func_type = animation_params["type"];
    if(func_type == NULL) {
        return NULL;
    }

    IBooleanFunc *func = NULL;
    if(strcmp(func_type, "const") == 0) {
        func = new ConstBooleanFunc();
    } else if(strcmp(func_type, "equal_spreads") == 0) {
        func = new EqualSpreadBooleanFunc();
    }

    if(func != NULL) {
        const JsonObject &params = animation_params["params"];
        func->InitFromJson(params);
    }

    return func;
}


