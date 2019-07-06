#ifndef __I_BOOLEAN_FUNC_H__
#define __I_BOOLEAN_FUNC_H__

#include <ArduinoJson.h>

class IBooleanFunc {

public:
    virtual ~IBooleanFunc() {}

public:
    virtual void InitFromJson(const JsonObject &animation_params) = 0;
    virtual bool GetValue(float relTime) = 0;
};

IBooleanFunc *BooleanFuncFactory(const JsonObject &animation_params);

#endif // __I_BOOLEAN_FUNC_H__
