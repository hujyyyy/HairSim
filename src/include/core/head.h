#ifndef HEAD_H
#define HEAD_H

#include<mass_spring/mass.h>
#include<mass_spring/mass_spring.h>


#include<mesh.h>
#include<model.h>
#include<filesystem.h>
#include<shader.h>
#include<stdio.h>
#include<util.hpp>


using namespace glm;


class Collider{
public:
    //without model
    vec3 _center;
    vec3 _buttomleft;
    vec3 _upright;
    
    vec3 _centerPos;
    vec3 _blPos;
    vec3 _urPos;
    
    std::vector<vec3> _centers;
    std::vector<float> _radius;
    
    Collider(vec3 bl,vec3 ur,mat4 model){
        _center = (bl+ur)/2.0f;
        _buttomleft = bl;
        _upright = ur;
        
        updatePosition(model);
        
        vec3 center1 = _center;
        center1.z += 45;
        //center1.y += 2;
        float radius1 = 9.9f;
        vec3 center2 = _center;
        center2.z += 40;
        center2.y += 20;
        float radius2 = 10.4f;
        vec3 center3 = _center;
        center3.x -= 1.5;
        center3.z -= 8;
        center3.y -= 20;
        center3 = center3;
        float radius3 = 8.0f;
        vec3 center4 = _center;
        center4.z -= 11;
        center4.y -= 90;
        float radius4 = 2.6f;
        vec3 center5 = _center;
        center5.z -= 60;
        center5.y -= 20;
        float radius5 = 7.5f;
        
        const int num_cld = 5;
        vec3 centers[num_cld] = {center1,center2,center3,center4,center5};
        float radius[num_cld] = {radius1,radius2,radius3,radius4,radius5};
        
        _centers = std::vector<vec3>(centers,centers+num_cld);
        _radius = std::vector<float>(radius,radius+num_cld);
        
    }
    
    void updatePosition(mat4 model){
        _centerPos = vec3(model*vec4(_center,1.0f));
        _blPos = vec3(model*vec4(_buttomleft,1.0f));
        _urPos = vec3(model*vec4(_upright,1.0f));
        
    }
    
    unsigned int VBO,VAO;
    void draw(mat4 model){
        std::vector<vec3> vertice;
//        int l = 10;
//        vertice.push_back(_buttomleft);
//        vec3 tmp = vec3(0);
//        tmp = _upright;
//        tmp.z = _buttomleft.z + 100;
//        vertice.push_back(tmp);
//        vertice.push_back(_upright);
        vec3 center = _center;
        center.z -= 50;
        center.y += 10;
        getBox(&vertice, center, 180);
        center.z -= 10;
        center.y -= 100;
        getBox(&vertice, center, 50);
        apply_model(&vertice, model);

        glGenVertexArrays(1,&VAO);
        glGenBuffers(1, &VBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertice.size() * sizeof(vertice[0]), &vertice[0], GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertice[0]), (void*)0);
        
        glDrawArrays(GL_LINE_LOOP,0,4);
        glDrawArrays(GL_LINE_LOOP,4,4);
        glDrawArrays(GL_LINE_LOOP,8,4);
        
        glDrawArrays(GL_LINE_LOOP,12,4);
        glDrawArrays(GL_LINE_LOOP,16,4);
        glDrawArrays(GL_LINE_LOOP,20,4);
        glBindVertexArray(0);
        
    }
    
    void getBox(std::vector<vec3>* vertice,vec3 center,float length){
        vec3 tmp = center;
        tmp.x-=length/2;
        tmp.y-=length/2;
        vertice->push_back(tmp);
        tmp.x+=length;
        vertice->push_back(tmp);
        tmp.y+=length;
        vertice->push_back(tmp);
        tmp.x-=length;
        vertice->push_back(tmp);
        
        tmp = center;
        tmp.x-=length/2;
        tmp.z-=length/2;
        vertice->push_back(tmp);
        tmp.x+=length;
        vertice->push_back(tmp);
        tmp.z+=length;
        vertice->push_back(tmp);
        tmp.x-=length;
        vertice->push_back(tmp);
        
        tmp = center;
        tmp.y-=length/2;
        tmp.z-=length/2;
        vertice->push_back(tmp);
        tmp.z+=length;
        vertice->push_back(tmp);
        tmp.y+=length;
        vertice->push_back(tmp);
        tmp.z-=length;
        vertice->push_back(tmp);
    }
    
    
};

