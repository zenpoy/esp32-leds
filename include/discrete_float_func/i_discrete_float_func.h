#ifndef __I_DISCRETE_FLOAT_FUNC_H__
#define __I_DISCRETE_FLOAT_FUNC_H__

#include <ArduinoJson.h>

class IDiscreteFloatFunc {

public:
    virtual ~IDiscreteFloatFunc() {}

public:
    virtual void InitFromJson(const JsonObject &animation_params) = 0;
    virtual float GetValue(int i, int total) = 0;

};

IDiscreteFloatFunc *DiscreteFloatFuncFactory(const JsonObject &animation_params);

#endif // __I_DISCRETE_FLOAT_FUNC_H__
