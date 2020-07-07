#pragma once
#include "../Core.h"

class Light{
public:
    enum struct LightType{
        Pt = 0,
        Directional,
        Spot,
    };
    static glm::vec3 globalAmbient; //Affects brightness of unlit scene
    glm::vec3 ambient; //Affects colour and intensity of ambient component of light
    glm::vec3 diffuse; //...
    glm::vec3 specular; //...
    static Light* CreateLight(const LightType& type){
        switch(type){
            case LightType::Pt:
                return new PtLight();
            case LightType::Directional:
                return new DirectionalLight();
            case LightType::Spot:
                return new Spotlight();
        }
    }
protected:
    Light():
        ambient(glm::vec3(.05f)),
        diffuse(glm::vec3(.8f)),
        specular(glm::vec3(1.f))
    {
    }
    virtual ~Light() = default;
};

glm::vec3 Light::globalAmbient = glm::vec3(.1f);

struct PtLight final: Light{
    PtLight(): PtLight(glm::vec3(0.f), 1.f, .09f, .032f){}
    PtLight(const glm::vec3& myPos, const float& myConstant, const float& myLinear, const float& myQuadratic):
        pos(myPos),
        constant(myConstant),
        linear(myLinear),
        quadratic(myQuadratic)
    {
    }
    glm::vec3 pos;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight final: Light{
    DirectionalLight(): DirectionalLight(glm::vec3(0.f, -1.f, 0.f)){}
    DirectionalLight(const glm::vec3& myDir):
        dir(myDir)
    {
    }
    glm::vec3 dir;
};

struct Spotlight final: Light{
    Spotlight(): Spotlight(glm::vec3(0.f), glm::vec3(0.f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))){}
    Spotlight(const glm::vec3& myPos, const glm::vec3& myDir, const float& myCosInnerCutoff, const float& myCosOuterCutoff):
        pos(myPos),
        dir(myDir),
        cosInnerCutoff(myCosInnerCutoff),
        cosOuterCutoff(myCosOuterCutoff)
    {
    }
    glm::vec3 pos;
    glm::vec3 dir;
    float cosInnerCutoff;
    float cosOuterCutoff;
};