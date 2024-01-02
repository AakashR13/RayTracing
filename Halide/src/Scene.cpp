#include "Scene.h"


bool Lambertian::scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const
{
	//ray.Origin = glm::vec3(payload.WorldPosition + payload.WorldNormal * 0.0001f); // Deals with Floating point error

	ray.Direction = glm::normalize(payload.WorldNormal + Utils::InUnitSphere(seed));
	return true;
}

bool Metal::scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const
{
	//ray.Origin = glm::vec3(payload.WorldPosition + payload.WorldNormal * 0.0001f); // Deals with Floating point error

	ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + Roughness * Utils::RandomVec3(seed, -0.5f, 0.5f));
	return true;
}


bool Dialectric::scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const
{
	return true;
}

glm::vec3 Dialectric::refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat)
{
	float cos_theta = fmin(glm::dot(-uv, n), 1.0);
	glm::vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	glm::vec3 r_out_par = -(float)sqrt(fabs(1.0 - glm::length(r_out_perp) * glm::length(r_out_perp))) * n;
	return r_out_par + r_out_perp;
}


bool Scene::addObject(MaterialClass type) 
{
	switch (type)
	{
	case LambertianMat:
	{
		auto object = new Lambertian();
		Materials.emplace_back(object);
		auto sphere = new Sphere({0.0f, 0.0f, 0.0f}, 1.0f, Materials.size()-1);
		Spheres.emplace_back(sphere);
		return true;
		break;
	}
	case MetalMat:
	{
		auto object = new Metal();
		Materials.emplace_back(object);
		auto sphere = new Sphere({ 0.0f, 0.0f, 0.0f }, 1.0f, Materials.size() - 1);
		Spheres.emplace_back(sphere);
		return true;
		break;
	}
	case DialectricMat:
	{
		auto object = new Dialectric();
		Materials.emplace_back(object);
		auto sphere = new Sphere({ 0.0f, 0.0f, 0.0f }, 1.0f, Materials.size() - 1);
		Spheres.emplace_back(sphere);
		return true;
		break;
	}
	default:
		return false;
		break;
	}
}

Scene::~Scene()
{
	for (auto light : Lights) {
		delete light;
	}
	Lights.clear();

	for (auto sphere : Spheres) {
		delete sphere;
	}
	Spheres.clear();

	for (auto material : Materials) {
		delete material;
	}
	Materials.clear();
}
