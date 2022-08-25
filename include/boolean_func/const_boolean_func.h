#ifndef __CONST_BOOLEAN_FUNC_H__
#define __CONST_BOOLEAN_FUNC_H__

#include <boolean_func/i_boolean_func.h>

class ConstBooleanFunc : public IBooleanFunc
{

public:
    void InitFromJson(const JsonObject &animation_params)
    {
        valueToReturn = animation_params["v"];
    }

    bool GetValue(float relTime)
    {
        return valueToReturn;
    }

    bool valueToReturn;
};

#endif // __CONST_BOOLEAN_FUNC_H__