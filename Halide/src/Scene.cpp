#include "Scene.h"
class Renderer;
class Ray;
bool Diffuse::scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const {
	ray.Direction = glm::normalize(payload.WorldNormal + Utils::InUnitSphere(seed));
	return true;
}

bool Metal::scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const {
	ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + Roughness * Utils::RandomVec3(seed, -0.5f, 0.5f));
	ray.Direction = glm::normalize(ray.Direction);
	return (glm::dot(ray.Direction, payload.WorldNormal) > 0);
}

bool Dialectric::scatter(Ray& ray, const HitPayload& payload, uint32_t& seed) const {
	return true;
}

glm::vec3 Dialectric::refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat)
{
	float cos_theta = fmin(glm::dot(-uv, n), 1.0);
	glm::vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	glm::vec3 r_out_par = -(float)sqrt(fabs(1.0 - glm::length(r_out_perp) * glm::length(r_out_perp))) * n;
	return r_out_par + r_out_perp;
}