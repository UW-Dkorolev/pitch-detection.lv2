#include <iostream>

#include "pitch.hh"
#include "musicalscale.hh"

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define AMP_URI "http://performous.org/lv2/pitch-detection"

typedef enum {
        AMP_INPUT  = 0,
        AMP_OUTPUT = 1
} PortIndex;

typedef struct {
        // Port buffers
        const float*  input;
        float*        output;
	Analyzer*     analyzer;
	MusicalScale* ms;
} Amp;

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
        Amp* amp = new Amp();
	amp->analyzer = new Analyzer(rate, "test");
	amp->ms = new MusicalScale();

        return (LV2_Handle)amp;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
        Amp* amp = (Amp*)instance;

        switch ((PortIndex)port) {
        case AMP_INPUT:
                amp->input = (const float*)data;
                break;
        case AMP_OUTPUT:
                amp->output = (float*)data;
                break;
        }
}

static void activate(LV2_Handle instance) {
}

static void run(LV2_Handle instance, uint32_t n_samples) {
        const Amp* amp = (const Amp*)instance;

        const float* const input  = amp->input;
        float* const       output = amp->output;

	amp->analyzer->input(input, input + n_samples);
	amp->analyzer->process();
	auto tone = amp->analyzer->findTone();
	if (tone != NULL) {
		amp->ms->clear().setFreq(tone->freq);
		std::cout << "Note: " << amp->ms->getStr();
		std::cout << ", Samples: " << n_samples;
		std::cout << ", Offset: " << amp->ms->getNoteOffset() << std::endl;
	}

        for (uint32_t pos = 0; pos < n_samples; pos++) {
                output[pos] = input[pos]; // simple copy
        }
}

static void deactivate(LV2_Handle instance) {
}

static void cleanup(LV2_Handle instance) {
        Amp* amp = (Amp*)instance;
	delete amp->analyzer;
	delete amp->ms;
        delete amp;
}

static const void* extension_data(const char* uri) {
        return NULL;
}

static const LV2_Descriptor descriptor = {
        AMP_URI,
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
