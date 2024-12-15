#include "Plane.h"

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
    GCS_SEND_TEXT(MAV_SEVERITY_INFO, "slow loop");
    // put your 3.3Hz code here
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
