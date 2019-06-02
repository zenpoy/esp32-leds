#ifndef __LINEAR_FLOAT_FUNC_H__
#define __LINEAR_FLOAT_FUNC_H__

#include <float_func/i_float_func.h>

class LinearFloatFunc : public IFloatFunc {

public:

    void InitFromJson(const JsonObject &func_params) {
        start_value = func_params["start"];
        end_value = func_params["end"];
        diff_end_to_start = end_value - start_value;
    }

    float GetValue(float relTime) {
        return start_value + relTime * diff_end_to_start;
    }

private:
    float start_value = 0.0;
    float end_value = 1.0;
    float diff_end_to_start = 1.0;

};

#endif // __LINEAR_FLOAT_FUNC_H__