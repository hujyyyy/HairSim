#ifndef CONFIG_H
#define CONFIG_H
#include <glm/glm.hpp>

using namespace glm;
///////internel parameters//////
#define hmass 0.5f //default mass for a single hair node
#define k 0.4f //spring constant
#define kd 0.3f // damping constant
#define restlength 1.0f //restlength
#define maxlength 1.1f //the max length a spring can reach
#define g 9.8f //gravity

#define k_cls 80.0f //parameter for collision penalty



////////flags///////
bool enable_curls = false; //enable curls effect (not work so well..)
bool enable_smooth = false; //enable smooth with bezier curves(still have strange results)
bool enable_GUI = true;

bool head_visible = true;
bool hair_visible = true;
bool collider_test_visible = false;


//animation parameters
int frame_num = 120;
int fps = 30;

///////externel parameters//////
const int intplt_rate = 100; //intepolation rate
const int smooth_rate = 10; //smooth rate(multiplier for bezier curves)

int num_nodes = 10;//num of nodes for each hair strand
int drate = 50; //downsampling rate on head to generate hair root
//for sampling on mesh
float y_threshold = 500.0f;
float z_threshold = 110.0f;

//for interpolation
float a = 0.0002*intplt_rate;
float b = 0.0003*intplt_rate;

vec3 lightPos = vec3(0.0f,0.0f,3.0f);
vec3 lightColor = vec3(1.0f,1.0f,1.0f);
//vec3 hairColor = normalize(vec3(255,215,0)figu);
vec3 hairColor = vec3(0);

bool rock = false;//WARNING:be careful for this flag



#endif
