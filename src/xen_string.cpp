#include "xen_string.h"

#include <memory.h>
#include "xen_assert.h"



String String::SubString(int _idx, int _len)
{
    ASSERT_DEFINED(m_data);
    _idx = (_idx < 0) ? -_idx : _idx;
    _idx = _idx % m_length;

    ASSERT_INBOUNDS(_idx, m_length);
    ASSERT_INBOUNDS((_idx + _len), (m_length + 1));

    const char* c = m_data + _idx;
    size_t s = _len;
    return String(c, s);

}

void String::PushBack(const char char_p)
{
}

void String::Clear()
{
    ASSERT_DEFINED(m_data);

    m_length = 0;
    m_data[0] = '\0';
}

String::String()
{
    m_length = 0;
    m_data = (char*)CALLOC_MEM(1);
    m_data[0] = '\0';
}

String::String(const char* str) {
    m_length = strlen(str);
    //m_data = new char[m_length + 1];
    m_data = (char*)CALLOC_MEM(m_length + 1);

    strcpy(m_data, str);
}

String::String(const char* _str, size_t _len)
{
    m_length = _len;
    m_data = (char*)ALLOC_MEM(m_length + 1);
    for (size_t i = 0; i < _len; i++)
    {
        m_data[i] = _str[i];
    }
    m_data[_len] = '\0';
}

String::String(const String& other) : m_length(other.m_length)
{
    m_data = (char*)CALLOC_MEM(m_length + 1);
    strcpy(m_data, other.m_data);
}

String::String(String&& other) noexcept : m_data(other.m_data), m_length(other.m_length)
{
    other.m_data = nullptr;
    other.m_length = 0;
}

String::~String()
{
    //delete[] m_data;
    FREE_MEM(m_data);
}

String& String::operator=(const char* other)
{
    if (m_data != other) {
        FREE_MEM(m_data);

        m_length = strlen(other);
        m_data = (char*)CALLOC_MEM(m_length + 1);

        strcpy(m_data, other);
    }
    return *this;
}

String& String::operator=(const String& other)
{
    if (this != &other) {
        FREE_MEM(m_data);

        m_length = other.Length();
        //m_data = new char[m_length + 1];
        m_data = (char*)CALLOC_MEM(m_length + 1);

        strcpy(m_data, other.m_data);
    }
    return *this;
}

// Move assignment operator
String& String::operator=(String&& other) noexcept {
    if (this != &other) {
        FREE_MEM(m_data);
        m_data = other.m_data;
        m_length = other.m_length;
        other.m_data = nullptr;
        other.m_length = 0;
    }
    return *this;
}

String String::operator+(const String& other) const {
    String result = *this;
    result += other;
    return result;
}

String String::operator+(const char* other) const
{
    String result = *this;
    //String result = String(other);
    result += String(other);
    return result;
}

String& String::operator+=(const String& other)
{
    size_t new_len = m_length + other.m_length;
    char* new_data = (char*)ALLOC_MEM(new_len + 1);

    strcpy(new_data, m_data);
    strcat(new_data, other.m_data);

    FREE_MEM(m_data);
    m_data = new_data;
    m_length = new_len;

    return *this;
}

String& String::operator+=(const char* other)
{
    size_t new_len = m_length + strlen(other);
    char* new_data = (char*)ALLOC_MEM(new_len + 1);

    strcpy(new_data, m_data);
    strcat(new_data, other);

    FREE_MEM(m_data);
    m_data = new_data;
    m_length = new_len;

    return *this;
}


//String& String::operator+(const char* lhs, const char* rhs)
//{
//    return String(lhs) + rhs;
//}

char& String::operator[](size_t _i)
{
    return m_data[_i];
}

const char& String::operator[](size_t _i) const
{
    return m_data[_i];
}
//
//String operator+(const String& lhs, const char* rhs) {
//    size_t len1 = strlen(lhs);
//    size_t len2 = strlen(rhs);
//    char* result = new char[len1 + len2 + 1];
//    strcpy(result, lhs);
//    strcat(result, rhs);
//
//    String concatenated(result);
//    delete[] result;
//    return concatenated;
//}

//String String::operator+(const char* lhs, const char* rhs) {
//    size_t len1 = strlen(lhs);
//    size_t len2 = strlen(rhs);
//    char* result = new char[len1 + len2 + 1]; // Allocate memory for concatenation
//    strcpy(result, lhs);
//    strcat(result, rhs);
//
//    String concatenated(result);
//    delete[] result; // Clean up temporary buffer
//    return concatenated;
//}
