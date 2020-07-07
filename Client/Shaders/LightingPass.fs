#version 330 core

///Multiple Render Targets (MRT, can get multiple fs outputs in 1 render pass)
///Layout location specifier used (location corresponds to colour attachment of currently bound FBO the frag, that the fs is currently processing, will be rendered to [fs writes to...])
layout (location = 0) out vec4 fragColour;
layout (location = 1) out vec4 brightFragColour;

struct Mtl{
    float shininess; //Impacts the scattering of light and hence radius of the specular highlight
};

struct PtLight{ //Positional light source
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 pos;
    float constant; //Constant term //Makes sure the denominator >= 1.f
    float linear; //Linear term //Multiplied with dist to reduce light intensity in a linear fashion
    float quadratic; //Quadratic term //Multiplied with the quadrant of the dist to set quadratic decreases in light intensity //Less significant compared to linear term when dist is small
};

struct DirectionalLight{ //Directional light source
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 dir;
};

struct Spotlight{ //Positional light that shoots light rays in 1 dir, objs within its cut-offs (a certain radius of its dir) are lit
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 pos;
    vec3 dir;
    float cosInnerCutoff;
    float cosOuterCutoff;
};

in vec2 TexCoords;
#define maxAmtP 10
#define maxAmtD 10
#define maxAmtS 10

uniform vec3 globalAmbient;
uniform int pAmt;
uniform int dAmt;
uniform int sAmt;
uniform PtLight ptLights[maxAmtP];
uniform DirectionalLight directionalLights[maxAmtD];
uniform Spotlight spotlights[maxAmtS];

uniform Mtl mtl;
uniform vec3 camPos;
uniform sampler2D posTex;
uniform sampler2D normalsTex;
uniform sampler2D albedoSpecTex;

const float gamma = 2.2f;

vec3 CalcAmbient(vec3 ambient){
}

vec3 CalcDiffuse(vec3 lightDir){
}

vec3 CalcSpecular(vec3 lightDir){
}

vec3 CalcPtLight(PtLight light){
}

vec3 CalcDirectionalLight(DirectionalLight light){
}

vec3 CalcSpotlight(Spotlight light){
}





struct Light{
    vec3 Position;
    vec3 Color;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];


void main(){
    vec3 Pos = texture(posTex, TexCoords).rgb;
    vec3 Normal = texture(normalsTex, TexCoords).rgb;
    vec3 Albedo = texture(albedoSpecTex, TexCoords).rgb;
    float Spec = texture(albedoSpecTex, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting = Albedo * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(viewPos - FragPos);
    for(int i = 0; i < NR_LIGHTS; ++i){
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * lights[i].Color;
        lighting += diffuse;
    }

    if(!(pAmt * dAmt * sAmt)){
        fragColour = vec4(CalcAmbient(globalAmbient), 1.f);
        brightFragColour = vec4(vec3(0.f), 1.f);
    } else{
        fragColour = vec4(vec3(0.f), 1.f);
        for(int i = 0; i < pAmt; ++i){
            fragColour.rgb += CalcPtLight(ptLights[i]);
        }
        for(int i = 0; i < dAmt; ++i){
            fragColour.rgb += CalcDirectionalLight(directionalLights[i]);
        }
        for(int i = 0; i < sAmt; ++i){
            fragColour.rgb += CalcSpotlight(spotlights[i]);
        }
        float brightness = dot(fragColour.rgb, vec3(.2126f, .7152f, .0722f)); //Transform fragColour to grayscale with dot product
        brightFragColour = vec4(fragColour.rgb * vec3(float(brightness > 2.f)), 1.f); //2.f is brightness threshold (outside LDR with HDR rendering so more control over what is considered bright)
    }
}