#pragma once
#include <math.h>

class PhysicsMath
{
private:

public:
    static double waterDensity(double temperature);
    static double calculateBoilingPoint(double pressurePa);
};

double PhysicsMath::waterDensity(double temperature) {
    double T = temperature;
    double density = -0.000004467711 * pow(temperature, 3)
                   - 0.000560288485 * pow(temperature, 2)
                   - 0.429148844451 * temperature
                   + 1010.035413387815;
    return std::max(density, 720.0); //kg/m³
}

double calculateBoilingPoint(double pressurePa) {

    if (pressurePa <= 0.1) pressurePa = 0.1;
    
    const double H_vap = 40650.0;   // Enthalpy of vaporization in J/mol
    const double R = 8.314;         // Gas constant in J/(mol·K)
    const double T1 = 373.15;       // Boiling point at 1 atm °Kelvin
    const double P1 = 101325.0;     // Standard pressure (1 atm) Pascals

    double lnP = std::log(pressurePa / P1);
    double invT2 = (1.0 / T1) - (R * lnP / H_vap);
    double T2 = 1.0 / invT2;

    return T2 - 273.15;
}

double condensationTemperature(double pressurePa) {
    // Antoine constants for water (valid roughly 1°C–100°C)
    const double A = 8.07131;
    const double B = 1730.63;
    const double C = 233.426;

    // Convert Pa to mmHg: 1 mmHg ≈ 133.322 Pa
    double P_mmHg = pressurePa / 133.322;

    // Inverse Antoine equation: T = B / (A - log10(P)) - C
    double T = B / (A - log10(P_mmHg)) - C;

    return T;
}