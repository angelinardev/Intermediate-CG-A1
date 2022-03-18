#include "DefaultSceneLayer.h"

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

#include <filesystem>

// Graphics
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"
#include "Graphics/Framebuffer.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Components/SteeringBehaviour.h"
#include "Gameplay/Components/FollowBehaviour.h"

#include "Gameplay/Components/PlayerMovementBehavior.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/InputEngine.h"

#include "Application/Application.h"
#include "Gameplay/Components/ParticleSystem.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Textures/Texture1D.h"


#include "Application/Layers/RenderLayer.h"

DefaultSceneLayer::DefaultSceneLayer() :
	ApplicationLayer()
{
	Name = "Default Scene";
	Overrides = AppLayerFunctions::OnAppLoad | AppLayerFunctions::OnUpdate;
}

DefaultSceneLayer::~DefaultSceneLayer() = default;

void DefaultSceneLayer::OnAppLoad(const nlohmann::json& config) {
	_CreateScene();
}
// Grab current time as the previous frame
double lastFrame = glfwGetTime();


bool on1 = true;
bool on2 = true;
bool on3 = true;

bool sepia = false;
bool cool = false;
bool warm = false;

bool lights = true;

bool new_shader = true;

bool spec_ramp = false;
bool diff_ramp = false;



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) return;
	if (key == GLFW_KEY_4)
	{
		on1 = !on1;
		std::cout << "toggle diffuse" << on1 <<std::endl;
	}
	if (key == GLFW_KEY_3)
	{
		on2 = !on2;
		std::cout << "toggle spec " << on2 << std::endl;
	}
	if (key == GLFW_KEY_2) {
		on3 = !on3;
		std::cout << "toggle ambient " << on3 << std::endl;
	}
	if (key == GLFW_KEY_0)
	{
		sepia = !sepia;
		std::cout << "toggle sepia lut " << sepia << std::endl;
	}
	if (key == GLFW_KEY_9)
	{
		cool = !cool;
		std::cout << "toggle cool lut " << cool << std::endl;
	}
	if (key == GLFW_KEY_8)
	{
		warm = !warm;
		std::cout << "toggle warm lut " << warm << std::endl;
	}
	if (key == GLFW_KEY_5)
	{
		new_shader = !new_shader;
		std::cout << "new shader " << new_shader << std::endl;
	}
	if (key == GLFW_KEY_7)
	{
		spec_ramp = !spec_ramp;
		std::cout << "toggle spec ramp " << spec_ramp << std::endl;
		
	}
	if (key == GLFW_KEY_6)
	{
		diff_ramp =  !diff_ramp;
		std::cout << "toggle diffuse ramp " << diff_ramp << std::endl;
		
	}
	if (key == GLFW_KEY_1)
	{
		lights = !lights;
		std::cout << "toggle lights " << lights << std::endl;
	}
	
}


