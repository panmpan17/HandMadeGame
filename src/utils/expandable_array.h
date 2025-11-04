#pragma once

#include <exception>
#include <string_view>
#include <iostream>
#include <type_traits>
#include "platform.h"

#if IS_PLATFORM_WINDOWS
#include <cstring>
#endif


class MemoryAllocateException : public std::exception {
public:
    MemoryAllocateException(const std::string_view& strMessage) : m_strMessage(strMessage) {}

    const char* what() const noexcept override {
        return m_strMessage.data();
    }

private:
    std::string_view m_strMessage;
};


template<typename T>
class PointerExpandableArray
{
public:
    static_assert(std::is_pointer<T>::value, "Template parameter must be a pointer type");

    PointerExpandableArray(int nInitialSize)
    {
        m_nSize = nInitialSize;
        m_pArray = static_cast<T*>(malloc(sizeof(T) * m_nSize));
        memset(m_pArray, 0, sizeof(T) * nInitialSize);
    }

    ~PointerExpandableArray()
    {
        for (int i = 0; i < m_nSize; ++i)
        {
            delete m_pArray[i];
        }
        free(m_pArray);
    }

    PointerExpandableArray(const PointerExpandableArray&) = delete;
    PointerExpandableArray& operator=(const PointerExpandableArray&) = delete;

    T getElement(int index) const
    {
        if (index < 0 || index >= m_nSize)
            throw std::out_of_range("Index out of range");

        return m_pArray[index];
    }

    int getElementIndex(T element) const
    {
        for (int i = 0; i < m_nSize; ++i)
        {
            if (m_pArray[i] == element)
                return i;
        }
        return -1; // Not found
    }

    /// @brief Adds a new element to the array. This will assume ownership of the element.
    /// @param element The element to add.
    /// @return The index at which the element was added.
    int addElement(T element)
    {
        for (int i = 0; i < m_nSize; ++i)
        {
            if (m_pArray[i] == nullptr)
            {
                m_pArray[i] = element;
                return i;
            }
        }

        int nIndex = m_nSize;
        expandTo(m_nSize * 2);

        if (nIndex >= m_nSize)
        {
            throw MemoryAllocateException("Failed to expand array for new element.");
        }
        m_pArray[nIndex] = element; // Add to the first new slot

        return nIndex;
    }

    /// @brief Returns and removes the last non-null element, does not shrink capacity.
    /// @param nIndex 
    /// @return last non-null element or nullptr if not found
    T removeElement(int nIndex)
    {
        if (nIndex < 0 || nIndex >= m_nSize)
            return nullptr;

        T pElement = m_pArray[nIndex];
        m_pArray[nIndex] = nullptr;
        return pElement;
    }

    T popLastElement()
    {
        for (int i = m_nSize - 1; i >= 0; --i)
        {
            if (m_pArray[i] != nullptr)
            {
                T pElement = m_pArray[i];
                m_pArray[i] = nullptr;
                return pElement;
            }
        }

        return nullptr;
    }

    void expandTo(int nSize)
    {
        if (nSize <= m_nSize)
            return;

        T* pNewArray = static_cast<T*>(malloc(sizeof(T) * nSize));
        if (pNewArray == nullptr)
        {
            throw MemoryAllocateException("Failed to allocate memory for array expansion.");
        }

        memcpy(pNewArray, m_pArray, sizeof(T) * m_nSize);
        memset(pNewArray + m_nSize, 0, sizeof(T) * (nSize - m_nSize));

        free(m_pArray);
        m_pArray = pNewArray;
        m_nSize = nSize;
    }

    void clear()
    {
        for (int i = 0; i < m_nSize; ++i)
        {
            delete m_pArray[i];
            m_pArray[i] = nullptr;
        }
    }

    int getSize() const { return m_nSize; }

    void printAll() const
    {
        for (int i = 0; i < m_nSize; ++i)
        {
            std::cout << "Index " << i << ": " << m_pArray[i] << std::endl;
        }
    }

    void swap(int index1, int index2)
    {
        if (index1 < 0 || index1 >= m_nSize || index2 < 0 || index2 >= m_nSize)
            throw std::out_of_range("Index out of range");

        T temp = m_pArray[index1];
        m_pArray[index1] = m_pArray[index2];
        m_pArray[index2] = temp;
    }

private:
    int m_nSize;
    T* m_pArray = nullptr;
};