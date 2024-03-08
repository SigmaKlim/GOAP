﻿#pragma once

#include <cassert>
#include <vector>
#include <boost/container_hash/hash.hpp>
#include <boost/mp11/integral.hpp>

template <typename T>
class PropertyList
{
public:
    PropertyList(size_t size) : _properties(size), _affectedMask(size, false){}
    PropertyList(const PropertyList& other) : _properties(other.Size()), _affectedMask(other.Size(), false)
    {
        for (unsigned i = 0; i < other.Size(); i++)
            if (other.IsAffected(i))
                SetProperty(i, other.GetProperty(i));
    }
    PropertyList& operator=(const PropertyList& other)
    {
        _properties.resize(other.Size());
        _affectedMask.resize(other.Size(), false);
        _affectedPropertiesNum = 0;
        for (unsigned i = 0; i < other.Size(); i++)
            if (other.IsAffected(i))
                SetProperty(i, other.GetProperty(i));
        return *this;
    }
    virtual ~PropertyList() = default;
    bool IsAffected(size_t index) const
    {
        assert(index < Size());
        return _affectedMask[index];
    }
    void SetProperty(size_t index, const T& property)
    {
        assert(index < Size());
        _properties[index] = property;
        _affectedMask[index] = true;
        _affectedPropertiesNum++;
    }
    const T& GetProperty(size_t index) const
    {
        assert(index < Size());
        return _properties[index];
    }
    //Marks property by index as unaffected. Does NOT delete it from memory.
    void ClearValue(size_t index)
    {
        assert(index < Size());
        _affectedMask[index] = false;
        _affectedPropertiesNum--;
    }
    size_t Size() const
    {
        return _properties.size();
    }

    size_t NumAffected() const
    {
        return _affectedPropertiesNum;
    }
protected:
    std::vector<T>      _properties;
    std::vector<bool>   _affectedMask;
    size_t            _affectedPropertiesNum = 0;
    
};

template <typename T>
bool operator==(const PropertyList<T>& left, const PropertyList<T>& right)
{
    if (left.Size() != right.Size())
        return false;
    for (size_t i = 0; i < left.Size(); i++)
        if (left.IsAffected(i) == right.IsAffected(i))
        {
            if (left.IsAffected(i) == true)
                if (!(left.GetProperty(i) == right.GetProperty(i)))
                    return false;
        }
        else
            return false;
    return true;
}


template <typename T>
std::size_t hash_value(const PropertyList<T>& plist)
{
    size_t hash = 0;
    for (size_t i = 0; i < plist.Size(); i++)
        if (plist.IsAffected(i))
            boost::hash_combine(hash, plist.GetProperty(i));
    return hash;
}