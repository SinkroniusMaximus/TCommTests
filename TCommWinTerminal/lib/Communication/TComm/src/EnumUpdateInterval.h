#ifndef ENUMUPDATEINTERVAL_H
#define ENUMUPDATEINTERVAL_H

namespace TComm
{
    enum EnumUpdateInterval
    {
        eOnChange,
        eClock_0_5hz,
        eClock_0_625hz,
        eClock_1_hz,
        eClock_2hz,
        eClock_2_5hz,
        eClock_5hz,
        eClock_10hz
    };
};

#endif // ENUMUPDATEINTERVAL_H