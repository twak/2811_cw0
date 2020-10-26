#ifndef MUSHROOM_H
#define MUSHROOM_H

#include "placeable.h"
#include "thing.h"

// a small glowing mushroom on the ground, found wherever adventurers wander. Can be picked up.
class Mushroom : public Thing, public Placeable
{

    string getName()
    {
        return "mushroom";
    }
};

#endif // MUSHROOM_H
