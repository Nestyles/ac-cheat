#pragma once

#include <ostream>

class Vector3 {
public:
    float x, y, z;
    friend std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
};

class Player
{
public:
    char pad_0000[4]; //0x0000
    Vector3 head_pos; //0x0004
    char pad_0010[24]; //0x0010
    Vector3 feet_pos; //0x0028
    float yaw; //0x0034
    float pitch; //0x0038
    char pad_003C[176]; //0x003C
    int health; //0x00EC
    int32_t armor; //0x00F0
    char pad_00F4[156]; //0x00F4
}; //Size: 0x0190
