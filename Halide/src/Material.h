#pragma once

#include "Utils.h"
#include "HitPayload.h"
#include "Ray.h"

// Make a proper constructor
//struct Material
//{
//
//	glm::vec3 Albedo{ 1.0f };
//	float Roughness = 1.0f;
//	float Metallic = 0.0f;
//
//	glm::vec3 EmissionColor{ 0.0f };
//	float EmissionPower = 0.0f;
//
//	virtual glm::vec3 GetEmission() const { return EmissionColor * EmissionPower; }
//
//	virtual ~Material() = default;
//	virtual bool scatter(const Ray& r_in, const HitPayload& payload, Ray& r_out, uint32_t& seed) const = 0;
//};
//
//class lambertian : public Material
//{
//	bool scatter(const Ray& r_in, const HitPayload& payload, Ray& r_out, uint32_t& seed) const override
//	{
//		r_out.Origin = glm::vec3(payload.WorldPosition + payload.WorldNormal * 0.0001f); // Deals with Floating point error
//
//		r_out.Direction = glm::normalize(payload.WorldNormal + Utils::InUnitSphere(seed));
//		return true;
//	}
//};
//
//class metal : public Material
//{
//	bool scatter(const Ray& r_in, const HitPayload& payload, class Ray& r_out, uint32_t& seed) const override
//	{
//		r_out.Origin = glm::vec3(payload.WorldPosition + payload.WorldNormal * 0.0001f); // Deals with Floating point error
//
//		r_out.Direction = glm::reflect(r_in.Direction, payload.WorldNormal + Roughness * Utils::RandomVec3(seed, -0.5f, 0.5f));
//		return true;
//	}
//};