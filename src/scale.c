#include <stdlib.h>
#include <math.h>
#include "scale.h"

static const char names[][8] = {
	"A",
	"Bb",
	"B",
	"C",
	"Db",
	"D",
	"Eb",
	"E",
	"F",
	"Gb",
	"G",
	"Ab"
};

struct tone calc_tone(double frequency) {
	if (isnan(frequency) || frequency <= 0)
		return (struct tone){.semitone = -1, .octave = -1, .cents = 0};

	static const double reference = 105.37631656229593; // = log2(440) * 12;
	double ours = log2(frequency) * 12;
	double diff = ours - reference;
	int semitones = round(diff);
	struct tone tone;
	tone.semitone = ((semitones % 12) + 12) % 12;
	tone.octave = (semitones - tone.semitone) / 12;
	tone.cents = (diff - semitones) * 100;
	return tone;
}

const char *tone_name(struct tone tone) {
	return (tone.semitone >= 0 && tone.semitone < 12) ? names[tone.semitone] : NULL;
}
