#include"spring.h"
#include"mass.h"

Spring::Spring(){
    _m1 = nullptr;
    _m2 = nullptr;
    _tangent = nullptr;
}

Spring::Spring(Mass* m1, Mass* m2, vec3* tangent){
    assert(m1!=NULL);
    _m1 = m1;
    _m2 = m2;
    _tangent = tangent;
}

Spring::updateTangent(){
    *_tangent = _m2->_position - _m1->_position;
}