class Head{
    
public:
    Model _headModel;
    Mesh _headMesh;
    std::vector<vec3> root_vertice;
    std::vector<MassSpring*> hairs;
    std::vector<vec3> hair_vertice;
    std::vector<vec3> smooth_vertice;
    
    std::vector<vec3> final_hair_vertice;
    
    Collider* _collider;
    mat4* _model;
    
    Head(std::string path,mat4* model):_headModel(FileSystem::getPath(path)){
        _headMesh = _headModel.getmesh(0);
        _collider = nullptr;
        _model = model;

    }

    
    int generate_roots(float y_threshold,float z_threshold,int d_rate){
        vec3 max_pos = vec3(0);
        vec3 min_pos = vec3(0);
        for(int i=0;i<_headMesh.vertices.size();i+=d_rate){
            max_pos.x = std::max(_headMesh.vertices[i].Position.x,max_pos.x);
            max_pos.y = std::max(_headMesh.vertices[i].Position.y,max_pos.y);
            max_pos.z = std::max(_headMesh.vertices[i].Position.z,max_pos.z);
            min_pos.x = std::min(_headMesh.vertices[i].Position.x,min_pos.x);
            min_pos.y = std::min(_headMesh.vertices[i].Position.y,min_pos.y);
            min_pos.z = std::min(_headMesh.vertices[i].Position.z,min_pos.z);
            if(_headMesh.vertices[i].Position.z>z_threshold||_headMesh.vertices[i].Position.y>y_threshold){
                if(_headMesh.vertices[i].Position.z<0)continue;
                vec3 tmp = _headMesh.vertices[i].Position;
                tmp.z -= 5;
                root_vertice.push_back(tmp);
            }
        }
        //_collider = new Collider(vec3(*_model*vec4(min_pos,1.0)),vec3(*_model*vec4(max_pos,1.0)));
        _collider = new Collider(min_pos,max_pos,*_model);
        
        return (int)root_vertice.size();
    }
    
    unsigned int VBO,VAO;
    
    void generate_hairs(int num_nodes){
        //generate vertice and push into hair_vertice(for VBO)
        int count = 0;
        for(int i =0;i<root_vertice.size();i++){
            vec3 root = root_vertice[i];
            //push back pos
            hair_vertice.push_back(root);
            count++;
            //push back tangent
            hair_vertice.push_back(vec3(0));
            count++;
      
            vec3 tmp = root;
            for(int j=1;j<num_nodes;j++){
                tmp += normalize(tmp-_collider->_centerPos) * restlength;
                //push back pos
                hair_vertice.push_back(vec3((*_model)*vec4(tmp,1.0)));
                count++;
                //push back tangent
                hair_vertice.push_back(hair_vertice[count-1]-hair_vertice[count-3]);
                count++;
                
            }
            
            for(int m=0;m<(intplt_rate-1)*num_nodes*2;m++)
                hair_vertice.push_back(vec3(0));
            count+=(intplt_rate-1)*num_nodes*2;

        }
        //extract the address from hair_vertice and generate the massspring* array
        //do this seperately due to the memory management of verctor in cpp
        count = 0;
        for(int i=0;i<root_vertice.size();i++){
            MassSpring* hair_strand = new MassSpring(1.0, &hair_vertice[count],&hair_vertice[count+1]);
            count+=2;
            for(int j=1;j<num_nodes;j++){
                Mass* tmp = hair_strand->addMass(hmass,&hair_vertice[count]);
                hair_strand->addSpring(tmp->last_node,tmp,&hair_vertice[count+1]);
                count+=2;
            }
            count+=(intplt_rate-1)*num_nodes*2;
            hairs.push_back(hair_strand);
        }
        
        
        for(int i=0;i<smooth_rate*hair_vertice.size();i++)
            smooth_vertice.push_back(vec3(0));
        
        //bind the VAO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(hair_vertice[0]), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(hair_vertice[0]), (void*)sizeof(hair_vertice[0]));
        glBindVertexArray(0);
        
    }

    
    void draw_head(Shader shader){
        _headModel.Draw(shader);
    }
    
