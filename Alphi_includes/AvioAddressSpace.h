#include "AlphiBoard.h"
#include "AddrSpace.h"

class AvioAddressSpace: public AddrSpace
{
public:
	inline AvioAddressSpace(const char *asName, void* baseAddr, size_t w)
	{
		strncpy(name, asName, nameLength);
		base = (uint8_t*)baseAddr;
		length = w;
	}
	inline size_t		getLength()
      { return length; }

	inline const char *		getName()
	{
		return name;
	}

	inline PCIeMini_status	writeU8(size_t offset, uint8_t val)
    {
		base[offset] = val;
		return 0;
    }
	inline PCIeMini_status	writeU16(size_t offset, uint16_t val)
    {
		*(uint16_t*)(base + offset) = val;
		return 0;
    }
	inline PCIeMini_status	writeU32(size_t offset, uint32_t val)
    {
		*(uint32_t*)(base + offset) = val;
		return 0;
    }
	inline uint8_t		readU8(size_t offset)
      { return base[offset]; }

	inline uint16_t		readU16(size_t offset)
      { return *(uint16_t*)(base + offset); }
	inline uint32_t		readU32(size_t offset)
      { return *(uint32_t*)(base + offset); }
	inline const char *		toString()
      { return "uninitialized"; }
private:
	size_t length;
	uint8_t *base;
	static const int nameLength = 20;
	char name[nameLength];
};

