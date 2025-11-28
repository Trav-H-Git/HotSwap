#pragma once

//#include <string>
//typedef std::string String;

//#include "types/xen_types.h"

//#include "math/xen_math.h"
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <cstring>
#include <stdio.h>


//#include <cstring>

class Memory;

class String
{
public:
    friend String;

    String SubString(int _idx, int _len);
    inline size_t Length() const { return m_length; };
    void PushBack(const char char_p);
    template <typename... Args>
    static String Format(const char* format, Args... args) {
        // Estimate a buffer size for the formatted string
        size_t bufferSize = 256; // Initial guess
        char* buffer = new char[bufferSize];

        // Attempt to format using snprintf to estimate the required buffer size
        //int formattedSize = sprintf_s(nullptr, 0, format, args...);
        //if (formattedSize < 0) {
        //    delete[] buffer;
        //    return String();
        //}

        //// Resize the buffer if needed
        //if (static_cast<size_t>(formattedSize) >= bufferSize) {
        //    delete[] buffer;
        //    bufferSize = formattedSize + 1;
        //    buffer = new char[bufferSize];
        //}

        // Format the string into the buffer
        sprintf_s(buffer, bufferSize, format, args...);

        // Create a String from the formatted buffer
        String result(buffer);
        delete[] buffer;

        return result;
    }

    void Clear();
    // Default constructor
    String();

    // Constructor from C-string
    String(const char* str);

    String(const char* _str, size_t _len);

    inline const char* CStr() const
    {
        return m_data;
    }

    // Copy constructor
    String(const String& other);

    // Move constructor
    String(String&& other) noexcept;

    // Destructor
    virtual ~String();

    //String operator+(const char* lhs, const char* rhs);
    //String operator+(const String& lhs, const char* rhs);
    // Conversion operator to char*
    inline operator const char* () const {
        return m_data;
    }

    // Copy assignment
    //inline String& operator=(const String& other) {
    //    if (this == &other)
    //        return *this; // Self-assignment check

    //    FREE_MEM(m_data);

    //    m_length = other.m_length;
    //    m_data = (char*)ALLOC_MEM(m_length + 1);
    //    strcpy(m_data, other.m_data);
    //    return *this;
    //}

    // Move assignment
    //inline String& operator=(String&& other) noexcept {
    //    if (this == &other)
    //        return *this; // Self-assignment check

    //    FREE_MEM(m_data);

    //    m_data = other.m_data;
    //    m_length = other.m_length;

    //    other.m_data = nullptr;
    //    other.m_length = 0;
    //    return *this;
    //}

    String& operator=(const char* other);
    // Concatenation operator
    String& operator=(const String& other);
    // Move assignment operator
    String& operator=(String&& other) noexcept;

    String operator+(const String& other) const;
    String operator+(const char* other) const;

    String& operator+=(const String& other);
    String& operator+=(const char* other);
    //String& operator+(const char* lhs, const char* rhs);

    char& operator[](size_t _i);
    const char& operator[](size_t _i) const;

    //// Concatenation operator for int
    //String operator+(int number) const {
    //    char buffer[12]; // Buffer to hold the integer as a string (-2^31 to 2^31-1 is 11 chars + null terminator)
    //    intToStr(number, buffer);
    //    return String(buffer);
    //}

    // Output operator
    /*friend std::ostream& operator<<(std::ostream& os, const String& str) {
        if (str.m_data)
            os << str.m_data;
        return os;
    }*/


    // Helper function to convert integer to string
    //inline static String FromInt(int number) {
    //    char buffer[12]; // Enough to hold any 32-bit integer
    //    intToStr(number, buffer);
    //    return String(buffer);
    //}


private:

    char* m_data = nullptr;
    size_t m_length = 0;

    //inline static void intToStr(int number, char* buffer) {
    //    char temp[12]; // Temporary buffer for reversed digits
    //    int i = 0;

    //    // Handle zero explicitly
    //    if (number == 0) {
    //        buffer[0] = '0';
    //        buffer[1] = '\0';
    //        return;
    //    }

    //    // Handle negative numbers
    //    bool isNegative = number < 0;
    //    if (isNegative)
    //        number = -number;

    //    // Extract digits
    //    while (number != 0) {
    //        temp[i++] = '0' + (number % 10);
    //        number /= 10;
    //    }

    //    if (isNegative)
    //        temp[i++] = '-';

    //    // Reverse the characters into the output buffer
    //    int j = 0;
    //    while (i > 0) {
    //        buffer[j++] = temp[--i];
    //    }

    //    buffer[j] = '\0'; // Null-terminate the string
    //}
};


//// Global operator+ for const char* + String
//String operator+(const char* lhs, const char* rhs) {
//    return String(lhs) + rhs;
//}
//
//// Global operator+ for const char* + int
//String operator+(const char* lhs, int rhs) {
//    return String(lhs) + String::FromInt(rhs);
//}
//
//// Global operator+ for String + int
//String operator+(const String& lhs, int rhs) {
//    return lhs + String::FromInt(rhs);
//}