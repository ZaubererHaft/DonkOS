
#ifndef TEST_NTCSENSOR_H
#define TEST_NTCSENSOR_H

struct NTCSensorArgs {
    float ResAt25DegCelInOhm;
    float beta;
};

class NTCSensor {
public:
    static constexpr float OffsetCelsiusKelvin = 273.15f;
    static constexpr float RefVoltCircuit = 3.33f;
    static constexpr float ResVoltDivInOhm = 10000.0f;
    static constexpr float TempAt25CelInKel = 25 + OffsetCelsiusKelvin;

    explicit NTCSensor(NTCSensorArgs args);

    float GetTemperatureInCelsius(float voltageIn) const;

    float GetTemperatureInKelvin(float voltageIn) const;

private:
    float ResAt25DegCel;
    float beta;
};


#endif //TEST_NTCSENSOR_H
