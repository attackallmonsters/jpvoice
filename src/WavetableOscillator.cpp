#include "WavetableOscillator.h"
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <limits.h>
#include <cstdlib>

// Ctor: expects an unique name for the waveform
// This name is used for managiong wavetable files
WavetableOscillator::WavetableOscillator(const std::string formName)
{
    // set the waveform name
    waveformName = formName;

    // to avoid vtable lookup
    registerSampleGenerator(&WavetableOscillator::generateSample);

    // Define the base frequencies at which a new wavetable is used
    // These roughly correspond to octave steps starting from 20 Hz
    baseFrequencies = {20.0, 40.0, 160.0, 640.0, 2560.0};

    // Define the corresponding table size for each frequency range
    // Higher frequencies require higher resolution to avoid interpolation artifacts
    tableSizes = {1024, 2048, 4096, 8192, 16384};

    // Initialize wavetable buffers
    wavetableBuffers.reserve(tableSizes.size());
}

void WavetableOscillator::Initialize()
{
    Oscillator::Initialize();

    DSP::log("Loading wavetable for %s", waveformName.c_str());

    if (!load())
    {
        DSP::log("Wavetable for %s does not exist: generating...", waveformName.c_str());

        for (size_t i = 0; i < tableSizes.size(); ++i)
        {
            size_t size = tableSizes[i];
            dsp_float freq = baseFrequencies[i];

            // Create a new DSPBuffer instance and resize it to the desired table size
            auto buffer = std::make_unique<DSPBuffer>();
            buffer->resize(size);

            // Let the subclass generate the actual waveform data
            createWavetable(*buffer, freq);

            // Store the buffer for later use (e.g., waveform lookup)
            wavetableBuffers.push_back(std::move(buffer));
        }

        DSP::log("Wavetable for %s generated: saving...", waveformName.c_str());
        save();
        DSP::log("Wavetable for %s generated saved", waveformName.c_str());
    }
}

void WavetableOscillator::setNumVoices(int count)
{
    // Clamp to [1, 9] and resize
    numVoices = clamp(count, 1, 9);
    voices.resize(numVoices);

    for (int i = 0; i < numVoices; ++i)
    {
        dsp_float center = (numVoices - 1) / 2.0;
        dsp_float offset = i - center;

        // Spread detune symmetrically
        voices[i].detune_ratio = detune * offset / center;

        // Equal amplitude for now, normalized by number of voices
        voices[i].amp_ratio = 3.5 / numVoices;

        // Randomize phase [0.0, 1.0)
        voices[i].phase = static_cast<dsp_float>(rand()) / RAND_MAX;

        // Stereo panning - from -1.0 (left) to +1.0 (right)
        dsp_float pan = (numVoices > 1)
                            ? static_cast<dsp_float>(i) / (numVoices - 1) * 2.0 - 1.0
                            : 0.0;

        voices[i].gainL = std::sqrt(0.5 * (1.0 - pan));
        voices[i].gainR = std::sqrt(0.5 * (1.0 + pan));
    }
}

// Sets the detune factor
void WavetableOscillator::setDetune(dsp_float value)
{
    detune = clamp(value, 0.0, 1.0) * 0.125;
    setNumVoices(numVoices);
}

DSPBuffer *WavetableOscillator::selectTable(double frequency)
{
    for (size_t i = baseFrequencies.size(); i-- > 0;)
    {
        if (frequency >= baseFrequencies[i])
            return wavetableBuffers[i].get();
    }

    // Fallback
    return wavetableBuffers.front().get();
}

