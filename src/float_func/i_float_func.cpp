#include <float_func/i_float_func.h>

#include <float_func/const_float_func.h>
#include <float_func/linear_float_func.h>
#include <float_func/sin_float_func.h>
#include <float_func/segment_float_func.h>
#include <float_func/steps.h>
#include <float_func/repeat_float_func.h>
#include <float_func/comb2.h>
#include <float_func/half.h>

IFloatFunc *FloatAnimationFactory(const JsonObject &animation_params) {

    const char *func_type = animation_params["t"];
    if(func_type == NULL) {
        return NULL;
    }

    IFloatFunc *func = NULL;
    if(strcmp(func_type, "const") == 0) {
        func = new ConstFloatFunc();
    } else if(strcmp(func_type, "lin") == 0) {
        func = new LinearFloatFunc();
    } else if(strcmp(func_type, "sin") == 0) {
        func = new SinFloatFunc();
    } else if(strcmp(func_type, "seg") == 0) {
        func = new SegmentFloatFunc();
    } else if(strcmp(func_type, "steps") == 0) {
        func = new StepsFloatFunc();
    } else if(strcmp(func_type, "repeat") == 0) {
        func = new RepeatFloatFunc();
    } else if(strcmp(func_type, "comb2") == 0) {
        func = new Comb2FloatFunc();
    } else if(strcmp(func_type, "half") == 0) {
        func = new HalfFloatFunc();
    }

    if(func == NULL) {
        return NULL;
    }

    func->InitFromJson(animation_params);
    return func;
}


