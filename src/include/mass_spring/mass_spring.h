#ifndef MASS_SPRING_H
#define MASS_SPRING_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "mass.h"
#include "spring.h"
#include <util.hpp>


using namespace glm;

class MassSpring {
public:
    std::vector<Mass*> masses;
    std::vector<Spring*> springs;
    int _numMass;
    
    MassSpring(){_numMass=0;}
    MassSpring(float mass,vec3* pos,vec3* tangent){
        _numMass=0;
        Mass* tmp = addMass(mass, pos);
        addSpring(tmp,tmp,tangent);
    }
    
    Mass* addMass(float mass, vec3* pos){
        Mass* last = (_numMass==0)?nullptr:masses[_numMass-1];
        Mass* tmp = new Mass(_numMass,mass,pos,last);
        masses.push_back(tmp);
        _numMass++;
        
        return tmp;
    }
    Spring* addSpring(Mass *mass1, Mass *mass2,vec3* tangent){
        Spring* tmp = new Spring(mass1,mass2,tangent);
        springs.push_back(tmp);
        return tmp;
    }
    void updateTangent(){
        for(std::vector<Spring*>::iterator p = springs.begin();p!=springs.end();p++){
            (*p)->updateTangent();
        }
    }
    
    void updateForceAndVelocity(float dt){
        for(std::vector<Mass*>::iterator p = masses.begin();p!=masses.end();p++){
            (*p)->updateForceAndVelocity(dt);
        }
    }
    void updatePosition(mat4 model,float dt){
        for(std::vector<Mass*>::iterator p = masses.begin();p!=masses.end();p++){
            (*p)->updatePosition(model,dt);
        }
        
    }
    void updatePenalty(std::vector<vec3> centers,std::vector<float> radius){
        for(std::vector<Mass*>::iterator p = masses.begin();p!=masses.end();p++){
            (*p)->updatePenalty(centers,radius);
        }
        
    }

};





#endif
