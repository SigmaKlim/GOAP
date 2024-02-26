#pragma once

#include <cassert>
#include <vector>
template <typename T>
class PropertyList
{
public:
    PropertyList(unsigned size) : _properties(size), _affectedMask(size, false), _affectedPropertiesNum(0){}
    virtual ~PropertyList() = default;
    bool IsAffected(unsigned index) const
    {
        assert(index < Size());
        return _affectedMask[index];
    }
    void SetProperty(unsigned index, const T& property)
    {
        assert(index < Size());
        _properties[index] = property;
        _affectedMask[index] = true;
        _affectedPropertiesNum++;
    }
    //Marks property by index as unaffected. Does NOT delete it from memory.
    void ClearValue(unsigned index)
    {
        assert(index < Size());
        _affectedMask[index] = false;
        _affectedPropertiesNum--;
    }
    unsigned Size() const
    {
        return _properties.size();
    }
protected:
    std::vector<T>      _properties;
    std::vector<bool>   _affectedMask;
    unsigned            _affectedPropertiesNum = 0;
};