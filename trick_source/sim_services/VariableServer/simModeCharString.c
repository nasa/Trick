
#include "trick/sim_mode.h"

const char * simModeCharString(SIM_MODE mode) {
    switch (mode)
    {
    case Initialization: return "Initialization";
    case Run:            return "Run";
    case Step:           return "Step";
    case Freeze:         return "Freeze";
    case ExitMode:       return "ExitMode";
    default:             return "InvalidMode";
    }
}