#ifndef __SIN_FLOAT_FUNC_H__
#define __SIN_FLOAT_FUNC_H__

#include <float_func/i_float_func.h>

#include <math.h>

class SinFloatFunc : public IFloatFunc {

public:

    void InitFromJson(const JsonObject &func_params) {
        min_value = func_params["min_value"];
        max_value = func_params["max_value"];
        phase = func_params["phase"];
        repeats = func_params["repeats"];

        diff_max_to_min = max_value - min_value;
    }

    float GetValue(float x) {
        float angleRad = (x * repeats + phase) * 2.0 * M_PI;
        float sinResult = sin(angleRad);
        return ((sinResult + 1.0) / 2.0) * diff_max_to_min + min_value;
    }

private:
    float min_value = 0.0;
    float max_value = 1.0;
    float phase = 0.0;
    float repeats = 1.0;
    float diff_max_to_min = max_value - min_value;

};

#endif // __SIN_FLOAT_FUNC_H__