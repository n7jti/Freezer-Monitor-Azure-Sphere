/* Copyright (c) 2019 Alan Ludwig. All rights reserved.
   Licensed under the MIT License.  See License.txt */
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>

// Constants from MCP9600 datasheet:  http://ww1.microchip.com/downloads/en/DeviceDoc/MCP9600-Family-Data-Sheet-20005426E.pdf
#define MCP9600_ADDR 0x67

#define MCP9600_REG_HOT_JUNCTION  0x00
#define MCP9600_REG_TEMP_DELTA 0x01
#define MCP9600_REG_COLD_JUNCTION 0x02
#define MCP9600_REG_RAW_ADC 0x03
#define MCP9600_REG_STATUS 0x04
#define MCP9600_REG_SENSOR_CONFIG 0x05
#define MCP9600_REG_DEVICE_CONFIG 0x06
#define MCP9600_REG_ALERT_1_CONFIG 0x08
#define MCP9600_REG_ALERT_2_CONFIG 0x09
#define MCP9600_REG_ALERT_3_CONFIG 0x0A
#define MCP9600_REG_ALERT_4_CONFIG 0x0B
#define MCP9600_REG_ALERT_1_HYST 0x0C
#define MCP9600_REG_ALERT_2_HYST 0x0D
#define MCP9600_REG_ALERT_3_HYST 0x0E
#define MCP9600_REG_ALERT_4_HYST 0x0F
#define MCP9600_REG_TALERT_1 0x10
#define MCP9600_REG_TALERT_2 0x11
#define MCP9600_REG_TALERT_3 0x12
#define MCP9600_REG_TALERT_4 0x13
#define MCP9600_REG_DEVICEID 0x20

#define	MCP9600_TYPE_K 0x0
#define MCP9600_TYPE_J 0x1
#define	MCP9600_TYPE_T 0x2
#define	MCP9600_TYPE_N 0x3
#define	MCP9600_TYPE_S 0x4
#define	MCP9600_TYPE_E 0x5
#define	MCP9600_TYPE_B 0x6
#define	MCP9600_TYPE_R 0x7

#define	MCP9600_ADC_RES_18 0
#define	MCP9600_ADC_RES_16 1
#define	MCP9600_ADC_RES_14 2
#define	MCP9600_ADC_RES_12 3

bool mcp9600_begin(uint16_t address);
bool setThermocoupleType(uint16_t address, uint16_t type);
bool getThermocoupleType(uint16_t address, uint16_t *type);
bool setFilterBits(uint16_t address, uint16_t bits);
bool getFilterBits(uint16_t address, uint16_t *bits);
bool setAdcResolution(uint16_t address, uint16_t type);
bool getAdcResolution(uint16_t address, uint16_t *type);
bool getTemprature(uint16_t address, float *temp);
