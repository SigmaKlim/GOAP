#pragma once
#include "Tools/Catalogue.h"
#include "Tools/PropertyList.h"

struct ValueSet : PropertyList<int>
{
    ValueSet(size_t size) : PropertyList(size) {}
    // WorldMask(const WorldMask& other) : PropertyList(other) {}
    static Catalogue<IAttribute*>* AttributeCataloguePtr;
};