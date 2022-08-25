#ifndef __I_FLOAT_FUNC_H__
#define __I_FLOAT_FUNC_H__

#include <ArduinoJson.h>

class IFloatFunc
{

public:
    virtual ~IFloatFunc() {}

public:
    virtual void InitFromJson(const JsonObject &animation_params) = 0;
    virtual float GetValue(float relTime) = 0;
};

IFloatFunc *FloatAnimationFactory(const JsonObject &animation_params);

#endif // __I_FLOAT_FUNC_H__
