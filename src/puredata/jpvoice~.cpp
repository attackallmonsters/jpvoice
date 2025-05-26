// jpvoice.cpp - Pure Data external wrapping the Voice audio synthesis class
#include "m_pd.h"
#include "Voice.h"
#include "clamp.h"

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
void jpvoice_tilde_f(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - n for carrier frequency f1: [f1 f(");
        return;
    }
    double f = atom_getfloat(argv);
    x->voice->setFrequency(f);
}

// Frequency offset modulator in halftones
void jpvoice_tilde_offset(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument -24 - 24 for modulator offset: [offset f(");
        return;
    }
    
    double offset = clamp(atom_getfloat(argv), -24.0f, 24.0f);
    x->voice->setPitchOffset(offset);
}

// Frequency fine tuning for modulator -100 - 100 [fine f(
void jpvoice_tilde_fine(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument -100 - 100 for modulator fine tune: [fine f(");
        return;
    }

    double finetune = clamp(atom_getfloat(argv), -100.0f, 100.0f);
    x->voice->setFineTune(finetune);
}

// Frequency of modulator set via list [detune factor(
void jpvoice_tilde_detune(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected float argument 0 - 1 for detune factor: [detune f(");
        return;
    }

    double detune = clamp(atom_getfloat(argv), 0.0f, 1.0f);
    x->voice->setDetune(detune);
}

// Oscillator type carrier [carrier n( 1 - 5
void jpvoice_tilde_carrier(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 1 - 5 for carrier oscillator type: [carrier n(");
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
    case 5:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Sine);
        break;
    default:
        x->voice->setCarrierOscillatorType(CarrierOscillatiorType::Supersaw);
        break;
    }
}

// Oscillator type carrier [modulator n( 1 - 4
void jpvoice_tilde_modulator(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 1 - 4 for modulation oscillator type: [modulator n(");
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

// Sets the type of noise to white (0) or pink (1)
void jpvoice_tilde_noisetype(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 (white) or 1 (pink) for noise type: [noisetype n(");
        return;
    }

    switch (atom_getint(argv))
    {
    case 0:
        x->voice->setNoiseType(NoiseType::White);
        break;
    case 1:
        x->voice->setNoiseType(NoiseType::Pink);
        break;
    default:
        x->voice->setNoiseType(NoiseType::White);
        break;
    }
}

// Oscillator mix [oscmix f(
void jpvoice_tilde_oscmix(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 - 1 for oscillator mix: [oscmix f(");
        return;
    }

    float mix = clamp(static_cast<float>(atom_getfloat(argv)), 0.0f, 1.0f);
    x->voice->setOscillatorMix(mix);
}

// Noise mix [noisemix f(
void jpvoice_tilde_noisemix(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 - 1 for noise mix: [noisemix f(");
        return;
    }

    float mix = clamp(static_cast<float>(atom_getfloat(argv)), 0.0f, 1.0f);
    x->voice->setNoiseMix(mix);
}

// Enables FM
void jpvoice_tilde_fm(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 (white) or 1 (pink) for noise type: [fm n(");
        return;
    }

    int enabled = clamp(static_cast<int>(atom_getint(argv)), 0, 1);
    x->voice->setFMEnabled(enabled == 1);
}

// Sets the FM modulation index [fmmod f(
void jpvoice_tilde_fmmod(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 - n for FM modulation index: [fmmod f(");
        return;
    }

    double idx = clampmin(static_cast<float>(atom_getfloat(argv)), 0.0f);
    x->voice->setFMModIndex(idx);
}

// Sets the pulse width 0 - 1 [pw f(
void jpvoice_tilde_pw(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 - 1 for pulse width: [pw f(");
        return;
    }

    float idx = clamp(static_cast<float>(atom_getfloat(argv)), 0.0f, 1.0f);
    x->voice->setDutyCycle(idx);
}

// Oscillator sync
void jpvoice_tilde_sync(t_jpvoice *x, t_symbol *, int argc, t_atom *argv)
{
    post("[jpvoice~] sync called");
    if (argc != 1 || argv[0].a_type != A_FLOAT)
    {
        pd_error(x, "[jpvoice~]: expected int argument 0 (unsynced) or 1 (synced) for oscillator sync: [oscsync n(");
        return;
    }

    int enabled = clamp(static_cast<int>(atom_getint(argv)), 0, 1);
    x->voice->setSyncEnabled(enabled == 1);
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
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_f, gensym("f"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_offset, gensym("offset"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_fine, gensym("fine"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_detune, gensym("detune"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_carrier, gensym("carrier"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_modulator, gensym("modulator"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_oscmix, gensym("oscmix"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_noisemix, gensym("noisemix"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_noisetype, gensym("noisetype"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_fm, gensym("fm"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_fmmod, gensym("fmmod"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_pw, gensym("pw"), A_GIMME, 0);
    class_addmethod(jpvoice_class, (t_method)jpvoice_tilde_sync, gensym("sync"), A_GIMME, 0);
}
