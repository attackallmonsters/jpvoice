// jpvoice.cpp - Pure Data external wrapping the Voice audio synthesis class
#include "m_pd.h"
#include "Voice.h"

static t_class *jpvoice_class;

typedef struct _jpvoice
{
    t_object x_obj;
    Voice *voice;
    t_outlet *audio_out;
} t_jpvoice;

// DSP perform function
t_int *jpvoice_tilde_perform(t_int *w)
{
    t_jpvoice *x = (t_jpvoice *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);

    for (int i = 0; i < n; i++)
        out[i] = static_cast<float>(x->voice->getSample());

    return (w + 4);
}

// DSP add function
void jpvoice_tilde_dsp(t_jpvoice *x, t_signal **sp)
{
    dsp_add(jpvoice_tilde_perform, 3, x, sp[0]->s_vec, sp[0]->s_n);
}

// Frequency of carrier set via list [f1 freq(
void jpvoice_tilde_f1(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 2 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - n for carrier frequency f1: [f1 f(");
        return;
    }
    double f = atom_getfloat(argv);
    x->voice->setFrequencyCarrier(f);
}

// Frequency of modulator set via list [f2 freq(
void jpvoice_tilde_f2(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 2 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - n for modulator frequency f2: [f2 f(");
        return;
    }
    double f = atom_getfloat(argv);
    x->voice->setFrequencyModulator(f);
}

// Constructor
void *jpvoice_tilde_new()
{
    t_jpvoice *x = (t_jpvoice *)pd_new(jpvoice_class);

    x->voice = new Voice();

    x->audio_out = outlet_new(&x->x_obj, &s_signal);

    return (void *)x;
}

// Destructor
void jpvoice_tilde_free(t_jpvoice *x)
{
    delete x->voice;
}

// Setup function
extern "C" void jpvoice_tilde_setup(void)
{
    jpvoice_class = class_new(gensym("jpvoice~"),
                              (t_newmethod)jpvoice_tilde_new,
                              (t_method)jpvoice_tilde_free,
                              sizeof(t_jpvoice),
                              CLASS_DEFAULT,
                              A_NULL);

    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_dsp, gensym("dsp"), A_CANT, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_f1, gensym("f1"), A_GIMME, 0);
}
