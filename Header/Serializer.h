#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include "SerializeMemory.h"

class CSerializer
{
public:
	CSerializer() {};
	~CSerializer() {};
	
	virtual void Serialize()=0;

private:
	CSerializeMemory m_memory;
};

#endif // !__SERIALIZER_H__
