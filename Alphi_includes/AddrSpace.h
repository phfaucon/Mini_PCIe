#pragma once
#include "AlphiDll.h"
#include "AlphiErrorCodes.h"

class AddrSpace
{
public:
	inline AddrSpace(){}
	virtual size_t		getLength()
      { return 0; }
	virtual const char *		getName()
      { return "uninitialized"; }
	virtual PCIeMini_status	writeU8(size_t offset, uint8_t val)
      { return 0; }
	virtual PCIeMini_status	writeU16(size_t offset, uint16_t val)
      { return 0; }
	virtual PCIeMini_status	writeU32(size_t offset, uint32_t val)
      { return 0; }
	virtual uint8_t		readU8(size_t offset)
      { return 0; }
	virtual uint16_t		readU16(size_t offset)
      { return 0; }
	virtual uint32_t		readU32(size_t offset)
      { return 0; }
	virtual const char *		toString()
      { return "uninitialized"; }
};

extern const char szHelpAddressSpace[];
void AddressSpace(char *szParm);
