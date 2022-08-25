#ifndef __COMB_2_FLOAT_FUNC_H__
#define __COMB_2_FLOAT_FUNC_H__

#include <float_func/i_float_func.h>

class Comb2FloatFunc : public IFloatFunc
{

public:
    ~Comb2FloatFunc()
    {
        delete func1;
        func1 = NULL;
        delete func2;
        func2 = NULL;
    }

    void InitFromJson(const JsonObject &func_params)
    {
        func1 = FloatAnimationFactory(func_params["f1"]);
        func2 = FloatAnimationFactory(func_params["f2"]);
        amount1 = func_params["amount1"];
        amount2 = func_params["amount2"];
    }

    float GetValue(float relTime)
    {
        return (amount1 * func1->GetValue(relTime)) + (amount2 * func2->GetValue(relTime));
    }

private:
    float amount1, amount2;
    IFloatFunc *func1, *func2;
};

#endif // __COMB_2_FLOAT_FUNC_H__