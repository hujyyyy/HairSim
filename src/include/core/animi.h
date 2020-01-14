#ifndef ANIMI_H
#define ANIMI_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>


//keyframe struct
struct key_frame{
    int frame;
    glm::mat4 model;
    glm::vec3 trans;
    glm::vec3 scl;
    glm::mat4 rot;
    
    key_frame(int theframe, glm::mat4 themodel, glm::vec3 thetrans, glm::vec3 thescl, glm::mat4 therot){
        frame = theframe;
        model = themodel;
        trans = thetrans;
        scl = thescl;
        rot = therot;
    }
    key_frame(){
        frame = -1; model = glm::mat4(0.0);trans = glm::vec3(0.0);scl=glm::vec3(1.0);rot=glm::mat4(1.0f);
    }
};


//Animi class
class Animi{
private:
    int start_kf;
    int start_kf_idx;
    int end_kf;
    int end_kf_idx;
    vector<key_frame>* _kfs;
    
    
public:
    
    
    Animi(int s_kf,int e_kf,vector<key_frame>* kfs){
        _kfs = kfs;
        this->start_kf = s_kf;
        this->end_kf = e_kf;
        int s_idx = is_keyframe(s_kf);
        int e_idx = is_keyframe(e_kf);
        assert(s_idx!=-1 && e_idx!=-1);
        this->start_kf_idx = is_keyframe(s_kf);
        this->end_kf_idx = is_keyframe(e_kf);
    }
    
    void update_self(){
        assert((*_kfs).size()>0);
        start_kf = (*_kfs)[0].frame;
        start_kf_idx = is_keyframe(start_kf);
        if((*_kfs).size()>1){
            end_kf = (*_kfs)[1].frame;
            end_kf_idx = 1;
        }else{
            end_kf = start_kf;
            end_kf_idx = start_kf_idx;
        }
    }
    
    bool update_model(int frame,int frame_num,glm::vec3* trans,glm::mat4* rot,glm::mat4* model,glm::vec3* scl){
        
        if(frame > frame_num) return false;
        if(start_kf==end_kf) return true;
        assert(frame>=start_kf&&frame<=end_kf);
        
        glm::quat q_s = glm::quat_cast((*_kfs)[start_kf_idx].rot);
        glm::quat q_e = glm::quat_cast((*_kfs)[end_kf_idx].rot);
        
        glm::vec3 trans_s = (*_kfs)[start_kf_idx].trans;
        glm::vec3 trans_e = (*_kfs)[end_kf_idx].trans;
        
        float t = float(frame-start_kf)/(end_kf-start_kf);
        //glm::quat q = q_s + t * (q_e-q_s);
        //printf("%f %f %f\n",q_s.w,q_e.w,q.w);
        glm::quat q = glm::slerp(q_s, q_e, t);
        *trans = glm::mix(trans_s,trans_e,t);
        
        glm::mat4 tmp = glm::mat4(1.0);
        tmp = glm::translate(tmp, *trans);
        *rot = glm::mat4_cast(q);
        
        //cout<<glm::to_string(rot)<<endl;
        
        *model = tmp*(*rot);
        *model = glm::scale(*model, *scl);
        
        if(frame==end_kf){
            start_kf = end_kf;
            start_kf_idx = end_kf_idx;
            if(end_kf_idx < (*_kfs).size()-1){
                end_kf_idx++;
                end_kf = (*_kfs)[end_kf_idx].frame;
            }
        }
        
        return true;
    }
    
    int is_keyframe(int frame){
        for(int i = 0; i<(*_kfs).size(); i++){
            if((*_kfs)[i].frame == frame) return i;
        }
        return -1;
    }
    
    
};




#endif
