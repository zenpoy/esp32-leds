#include <boolean_func/i_boolean_func.h>

#include <boolean_func/const_boolean_func.h>
#include <boolean_func/equal_spreads_boolean_func.h>

IBooleanFunc *BooleanFuncFactory(const JsonObject &func_params) {

    const char *func_type = func_params["t"];
    if(func_type == NULL) {
        return NULL;
    }

    IBooleanFunc *func = NULL;
    if(strcmp(func_type, "const") == 0) {
        func = new ConstBooleanFunc();
    } else if(strcmp(func_type, "equalSpreads") == 0) {
        func = new EqualSpreadBooleanFunc();
    }

    if(func == NULL) {
        return NULL;
    }

    func->InitFromJson(func_params);

    return func;
}


