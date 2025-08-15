#include <complex>
#include "NTCSensor.h"

NTCSensor::NTCSensor(NTCSensorArgs args) : RefVoltCircuit{args.RefVoltCircuit}, ResAt25DegCel{args.ResAt25DegCelInOhm}, beta{args.beta} {

}

float NTCSensor::GetTemperatureInCelsius(float voltageIn) const {
    return GetTemperatureInKelvin(voltageIn) - OffsetCelsiusKelvin;
}

float NTCSensor::GetTemperatureInKelvin(float voltageIn) const {
    // Formula inspired from texas instrument: https://www.ti.com/lit/an/sbaa338a/sbaa338a.pdf?ts=1725297395650&ref_url=https%253A%252F%252Fwww.google.com%252F
    return 1.0f / ((1.0f / TempAt25CelInKel) + (1.0f / beta) * std::log(
            (ResVoltDivInOhm * voltageIn) / (ResAt25DegCel * (RefVoltCircuit - voltageIn))));
}
