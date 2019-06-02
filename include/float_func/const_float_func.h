#ifndef __CONST_FLOAT_FUNC_H__
#define __CONST_FLOAT_FUNC_H__

#include <float_func/i_float_func.h>

class ConstFloatFunc : public IFloatFunc {

public:

    void InitFromJson(const JsonObject &func_params) {
        value_to_return = func_params["value"];
    }

    float GetValue(float relTime) {
        return value_to_return;
    }

private:
    float value_to_return = 0.0;

};

#endif // __CONST_FLOAT_FUNC_H__