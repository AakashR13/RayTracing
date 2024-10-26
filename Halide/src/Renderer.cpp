#include "Renderer.h"

#include "Walnut/Random.h"

#include <execution>


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
	for (uint32_t i=0; i < height; i++)
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
	std::for_each(std::execution::par, m_ImageVerticalIterator.begin(), m_ImageVerticalIterator.end(),
		[this](uint32_t y)
		{
			std::for_each(std::execution::par, m_ImageHorizontalIterator.begin(), m_ImageHorizontalIterator.end(),
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

	int bounces = 15;
	for (int i = 0; i < bounces; i++)
	{
		seed += i;
		HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			glm::vec3 unit_direction = glm::normalize(ray.Direction);
			float a = 0.5f * (unit_direction.y + 1.0f);
			light += ((1.0f - a) * glm::vec3(1.0f, 1.0f, 1.0f) + a * glm::vec3(0.5f, 0.7f, 1.0f)) * throughput;

			//light += m_ActiveScene->SkyLight * throughput;
			break;
		}

		const Sphere* sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material* material = m_ActiveScene->Materials[sphere->MaterialIndex];

		throughput *= material->Albedo;	
		//light += material.GetEmission();  

		if (!material->scatter(ray, payload, seed)) {
			light = glm::vec3(0.0f);
			break;
		}

	}
#define GM 1
#if GM
	return glm::vec4(linearToGamma(light), 1.0f);
#else
	return glm::vec4(light, 1.0f);
#endif

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
	for (size_t i =0;i < m_ActiveScene->Spheres.size();i++)
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

//
//void Renderer::Render(const Scene& scene, const Camera& camera)
//{
//	m_ActiveCamera = &camera;
//	m_ActiveScene = &scene;
//
//	if (m_FrameIndex == 1)
//		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));
//
//
//
//	// Try implementing threadpool
//	// ~2m -> 1920x1080
//	std::for_each(std::execution::par, m_ImageVerticalIterator.begin(), m_ImageVerticalIterator.end(),
//		[this](uint32_t y)
//		{
//			std::for_each(std::execution::par, m_ImageHorizontalIterator.begin(), m_ImageHorizontalIterator.end(),
//			[this, y](uint32_t x)
//				{
//					glm::vec4 color(0.0f); // Accumulate color for this pixel across samples
//					for (int s = 0; s < m_SamplesPerPixel; s++)
//					{
//						color += PerPixel(x, y); // Jittered PerPixel
//					}
//					color /= static_cast<float>(m_SamplesPerPixel); // Average samples
//					m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;
//
//					glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
//					accumulatedColor /= (float)m_FrameIndex;
//
//					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
//					m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
//				});
//		});
//
//
//
//	m_FinalImage->SetData(m_ImageData);
//
//	if (m_Settings.Accumulate)
//		m_FrameIndex++;
//	else
//		m_FrameIndex = 1;
//}
//
//
//
//glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
//{
//	Ray ray;
//	ray.Origin = m_ActiveCamera->GetPosition();
//	uint32_t seed = x + y * m_FinalImage->GetWidth();
//
//	// Get the ray direction for the center of the pixel
//	glm::vec3 pixelCenter = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];
//
//	// Jitter values for anti-aliasing (e.g., in the range of -0.5 to 0.5)
//	float jitterX = Utils::RandomFloat(seed, -0.5f, 0.5f);
//	float jitterY = Utils::RandomFloat(seed, -0.5f, 0.5f);
//
//	// Calculate the jittered pixel position
//	pixelCenter += glm::vec3(jitterX / m_FinalImage->GetWidth(), jitterY / m_FinalImage->GetHeight(), 0.0f);
//
//	ray.Direction = glm::normalize(pixelCenter);
//
//	glm::vec3 light(0.0f);
//	glm::vec3 throughput(1.0f); // also called contribution
//
//	seed *= m_FrameIndex;
//
//	int bounces = 15;
//	for (int i = 0; i < bounces; i++)
//	{
//		seed += i;
//		HitPayload payload = TraceRay(ray);
//		if (payload.HitDistance < 0.0f)
//		{
//			glm::vec3 unit_direction = glm::normalize(ray.Direction);
//			float a = 0.5f * (unit_direction.y + 1.0f);
//			light += ((1.0f - a) * glm::vec3(1.0f, 1.0f, 1.0f) + a * glm::vec3(0.5f, 0.7f, 1.0f)) * throughput;
//			break;
//		}
//
//		const Sphere* sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
//		const Material* material = m_ActiveScene->Materials[sphere->MaterialIndex];
//
//		throughput *= material->Albedo;
//		// light += material.GetEmission();  
//
//		if (!material->scatter(ray, payload, seed)) {
//			light = glm::vec3(0.0f);
//			break;
//		}
//	}
//
//#define GM 0
//#if GM
//	return glm::vec4(linearToGamma(light), 1.0f);
//#else
//	return glm::vec4(light, 1.0f);
//#endif
//
//}
