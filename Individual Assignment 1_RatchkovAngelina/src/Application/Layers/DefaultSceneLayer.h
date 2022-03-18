#pragma once
#include "Application/ApplicationLayer.h"
#include "json.hpp"
#include <GLM/glm.hpp>
#include "Utils/Macros.h"
#include "Gameplay/Scene.h"
#include "Gameplay/MeshResource.h"
#include "Gameplay/Material.h" 


/**
 * This example layer handles creating a default test scene, which we will use 
 * as an entry point for creating a sample scene
 */
class DefaultSceneLayer final : public ApplicationLayer {
public:
	MAKE_PTRS(DefaultSceneLayer)

	DefaultSceneLayer();
	virtual ~DefaultSceneLayer();

	// Inherited from ApplicationLayer
	void OnUpdate() override;

	virtual void OnAppLoad(const nlohmann::json& config) override;

protected:
	void _CreateScene();
	// The current scene that the application is working on

	Gameplay::Material::Sptr slimeMaterial;
	Gameplay::Material::Sptr slimeMaterial2;

	Gameplay::Material::Sptr planeMaterial;
	Gameplay::Material::Sptr monkeyMaterial;
	Gameplay::Material::Sptr monkeyMaterial2;
	Gameplay::Material::Sptr boxMaterial;
	Gameplay::Material::Sptr boxMaterial2;
	Gameplay::Material::Sptr toyMaterial;
	Gameplay::Material::Sptr toyMaterial2;

	Texture2D::Sptr    boxTextureMain;
	Texture2D::Sptr    brickTextureMain;

	std::vector<Gameplay::Material::Sptr> all_objects;
	Gameplay::Scene::Sptr _currentScene;
	bool activated = false;

	bool run_once = false;


	float itime = 0;

	Texture3D::Sptr lutsepia;
	Texture3D::Sptr lutcool;
	Texture3D::Sptr lutwarm;

};