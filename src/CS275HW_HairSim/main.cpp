#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>
#include <nanogui/nanogui.h>
#include <animi.h>
#include <mass_spring/mass_spring.h>
#include <head.h>



#include <iostream>
#include <filesystem.h>
#include <util.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int modifiers);
void processInput(GLFWwindow *window);

//button callback functions
void update_slider_box(nanogui::Slider* slider, nanogui::TextBox* textBox);

//callback functions used for UI
void nextkf_callback();
void addkf_callback();
void setGUI(nanogui::Slider*,nanogui::TextBox*,nanogui::Window*,nanogui::Widget*,nanogui::FormHelper*);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


vector<key_frame> key_frames;
//init default keyframe, which is the frame at start time
key_frame default_kf = key_frame();

//init model matrix
glm::mat4 model = glm::mat4(1.0);
glm::mat4 rot = glm::mat4(1.0);
glm::vec3 trans = glm::vec3(0.0);
glm::vec3 scl = glm::vec3(1.0);


Animi* animi;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int current_frame = 0;
bool is_play = false;



bool ui_focused = true;
nanogui::Screen *screen = nullptr;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HairSim", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    
    //init the model matrix
    trans = glm::vec3(0.0f, -8.75f, -80.0f);
    scl = glm::vec3(0.1f, 0.1f, 0.1f);
    rot = glm::rotate(rot, glm::radians(-90.0f), glm::vec3(1.0,0.0,0.0));
    //rot = glm::rotate(rot, glm::radians(90.0f), glm::vec3(1.0,0.0,0.0));
    //rot = glm::rotate(rot, glm::radians(180.0f), glm::vec3(0.0,0.0,1.0));
    
    model = glm::translate(model, trans); // translate it down so it's at the center of the scen
    model = model*rot;
    model = glm::scale(model, scl); // it's a bit too big for our scene, so scale it down
    
    screen = new nanogui::Screen();
    screen->initialize(window, true);
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    

    
    // build and compile shaders
    // -------------------------
    Shader ourShader("shaders/model_loading.vs", "shaders/model_loading.fs");
    Shader hairShader("shaders/hair_shader.vs","shaders/hair_shader.fs");
    
    hairShader.use();
    hairShader.setVec3("lightPos",lightPos);
    hairShader.setVec3("cameraPos", camera.Position);
    
    hairShader.setVec3("hairColor", hairColor);
    hairShader.setVec3("lightColor", lightColor);

    // load models
    // -----------
    //Model ourModel(FileSystem::getPath ("models/Pasha_guard_head.obj"));
    Head head("models/Pasha_guard_head.obj",&model);
    int num_roots = head.generate_roots(y_threshold,z_threshold,drate);
    cout<<"num of roots generated: "<<num_roots<<endl;
    cout<<"total num of hair simulated: "<<num_roots*intplt_rate<<endl;
    head.generate_hairs(num_nodes);
    
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    /////////////////////////////////////
    // Create nanogui gui
    nanogui::FormHelper *gui = new nanogui::FormHelper(screen);
    gui->setFixedSize(Eigen::Vector2i(100,1000));
    nanogui::Window* nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Control Panel");
    
    nanogui::Widget *panel = new nanogui::Widget(nanoguiWindow);
    nanogui::Slider *slider = new nanogui::Slider(panel);
    nanogui::TextBox *textBox = new nanogui::TextBox(panel);

    //set up the gui layer
    setGUI(slider,textBox,nanoguiWindow,panel,gui);

    screen->setVisible(true);
    if(enable_GUI)screen->performLayout();
    ////////////////////////////////////////////////////
    

    
    //update the default frame after placing the model at the center of the screen
    default_kf = key_frame(0,model,trans,scl,rot);
    
    
    //init keyframes buffer
    key_frames.push_back(default_kf);
    
    //init Animi class
    animi = new Animi(0,0,&key_frames);
    float time_count = 0;
    head.sim_hair(1.0f/fps);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        time_count += deltaTime;

        // input
        // -----
        processInput(window);

        // render
        // ------
        float time_val0 = sin(glfwGetTime()*6) + 0.5;
        float time_val1 = cos(glfwGetTime()*6) + 0.5;
        float time_val2 = cos(glfwGetTime()) ;
        if(rock){
            glClearColor(time_val0, time_val1, 1, 1.0f);
        }else{
            glClearColor(1.0f,1.0f,1.0f,1.0f);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();
        

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        
        
        
        if(is_play&&time_count>=(float)1/fps){
            is_play = animi->update_model(current_frame,frame_num,&trans,&rot,&model,&scl);
            if(is_play) current_frame++;
            else current_frame = 0;
            slider -> setValue((float)current_frame/frame_num);
            textBox -> setValue(std::to_string(current_frame));
            time_count = 0;
        }

        ui_focused = nanoguiWindow->focused();
        
        ourShader.setMat4("model", model);
        if(head_visible)head.draw_head(ourShader);
        
        
        if(hair_visible){
            head.sim_hair(1.0f/fps);
            
            hairShader.use();
            hairShader.setMat4("projection", projection);
            hairShader.setMat4("view", view);
            
            //set the light color the same as the background
            if(rock){
                lightColor = vec3(time_val0, time_val1, 1);
                hairShader.setVec3("lightColor", lightColor);
            }
            head.draw_hair(hairShader);
        }
        if(collider_test_visible) head._collider->draw(model);
        screen->drawContents();
        screen->drawWidgets();
        glEnable(GL_DEPTH_TEST);

        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        //tmp solution to the bug that occur on macos mojave
        #ifdef __APPLE__
                static bool macMoved = false;
        
                if(!macMoved) {
                    int x, y;
                    glfwGetWindowPos(window, &x, &y);
                    glfwSetWindowPos(window, ++x, y);
                    macMoved = true;
                }
        #endif
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
    float spd = 0.25f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        trans += glm::vec3(0.0,1.0,0.0) *spd;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        trans += glm::vec3(0.0,-1.0,0.0)*spd;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        trans += glm::vec3(-1.0,0.0,0.0)*spd;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        trans += glm::vec3(1.0,0.0,0.0)*spd;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        trans += glm::vec3(0.0,0.0,1.0)*spd;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        trans += glm::vec3(0.0,0.0,-1.0)*spd;

    glm::mat4 tmp = glm::translate(glm::mat4(1.0), trans);
    model = glm::scale(tmp*rot,scl);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    //printf("%f %f",xpos,ypos);
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    //camera.ProcessMouseMovement(xoffset, yoffset);
    //printf("%f %f\n",xoffset,yoffset);
    //manipulate the model
    if(!is_play){
        screen->cursorPosCallbackEvent(xpos, ypos);
        if(!ui_focused){
            glm::mat4 tmp = glm::mat4(1.0);
            if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)==GLFW_PRESS){
                rot = glm::rotate(rot, glm::radians(xoffset), glm::vec3(0.0,1.0,0.0));
                rot = glm::rotate(rot, glm::radians(yoffset), glm::vec3(-1.0,0.0,0.0));
                model = tmp*rot;
                model = glm::scale(model, scl);
            }
            if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)==GLFW_PRESS){
                rot = glm::rotate(rot, glm::radians(xoffset), glm::vec3(0.0,0.0,1.0));
                model = tmp*rot;
                model = glm::scale(model, scl);

            }
        }
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


