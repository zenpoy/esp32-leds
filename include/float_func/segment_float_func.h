#ifndef __SEGMENT_FLOAT_FUNC_H__
#define __SEGMENT_FLOAT_FUNC_H__

#include <float_func/i_float_func.h>

class SegmentFloatFunc : public IFloatFunc {

public:

    void InitFromJson(const JsonObject &func_params) {
        start_x = func_params["start_x"];
        end_x = func_params["end_x"];
        segment_value = func_params["segment_value"];
        non_segment_value = func_params["non_segment_value"];
    }

    float GetValue(float x) {
        return (x >= start_x && x < end_x) ? segment_value : non_segment_value;
    }

private:
    float start_x = 0.0;
    float end_x = 1.0;
    float segment_value = 1.0;
    float non_segment_value = 0.0;

};

#endif // __SEGMENT_FLOAT_FUNC_H__