#version 330 core
uniform vec3 lightColor;
uniform vec3 hairColor;


in vec3 tangent;
in vec3 lightDir;
in vec3 viewDir;

out vec4 FragColor;

void main(){
    //reverse the tangent cuz the direction was misunderstood..
    vec3 T = -normalize(tangent);
    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewDir);
    
    float TdotL = dot(T,L);
    float TdotV = dot(T,V);
    
    // The diffuse component
    float kajiyaDiff = sin(acos(TdotL));
    if (kajiyaDiff < 0) { kajiyaDiff = 0; }
    kajiyaDiff = pow(kajiyaDiff, 10);
    
    // The specular component
    float kajiyaSpec = cos(abs(acos(TdotL) - acos(-TdotV)));
    
    if (kajiyaSpec < 0) { kajiyaSpec = 0; }
    kajiyaSpec = pow(kajiyaSpec, 1000);
    
    // The fragment color
    FragColor = vec4(hairColor * 0.6 +
                      hairColor * 0.5 * kajiyaDiff + 0.7*
                      lightColor * kajiyaSpec, 0.725);
}

