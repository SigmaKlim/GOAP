#pragma once
#include "World.h"
#include "../Navigation/Navigator.h"

struct MyWsData : WsUserData
{
    MyWsData() = default;
    MyWsData(const MyWsData& other)
    {
        location = other.location;
        isLocationValid = other.isLocationValid;
    };
    Location location;
    bool isLocationValid = false;   //whether location stored correctly denotes agent's planned location
                                    //(SPOILER: actions like search make location in valid:
                                    //we can't determine in advance what its value is going got be)
};
