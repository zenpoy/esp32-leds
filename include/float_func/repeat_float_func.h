#ifndef __REPEAT_FLOAT_FUNC_H__
#define __REPEAT_FLOAT_FUNC_H__

#include <float_func/i_float_func.h>

#include <math.h>

class RepeatFloatFunc : public IFloatFunc
{

public:
    ~RepeatFloatFunc()
    {
        delete func;
        func = NULL;
    }

    void InitFromJson(const JsonObject &func_params)
    {
        numOfTimes = func_params["num"];
        func = FloatAnimationFactory(func_params["func"]);
    }

    float GetValue(float relTime)
    {
        double positionInStep = fmod((relTime * numOfTimes), 1.0f);
        return func->GetValue(positionInStep);
    }

private:
    float numOfTimes;
    IFloatFunc *func;
};

#endif // __REPEAT_FLOAT_FUNC_H__