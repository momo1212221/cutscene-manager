#include <cassert>
#include "logic.hpp"
int main(){ assert(qol::should_skip(2,false,50,45)); assert(qol::should_skip(1,true,45,45)); assert(!qol::should_skip(1,false,100,45)); }