// Generates the requested sample for Oscillator
void WavetableOscillator::generateSample(Oscillator *osc, const dsp_float &frequency, const dsp_float &phase, dsp_float &left, dsp_float &right)
{
    WavetableOscillator *wto = static_cast<WavetableOscillator *>(osc);

    if (wto->numVoices > 1)
    {
        // Lookup table once for base frequency
        const DSPBuffer &table = *(wto->selectTable(frequency));
        size_t tableSize = table.size();

        dsp_float sumL = 0.0;
        dsp_float sumR = 0.0;

        for (auto &v : wto->voices)
        {
            dsp_float voiceFreq = frequency * (1.0 + v.detune_ratio);

            // Calculate fractional index into shared table
            dsp_float index = v.phase * tableSize;
            size_t i0 = static_cast<size_t>(index);
            size_t i1 = (i0 + 1) % tableSize;
            dsp_float frac = index - i0;

            // Linear interpolation
            dsp_float sample = (1.0 - frac) * table[i0] + frac * table[i1];

            // Apply voice-specific gain and pan
            sumL += sample * v.amp_ratio * v.gainL;
            sumR += sample * v.amp_ratio * v.gainR;

            // Phase advance per-voice
            dsp_float inc = voiceFreq / DSP::sampleRate;
            v.phase += inc;
            if (v.phase >= 1.0)
                v.phase -= 1.0;
        }

        left = sumL;
        right = sumR;
    }
    else
    {
        // Fallback to single-voice mode
        if (frequency != wto->lastFrequency)
        {
            wto->cachedTable = wto->selectTable(frequency);
            wto->cachedTableSize = wto->cachedTable->size();
            wto->lastFrequency = frequency;
        }

        dsp_float index = phase * wto->cachedTableSize;
        size_t i0 = static_cast<size_t>(index);
        size_t i1 = (i0 + 1) % wto->cachedTableSize;
        dsp_float frac = index - i0;

        dsp_float sample = (1.0 - frac) * (*wto->cachedTable)[i0] + frac * (*wto->cachedTable)[i1];
        left = right = static_cast<dsp_float>(sample);
    }
}

static void createDir()
{
    // Check if directory exists
    if (access("tables", F_OK) == -1)
    {
        // Create directory with rwx------ (0700), adjust if needed
        mkdir("tables", 0700);
    }
}

static std::string absolutePath(const std::string &relativePath)
{
    char fullPath[PATH_MAX];
    if (realpath(relativePath.c_str(), fullPath) != nullptr)
    {
        return std::string(fullPath);
    }
    else
    {
        return relativePath; // fallback
    }
}

bool WavetableOscillator::load()
{
    std::string fileName = "tables/" + waveformName + "_" + std::to_string(static_cast<int>(DSP::sampleRate)) + ".wave";

    DSP::log("Try loading wavetable %s", absolutePath(fileName).c_str());

    std::ifstream inFile(fileName.c_str());
    if (!inFile.is_open())
        return false;

    DSP::log("Found wavetable %s", absolutePath(fileName).c_str());

    wavetableBuffers.clear();
    baseFrequencies.clear();
    tableSizes.clear();

    std::string line;
    while (std::getline(inFile, line))
    {
        std::stringstream ss(line);
        std::string item;

        try
        {
            // Read frequency
            if (!std::getline(ss, item, ','))
                continue;
            double freq = std::stod(item);

            // Read size
            if (!std::getline(ss, item, ','))
                continue;
            size_t size = static_cast<size_t>(std::stoul(item));

            DSPBuffer buffer;
            buffer.resize(size);

            // Read data
            size_t sampleCount = 0;
            while (std::getline(ss, item, ',') && sampleCount < size)
            {
                buffer[sampleCount++] = static_cast<dsp_float>(std::stod(item));
            }

            if (sampleCount != size)
            {
                DSP::log("Invalid format in wavetable %s", absolutePath(fileName).c_str());
                return false;
            }

            baseFrequencies.push_back(freq);
            tableSizes.push_back(size);
            wavetableBuffers.push_back(std::make_unique<DSPBuffer>(buffer));
        }
        catch (const std::exception &ex)
        {
            DSP::log("Error generating wave forms from wavetable %s (%s)", absolutePath(fileName), ex.what());
            return false;
        }
        catch (...)
        {
            DSP::log("Error reading wavetable %s", absolutePath(fileName).c_str());
            return false;
        }
    }

    DSP::log("Wavetable %s loaded", absolutePath(fileName).c_str());

    return !wavetableBuffers.empty();
}

void WavetableOscillator::save() const
{
    createDir();

    std::string fileName = "tables/" + waveformName + "_" + std::to_string(static_cast<int>(DSP::sampleRate)) + ".wave";
    std::ofstream outFile(fileName.c_str());

    if (!outFile.is_open())
    {
        DSP::log("Wavetable %s does not exist", absolutePath(fileName).c_str());
        return;
    }

    try
    {
        for (size_t i = 0; i < wavetableBuffers.size(); ++i)
        {
            const DSPBuffer &buffer = *wavetableBuffers[i];
            outFile << baseFrequencies[i] << "," << buffer.size();

            for (size_t j = 0; j < buffer.size(); ++j)
            {
                outFile << "," << buffer[j];
            }

            outFile << "\n";
        }
    }
    catch (const std::exception &ex)
    {
        DSP::log("Error writing wave form to wavetable %s (%s)", absolutePath(fileName).c_str(), ex.what());
    }
    catch (...)
    {
        DSP::log("Error writung wave form to wavetable %s", absolutePath(fileName).c_str());
    }
}
