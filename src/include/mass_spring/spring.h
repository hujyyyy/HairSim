#ifndef SPRING_H
#define SPRING_H
#include <glm/glm.hpp>
#include "mass.h"

using namespace glm;


class Spring {
public:
    Mass *_m1; // head
    Mass *_m2; // tail
    vec3* _tangent;
public:
    Spring(){
        _m1 = nullptr;
        _m2 = nullptr;
        _tangent = nullptr;
    }
    Spring(Mass* m1, Mass* m2, vec3* tangent){
        assert(m1!=NULL);
        _m1 = m1;
        _m2 = m2;
        _tangent = tangent;
    }
    void updateTangent(){
        *_tangent = *(_m2->_position) - *(_m1->_position);
        if(!enable_smooth) interpolate(_tangent,1);
    }

    
};


#endif
