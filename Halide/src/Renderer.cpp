#include "Renderer.h"

#include <execution>
#include <algorithm>

#include "Utils.h"


class HitPayload;

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage) {

		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;//Does the check for m_ImageData
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	m_ImageHorizontalIterator.resize(width);
	m_ImageVerticalIterator.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIterator[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIterator[i] = i;
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveCamera = &camera;
	m_ActiveScene = &scene;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));



	// Try implementing threadpool


	// ~2m -> 1920x1080
	// std::execution::par vs par_unseq?
	std::for_each(std::execution::par_unseq, m_ImageVerticalIterator.begin(), m_ImageVerticalIterator.end(),
		[this](uint32_t y)
		{
			std::for_each(std::execution::par_unseq, m_ImageHorizontalIterator.begin(), m_ImageHorizontalIterator.end(),
			[this, y](uint32_t x)
				{
					glm::vec4 color = PerPixel(x, y);
					m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

					glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
					accumulatedColor /= (float)m_FrameIndex;

					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
					m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
				});
		});



	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}



glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	glm::vec3 light(0.0f);
	glm::vec3 throughput(1.0f); //  also called contribution

	uint32_t seed = x + y * m_FinalImage->GetWidth();
	seed *= m_FrameIndex;
	int bounces = 10;
	for (int i = 0; i < bounces; i++)
	{
		seed += i;
		HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			auto skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			light += skyColor * throughput;
			break;
		}

		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
		float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f);  // == cos()

		const Sphere* sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material* material = m_ActiveScene->Materials[sphere->MaterialIndex];
		glm::vec3 sphereColor = material->Albedo;
		sphereColor *= lightIntensity;
		light += sphereColor * throughput;

		throughput *= material->Albedo;
		light += material->GetEmission();


		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f; // Deals with Floating point error

		material->scatter(ray, payload, seed);
		//ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + material->Roughness * Utils::RandomVec3(seed,-0.5f, 0.5f));

		//if (m_Settings.FastRandom)
		//	ray.Direction = glm::normalize(payload.WorldNormal + Utils::InUnitSphere(seed));
		//else
		//	ray.Direction = glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere());
	}

	return glm::vec4(light, 1.0f);
}

HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;


	const Sphere* closestSphere = m_ActiveScene->Spheres[objectIndex];
	glm::vec3 origin = ray.Origin - closestSphere->Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere->Position;

	return payload;
}

HitPayload Renderer::Miss(const Ray& ray)
{
	HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}

HitPayload Renderer::TraceRay(const Ray& ray)
{
	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit radius

	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere* sphere = m_ActiveScene->Spheres[i];
		glm::vec3 origin = ray.Origin - sphere->Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(ray.Direction, origin);
		float c = glm::dot(origin, origin) - sphere->Radius * sphere->Radius;

		// Quadratic formula discriminant
		// (b^2 - 4ac)

		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0)
			continue;

		// (-b +- sqrt(discriminant)) / 2a

		// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (closestT > 0.0f && closestT < hitDistance)
		{
			hitDistance = closestT;
			closestSphere = (int)i;
		}
	}

	if (closestSphere < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::~Renderer()
{
}