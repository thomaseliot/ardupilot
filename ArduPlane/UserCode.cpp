#include "Plane.h"
#include "AP_BattMonitor/AP_BattMonitor.h"
#include "AP_HAL/AP_HAL.h"

#define HEAT_BATTERY_BELOW_TEMP_C 0.0f
#define RELAY_INDEX 0

// Release the gripper above this altitude
// This is absolute height above ground level because it's the easiest to get... if we're launchign from higher levels and care about precision, we'll need to change this
#define RELEASE_ABOVE_M 100.0f
// Release the gripper if the altitude has decreased by this much
#define RELEASE_AFTER_FALL_M 33.0f
// Release the gripper after this time
#define RELEASE_AFTER_TIME_S 30
// Start the gripper release timer after this height
#define START_TIMER_AFTER_HEIGHT_M 30.0f


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

    // Battery Heater Loop

    // Seems like get_temperature still returns true when I disconnect the sensor, so also check for a wacky temperature.
    // TODO This should be a prefight failure, though
    if (!result || battery_temperature_c < -100.0f ){
        if(relay.get(RELAY_INDEX))
        {
            GCS_SEND_TEXT(MAV_SEVERITY_INFO, "Failed to get batt temp (%f), disabling heater", battery_temperature_c);
            relay.off(RELAY_INDEX);
        }
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

    // Gripper control loop
    // no need to close gripper once opened
    static float highest_height_m = -10000.0;
    static uint32_t release_at_ms = 0;
    static bool timer_enabled = false;
    // static uint32_t cnt = 0;

    uint32_t time_ms = AP_HAL::millis();
    Vector3f pos;
    
    if (plane.ahrs.get_relative_position_NED_home(pos))
    {
        float height_m = pos.z*-1;
        // if(!cnt)
        // {
        //     GCS_SEND_TEXT(MAV_SEVERITY_INFO, "height is %fm", height_m);
        // }
        // cnt = (cnt+1)%15;

        if(height_m > highest_height_m)
        {
            highest_height_m = height_m;
        }
        if ((height_m > START_TIMER_AFTER_HEIGHT_M) && !timer_enabled)
        {
            timer_enabled = true;
            release_at_ms = time_ms + RELEASE_AFTER_TIME_S*1000;
            GCS_SEND_TEXT(MAV_SEVERITY_INFO, "Countdown begun, current=%lu ms, release=%lu ms", time_ms, release_at_ms);
        }

        if ((height_m > RELEASE_ABOVE_M ) && !plane.g2.gripper.released())
        {
            GCS_SEND_TEXT(MAV_SEVERITY_INFO, "Release at %f m, max alt", height_m);
            plane.g2.gripper.release();
        }

        if (((highest_height_m - height_m) > RELEASE_AFTER_FALL_M ) && !plane.g2.gripper.released())
        {
            GCS_SEND_TEXT(MAV_SEVERITY_INFO, "Releasing gripper at %f m, height loss", height_m);
            plane.g2.gripper.release();
        }
    }

    if (timer_enabled && ((time_ms - release_at_ms) > 0) && !plane.g2.gripper.released())
    {
        GCS_SEND_TEXT(MAV_SEVERITY_INFO, "Pretending to release gripper at %lums, time", time_ms);
        // plane.g2.gripper.release();
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
