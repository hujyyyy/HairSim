#ifndef UTIL_H
#define UTIL_H

#include<config.h>
#include<stdio.h>
#include<glm/glm.hpp>
using namespace glm;

//functions for utilities or debug
void apply_model(std::vector<vec3>* array,mat4 model);
void debug_printvec(vec3* vec);
void interpolate(vec3*,int);

void interpolate(vec3* base,int flag){
    //flag 0 for position interpolation and 1 for tangent
    switch (flag) {
        case 0:
            for(int j=1;j<intplt_rate;j++){
                float angle = j*0.5;
                float new_x = (a + b * angle) * cos(angle);
                float new_y = (a + b * angle) * sin(angle);
                *(base+j*num_nodes*2) = *base + vec3(new_x,new_y,0);
            }
            break;
        case 1:
            for(int j=1;j<intplt_rate;j++)
                *(base+j*num_nodes*2) = *base;
            break;
        default:
            break;
    }
}
    



void apply_model(std::vector<vec3>* array,mat4 model){
    for(std::vector<vec3>::iterator p = array->begin();p!=array->end();p++){
        *p = vec3(model*vec4(*p,1.0f));
    }
}

void debug_printvec(vec3* vec){
    printf("x: %f, y: %f, z: %f\n",vec->x,vec->y,vec->z);
}
#endif
