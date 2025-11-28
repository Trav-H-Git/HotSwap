#pragma once

#include "core/types/hash_func.h"

#include "core/memory/memory.h"

#include <stdint.h>
//#include <unordered_map>

template<typename Tkey, typename Tval>
class HashMap
{
public:
	//temp
	template<typename Tkey, typename Tval>
	struct Element
	{
		Tkey key;
		Tval value;
		Element<Tkey, Tval>* prev = nullptr;
		Element<Tkey, Tval>* next = nullptr;
		Element() {};
		Element(const Tkey& _key, const Tval& _val) :
			key(_key), value(_val) {};
	};

	inline HashMap() = default;
	
	virtual inline ~HashMap()
	{
		Clear();

		if (m_elements)
		{
			FREE_MEM(m_elements);
		}
		if (m_hashes)
		{
			FREE_MEM(m_hashes);
		}
	}

	inline Tval& operator[](const Tkey& _key)
	{
		return Get(_key);
	}

	inline bool Has(const Tkey& _key) const
	{
		uint32_t pos;
		return _TryGetPos(_key, pos);
	}

	inline Tval& Get(const Tkey& _key)
	{
		uint32_t hash_pos;
		if (!_TryGetPos(_key, hash_pos))
		{
			return _Insert(_key, Tval())->value;
		}
		return m_elements[hash_pos]->value;
	}

	inline void Clear()
	{
		if (m_elements == nullptr || m_elements_count == 0) {
			return;
		}

		uint32_t capacity = prime_capacity_table[m_capacity_idx];
		for (uint32_t i = 0; i < capacity; i++) {
			if (m_hashes[i] == EMPTY_HASH) 
			{
				continue;
			}

			m_hashes[i] = EMPTY_HASH;
			DELETE_OBJ(m_elements[i]);
			m_elements[i] = nullptr;
		}

		//tail_element = nullptr;
		//head_element = nullptr;
		m_elements_count = 0;
	}
private:
	int m_capacity_idx = 0;
	static constexpr int MIN_CAPACITY_IDX = 2;
	static constexpr uint32_t EMPTY_HASH = 0u;
	//static constexpr uint32_t DEF_SEED = 0x07F07C65;

	uint32_t* m_hashes = nullptr;

	Element<Tkey, Tval>** m_elements = nullptr;
	uint32_t m_elements_count = 0;
	static constexpr float MAX_CAPACITY_PERC = .75f;

	//std::unordered_map<Tkey, Tval> m_data;
	

	//end temp
	inline bool _TryGetPos(const Tkey& _key, uint32_t& _pos) const
	{
		if (m_elements == nullptr || m_elements_count == 0) {
			return false; // Failed lookups, no elements
		}

		uint32_t hash = _Hash(_key);
		uint32_t pos = FastMod(hash, m_capacity_idx);

		uint32_t probe_len = 0;
		while (true)
		{
			if (m_hashes[pos] == EMPTY_HASH)
			{
				return false;
			}
			if (probe_len > _GetProbeLength(pos, m_hashes[pos], m_capacity_idx))
			{
				return false;
			}
			if (m_hashes[pos] == hash)
			{
				_pos = pos;
				return true;
			}

			pos = FastMod((pos + 1), m_capacity_idx);
			probe_len++;
		}
	}

	inline uint32_t _Hash(const Tkey& _key) const
	{
		uint32 hash = Hashes::Hash(_key);
		if (hash == EMPTY_HASH)
		{
			return EMPTY_HASH + 1;
		}
		return hash;
	}

	inline void _FormMap(int _new_capacity_idx)
	{
		_new_capacity_idx = (_new_capacity_idx < MIN_CAPACITY_IDX) ? MIN_CAPACITY_IDX : _new_capacity_idx;

		//TODO assert idx inbounds

		uint32_t* old_hashes = m_hashes;
		Element<Tkey, Tval>** old_elements = m_elements;

		uint32_t old_capacity = prime_capacity_table[m_capacity_idx];
		uint32_t new_capacity = prime_capacity_table[_new_capacity_idx];
		m_capacity_idx = _new_capacity_idx;

		m_hashes = (uint32_t*)CALLOC_MEM(new_capacity * sizeof(uint32_t));
		m_elements = (Element<Tkey, Tval>**)CALLOC_MEM(new_capacity * sizeof(Element<Tkey, Tval>**));

		if (m_elements_count == 0)
		{
			return;
		}
		m_elements_count = 0;

		for (size_t i = 0; i < old_capacity; i++)
		{
			if (old_hashes[i] == EMPTY_HASH) {
				continue;
			}

			_InsertElement(old_hashes[i], old_elements[i]);
		}

		FREE_MEM(old_hashes);
		FREE_MEM(old_elements);
	}


	inline Element<Tkey, Tval>* _Insert(const Tkey& _key, const Tval& _val)
	{

		if (m_hashes == nullptr || m_elements == nullptr)
		{
			_FormMap(m_capacity_idx);
		}
		//get pos
		uint32_t pos;
		bool found = _TryGetPos(_key, pos);

		//if exists insert
		if (found)
		{
			m_elements[pos]->value = _val;
			return m_elements[pos];
		}
		else//else check capacity limit and adjust
		{
			if (m_elements_count + 1 > (MAX_CAPACITY_PERC * prime_capacity_table[m_capacity_idx]))
			{
				//increase capacity
				_FormMap(m_capacity_idx + 1);
			}
			//alloc new element
			//Element<Tkey, Tval>* new_element = new ("") Element<Tkey, Tval>(_key, _val);
			Element<Tkey, Tval>* new_element = NEW_OBJ_ARGS((Element<Tkey, Tval>), _key, _val);
			//Element<Tkey, Tval>* new_element = NewObject(new ("") (Element<Tkey, Tval>)(_key, _val));

			//define next/prev

			//insert element
			_InsertElement(_Hash(_key), new_element);
			return new_element;
		}

	}
	
	inline Element<Tkey,Tval>* _InsertElement(uint32_t _hash, Element<Tkey,Tval>* _element)
	{
		//const uint32_t capacity = prime_capacity_table[m_capacity_idx];
		//const uint64_t capacity_inv = prime_capacity_table_inverse[m_capacity_idx];
		uint32_t hash = _hash;
		Element<Tkey, Tval>* element = _element;
		uint32_t pos = FastMod(hash, m_capacity_idx);

		uint32_t current_probe_len = 0;
		while (true) {
			if (m_hashes[pos] == EMPTY_HASH)
			{
				m_hashes[pos] = hash;
				m_elements[pos] = element;
				m_elements_count++;
				return element;
			}

			// Not an empty slot, let's check the probing length of the existing one.
			uint32_t existing_probe_len = _GetProbeLength(pos, hash, m_capacity_idx);
			if (existing_probe_len < current_probe_len) {
				_Swap(hash, m_hashes[pos]);
				_Swap(element, m_elements[pos]);
				current_probe_len = existing_probe_len;
			}

			pos = FastMod((pos + 1), m_capacity_idx);
			current_probe_len++;
		}
	}

	

	template <typename T>
	inline void _Swap(T& x, T& y)
	{
		T temp = x;
		x = y;
		y = temp;
	}

	inline uint32_t _GetProbeLength(const uint32_t _pos, const uint32_t _hash, int _capacity_idx) const
	{
		uint32_t first_pos_choice = FastMod(_hash, _capacity_idx);
		return FastMod(_pos - first_pos_choice + prime_capacity_table[_capacity_idx], _capacity_idx);
	}
	

};

