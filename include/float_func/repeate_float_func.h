#ifndef __REPEATE_FLOAT_FUNC_H__
#define __REPEATE_FLOAT_FUNC_H__

#include <float_func/i_float_func.h>

#include <math.h>

class RepeateFloatFunc : public IFloatFunc {

public:

    ~RepeateFloatFunc() {
        delete func;
        func = NULL;
    }

    void InitFromJson(const JsonObject &func_params) {
        numOfTimes = func_params["num_of_times"];
        func = FloatAnimationFactory(func_params["func"]);
    }

    float GetValue(float relTime) {
        double positionInStep = fmod((relTime * numOfTimes), 1.0f);
        return func->GetValue(positionInStep);
    }

private:
    int numOfTimes;
    IFloatFunc *func;

};

#endif // __REPEATE_FLOAT_FUNC_H__