/*
Will Merges
*/

#ifndef RF_PACKET_H
#define RF_PACKET_H

#ifdef __cplusplus
#include <stdbool.h>
typedef bool _Bool;
extern "C" {
#endif

#include <sys/types.h>
#include <stdint.h>

// define DEBUG in order to enable print statements
// ex) gcc rf_protocol.c -D DEBUG

/*  Set the start byte of the packet to represent endianness of the system
*   carrot points to direction read bytes in (most to least sig.)
*   > is big endian (read from l to r most to least significant)
*   < is little endian
*   ? is unknown (this hopefully never occurs)
*/
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define START_BYTE 0x3E // >
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define START_BYTE 0x3C // <
#else // unknow endianness
    #define START_BYTE 0x3F // ?
#endif

// TODO add support for translating packet from one endianness to another
// ex) if received bytes from a big endian machine the data in the packet
// would all be in reversed byte order
// use the C "swab" functions from unistd.h
// perhaps only include translation functions and leave the packet data as is
// For this project it does not matter (ARM (teensy) and x86 (ground station) are both little endian)

// this enum needs to be reordered if packet structure changes
typedef enum {ALT, LAT, LONG, ALTGPS, A200G, A16G, MAG16G, PITCH, \
              ROLL, UPTIME, TIMEACCEL, TEMP1, TEMP2, CHARGES, CONTINUITY} update_bit_pos;

// no longer necessary, not hiding struct implementation (moved to h file)
//#ifndef RF_DATA_PACKET
//struct rf_data_s {};
//#endif

#pragma pack(1)
struct rf_data_s {
    uint8_t start_byte; //0
    uint16_t update_mask : 14; //1-2
    float alt; //3-6
    float lat; //7-10
    float lon; //11-14
    float alt_gps; //15-18
    float x200g; //19-22
    float y200g; //23-26
    float z200g; //27-30
    float x16g; //31-34
    float y16g; //35-38
    float z16g; //39-42
    float x16mag; //43-46
    float y16mag; //47-50
    float z16mag; //50-53
    float pitch; //54-57
    float roll; //58-61
    uint32_t uptime; //62-65
    uint32_t time_since_accel; //66-69
    float temp1; //70-73
    float temp2; //74-77
    // potentially move this to front to pack better, not divisible by 4, doesnt matter currently
    // (bytes 78-81)
    uint8_t charges : 4; //deployed charges (0-3)
    uint8_t continuity : 4; //continuity of charges (0-3)
};
// each struct member has a bit in update_mask
// any data with an xyz only has one bit however
// assumed all xyz data is updated each time new data is added
// ex. alt is bit 0 (least significant)
// if bit is a 1, the data was updated, if it's a 0 it was not


union rf_data_u {
    struct rf_data_s data;
    uint8_t serialized[sizeof(struct rf_data_s)];
};

typedef union rf_data_u* rf_data;
extern size_t packet_size;

// general functions
rf_data create_packet();
void destroy_packet(rf_data);
rf_data clear_packet(rf_data);
size_t reduce_packet(rf_data);
rf_data decompress_packet(rf_data);
void cleanup_packet_lib();
_Bool isLittleEndian(rf_data);
_Bool isBigEndian(rf_data);

// functions that add data to packet
void add_alt(rf_data, float alt);
void add_lat(rf_data, float lat);
void add_long(rf_data, float lon);
void add_alt_gps(rf_data, float alt);
void add_200g_accel(rf_data, float x, float y, float z);
void add_16g_accel(rf_data, float x, float y, float z);
void add_16g_mag(rf_data, float x, float y, float z);
void add_pitch(rf_data, float pitch);
void add_roll(rf_data, float roll);
void add_uptime(rf_data, uint32_t seconds);
void add_time_since_accel(rf_data, uint32_t seconds);
void add_temp1(rf_data, float temp);
void add_temp2(rf_data, float temp);
void set_charge(rf_data, int charge, _Bool active); //charges ordered 0-3
void set_continuity(rf_data, int charge, _Bool has_continuity);

// functions that edit the update field of the packet
void set_alt_change(rf_data);
void set_lat_change(rf_data);
void set_long_change(rf_data);
void set_altgps_change(rf_data);
void set_200gaccel_change(rf_data);
void set_16gaccel_change(rf_data);
void set_16gmag_change(rf_data);
void set_pitch_change(rf_data);
void set_roll_change(rf_data);
void set_uptime_change(rf_data);
void set_uptimeaccel_change(rf_data);
void set_temp1_change(rf_data);
void set_temp2_change(rf_data);

// functions that get data from packet
float get_alt(rf_data);
float get_lat(rf_data);
float get_long(rf_data);
float get_alt_gps(rf_data);
float get_200g_x(rf_data);
float get_200g_y(rf_data);
float get_200g_z(rf_data);
float get_16g_x(rf_data);
float get_16g_y(rf_data);
float get_16g_z(rf_data);
float get_16gmag_x(rf_data);
float get_16gmag_y(rf_data);
float get_16gmag_z(rf_data);
float get_pitch(rf_data);
float get_roll(rf_data);
int get_uptime(rf_data);
int get_time_since_accel(rf_data);
float get_temp1(rf_data);
float get_temp2(rf_data);
_Bool get_charge1(rf_data);
_Bool get_charge2(rf_data);
_Bool get_charge3(rf_data);
_Bool get_charge4(rf_data);
_Bool get_charge(rf_data, int charge);
_Bool get_continuity1(rf_data);
_Bool get_continuity2(rf_data);
_Bool get_continuity3(rf_data);
_Bool get_continuity4(rf_data);
_Bool get_continuity(rf_data, int charge);

//debug function
#ifdef DEBUG
void print_packet(const rf_data);
#endif

#ifdef __cplusplus
}
#endif

#endif
