#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "Material.h"

enum MaterialClass
{
	LambertianMat,
	MetalMat,
	DialectricMat,
	None,
};

struct Material
{
	MaterialClass type = None;
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;

	glm::vec3 EmissionColor{ 0.0f };
	float EmissionPower = 0.0f;

	Material() = default;

	Material(glm::vec3 Albedo, float Roughness, float Metallic):
		Albedo(Albedo), Roughness(Roughness), Metallic(Metallic) {}

	glm::vec3 GetEmission() const { return EmissionColor * EmissionPower; }

	virtual bool scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const = 0;

	virtual ~Material() = default;
};

struct Lambertian : public Material
{
	Lambertian():
		Material() {}
	Lambertian(glm::vec3 Albedo, float Roughness, float Metallic):
		Material(Albedo, Roughness, Metallic) {}

	bool scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const override;
};

struct Metal : public Material
{
	Metal():
		Material() {}
	Metal(glm::vec3 Albedo, float Roughness, float Metallic):
		Material(Albedo, Roughness, Metallic) {}

	bool scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const override;
};

struct Dialectric : public Material
{

	float index_of_refraction = 1.0f;
	Dialectric():
		Material() {}
	Dialectric(glm::vec3 Albedo, float Roughness, float Metallic, float IR):
		Material(Albedo, Roughness, Metallic), index_of_refraction(IR) {}

	bool scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const override;

	glm::vec3 refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat);
};

struct Sphere
{
	Sphere(glm::vec3 Pos, float rad, int MatInd):
		Position(Pos), Radius(rad), MaterialIndex(MatInd) {}

	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;
	int MaterialIndex = 0;
};
struct Light
{
	Light(glm::vec3 direction, glm::vec3 color):
		Dir(direction), Col(color) {}

	glm::vec3 Dir{-1.0f};
	glm::vec3 Col{ 0.5f };
};
struct Scene
{
	std::vector<Light*> Lights;
	std::vector<Sphere*> Spheres;
	std::vector<Material*> Materials;

	bool addObject(MaterialClass type);
	~Scene();
};