void mouse_button_callback(GLFWwindow * window, int button, int action, int modifiers){
    
    screen->mouseButtonCallbackEvent(button, action, modifiers);
    
}


void update_slider_box(nanogui::Slider* slider, nanogui::TextBox* textBox){
    int cframe = current_frame;
    
    slider ->setValue((float)cframe/frame_num);
    textBox ->setValue(std::to_string(cframe));
    
}

void nextkf_callback(){
    int cframe = current_frame;
    int i;
    
    
    for(i=0;i<key_frames.size();i++){
        if(key_frames[i].frame>cframe){
            model = key_frames[i].model;
            trans = key_frames[i].trans;
            scl = key_frames[i].scl;
            rot = key_frames[i].rot;
            current_frame = key_frames[i].frame;
            printf("key frame: %d\n",key_frames[i].frame);
            return;
        }
    }
    
    cframe = 0;
    for(i=0;i<key_frames.size();i++){
        if(key_frames[i].frame>=cframe){
            model = key_frames[i].model;
            trans = key_frames[i].trans;
            scl = key_frames[i].scl;
            rot = key_frames[i].rot;
            current_frame = key_frames[i].frame;
            printf("key frame: %d\n",key_frames[i].frame);
            return;
        }
    }
    
    printf("No more key frames.\n");
    return;
}

void addkf_callback(){
    int cframe = current_frame;
    assert(cframe>=0&&cframe<=frame_num);
    for(auto f = key_frames.begin();f != key_frames.end();f++){
        if(f->frame == cframe){
            f->model = model;
            f->trans = trans;
            f->scl = scl;
            f->rot = rot;
            return;
        }
        
        if(f->frame>cframe){
            key_frames.insert(f,key_frame(cframe,model,trans,scl,rot));
            return;
        }
        
    }
    
    key_frames.push_back(key_frame(cframe,model,trans,scl,rot));
    return;
    
}

void setGUI(nanogui::Slider* slider,nanogui::TextBox* textBox,nanogui::Window* nanoguiWindow,nanogui::Widget* panel,
            nanogui::FormHelper *gui){
    
    panel->setWidth(SCR_WIDTH-10);
    panel->setHeight(50);
    
    panel->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                            nanogui::Alignment::Middle, 0, 30));
    gui->addWidget("Frames", panel);
    slider->setValue(0.0f);
    slider->setFixedWidth(SCR_WIDTH-200*3);
    
    textBox->setFixedSize(Eigen::Vector2i(60, 25));
    textBox->setValue("0");
    slider->setCallback([slider,textBox](float value) {
        float thevalue = (float)(int)(value*frame_num)/frame_num;
        textBox->setValue(std::to_string((int) (thevalue * frame_num)));
        slider->setValue(thevalue);
    });
    slider->setFinalCallback([&](float value) {
        current_frame = value * frame_num;
        cout << "Final slider value: " << (int) (value * frame_num) << endl;
        
    });
    textBox->setFixedSize(Eigen::Vector2i(60,25));
    textBox->setFontSize(20);
    textBox->setAlignment(nanogui::TextBox::Alignment::Right);
    
    
    
    
    gui->addButton("Next key frame", [slider,textBox]() { nextkf_callback();\
        update_slider_box(slider, textBox);} )\
    ->setTooltip("switch to the next key frame");
    
    gui->addButton("Add key frame", [slider,textBox]() { addkf_callback();})\
    ->setTooltip("Add a new key frame. Update it if already added.");
    
    
    gui->addButton("play", [slider,textBox]() {  \
        current_frame = 0;
        animi->update_self();\
        is_play = true; \
        slider -> setValue((float)current_frame/frame_num);\
        textBox -> setValue(std::to_string(current_frame));\
    });
    //->setTooltip("Play the animation based on the give key frames");
    
    gui->addButton("clear all key frames", [slider,textBox]() { key_frames.clear();key_frames.push_back(default_kf);})\
    ->setTooltip("clear all key frames except for the first one");
    
}

