/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include "espressif/esp_sta.h" /* Wifi Station */
#include "espressif/esp_wifi.h"
#include "lwip/lwip/netif.h"
#include "uart.h"

extern void user_task();

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
 *******************************************************************************/
uint32 user_rf_cal_sector_set(void) {
	flash_size_map size_map = system_get_flash_size_map();
	uint32 rf_cal_sec = 0;

	switch (size_map) {
		case FLASH_SIZE_4M_MAP_256_256:
			rf_cal_sec = 128 - 5;
			break;

		case FLASH_SIZE_8M_MAP_512_512:
			rf_cal_sec = 256 - 5;
			break;

		case FLASH_SIZE_16M_MAP_512_512:
		case FLASH_SIZE_16M_MAP_1024_1024:
			rf_cal_sec = 512 - 5;
			break;

		case FLASH_SIZE_32M_MAP_512_512:
		case FLASH_SIZE_32M_MAP_1024_1024:
			rf_cal_sec = 1024 - 5;
			break;
		case FLASH_SIZE_64M_MAP_1024_1024:
			rf_cal_sec = 2048 - 5;
			break;
		case FLASH_SIZE_128M_MAP_1024_1024:
			rf_cal_sec = 4096 - 5;
			break;
		default:
			rf_cal_sec = 0;
			break;
	}

	return rf_cal_sec;
}

void manual_netif_setup() {
    struct netif *interface = netif_find("en0"); 

    bool is_interface_up = netif_is_up(interface);

    if (!is_interface_up) {
        netif_set_up(interface);
    }

    netif_create_ip6_linklocal_address(interface, 0);
}

void wifi_event_cb(System_Event_t *event) {
    if (event == NULL) {
        os_printf("event is null\n");
        return;
    }
    os_printf("event id: %d\n", event->event_id);

    switch(event->event_id) {
        case EVENT_STAMODE_GOT_IP:
            os_printf("Got Ip. Starting user task\n");
            //user_task();
            break;
        case EVENT_STAMODE_CONNECTED:
            os_printf("Connected to AP\n");
            manual_netif_setup();
            user_task();
            break;
        default:
            os_printf("Did not receive an ip\n");
            break;
    }
}

const char *SSID = "wireless-n";
const char *PASSWORD = "esp8266pw";

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void user_init(void) {
    // Initialize uart
    uart_init_new();
	printf("SDK version:%s\n", system_get_sdk_version());

    // Wifi related settings
    wifi_set_opmode(STATION_MODE);
    struct station_config config;
    (void) memset(&config, 0, sizeof(config));
    sprintf((char *)config.ssid, SSID);
    sprintf((char *)config.password, PASSWORD);

    wifi_station_set_config(&config);

    wifi_set_event_handler_cb(wifi_event_cb);
}