void DefaultSceneLayer::OnUpdate()
{
	Application& app = Application::Get();
	_currentScene = app.CurrentScene();
	

	// Figure out the current time, and the time since the last frame
	double thisFrame = glfwGetTime();
	float dt = static_cast<float>(thisFrame - lastFrame);
	//fetch resources
	if (!activated)
	{
		slimeMaterial = _currentScene->FindObjectByName("Foliage Sphere")->Get<RenderComponent>()->GetMaterial();
		slimeMaterial2 = _currentScene->FindObjectByName("Foliage Sphere 2")->Get<RenderComponent>()->GetMaterial();
		planeMaterial = _currentScene->FindObjectByName("Plane")->Get<RenderComponent>()->GetMaterial();
		monkeyMaterial = _currentScene->FindObjectByName("Monkey 1")->Get<RenderComponent>()->GetMaterial();
		monkeyMaterial2 = _currentScene->FindObjectByName("Monkey 2")->Get<RenderComponent>()->GetMaterial();
		boxMaterial = _currentScene->FindObjectByName("Specular Object")->Get<RenderComponent>()->GetMaterial();
		boxMaterial2 = _currentScene->FindObjectByName("Specular Object 2")->Get<RenderComponent>()->GetMaterial();
		toyMaterial = _currentScene->FindObjectByName("Toy")->Get<RenderComponent>()->GetMaterial();
		toyMaterial2 = _currentScene->FindObjectByName("Toy2")->Get<RenderComponent>()->GetMaterial();
		all_objects.push_back(slimeMaterial);
		all_objects.push_back(slimeMaterial2);
		all_objects.push_back(planeMaterial);
		all_objects.push_back(monkeyMaterial);
		all_objects.push_back(monkeyMaterial2);
		all_objects.push_back(boxMaterial);
		all_objects.push_back(boxMaterial2);
		all_objects.push_back(toyMaterial);
		all_objects.push_back(toyMaterial2);
		//only run this once
		activated = true;

		glfwSetKeyCallback(app.GetWindow(), key_callback);
		
	}
	itime += 0.1;
	slimeMaterial->Set("iTime", itime);
	slimeMaterial2->Set("iTime", itime);
	
	//change diffuse
	if (!diff_ramp)
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("diff_ramp", false);
		}
	}
	else
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("diff_ramp", true);
		}
	}
	if (!spec_ramp)
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("spec_ramp", false);
		}
	}
	else
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("spec_ramp", true);
		}
	}
	if (new_shader)
	{
		slimeMaterial->Set("activated", true);
		slimeMaterial2->Set("activated", true);

	}
	else
	{
		slimeMaterial->Set("activated", false);
		slimeMaterial2->Set("activated", false);
	}
	if (on1)
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("toggle_diffuse", true);
		}
	
	}
	else
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("toggle_diffuse", false);
		}
		
	}

	if (on2)
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("toggle_specular", true);
		}
	}
	else
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("toggle_specular", false);
		}
	}
	if (on3)
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("toggle_ambient", true);
		}	
	}
	else
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			all_objects[i]->Set("toggle_ambient", false);
		}	
	}
	if (sepia)
	{
		_currentScene->SetColorLUT(lutsepia);
	}
	if (cool)
	{
		_currentScene->SetColorLUT(lutcool);
	}
	if (warm)
	{
		_currentScene->SetColorLUT(lutwarm);
	}
	
	RenderFlags flags = app.GetLayer<RenderLayer>()->GetRenderFlags();
	if (!sepia && !cool && !warm && lights) //only lights and not color
	{
		app.GetLayer<RenderLayer>()->SetRenderFlags(RenderFlags::EnableLights);
	}
	else if (!sepia && !cool && !warm) //not color and not light
	{
		app.GetLayer<RenderLayer>()->SetRenderFlags(RenderFlags::None);
	}
	else if (!lights) //not light and color
	{
		app.GetLayer<RenderLayer>()->SetRenderFlags(RenderFlags::EnableColorCorrection);
	}
	else //light and color
	{
		app.GetLayer<RenderLayer>()->SetRenderFlags(RenderFlags::EnableColorCorrection | RenderFlags::EnableLights);
	}

}
void DefaultSceneLayer::_CreateScene()
{
	using namespace Gameplay;
	using namespace Gameplay::Physics;

	Application& app = Application::Get();

	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	} else {
		// This time we'll have 2 different shaders, and share data between both of them using the UBO
		// This shader will handle reflective materials 
		ShaderProgram::Sptr reflectiveShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_environment_reflective.glsl" }
		});
		reflectiveShader->SetDebugName("Reflective");

		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr basicShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
		});
		basicShader->SetDebugName("Blinn-phong");

		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr slimeShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_texturedSLIME.glsl" }
		});
		slimeShader->SetDebugName("Slime");

		// This shader handles our foliage vertex shader example
		ShaderProgram::Sptr foliageShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/foliage.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/screendoor_transparency.glsl" }
		});
		foliageShader->SetDebugName("Foliage");


		// This shader handles our displacement mapping example
		ShaderProgram::Sptr displacementShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});
		displacementShader->SetDebugName("Displacement Mapping");

		// This shader handles our tangent space normal mapping
		ShaderProgram::Sptr tangentSpaceMapping = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});
		tangentSpaceMapping->SetDebugName("Tangent Space Mapping");

		
		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr specShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/textured_specular.glsl" }
		});


		// Load in the meshes
		MeshResource::Sptr monkeyMesh = ResourceManager::CreateAsset<MeshResource>("Monkey.obj");

		// Load in some textures
		Texture2D::Sptr    boxTexture   = ResourceManager::CreateAsset<Texture2D>("textures/box-diffuse.png");
		boxTextureMain = boxTexture;
		Texture2D::Sptr    boxSpec      = ResourceManager::CreateAsset<Texture2D>("textures/box-specular.png");
		Texture2D::Sptr    monkeyTex    = ResourceManager::CreateAsset<Texture2D>("textures/monkey-uvMap.png");
		Texture2D::Sptr    leafTex      = ResourceManager::CreateAsset<Texture2D>("textures/leaves.png");
		leafTex->SetMinFilter(MinFilter::Nearest);
		leafTex->SetMagFilter(MagFilter::Nearest);

		Texture2D::Sptr    brickTexture = ResourceManager::CreateAsset<Texture2D>("textures/terrain/grass.png");
		Texture2D::Sptr    brickTexture2 = ResourceManager::CreateAsset<Texture2D>("textures/bricks_diffuse.png");
		brickTextureMain = brickTexture2;

		// Loading in a 1D LUT
		Texture1D::Sptr toonLut = ResourceManager::CreateAsset<Texture1D>("luts/toon-1D.png"); 
		toonLut->SetWrap(WrapMode::ClampToEdge);

		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/ocean.jpg");
		ShaderProgram::Sptr      skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" }
		});

		// Create an empty scene
		Scene::Sptr scene = std::make_shared<Scene>(); 

		// Setting up our enviroment map
		scene->SetSkyboxTexture(testCubemap); 
		scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up 
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

		// Loading in a color lookup table
		Texture3D::Sptr lut = ResourceManager::CreateAsset<Texture3D>("luts/sepia.CUBE");  //MY CUSTOM
		lutsepia = lut;
		Texture3D::Sptr lut2 = ResourceManager::CreateAsset<Texture3D>("luts/cool.CUBE");  //MY CUSTOM
		lutcool = lut2;
		Texture3D::Sptr lut3 = ResourceManager::CreateAsset<Texture3D>("luts/warm.CUBE");  //MY CUSTOM
		lutwarm = lut3;

		// Configure the color correction LUT
		//scene->SetColorLUT(lut);

		// Create our materials
		// This will be our box material, with no environment reflections
		Material::Sptr boxMaterial = ResourceManager::CreateAsset<Material>(specShader);
		{
			boxMaterial->Name = "Box";
			boxMaterial->Set("u_Material.Diffuse", boxTexture);
			boxMaterial->Set("u_Material.Specular", boxSpec);
			boxMaterial->Set("u_Material.Shininess", 10.1f);
			
		}
		//brick material
		Material::Sptr brickMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			brickMaterial->Name = "Brick";
			brickMaterial->Set("u_Material.Diffuse", brickTexture);
			brickMaterial->Set("u_Material.Shininess", 10.1f);
		}

		// This will be the reflective material, we'll make the whole thing 90% reflective
		Material::Sptr monkeyMaterial = ResourceManager::CreateAsset<Material>(reflectiveShader);
		{
			monkeyMaterial->Name = "Monkey";
			monkeyMaterial->Set("u_Material.Diffuse", monkeyTex);
			monkeyMaterial->Set("u_Material.Shininess", 10.5f);
		}
		// This will be the reflective material, we'll make the whole thing 90% reflective
		Material::Sptr monkeyMaterial2 = ResourceManager::CreateAsset<Material>(basicShader);
		{
			monkeyMaterial2->Name = "Monkey";
			monkeyMaterial2->Set("u_Material.Diffuse", monkeyTex);
			monkeyMaterial2->Set("u_Material.Shininess", 10.5f);
		}
		Texture2D::Sptr    slimeTexture = ResourceManager::CreateAsset<Texture2D>("textures/goo.png");

		// This will be the reflective material, we'll make the whole thing 90% reflective
		Material::Sptr testMaterial = ResourceManager::CreateAsset<Material>(slimeShader);
		{
			testMaterial->Name = "Box-Specular";
			testMaterial->Set("u_Material.Diffuse", slimeTexture);
			//testMaterial->Set("u_Material.Specular", boxSpec);
			testMaterial->Set("iTime", 1.0f);
		}

		// Our foliage vertex shader material
		Material::Sptr foliageMaterial = ResourceManager::CreateAsset<Material>(foliageShader);
		{
			foliageMaterial->Name = "Foliage Shader";
			foliageMaterial->Set("u_Material.Diffuse", leafTex);
			foliageMaterial->Set("u_Material.Shininess", 10.1f);
			foliageMaterial->Set("u_Material.Threshold", 0.1f);

			foliageMaterial->Set("u_WindDirection", glm::vec3(1.0f, 1.0f, 0.0f));
			foliageMaterial->Set("u_WindStrength", 0.5f);
			foliageMaterial->Set("u_VerticalScale", 1.0f);
			foliageMaterial->Set("u_WindSpeed", 1.0f);
		}


		Material::Sptr displacementTest = ResourceManager::CreateAsset<Material>(displacementShader);
		{
			Texture2D::Sptr displacementMap = ResourceManager::CreateAsset<Texture2D>("textures/displacement_map.png");
			Texture2D::Sptr normalMap       = ResourceManager::CreateAsset<Texture2D>("textures/normal_map.png");
			Texture2D::Sptr diffuseMap      = ResourceManager::CreateAsset<Texture2D>("textures/bricks_diffuse.png");

			displacementTest->Name = "Displacement Map";
			displacementTest->Set("u_Material.Diffuse", diffuseMap);
			displacementTest->Set("s_Heightmap", displacementMap);
			displacementTest->Set("s_NormalMap", normalMap);
			displacementTest->Set("u_Material.Shininess", 10.5f);
			displacementTest->Set("u_Scale", 0.1f);
		}

		Material::Sptr normalmapMat = ResourceManager::CreateAsset<Material>(tangentSpaceMapping);
		{
			Texture2D::Sptr normalMap       = ResourceManager::CreateAsset<Texture2D>("textures/normal_map.png");
			Texture2D::Sptr diffuseMap      = ResourceManager::CreateAsset<Texture2D>("textures/bricks_diffuse.png");

			normalmapMat->Name = "Tangent Space Normal Map";
			normalmapMat->Set("u_Material.Diffuse", diffuseMap);
			normalmapMat->Set("s_NormalMap", normalMap);
			normalmapMat->Set("u_Material.Shininess", 10.5f);
			normalmapMat->Set("u_Scale", 0.1f);
		}
		
		// Create some lights for our scene
		scene->Lights.resize(3);
		scene->Lights[0].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = 100.0f;

		scene->Lights[1].Position = glm::vec3(-5.46f, -1.0f, 1.01f);
		scene->Lights[1].Color = glm::vec3(0.2f, 0.8f, 0.1f);
		scene->Lights[1].Range = 10.0f;

		scene->Lights[2].Position = glm::vec3(-2.6f, 0.0f, 0.0f);
		scene->Lights[2].Color = glm::vec3(1.0f, 0.2f, 0.1f);
		scene->Lights[2].Range = 10.0f;

		// We'll create a mesh that is a simple plane that we can resize later
		MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<MeshResource>();
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();

		MeshResource::Sptr sphere = ResourceManager::CreateAsset<MeshResource>();
		sphere->AddParam(MeshBuilderParam::CreateIcoSphere(ZERO, ONE, 5));
		sphere->GenerateMesh();

		// Set up the scene's camera
		GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		{
			camera->SetPostion({ -5.499, -9.633, 2.881 });
			camera->SetRotation(glm::vec3(86.8f, 0.0f, 0.0f));
			//camera->LookAt(glm::vec3(0.0f));

			camera->Add<SimpleCameraControl>();

			// This is now handled by scene itself!
			//Camera::Sptr cam = camera->Add<Camera>();
			// Make sure that the camera is set as the scene's main camera!
			//scene->MainCamera = cam;
		}

		// Set up all our sample objects
		GameObject::Sptr plane = scene->CreateGameObject("Plane");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(brickMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = plane->Add<RigidBody>(/*static by default*/);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({ 0,0,-1 });
			plane->SetPostion(glm::vec3(0.0f, 0.0f, 0.1f));
		}

		GameObject::Sptr monkey1 = scene->CreateGameObject("Monkey 1");
		{
			// Set position in the scene
			monkey1->SetPostion(glm::vec3(-5.46f, -1.0f, 1.01f));

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = monkey1->Add<RenderComponent>();
			renderer->SetMesh(monkeyMesh);
			renderer->SetMaterial(monkeyMaterial);

			// Example of a trigger that interacts with static and kinematic bodies as well as dynamic bodies
			TriggerVolume::Sptr trigger = monkey1->Add<TriggerVolume>();
			trigger->SetFlags(TriggerTypeFlags::Statics | TriggerTypeFlags::Kinematics);
			trigger->AddCollider(BoxCollider::Create(glm::vec3(1.0f)));

			monkey1->Add<TriggerVolumeEnterBehaviour>();

			RotatingBehaviour::Sptr behaviour2 = monkey1->Add<RotatingBehaviour>();
			behaviour2->RotationSpeed = glm::vec3(0.0f, 0.0f, 30.0f);
		}
		//player controller
		GameObject::Sptr monkey2 = scene->CreateGameObject("Monkey 2");
		{
			// Set position in the scene
			monkey2->SetPostion(glm::vec3(-5.46f, -1.0f, 1.01f));

			// Add some behaviour that relies on the physics body
			monkey2->Add<JumpBehaviour>();

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = monkey2->Add<RenderComponent>();
			renderer->SetMesh(monkeyMesh);
			renderer->SetMaterial(monkeyMaterial2);

			// Add a dynamic rigid body to this monkey
			Gameplay::Physics::RigidBody::Sptr physics = monkey2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Dynamic);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
			physics->AddCollider(box);

			Gameplay::Physics::TriggerVolume::Sptr volume = monkey2->Add<Gameplay::Physics::TriggerVolume>();
			Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
			volume->AddCollider(box2);

			PlayerMovementBehavior::Sptr movement = monkey2->Add<PlayerMovementBehavior>();
		}

		
		// Box to showcase the specular material
		GameObject::Sptr specBox = scene->CreateGameObject("Specular Object");
		{
			MeshResource::Sptr boxMesh = ResourceManager::CreateAsset<MeshResource>();
			boxMesh->AddParam(MeshBuilderParam::CreateCube(ZERO, ONE));
			boxMesh->GenerateMesh();

			// Set and rotation position in the scene
			specBox->SetPostion(glm::vec3(-8.44f, -4.92f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = specBox->Add<RenderComponent>();
			renderer->SetMesh(boxMesh);
			renderer->SetMaterial(boxMaterial);

		}
		GameObject::Sptr specBox2 = scene->CreateGameObject("Specular Object 2");
		{
			MeshResource::Sptr boxMesh2 = ResourceManager::CreateAsset<MeshResource>();
			boxMesh2->AddParam(MeshBuilderParam::CreateCube(ZERO, ONE));
			boxMesh2->GenerateMesh();

			// Set and rotation position in the scene
			specBox2->SetPostion(glm::vec3(-2.55f, -4.92f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = specBox2->Add<RenderComponent>();
			renderer->SetMesh(boxMesh2);
			renderer->SetMaterial(boxMaterial);

		}

		// sphere to showcase the foliage material
		GameObject::Sptr foliageBall = scene->CreateGameObject("Foliage Sphere");
		{
			// Set and rotation position in the scene
			foliageBall->SetPostion(glm::vec3(-8.44f, -4.92f, 1.94f));
			foliageBall->SetScale(glm::vec3(0.5f));

			// Add a render component
			RenderComponent::Sptr renderer = foliageBall->Add<RenderComponent>();
			renderer->SetMesh(sphere);
			renderer->SetMaterial(testMaterial);

			
			RotatingBehaviour::Sptr behaviour2 = foliageBall->Add<RotatingBehaviour>();
			behaviour2->RotationSpeed = glm::vec3(10.0f, 10.0f, 50.0f);
		}
		GameObject::Sptr foliageBall2 = scene->CreateGameObject("Foliage Sphere 2");
		{
			// Set and rotation position in the scene
			foliageBall2->SetPostion(glm::vec3(-2.54f, -4.92f, 1.88f));
			foliageBall2->SetScale(glm::vec3(0.5f));
			// Add a render component
			RenderComponent::Sptr renderer = foliageBall2->Add<RenderComponent>();
			renderer->SetMesh(sphere);
			renderer->SetMaterial(testMaterial);

			
			RotatingBehaviour::Sptr behaviour2 = foliageBall2->Add<RotatingBehaviour>();
			behaviour2->RotationSpeed = glm::vec3(10.0f, 10.0f, 50.0f);
		}
		//setup moving toy
		Gameplay::MeshResource::Sptr toyMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("toy.obj");
		Texture2D::Sptr toyTex = ResourceManager::CreateAsset<Texture2D>("textures/toy.jpg");
		// Create our material
		Gameplay::Material::Sptr toyMaterial = ResourceManager::CreateAsset<Gameplay::Material>(basicShader);
		{
			toyMaterial->Name = "Toy";
			toyMaterial->Set("u_Material.Diffuse", toyTex);
			toyMaterial->Set("u_Material.Shininess", 10.0f);

		}
		Gameplay::GameObject::Sptr toyM = scene->CreateGameObject("Toy");
		{
			//toyM->SetPostion(glm::vec3(-0.19f, -4.25f, -0.63f));
			toyM->SetPostion(glm::vec3(-2.6f, 0.0f, 0.0f));
			toyM->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			toyM->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));
			// Add a render component
			RenderComponent::Sptr renderer = toyM->Add<RenderComponent>();
			renderer->SetMesh(toyMesh);
			renderer->SetMaterial(toyMaterial);
			// Add a dynamic rigid body to this monkey
			Gameplay::Physics::RigidBody::Sptr physics = toyM->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
			box->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
			box->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
			physics->AddCollider(box);
			Gameplay::Physics::TriggerVolume::Sptr volume = toyM->Add<Gameplay::Physics::TriggerVolume>();
			Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
			box2->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
			box2->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
			volume->AddCollider(box2);
			SteeringBehaviour::Sptr behaviour2 = toyM->Add<SteeringBehaviour>();
			std::vector<glm::vec3> points;
			//points for catmull movement
			points.push_back(glm::vec3(-2.6f, 0.0f, 0.0f)); //-2.41
			points.push_back(glm::vec3(-9.59f, 0.0f, 0.0f));
			points.push_back(glm::vec3(-6.66f, -2.76f, 0.0f));
			points.push_back(glm::vec3(-3.08f, -3.07f, 0.0f));
			behaviour2->SetPoints(points);

		}
	
		Gameplay::GameObject::Sptr toyM2 = scene->CreateGameObject("Toy2");
		{
			toyM2->SetPostion(glm::vec3(-0.19f, -4.25f, -0.63f));
			toyM2->SetPostion(glm::vec3(-2.6f, 0.0f, 0.0f));
			toyM2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			toyM2->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));
			// Add a render component
			RenderComponent::Sptr renderer = toyM2->Add<RenderComponent>();
			renderer->SetMesh(toyMesh);
			renderer->SetMaterial(toyMaterial);
			// Add a dynamic rigid body to this monkey
			Gameplay::Physics::RigidBody::Sptr physics = toyM2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Kinematic);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();
			box->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
			box->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
			physics->AddCollider(box);
			Gameplay::Physics::TriggerVolume::Sptr volume = toyM2->Add<Gameplay::Physics::TriggerVolume>();
			Gameplay::Physics::BoxCollider::Sptr box2 = Gameplay::Physics::BoxCollider::Create();
			box2->SetPosition(glm::vec3(0.00f, 0.22f, 0.0f));
			box2->SetScale(glm::vec3(0.28f * 0.4f, 0.24f * 0.4f, 0.57f * 0.4f));
			volume->AddCollider(box2);
			FollowBehaviour::Sptr behaviour2 = toyM2->Add<FollowBehaviour>();
			behaviour2->SetTarget(toyM);

		}
		GameObject::Sptr particles = scene->CreateGameObject("Particles");
		{
			ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();  
			particleManager->AddEmitter(glm::vec3(-9.64f, -4.9f, 1.0f), glm::vec3(4.0f, 0.0f, 10.0f), 20.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)); 
			
		}

		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");

		// Send the scene to the application
		app.LoadScene(scene);
	}
}
