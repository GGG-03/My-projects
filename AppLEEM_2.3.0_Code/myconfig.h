#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <QString>

const double gravity = 9.81;

// Vector size
const int maxTCouples = 14;
const int nBenchData = 3;
const int nLaunchData = 2;
const int nIgnitorsData = 3;

enum AppModes
{
    defaultMode = 0,
    benchMode,
    rocketMode,
    ignitorsMode
};

enum fileErrorCode
{
    oldFile = 0,
    oldFileOpen,
    newFileSave
};

namespace GeneralSymb
{
    enum Orders
    {
        deviceConnected = 1,
    };

    enum Devices
    {
        benchDevice = 1,
        rocketDevice = 2
    };
}

namespace BenchSymb
{
    enum Orders
    {
        arduinoConnected = 1,
        enableTransducer,
        disableTransducer,
        enableHydroStatic,
        disableHydroStatic,
        startPerforming = 8,
        stopPerforming,
        startIgnition = 11,
        stopIgnition,
        tare,
        errorSD = 20,
        errorSDFile,
        errorFlash,
        errorFlashFile,
    };

    enum Phases
    {
        standby = 0,
        tared,
        ready,
        ignited,
        finished
    };

    enum DataType
    {
        timeInS = 0,
        thrustInKg,
        pressureInBar,
        THTemperature
    };
}


namespace RocketSymb
{
    enum Orders
    {
        device_connected = 1,
        ping,
        tare,
        change_phase,
        alarm_on,
        alarm_off,
        opening_drogue,
        opening_main,
        start_to_debug = 20,
        debug_to_standby,
        standby_to_armed,
        armed_to_ready,
        ready_to_boost,
        boost_to_apogee,
        apogee_to_drogue_descent,
        drogue_descent_to_main_deployed,
        main_deployed_to_main_descent,
        main_descent_to_recovery,
        error_SD = 30
    };

    enum Phases
    {
        debugging = 0,
        standby,
        armed,
        ready,
        boost,
        apogee,
        drogue_descent,
        main_deployed,
        main_descent,
        recovery
    };

    enum Gauges
    {
        acceleration = 0,
        speed,
        rotX,
        rotY,
        rotZ,
        altitude,
        maxAltitude,
        time_seconds,
        status
    };

    enum Raw
    {
        raw_time,
        raw_phase,
        raw_altitude,
        raw_bat_volt,
        raw_bat_amp,
        raw_pyro1_amp,
        raw_pyro2_amp,
        raw_teensy_temp,
        raw_stm32_temp,
        raw_acc_x,
        raw_acc_y,
        raw_acc_z,
        raw_yaw,
        raw_pitch,
        raw_roll,
        raw_speed_x,
        raw_speed_y,
        raw_speed_z,
        raw_lat,
        raw_long
    };

    const QString PhasesName[] =  {"Debug", "Standby", "Armed", "Ready", "Rising", "Apogee", "Drogue Descent", "Main Deployted", "Main Descent", "Recovery"};
}

namespace PowerSymb
{
    enum Gauges
    {
        Bat_Amp,
        _5V_Amp,
        _3V3_Amp,
        Pyro1_Amp,
        Pyro2_Amp,
        Bat_Volt,
        _5V_Volt,
        _3V3_Volt,
        Pyro1_Volt,
        Pyro2_Volt
    };
}

#endif // MYCONFIG_H
