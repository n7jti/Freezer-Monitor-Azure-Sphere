#include "mcp9600.h" 

CMcp9600::CMcp9600(I2C_InterfaceId id, I2C_DeviceAddress address) 
	: _id(id)
	, _address(address)
{

}

void CMcp9600::setThermocoupleType(MCP9600_TYPE type)
{

}

MCP9600_TYPE CMcp9600::getThermocoupleType()
{
	return  0; 
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
	return 0;
}

float CMcp9600::getTemprature()
{
	return 0.0f; 
}
