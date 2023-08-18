#pragma once

#include "Walnut/Image.h"

#include "Camera.h"
#include "Ray.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
		Renderer() = default;

		void OnResize(uint32_t width, uint32_t height);
		void Render(const Camera& camera);
		// void Render(float col[4]);

		~Renderer();

		std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; };

private:
	glm::vec4 TraceRay(const Ray& ray);
private:
	std::shared_ptr < Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
	// glm::vec3 sphereColor = glm::vec3(1, 0, 1);

};