    void draw_hair(Shader shader){
        if(!root_vertice.size()) return;
        if(enable_smooth)smooth_hair();
        //interpolate_hair();
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, hair_vertice.size() * sizeof(hair_vertice[0]), &hair_vertice[0], GL_STATIC_DRAW);
        
        if(num_nodes==1) glDrawArrays(GL_POINTS,0,(int)hair_vertice.size());
        else{
            int num_strands = enable_smooth?num_nodes * smooth_rate:num_nodes;
            int num_roots = (int)root_vertice.size()*intplt_rate;
            
            for(int i=0;i<num_roots;i++)
                glDrawArrays(GL_LINE_STRIP,i*num_strands,num_strands);
        }
        glBindVertexArray(0);
    }
    
    void sim_hair(float dt){
        
        _collider->updatePosition(*_model);
        
        std::vector<vec3> centers = _collider->_centers;
        std::vector<float> radius = _collider->_radius;
        apply_model(&centers, *_model);
        
        #pragma omp parallel for
        for (int i =0;i<(int)hairs.size();i++){
            hairs[i]->updatePenalty(centers, radius);
            hairs[i]->updateForceAndVelocity(dt);
            hairs[i]->updatePosition(*_model,dt);
            hairs[i]->updateTangent();
        }
        //debug_printvec(&hair_vertice[3]);
    }
    
    vec3 bezier(int start,int length,float t){
        if(length>2) return (1-t)*bezier(start,length-1,t) + t*bezier(start+2,length-1,t);
        else{
            return (1-t)*hair_vertice[start] + t*hair_vertice[start+2];
        }
    }
    
    void smooth_hair(){
        int num_nodes = (int)hair_vertice.size()/root_vertice.size()/2;
        int num_bezierPoints = num_nodes*smooth_rate;
        
        #pragma omp parallel for
        for(int i=0;i<(int)root_vertice.size();i++){
            for(int j=0;j<num_bezierPoints*2;j+=2){
                float t = j*1.0f/num_bezierPoints;
                smooth_vertice[i*num_bezierPoints*2+j] = bezier(i*num_nodes*2,num_nodes,t);
                
                if(j==0) smooth_vertice[i*num_bezierPoints*2+j+1] = vec3(0);
                else smooth_vertice[i*num_bezierPoints*2+j+1] = smooth_vertice[i*num_bezierPoints*2+j] - smooth_vertice[i*num_bezierPoints*2+j-2];
            }
        }
        
    }

// DEPRECATED
//    void interpolate_hair(){
//        std::vector<vec3>* vertice;
//        if(enable_smooth) vertice = &smooth_vertice;
//        else vertice = &hair_vertice;
//
//        if(intplt_rate==1) {final_hair_vertice = *vertice;return;}
//        int num_nodes = (int)vertice->size()/root_vertice.size()/2;
//        int original_size = (int)vertice->size();
//
//        #pragma omp parallel for
//        for(int j=0;j<intplt_rate;j++){
//            float angle = j*0.5;
//            float new_x = (a + b * angle) * cos(angle);
//            float new_y = (a + b * angle) * sin(angle);
//            vec3 pos = vec3(0);
//            std::vector<vec3>::iterator it_out = final_hair_vertice.begin()+j*original_size;
//            for(std::vector<vec3>::iterator p=vertice->begin();p!=vertice->end();p+=2){
//
//                if(*(p+1)==vec3(0)){
//                     pos = *p+vec3(new_x,new_y,0);
//                }else{
//                     pos += *(p+1);
//                }
//                *it_out = pos;
//                it_out++;
//                *it_out = *(p+1);
//                it_out++;
//
//            }
//
//        }
//
//    }
    
 
    

};






#endif
