#include "mcp9600.h" 

CMcp9600::CMcp9600(I2C_InterfaceId id, I2C_DeviceAddress address) 
	: _id(id)
	, _address(address)
{

}

static bool write8(int fd, I2C_DeviceAddress address, uint8_t reg, uint8_t value)
{
	uint8_t valueToWrite[2];
	valueToWrite[0] = reg;
	valueToWrite[1] = value;

	ssize_t transferredBytes = I2CMaster_Write(fd, address, valueToWrite, sizeof(valueToWrite));
	if (!CMcp9600::CheckTransferSize("I2CMaster_Write", sizeof(valueToWrite), transferredBytes))
	{
		Log_Debug("Write8 Failed\n");
		return false;
	}
	return true;
}

static uint8_t read8(int fd, I2C_DeviceAddress address, uint8_t reg)
{
	uint8_t value = 0xFF;
	ssize_t transferredBytes = I2CMaster_WriteThenRead(fd, address, &reg, sizeof(reg), &value, sizeof(value));
	if (!CMcp9600::CheckTransferSize("I2CMaster_Read", sizeof(reg) + sizeof(value), transferredBytes))
	{
		Log_Debug("Read8 Failed");
	}
	return value;
}

/*
static uint16_t read16(int fd, I2C_DeviceAddress address, uint8_t reg)
{
	uint16_t value = 0xFFFF;
	ssize_t transferredBytes = I2CMaster_WriteThenRead(fd, address, &reg, sizeof(reg), (uint8_t *)&value, sizeof(value));
	CMcp9600::CheckTransferSize("I2CMaster_Read", sizeof(reg) + sizeof(value), transferredBytes);
	return value;
}

*/

bool CMcp9600::mcp9600_begin() {
	_fd = I2CMaster_Open(_id);
	if (_fd < 0)
	{
		Log_Debug("ERROR: I2CMaster_Open: errno=%d (%s)\n", errno, strerror(errno), errno);
		return false;
	}
	int result = I2CMaster_SetBusSpeed(_fd, I2C_BUS_SPEED_STANDARD);
	if (result != 0) {
		Log_Debug("ERROR: I2CMaster_SetBusSpeed: errno=%d (%s)\n", errno, strerror(errno));
		return false;
	}
	result = I2CMaster_SetTimeout(_fd, 100);
	if (result != 0) {
		Log_Debug("ERROR: I2CMaster_SetTimeout: errno=%d (%s)\n", errno, strerror(errno));
		return false;
	}
	return true;
}

int CMcp9600::testTermocouple()
{
	// Test Device Id
	uint8_t expectedDeviceId = 0x40;
	uint8_t actualDeviceId;

	actualDeviceId = read8(_fd, _address, MCP9600_REG_DEVICEID);
	Log_Debug("Actual Value=%d\n", actualDeviceId);

	if (actualDeviceId != expectedDeviceId) {
		Log_Debug("ERROR: Unexpected value.\n");
		return -1;
	}

	// Test Writing to the thermocouple the Thermocouple
	uint8_t valueToWrite = 0x77;
	uint8_t valueReadBack;

	if (write8(_fd, _address, MCP9600_REG_SENSOR_CONFIG, valueToWrite)) 
	{
		valueReadBack = read8(_fd, _address, MCP9600_REG_SENSOR_CONFIG);
		Log_Debug("Value Read Back=%d\n", valueReadBack);
	}
	else
	{
		Log_Debug("ERROR: Reading back the value\n");
		return -1;
	}
	return 1;
}

bool CMcp9600::setThermocoupleType(MCP9600_TYPE type)
{	
	int typevalue = static_cast<int>(type) << 4; 
	uint8_t valueToWrite = static_cast<uint8_t>(typevalue);
	(void)valueToWrite; 
	return false; 
}

MCP9600_TYPE CMcp9600::getThermocoupleType()
{
	return  MCP9600_TYPE_K; 
}

void CMcp9600::setFilterBits(uint16_t bits) {

}

bool CMcp9600::getFilterBits(uint16_t* bits)
{
	return false; 
}

void CMcp9600::setAdcResolution(MCP9600_ADC_RES res)
{
	
}

MCP9600_ADC_RES CMcp9600::getAdcResolution()
{
	return MCP9600_ADC_RES_18;
}

bool CMcp9600::CheckTransferSize(const char* desc, size_t expectedBytes, ssize_t actualBytes) {
	if (actualBytes < 0) {
		Log_Debug("ERROR: %s: errno=%d (%s)\n", desc, errno, strerror(errno));
		return false;
	}
	if (actualBytes != (ssize_t)expectedBytes) {
		Log_Debug("ERROR: %s: transferred %zd bytes; expected %zd\n", desc, actualBytes,
			expectedBytes);
		return false;
	}
	return true;
}

float CMcp9600::getTemprature()
{
	return 0.0f; 
}

CMcp9600::~CMcp9600()
{
	close(_fd);
}
