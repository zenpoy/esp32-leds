#ifndef __EQUAL_SPREAD_BOOLEAN_FUNC_H__
#define __EQUAL_SPREAD_BOOLEAN_FUNC_H__

#include <boolean_func/i_boolean_func.h>

class EqualSpreadBooleanFunc : public IBooleanFunc 
{

public:

    void InitFromJson(const JsonObject &animation_params)
    {
        numberOfSpreads = animation_params["num"];
        initialState = animation_params["init"];
    }

    bool GetValue(float relTime)
    {
        bool segmentVal = ((int) (relTime * numberOfSpreads)) % 2 == 0;
        return segmentVal ^ initialState;
    }

private:
    int numberOfSpreads;
    bool initialState;
};

#endif // __EQUAL_SPREAD_BOOLEAN_FUNC_H__