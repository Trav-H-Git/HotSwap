#include "memory.h"

uint32_t Memory::m_alloc_count = 0u;
uint32_t Memory::m_bytes_uses = 0u;

void* Memory::Allocate(size_t _size)
{
	bool pad = false;
#ifdef XEN_CONFIG_DEBUG
	pad = true;
#endif
	
	uint64_t* ptr = (uint64_t*)malloc(_size + sizeof(uint64_t));
	if (ptr)
	{
		*ptr = _size;
	}
	else
	{
		//print error
		return nullptr;
	}
	m_alloc_count++;
	m_bytes_uses += _size;

	return (void*)(++ptr);
}

void* Memory::Callocate(size_t _size)
{
	bool pad = false;
#ifdef XEN_CONFIG_DEBUG
	pad = true;
#endif

	uint64_t* ptr = (uint64_t*)calloc(1, _size + sizeof(uint64_t));
	if (ptr)
	{
		*ptr = _size;
	}
	else
	{
		//print error
		return nullptr;
	}
	m_alloc_count++;
	m_bytes_uses += _size;

	return (void*)(++ptr);
}

void Memory::DeAllocate(void* _ptr)
{
	if (_ptr == nullptr)
	{
		return;
	}

 	uint8_t* ptr = (uint8_t*)_ptr;
	if (!ptr)
	{
		//print error
		return;
	}

	m_alloc_count--;
	uint64_t* mem = (uint64_t*)ptr;
	m_bytes_uses -= *(--mem);
	free((void*)mem);
	_ptr = nullptr;
}

void* operator new(size_t _size, const char* _str)
{
	return Memory::Allocate(_size);
}
