#include "mcp9600.h" 

CMcp9600::CMcp9600(I2C_InterfaceId id, I2C_DeviceAddress address) 
	: _id(id)
	, _address(address)
{
	mcp9600_begin();
}

void CMcp9600::mcp9600_begin() {
	_fd = I2CMaster_Open(_id);
	if (_fd == -1)
	{
		Log_Debug(
			"ERROR: I2CMaster_Open: errno=%d (%s)\n", errno, strerror(errno), errno);
	}
	I2CMaster_SetBusSpeed(_fd, 100000);
}

bool CMcp9600::setThermocoupleType(MCP9600_TYPE type)
{
	uint8_t mcp9600Type = type;
	ssize_t transferedbytes = I2CMaster_Write(_fd, _address, &mcp9600Type, sizeof(mcp9600Type));
	return CheckTransferSize("I2CMaster_Write (mcp9600Type)", sizeof(mcp9600Type), transferedbytes);
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
