#pragma once

class Attribute
{
public:
    Attribute(const std::vector<std::string>& enumeratorNames)
    {
        _size = (unsigned char)enumeratorNames.size();
        _enumeratorNames.resize(_size);
        for (unsigned i = 0; i < _size; i++)
            _enumeratorNames[i] = &_enumeratorNameIdPairs.insert({enumeratorNames[i], i}).first->first;
    }
    virtual                     ~Attribute          () = default;
    const unsigned char*        GetEnumeratorId     (const std::string& name) const
    {
        auto search = _enumeratorNameIdPairs.find(name);
        if (search == _enumeratorNameIdPairs.end())
            return nullptr;
        return &(search->second);
    }
    const std::string*          GetEnumeratorName   (unsigned char id) const
    {
        if (id >= _size)
            return nullptr;
        return _enumeratorNames[id];
    }
    bool                        IsEnumeratorNameValid(const std::string& name) const
    {
        return _enumeratorNameIdPairs.find(name) != _enumeratorNameIdPairs.end();
    }
    std::vector <unsigned char> GetValues           (unsigned mask) const
    {
        std::vector<unsigned char> ids;
        for (unsigned char i = 0; i < _size; i++)
            if ((mask << i & 1) == 1)
                ids.push_back(i);
        return ids;
    }
    
    virtual unsigned GetDifference (unsigned attributeConditionMask, unsigned attributeTargetMask) const = 0;
protected:
    unsigned char _size = 0;
    std::unordered_map<std::string, unsigned char> _enumeratorNameIdPairs; //name <-> id function
    std::vector<const std::string*> _enumeratorNames;
    
};
