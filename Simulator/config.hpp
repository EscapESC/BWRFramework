#pragma once

constexpr int ATMOSPHERIC_PRESSURE = 101325;

//Simulation performance config
constexpr int MAX_TPS_REACTOR = 60;
constexpr int MAX_TPS_COOLANT = 60;

//Reactor config
constexpr int REACTOR_MAX_THERMAL_POWER = 3200; //MW
constexpr int REACTOR_MAX_ELETRIC_POWER = 1000; //MW

//Core shape config
constexpr int REACTOR_CORE_SIZE = 8;
constexpr int REACTOR_CORE_HEIGHT = 10;
constexpr bool REACTOR_CORE_CIRCLE = true;
constexpr int REACTOR_VOLUME = 120;//m³

//Simulation neutron config
constexpr long long int MAXIMUM_NEUTRON_COUNT = 1000000000000;
constexpr int IDLE_NEUTRON_COUNT = 1000;

//PUMPS
constexpr int NUM_OF_RC_PUMP = 2;
constexpr int NUM_OF_FW_PUMP = 2;

//Condenser
constexpr int CONDNENSER_VOLUME = 7000;
constexpr int CONDENSER_AIR_LEKAGE = 9;//kgs/s
constexpr int NUM_OF_CONDENSER_RC_PUMP = 2;
constexpr int NUM_OF_CONDENSER_CAR = 2;
constexpr int NUM_OF_CONDENSER_SJAE = 2;

//CAR
constexpr int CAR_RATED_AIR_REMOVAL = 40;//kgs/s

//SJAE
constexpr int SJAE_RATED_AIR_REMOVAL = 10;//kgs/s 