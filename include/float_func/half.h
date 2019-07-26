#ifndef __HALF_FLOAT_FUNC_H__
#define __HALF_FLOAT_FUNC_H__

#include <float_func/i_float_func.h>

class HalfFloatFunc : public IFloatFunc {

public:

    ~HalfFloatFunc() {
        delete func1;
        func1 = NULL;
        delete func2;
        func2 = NULL;
    }

    void InitFromJson(const JsonObject &func_params) {
        func1 = FloatAnimationFactory(func_params["f1"]);
        func2 = FloatAnimationFactory(func_params["f2"]);
    }

    float GetValue(float relTime) {
        if(relTime < 0.5f)
            return func1->GetValue(relTime * 2.0f);
        else
            return func2->GetValue((relTime - 0.5f) * 2.0f);
    }

private:
    IFloatFunc *func1, *func2;

};

#endif // __HALF_FLOAT_FUNC_H__