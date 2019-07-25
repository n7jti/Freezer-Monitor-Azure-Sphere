#include "mcp9600.h" 

CMcp9600::CMcp9600(I2C_InterfaceId id, I2C_DeviceAddress address) 
	: _id(id)
	, _address(address)
{

}

bool CMcp9600::write8(int fd, I2C_DeviceAddress address, uint8_t reg, uint8_t value)
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

uint8_t CMcp9600::read8(int fd, I2C_DeviceAddress address, uint8_t reg)
{
	uint8_t value = 0xFF;
	ssize_t transferredBytes = I2CMaster_WriteThenRead(fd, address, &reg, sizeof(reg), &value, sizeof(value));
	if (!CMcp9600::CheckTransferSize("I2CMaster_Read", sizeof(reg) + sizeof(value), transferredBytes))
	{
		Log_Debug("Read8 Failed\n");
	}
	return value;
}

uint16_t CMcp9600::read16(int fd, I2C_DeviceAddress address, uint8_t reg)
{
	uint8_t value[2];
	ssize_t transferredBytes = I2CMaster_WriteThenRead(fd, address, &reg, sizeof(reg), value, sizeof(value));
	if (!CMcp9600::CheckTransferSize("I2CMaster_Read", sizeof(reg) + sizeof(value), transferredBytes))
	{
		Log_Debug("Read16 Failed\n");
	}
	uint16_t result = 0;
	Log_Debug("Array 1: 0x%02x\n", value[0]);
	Log_Debug("Array 2: 0x%02x\n", value[1]);
	result = (value[0] << 8) | result;
	result = value[1] | result;
	return result;
}

/*
uint16_t CMcp9600::read16(int fd, I2C_DeviceAddress address, uint8_t reg)
{
	uint16_t value = 0xFFFF;
	ssize_t transferredBytes = I2CMaster_WriteThenRead(fd, address, &reg, sizeof(reg), reinterpret_cast<uint8_t *>(&value), sizeof(value));
	if (!CMcp9600::CheckTransferSize("I2CMaster_Read", sizeof(reg) + sizeof(value), transferredBytes))
	{
		Log_Debug("Read16 Failed\n");
	}
	return value;
}*/

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
	uint8_t reg = MCP9600_REG_SENSOR_CONFIG;
	uint8_t dataToWrite = type;
	if (!write8(_fd, _address, reg, dataToWrite))
	{
		return false;
	}
	return true; 
}

MCP9600_TYPE CMcp9600::getThermocoupleType()
{
	return  MCP9600_TYPE_K; 
}

bool CMcp9600::setFilterBits(uint8_t bits) {
	uint8_t reg = MCP9600_REG_SENSOR_CONFIG;
	uint8_t previousData = read8(_fd, _address, reg);
	uint8_t dataToWrite = (previousData&0xf0)|bits;
	if (!write8(_fd, _address, reg, dataToWrite))
	{
		return false;
	}
	return true;
}

uint8_t CMcp9600::getFilterBits() {
	return 0x4;
}

bool CMcp9600::setAdcResolution(MCP9600_ADC_RES res)
{
	uint8_t reg = MCP9600_REG_DEVICE_CONFIG;
	uint8_t previousData = read8(_fd, _address, reg);
	uint8_t dataToWrite = previousData | (res&0x60);
	if (!write8(_fd, _address, reg, dataToWrite))
	{
		return false;
	}
	return true;
}

MCP9600_ADC_RES CMcp9600::getAdcResolution()
{
	return MCP9600_ADC_RES_18;
}

// Get Temperature using Hot Junction Themperature Register
float CMcp9600::getTemprature()
{
	float result;
	uint8_t reg = MCP9600_REG_HOT_JUNCTION;
	uint16_t tempBits = read16(_fd, _address, reg);
	Log_Debug("Bits Returned from Hot Junction Register: 0x%02x\n", tempBits);
	/*
	if (tempBits & 0x8000)
	{
		result = (((tempBits >> 8) * 16.0) + ((tempBits & 0x00ff) / 16.0)) - 4096.0;
	}
	else 
	{
		result = ((tempBits >> 8) * 16.0) + ((tempBits & 0x00ff) / 16.0);
	}*/
		
	result = tempBits;
	result *= 0.0625;  // 0.0625*C per LSB!
	Log_Debug("Bits After Conversion to Celsius: %f\n", result);
	return result;
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

CMcp9600::~CMcp9600()
{
	close(_fd);
}
