#include <stdint.h>
#include <string.h>

#include "app_error.h"
#include "nrf_log.h"

#include "ble_my.h"

static uint32_t add_data_characteristic(ble_my_profile_t * p_profile, uint8_t * data)
{
	uint32_t err_code;

	ble_my_service_t * service = &p_profile->service;

	ble_uuid_t ble_uuid;

	ble_gatts_char_md_t char_md;
	ble_gatts_attr_md_t attr_md;
	ble_gatts_attr_t    attr_char_value;

	memset(&char_md, 0, sizeof(char_md));
	memset(&attr_md, 0, sizeof(attr_md));
	memset(&attr_char_value, 0, sizeof(attr_char_value));

	char_md.char_props.read   = 1;
	char_md.char_props.write  = 0;
	char_md.char_props.notify = 0;

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
	attr_md.vloc       = BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth    = 0;
	attr_md.wr_auth    = 0;
	attr_md.vlen       = 1;

	ble_uuid.type = p_profile->uuid_type;
	ble_uuid.uuid = BLE_UUID_MY_SERVICE_CHAR;

	attr_char_value.p_uuid    = &ble_uuid;
	attr_char_value.p_attr_md = &attr_md;
	attr_char_value.init_len  = sizeof(uint8_t);
	attr_char_value.init_offs = 0;
	attr_char_value.max_len   = 256;
	attr_char_value.p_value   = data;

	err_code = sd_ble_gatts_characteristic_add(
		service->service_handle,
		&char_md, &attr_char_value,
		&service->char_handles
	);
	VERIFY_SUCCESS(err_code);

	NRF_LOG_DEBUG("Characteristic value handle: 0x%04x", service->char_handles.value_handle);
	NRF_LOG_DEBUG("Characteristic user desc handle: 0x%04x\n", service->char_handles.user_desc_handle);

	return NRF_SUCCESS;
}


uint32_t update_data_characteristic(ble_my_profile_t * p_profile, uint16_t new_size) {
	uint32_t err_code;

	if (p_profile == NULL)
	{
		return NRF_ERROR_NULL;
	}

	ble_gatts_value_t value_md = {
		.len = new_size,
		.offset = 0,
		// passing NULL is valid if only length needs to be updated according to
		// https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v2.0.0%2Fstructble__gatts__value__t.html
		// given that we don't replace the buffer, just modify it, this should be correct?
		.p_value = NULL
	};
	err_code = sd_ble_gatts_value_set(
		BLE_CONN_HANDLE_INVALID,
		p_profile->service.char_handles.value_handle,
		&value_md
	);
	VERIFY_SUCCESS(err_code);

	return NRF_SUCCESS;
}

uint32_t my_profile_init(ble_my_profile_t * p_profile, uint8_t * buffer)
{
	if (p_profile == NULL) {
		return NRF_ERROR_NULL;
	}

	uint32_t err_code;

	p_profile->buffer = buffer;

	ble_uuid128_t base_uuid = { BLE_UUID_MY_BASE };

	err_code = sd_ble_uuid_vs_add(&base_uuid, &p_profile->uuid_type);
	VERIFY_SUCCESS(err_code);

	// Create a Service
	p_profile->service.uuid.type = p_profile->uuid_type;
	p_profile->service.uuid.uuid = BLE_UUID_MY_SERVICE;

	err_code = sd_ble_gatts_service_add(
			BLE_GATTS_SRVC_TYPE_PRIMARY,
			&p_profile->service.uuid,
			&p_profile->service.service_handle
			);
	VERIFY_SUCCESS(err_code);

	NRF_LOG_DEBUG("Service UUID: 0x%04x", p_profile->service.uuid.uuid);
	NRF_LOG_DEBUG("Service UUID type: 0x%02x", p_profile->service.uuid.type);
	NRF_LOG_DEBUG("Service handle: 0x%04x\n", p_profile->service.service_handle);

	err_code = add_data_characteristic(p_profile, buffer);
	VERIFY_SUCCESS(err_code);

	return NRF_SUCCESS;
}
