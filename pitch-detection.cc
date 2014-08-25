#include <iostream>

#include "pitch.hh"
#include "musicalscale.hh"

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define DETECTION_URI "http://performous.org/lv2/pitch-detection"

typedef enum {
	DETECTION_INPUT  = 0,
	DETECTION_OUTPUT = 1,
	DETECTION_DETECTED = 2,
	DETECTION_DIFFERENCE = 3,
	DETECTION_FREQUENCY = 4,
	DETECTION_NOTE = 5
} PortIndex;

typedef struct {
	// Port buffers
	const float*  input;
	float*        output;
	Analyzer*     analyzer;
	MusicalScale* ms;
	// Outputs
	float* detected;
	float* difference;
	float* frequency;
	float* note;
} Detection;

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
	    double                    rate,
	    const char*               bundle_path,
	    const LV2_Feature* const* features) {
	Detection* d = new Detection();
	d->analyzer = new Analyzer(rate, "test");
	d->ms = new MusicalScale();

	return (LV2_Handle)d;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
	Detection* d = (Detection*)instance;

	switch ((PortIndex)port) {
	case DETECTION_INPUT:
		d->input = (const float*)data;
		break;
	case DETECTION_OUTPUT:
		d->output = (float*)data;
		break;
	case DETECTION_DETECTED:
		d->detected = (float*)data;
		break;
	case DETECTION_DIFFERENCE:
		d->difference = (float*)data;
		break;
	case DETECTION_FREQUENCY:
		d->frequency = (float*)data;
		break;
	case DETECTION_NOTE:
		d->note = (float*)data;
		break;
	}
}

static void activate(LV2_Handle instance) {
}

static void run(LV2_Handle instance, uint32_t n_samples) {
	const Detection* d = (const Detection*)instance;

	const float* const input  = d->input;
	float* const       output = d->output;

	d->analyzer->input(input, input + n_samples);
	d->analyzer->process();
	auto tone = d->analyzer->findTone();
	if (tone != NULL) {
		double frequency = tone->freq;
		d->ms->clear().setFreq(frequency);
		double difference = d->ms->getNoteOffset();
		unsigned note = d->ms->getNum();
		// fill the output
		*d->detected = 1.0;
		*d->difference = float(difference);
		*d->frequency = float(frequency);
		*d->note = float(note);
	} else {
		*d->detected = 0.0;
		*d->difference = 0.0;
		*d->frequency = 0.0;
		*d->note = 0.0;
	}

	for (uint32_t pos = 0; pos < n_samples; pos++) {
		output[pos] = input[pos]; // simple copy
	}
}

static void deactivate(LV2_Handle instance) {
}

static void cleanup(LV2_Handle instance) {
	Detection* d = (Detection*)instance;
	delete d->analyzer;
	delete d->ms;
	delete d;
}

static const void* extension_data(const char* uri) {
	return NULL;
}

static const LV2_Descriptor descriptor = {
	DETECTION_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) {
	switch (index) {
	case 0:  return &descriptor;
	default: return NULL;
	}
}
