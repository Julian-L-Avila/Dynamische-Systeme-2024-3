#ifndef CONSTANTS_H
#define CONSTANTS_H

// General
constexpr double kAmplitude = 110.0;
constexpr double kOnTime    = 170.0;
constexpr double kOffset    = 20.0;

// Hand 1st Order (1)
constexpr double kKpF1 = 2.0;
constexpr double kTpF1 = 303.0;
constexpr double kTdF1 = 30.0;

// Hand 1st Order (2)
constexpr double kKpF2 = 2.0;
constexpr double kTpF2 = 291.2;
constexpr double kTdF2 = 206.16 - kOnTime;

// MATLAB 1st Order
constexpr double kKpFM = 2.000635678404079;
constexpr double kTpFM = 298.8828592591657;
constexpr double kTdFM = 36.91;

// Hand 2nd Order (1)
constexpr double kKpS1  = 2.0;
constexpr double kTp1S1 = 66.8407;
constexpr double kTp2S1 = 263.6048;
constexpr double kTdS1  = -20.1693;

// Hand 2nd Order (2)
constexpr double kKpS2  = 2.0;
constexpr double kTp1S2 = 0.4039819387;
constexpr double kTp2S2 = 312.9882006;
constexpr double kTdS2  = 200 - kOnTime;

// MATLAB 2nd Order
constexpr double kKpSM  = 1.989015963633353;
constexpr double kTp1SM = 25.57774292268281;
constexpr double kTp2SM = 290.5659847649038;
constexpr double kTdSM  = 14.49;

#endif
