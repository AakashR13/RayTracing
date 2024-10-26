#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include "glm/gtc/type_ptr.hpp"

// #include <thread>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) 
	{
		m_Scene.SkyLight = glm::vec3{ 0.6f, 0.7f, 0.9f };

		Light* not_skyLight = new Light(glm::vec3{ -1.0f, -1.0f,-1.0f }, glm::vec3{ 0.6f, 0.7f, 0.9f });
		m_Scene.Lights.emplace_back(not_skyLight);

		Diffuse* material_ground = new Diffuse(glm::vec3{ 0.8f, 0.8f, 0.0f });
		m_Scene.Materials.emplace_back(material_ground);

		Diffuse* material_center = new Diffuse(glm::vec3{ 0.1f, 0.2f, 0.5f });
		m_Scene.Materials.emplace_back(material_center);

		Metal* material_left = new Metal(glm::vec3{ 0.8f, 0.8f, 0.8f },0.0f);
		m_Scene.Materials.emplace_back(material_left);

		Metal* material_right = new Metal(glm::vec3{ 0.8f, 0.6f, 0.2f }, 0.0f);
		m_Scene.Materials.emplace_back(material_right);

		{
			Sphere* sphere = new Sphere({ 0.0f, -100.5f, -1.0f }, 100.0f, 0);
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere* sphere = new Sphere({ 0.0f, 0.0f, -1.2f }, 0.5f, 1);
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere* sphere = new Sphere({ -1.0f, 0.0f, -1.0f }, 0.5f, 2);
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere* sphere = new Sphere({ 1.0f, 0.0f, -1.0f }, 0.5f, 3);
			m_Scene.Spheres.push_back(sphere);
		}
	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render: %.3fms", m_LastRenderTime);
		// ImGui::Text("Threads: %d", std::thread::hardware_concurrency());
		if (ImGui::Button("Render"))
		{
			Render();
		}
  
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset"))
		{
			m_Renderer.ResetFrameIndex();
		}
		ImGui::End();

		ImGui::Begin("Scene");
		ImGui::Text("Lights");
		ImGui::ColorEdit3("SkyLight Color", glm::value_ptr(m_Scene.SkyLight));

		for(size_t i =0; i<m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::Text("Object %d:", i);
			Sphere* sphere = m_Scene.Spheres[i];
			Material* material = m_Scene.Materials[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere->Position), 0.01f);
			ImGui::DragFloat("Radius", &sphere->Radius, 0.01f);
			// ImGui::DragInt("MatIndex", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material->Albedo));
			switch (material->matType) {
				case materialType::DiffuseMat:
					ImGui::Text("Diffuse");
				break;
				case materialType::MetalMat:
					ImGui::Text("Metal");
					ImGui::DragFloat("Roughness", &static_cast<Metal*>(material)->Roughness, 0.05f, 0.0f, 1.0f);
				break;
				case materialType::DialectricMat:
					ImGui::Text("Dialectric");
					ImGui::DragFloat("Roughness", &static_cast<Dialectric*>(material)->Roughness, 0.05f, 0.0f, 1.0f);
					ImGui::DragFloat("Refractive Index", &static_cast<Dialectric*>(material)->Refract_ind, 0.05f, 1.0f, FLT_MAX);
				break;
				case materialType::EmissiveMat:
					ImGui::Text("Emissive");
					ImGui::ColorEdit3("Emission Color", glm::value_ptr(static_cast<Emissive*>(material)->EmissiveColor), 0.01f);
					ImGui::DragFloat("Emission Power", &static_cast<Emissive*>(material)->EmissivePower, 0.05f, 0.0f, FLT_MAX);
				break;
				case materialType::None:
				break;
				default:
				break;
			}

			ImGui::Separator();
			ImGui::PopID();
		}

			ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if(image)
			ImGui::Image(image->GetDescriptorSet(), 
						{ (float)image->GetWidth(), (float)image->GetHeight() }, 
						ImVec2(0,1),ImVec2(1,0));//Flips the uv(image)*
		
		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render() 
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene,m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Halide";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
};