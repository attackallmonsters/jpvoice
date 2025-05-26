// jpvoice.cpp - Pure Data external wrapping the Voice audio synthesis class
#include "m_pd.h"
#include "Voice.h"

static t_class *jpvoice_class;

typedef struct _jpvoice
{
    t_object x_obj;
    Voice *voice;
    t_outlet *left_out;
    t_outlet *right_out;
    double left;
    double right;
} t_jpvoice;

// DSP perform function
t_int *jpvoice_tilde_perform(t_int *w)
{
    t_jpvoice *x = (t_jpvoice *)(w[1]);
    t_sample *outL = (t_sample *)(w[2]);
    t_sample *outR = (t_sample *)(w[3]);
    int n = (int)(w[4]);

    for (int i = 0; i < n; i++)
    {
        x->voice->getSample(x->left, x->right);
        outL[i] = static_cast<t_sample>(x->left);
        outR[i] = static_cast<t_sample>(x->right);
    }

    return (w + 5);
}

// DSP add function
void jpvoice_tilde_dsp(t_jpvoice *x, t_signal **sp)
{
    x->voice->setSampleRate(sp[0]->s_sr);
    dsp_add(jpvoice_tilde_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

// Frequency of carrier set via list [f1 freq(
void jpvoice_tilde_f1(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - n for carrier frequency f1: [f1 f(");
        return;
    }
    double f = atom_getfloat(argv);
    x->voice->setFrequencyCarrier(f);
}

// Frequency offset modulator in halftones
void jpvoice_tilde_offset(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - n for modulator frequency f2: [f2 f(");
        return;
    }
    double f = atom_getfloat(argv);
    x->voice->setFrequencyModulator(f);
}

// Frequency offset modulator in halftones
void jpvoice_tilde_fine(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - n for modulator frequency f2: [f2 f(");
        return;
    }
    double f = atom_getfloat(argv);
    x->voice->setFrequencyModulator(f);
}

// Frequency of modulator set via list [detune factor(
void jpvoice_tilde_detune(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - 1 for detune factor: [detune f(");
        return;
    }
    double detune = atom_getfloat(argv);
    x->voice->setDetune(detune);
}

// Oscillator type carrier [cosc n( 1 - 4
void jpvoice_tilde_cosc(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 1 - 4 for carrier oscillator type: [cosc n(");
        return;
    }

    switch (atom_getint(argv))
    {
    case 1:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Supersaw);
        break;
    case 2:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Saw);
        break;
    case 3:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Square);
        break;
    case 4:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Triangle);
        break;
    default:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Supersaw);
        break;
    }
}

// Oscillator type carrier [mosc n( 1 - 4
void jpvoice_tilde_mosc(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 1 - 4 for modulation oscillator type: [mosc n(");
        return;
    }

    switch (atom_getint(argv))
    {
    case 1:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Saw);
        break;
    case 2:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Square);
        break;
    case 3:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Triangle);
        break;
    case 4:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Sine);
        break;
    default:
        x->voice->setModulatorOscillatorType(ModulatorOscillatorType::Sine);
        break;
    }
}

// Constructor
void *jpvoice_tilde_new()
{
    t_jpvoice *x = (t_jpvoice *)pd_new(jpvoice_class);

    x->voice = new Voice();

    x->left_out = outlet_new(&x->x_obj, &s_signal);
    x->right_out = outlet_new(&x->x_obj, &s_signal);

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
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_f1, gensym("f"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_offset, gensym("offset"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_fine, gensym("fine"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_detune, gensym("detune"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_cosc, gensym("cosc"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_mosc, gensym("mosc"), A_GIMME, 0);
}
