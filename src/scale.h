#ifndef RAMBAJZ_SCALE_H
#define RAMBAJZ_SCALE_H

struct tone {
	short semitone;
	short octave;
	double cents;
};

struct tone calc_tone(double frequency);
const char *tone_name(struct tone);

#endif
