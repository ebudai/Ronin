#pragma once

#include "grammar.h"

namespace ronin
{
    struct semantic
    {
        template <statement::type T> static void analysis();
    };
}
