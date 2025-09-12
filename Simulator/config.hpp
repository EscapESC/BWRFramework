#pragma once

//Simulation performance config
constexpr int MAX_TPS_REACTOR = 1000;
constexpr int MAX_TPS_COOLANT = 1000;

//Reactor config
constexpr int REACTOR_MAX_THERMAL_POWER = 3200; //MW
constexpr int REACTOR_MAX_ELETRIC_POWER = 1000; //MW

//Core shape config
constexpr int REACTOR_CORE_SIZE = 8;
constexpr int REACTOR_CORE_HEIGHT = 10;
constexpr bool REACTOR_CORE_CIRCLE = true;

//Simulation neutron config
constexpr long long int MAXIMUM_NEUTRON_COUNT = 100000000000;
constexpr int IDLE_NEUTRON_COUNT = 1000;

//PUMPS
constexpr int NUM_OF_RC_PUMP = 2;
constexpr int NUM_OF_FW_PUMP = 2;
