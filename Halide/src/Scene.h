#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Ray.h"
#include <string>
#include "Utils.h"
#include "HitPayload.h"

// Define materialType before any references to it
enum class materialType {
    DiffuseMat = 0,
    MetalMat = 1,
    DialectricMat = 2,
    EmissiveMat = 3,
    None = -1
};

// Base Material struct
struct Material
{
public:
    glm::vec3 Albedo{ 1.0f };
    materialType matType = materialType::None;

    Material() = default;
    Material(glm::vec3 albedo, materialType type = materialType::None)
        : Albedo(albedo), matType(type) {}

    virtual ~Material() = default;
    virtual bool scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const = 0;
};

// Derived Diffuse material
struct Diffuse : public Material
{
    Diffuse(glm::vec3 albedo)
        : Material(albedo, materialType::DiffuseMat) {}

    bool scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const override;
};

// Derived Metal material with roughness
struct Metal : public Material
{
    float Roughness = 0.0f;

    Metal(glm::vec3 albedo, float roughness)
        : Material(albedo, materialType::MetalMat), Roughness(roughness) {}

    bool scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const override;
};

// Derived Dialectric material with refraction index
struct Dialectric : public Material
{
    float Roughness = 0.0f;
    float Refract_ind = 0.0f;

    Dialectric(glm::vec3 albedo, float roughness, float IR)
        : Material(albedo, materialType::DialectricMat), Roughness(roughness), Refract_ind(IR) {}

    bool scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const override;
    glm::vec3 refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat);
};

// Derived Emissive material for light-emitting objects
struct Emissive : public Material
{
    glm::vec3 EmissiveColor{ 1.0f };
    float EmissivePower = 1.0f;

    Emissive(glm::vec3 albedo, glm::vec3 emissiveColor, float emissivePower)
        : Material(albedo, materialType::EmissiveMat), EmissiveColor(emissiveColor), EmissivePower(emissivePower) {}

    bool scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const override;
};

// Sphere struct with position, radius, and material index
struct Sphere
{
    glm::vec3 Position{ 0.0f };
    float Radius = 0.5f;
    int MaterialIndex = 0;

    Sphere(glm::vec3 pos, float rad, int matInd)
        : Position(pos), Radius(rad), MaterialIndex(matInd) {}
};

// Light struct with direction and color
struct Light
{
    glm::vec3 Dir{ -1.0f };
    glm::vec3 Col{ 0.5f };

    Light(glm::vec3 direction, glm::vec3 color)
        : Dir(direction), Col(color) {}
};

// Scene struct with vectors of lights, spheres, and materials
struct Scene
{
    std::vector<Light*> Lights;
    std::vector<Sphere*> Spheres;
    std::vector<Material*> Materials;
    glm::vec3 SkyLight;

    void add();
};
