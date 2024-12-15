#include "Plane.h"
#include "AP_BattMonitor/AP_BattMonitor.h"
#include "AP_HAL/AP_HAL.h"

#define HEAT_BATTERY_BELOW_TEMP_C 28.0f
#define RELAY_INDEX 0

#ifdef USERHOOK_INIT
void Plane::userhook_init()
{
    // put your initialisation code here
    // this will be called once at start-up
}
#endif

#ifdef USERHOOK_FASTLOOP
void Plane::userhook_FastLoop()
{
    // put your 100Hz code here
}
#endif

#ifdef USERHOOK_50HZLOOP
void Plane::userhook_50Hz()
{
    // put your 50Hz code here
}
#endif

#ifdef USERHOOK_MEDIUMLOOP
void Plane::userhook_MediumLoop()
{
    // put your 10Hz code here
}
#endif

#ifdef USERHOOK_SLOWLOOP
void Plane::userhook_SlowLoop()
{

    float battery_temperature_c = -300.0f;
    bool result = battery.get_temperature(battery_temperature_c);

    // OH crap, seems like get_temperature still returns true when I disconnect the sensor. 
    if (!result || battery_temperature_c < -100.0f){
        GCS_SEND_TEXT(MAV_SEVERITY_INFO, "Failed to get batt temp (%f), disabling heater", battery_temperature_c);
        relay.off(RELAY_INDEX);
    }
    else if (battery_temperature_c < HEAT_BATTERY_BELOW_TEMP_C && !relay.get(RELAY_INDEX))
    {
        GCS_SEND_TEXT(MAV_SEVERITY_INFO, "batt temp is low at (%f°C), heat on", battery_temperature_c);
        // Turning the relay on turns on the heater.
        relay.on(RELAY_INDEX);
    }
    else if (battery_temperature_c > HEAT_BATTERY_BELOW_TEMP_C && relay.get(RELAY_INDEX))
    {
        GCS_SEND_TEXT(MAV_SEVERITY_INFO, "batt temp is high at (%f°C), heat off", battery_temperature_c);
        relay.off(RELAY_INDEX);
    }

}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Plane::userhook_SuperSlowLoop()
{
    // put your 1Hz code here
}
#endif

// #ifdef USERHOOK_AUXSWITCH
// void Plane::userhook_auxSwitch1(const RC_Channel::AuxSwitchPos ch_flag)
// {
//     // put your aux switch #1 handler here (CHx_OPT = 47)
// }

// void Plane::userhook_auxSwitch2(const RC_Channel::AuxSwitchPos ch_flag)
// {
//     // put your aux switch #2 handler here (CHx_OPT = 48)
// }

// void Plane::userhook_auxSwitch3(const RC_Channel::AuxSwitchPos ch_flag)
// {
//     // put your aux switch #3 handler here (CHx_OPT = 49)
// }
// #endif
