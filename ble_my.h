#ifndef BLE_MY_H
#define BLE_MY_H

#include <stdint.h>
#include <stdbool.h>

#include "ble.h"
#include "ble_srv_common.h"

#define BLE_UUID_MY_BASE {0xFF, 0x33, 0x32, 0xF0, \
		          0x7A, 0x35, 0x20, 0xA0, \
		          0x61, 0x42, 0x3D, 0xA3, \
		          0x00, 0x00, 0xDB, 0x9A}

#define BLE_UUID_MY_SERVICE 0x1800
#define BLE_UUID_MY_SERVICE_CHAR 0x1801

#define BLE_MY_PROFILE(_name) \
static ble_my_profile_t _name; \

typedef struct {
	uint16_t service_handle;
	ble_gatts_char_handles_t char_handles;
	ble_uuid_t uuid;
} ble_my_service_t;

typedef struct {
	uint8_t uuid_type;
	ble_my_service_t service;
	uint8_t * buffer;

} ble_my_profile_t;

uint32_t my_profile_init(ble_my_profile_t *, uint8_t *);

uint32_t update_data_characteristic(ble_my_profile_t * p_pib_profile, uint16_t new_size);

#endif /* end of include guard: BLE_MY_H */
