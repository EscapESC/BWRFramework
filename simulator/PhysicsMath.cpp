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
    double density = 999.83952 
                     + 16.945176 * T 
                     - 7.9870401e-3 * pow(T, 2)
                     - 46.170461e-6 * pow(T, 3)
                     + 105.56302e-9 * pow(T, 4)
                     - 280.54253e-12 * pow(T, 5);
    return density; //kg/m³
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