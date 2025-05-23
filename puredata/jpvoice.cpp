// jpvoice.cpp - Pure Data external wrapping the Voice audio synthesis class
#include "m_pd.h"
#include "Voice.h"

static t_class *jpvoice_class;

// Object structure
typedef struct _jpvoice {
    t_object x_obj;
    Voice *voice;
    t_outlet *audio_out;
} t_jpvoice;

// DSP perform function
t_int *jpvoice_perform(t_int *w) {
    t_jpvoice *x = (t_jpvoice *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);

    for (int i = 0; i < n; i++) {
        out[i] = static_cast<float>(x->voice->getSample());
    }

    return (w + 4);
}

// DSP add function
void jpvoice_dsp(t_jpvoice *x, t_signal **sp) {
    dsp_add(jpvoice_perform, 3, x, sp[0]->s_vec, sp[0]->s_n);
}

// Set frequency
void jpvoice_set_frequency(t_jpvoice *x, t_floatarg f) {
    x->voice->setFrequency(f);
}

// Set FM enabled
void jpvoice_set_fm_enabled(t_jpvoice *x, t_floatarg f) {
    x->voice->setFMEnabled(f != 0);
}

// Set sync enabled
void jpvoice_set_sync_enabled(t_jpvoice *x, t_floatarg f) {
    x->voice->setSyncEnabled(f != 0);
}

// Set modulation index
void jpvoice_set_mod_index(t_jpvoice *x, t_floatarg f) {
    x->voice->setModIndex(f);
}

// Constructor
void *jpvoice_new() {
    t_jpvoice *x = (t_jpvoice *)pd_new(jpvoice_class);

    x->voice = new Voice(new Oscillator(), new Oscillator());

    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("float"));
    x->audio_out = outlet_new(&x->x_obj, &s_signal);

    return (void *)x;
}

// Destructor
void jpvoice_free(t_jpvoice *x) {
    delete x->voice;
}

// Setup
extern "C" void jpvoice_tilde_setup(void) {
    jpvoice_class = class_new(gensym("jpvoice~"),
                              (t_newmethod)jpvoice_new,
                              (t_method)jpvoice_free,
                              sizeof(t_jpvoice),
                              CLASS_DEFAULT,
                              A_NULL);

    class_addmethod(jpvoice_class, (t_method)jpvoice_dsp, gensym("dsp"), A_CANT, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_set_f1, gensym("f1"), A_DEFFLOAT, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_set_f2, gensym("f2"), A_DEFFLOAT, 0);    class_addmethod(jpvoice_class, (t_method)jpvoice_set_fm_enabled, gensym("fm_enabled"), A_DEFFLOAT, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_set_sync_enabled, gensym("sync_enabled"), A_DEFFLOAT, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_set_mod_index, gensym("mod_index"), A_DEFFLOAT, 0);

    CLASS_MAINSIGNALIN(jpvoice_class, t_jpvoice, audio_out);
}
