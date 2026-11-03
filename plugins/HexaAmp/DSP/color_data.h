// Jonssonic H E X A - A M P
// Cabinet Color model data.
// SPDX-License-Identifier: MIT

#pragma once

namespace jnsc::hexa_amp::color_data {

// Peak columns: gain at 0/100%, frequency at 0/100%, Q at 0/100%.
inline constexpr float peaks[8][6] = {
    {6.265778f, 16.982200f, 167.280256f, 103.449150f, 0.677310f, 1.325873f},
    {3.160047f, -2.853703f, 426.783914f, 441.726688f, 5.898264f, 2.234056f},
    {-5.343315f, -5.466822f, 702.017354f, 558.925125f, 2.697462f, 7.349530f},
    {-2.877154f, 22.116433f, 904.540368f, 1249.000000f, 8.633820f, 1.216476f},
    {-3.252761f, -24.000000f, 1358.430005f, 1373.757921f, 6.462379f, 0.987502f},
    {4.896619f, 3.869994f, 2240.551430f, 2267.334293f, 4.119766f, 6.397113f},
    {8.854934f, 8.819347f, 4499.000000f, 4499.000000f, 1.011136f, 1.165484f},
    {-7.870946f, -14.610344f, 7092.799368f, 7217.482718f, 4.181211f, 2.833438f},
};


inline constexpr float highpassFrequencyHz[2] = {52.681480f, 131.826991f};
inline constexpr float highpassQ = 0.7071067812f;
inline constexpr float shelfFrequencyHz[2] = {5746.987592f, 10850.262937f};
inline constexpr float shelfGainDb[2] = {-17.0222165f, -17.1821385f};
inline constexpr float shelfQ = 0.7071067812f;
inline constexpr float lowpassFrequencyHz[2] = {17066.778091f, 19175.354557f};
inline constexpr float lowpassQ[2] = {0.5411961001f, 1.3065629649f};

} // namespace jnsc::hexa_amp::color_data
