#ifndef SOUNDEFFECTS_H
#define SOUNDEFFECTS_H

#include "mode-options.h"

#include <vector>
#include <cstdint>

// TODO: Add comments about possible exceptions

namespace effects
{
  // Trim effect.
  // Removes WAV data to the left and to the right of selected fragment.
  void trim(std::vector<uint8_t>& bytes, TrimOptions& options);

  // Fade effect.
  // Gradually reduces volume of WAV data from start point to end point.
  // The volume at start point is 100%. The volume at end point can be selected (0% by default).
  void fade(std::vector<uint8_t>& bytes, FadeOptions& options);

  // Reverb effect.
  // Adds reverberation to WAV data with selected delay and decay coefficient.
  void reverb(std::vector<uint8_t>& bytes, ReverbOptions& options);
}

#endif
