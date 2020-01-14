#ifndef MASS_H
#define MASS_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <util.hpp>


using namespace glm;


class Mass {
        //vec3 gravity;  // usually (0,-9.8,0)
public:
    int _idx;
    float _mass;    // in kilograms
    vec4 _fixed_pos;
    vec3* _position;
    vec3 _velocity;
    
    vec3 _force;
    vec3 _downforce;
    
    vec3 _penaltyForce;
    bool _isFixedPosition;  // masses in fixed position do not move
    
    Mass* last_node;
    Mass* next_node;

public:
    Mass(int idx,float mass,vec3* pos,Mass* last){
        assert(pos!=NULL);
        _idx = idx;
        _mass = mass;
        _fixed_pos = vec4(*pos,1.0);
        _position = pos;
        _velocity = vec3(0);
        _force = vec3(0);
        _downforce = vec3(0);
        _penaltyForce = vec3(0);
        _isFixedPosition = (idx==0)?true:false;
        
        next_node = nullptr;
        last_node = last;
        if(!_isFixedPosition) last_node->next_node = this;
    }
    
    bool isfixed(){
        return _isFixedPosition;
    }
    vec3 getForce(){
        
        return _force + _penaltyForce + _mass*vec3(0,-g,0);
    }
    
    vec3 calculateForces(Mass* refnode){
        if(refnode==NULL) return vec3(0,0,0);
        else{
            
            //if refnode is the last node, inherit the downforce here to accelerate
            if(refnode->_idx == _idx-1) {
                return -(refnode->_downforce)-kd*_velocity;
            }
            
            vec3 dir = *(refnode->_position) - *_position;
            float dis = length(dir);
            float delta_dis = dis-restlength;
            vec3 udir = normalize(dir);
            
            _downforce = k*(delta_dis)*udir;
            vec3 force = _downforce -  kd*_velocity;
            
            return force;
        }
    }
    void updateForceAndVelocity(float dt){
        vec3 total_force = getForce();
        vec3 accel = total_force/_mass;
        
        // update velocity
        _velocity += accel*dt;
        
        //update force
        _force = vec3(0,0,0);
        _penaltyForce = vec3(0,0,0);
        _force += calculateForces(last_node);
        _force += calculateForces(next_node);
        if(_idx%2==1&&enable_curls){
            if(next_node!=NULL)_force += calculateForces(next_node->next_node);
            if(last_node!=NULL)_force += calculateForces(last_node->last_node);
        }
        
    }
    void updatePosition(mat4 model,float dt){
        
        if(_isFixedPosition) {
            vec3 tmp = vec3(model*_fixed_pos);
            *_position = tmp;
        }
        else {
            *_position += _velocity*dt;
            set_constrain(last_node);
            if(_idx%2==1&&enable_curls) set_constrain(last_node->last_node);
        }
        
        if(!enable_smooth)interpolate(_position,0);
        
    }
    
    void set_constrain(Mass* refnode){
        if(refnode==NULL) return;
        vec3 dir = *(refnode->_position) - *_position;
        float dis = length(dir);
        if(dis>maxlength) *_position = *(refnode->_position) - normalize(dir)*maxlength;
        
    }
    
    void updatePenalty(std::vector<vec3> centers,std::vector<float> radius){
        vec3 dir;
        float dis,r;
        for(int i =0;i<centers.size();i++){
            r = radius[i];
            dir = *_position - centers[i];
            dis = length(dir);
            if(dis<r){
                _penaltyForce += k_cls*(r-dis)*normalize(dir);
            }
        }
    }
};

#endif
