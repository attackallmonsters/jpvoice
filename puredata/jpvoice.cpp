#pragma GCC diagnostic ignored "-Wcast-function-type"

#include "m_pd.h"
#include <cmath>
#include <cstdlib>

static t_class *supersaw_class;

constexpr int NUM_VOICES = 7;

constexpr float DETUNE_RATIOS[NUM_VOICES] = {-0.24f, -0.14f, -0.06f, 0.00f, 0.06f, 0.14f, 0.24f};
constexpr float AMP_RATIOS[NUM_VOICES] = {0.25f, 0.5f, 0.75f, 1.0f, 0.75f, 0.5f, 0.25f};

struct Voice
{
    float phase;
    float detune_ratio;
    float amp_ratio;
};

struct t_supersaw
{
    t_object x_obj;
    t_float f_freq;
    t_float f_detune;
    Voice voices[NUM_VOICES];
    t_inlet *in_detune;
    t_outlet *x_out;
};

// Perform-Routine
t_int *supersaw_perform(t_int *w)
{
    t_supersaw *x = (t_supersaw *)(w[1]);
    t_sample *in_freq = (t_sample *)(w[2]);
    t_sample *in_detune = (t_sample *)(w[3]);
    t_sample *out = (t_sample *)(w[4]);
    int n = (int)(w[5]);

    float sr = sys_getsr();
    if (sr <= 0)
        sr = 44100.0f;

    while (n--)
    {
        float freq = *in_freq++;
        float detune = *in_detune++;

        freq = std::fmax(1.0f, freq);
        detune = std::fmin(std::fmax(detune, 0.0f), 1.0f);

        float sample = 0.0f;

        for (int i = 0; i < NUM_VOICES; ++i)
        {
            Voice &v = x->voices[i];

            // Berechne individuelle Frequenz mit Detune
            float detune_factor = v.detune_ratio * detune;
            float voice_freq = freq * (1.0f + detune_factor);
            float phase_inc = voice_freq / sr;

            float t = v.phase;
            float val = 2.0f * t - 1.0f;

            v.phase += phase_inc;
            if (v.phase >= 1.0f)
                v.phase -= 1.0f;

            sample += val * v.amp_ratio;
        }

        sample /= NUM_VOICES; // Normalisieren
        *out++ = sample;
    }

    return (w + 6);
}

// DSP-Methode
void supersaw_dsp(t_supersaw *x, t_signal **sp)
{
    dsp_add(supersaw_perform, 5, x,
            sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

// Konstruktor
void *supersaw_new(t_floatarg f)
{
    t_supersaw *x = (t_supersaw *)pd_new(supersaw_class);
    x->f_freq = f > 0 ? f : 440.0f;
    x->f_detune = 0.0f;

    // Initialisiere 7 Stimmen mit fester Detune-Ratio
    // float ratios[NUM_VOICES] = {-0.028f, -0.017f, -0.008f, 0.0f, 0.006f, 0.015f, 0.027f};

    for (int i = 0; i < NUM_VOICES; ++i)
    {
        x->voices[i].phase = static_cast<float>(rand()) / RAND_MAX;
        x->voices[i].detune_ratio = DETUNE_RATIOS[i];
        x->voices[i].amp_ratio = AMP_RATIOS[i];
    }

    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal); // Detune
    outlet_new(&x->x_obj, &s_signal);                            // Audio
    return (void *)x;
}

// Setup-Funktion
extern "C"
{
    void supersaw_tilde_setup(void)
    {
        supersaw_class = class_new(gensym("supersaw~"),
                                   (t_newmethod)supersaw_new,
                                   0, sizeof(t_supersaw),
                                   CLASS_DEFAULT,
                                   A_DEFFLOAT, 0);

        class_addmethod(supersaw_class,
                        (t_method)supersaw_dsp, gensym("dsp"), A_CANT, 0);

        CLASS_MAINSIGNALIN(supersaw_class, t_supersaw, f_freq);
    }
}
