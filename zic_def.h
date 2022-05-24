#ifndef ZIC_DEF_H_
#define ZIC_DEF_H_

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100
#endif

// sample frequency = 44100 Hz, so one second equals 44100 samples.
#ifndef DELTA_TIME
#define DELTA_TIME 1.0f / SAMPLE_RATE
#endif

#ifndef SAMPLE_PER_MS
#define SAMPLE_PER_MS SAMPLE_RATE / 1000 // 44.1
#endif

#ifndef MAX_FREQUENCY
#define MAX_FREQUENCY SAMPLE_RATE * 0.25
#endif

#ifndef between
#define between(x, a, b) (((a) <= (x)) && ((x) <= (b)))
#endif

#ifndef range
#define range(x, _min, _max) ((x) < (_max) ? ((x) > (_min) ? (x) : (_min)) : (_max))
#endif

#endif