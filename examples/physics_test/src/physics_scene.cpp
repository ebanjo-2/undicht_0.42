#include "physics_scene.h"
#include "scene_loader/texture_loader.h"
#include "scene_loader/scene_loader.h"
#include "renderer/vulkan/transfer_buffer.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/quaternion.hpp>
#include "file_tools.h"

#include "debug.h"

#include "physics.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

using namespace undicht;
using namespace graphics;
using namespace vulkan;
using namespace tools;
using namespace physics;

using namespace JPH;

PhysicsScene::PhysicsScene() :
    _physics_allocator(10 * 1024 * 1024),
    _physics_job_system(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1) {

}


void PhysicsScene::init(const LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, DescriptorSetCache& material_descriptor_cache, DescriptorSetCache& node_descriptor_cache, const Sampler& sampler) {

    initGraphics(device, allocator, material_descriptor_cache, node_descriptor_cache, sampler);
    initPhysics();

}

void PhysicsScene::cleanUp() {

    _graphics_scene.cleanUp();

    _body_interface->RemoveBodies(_body_ids.data(), _body_ids.size());
    _body_interface->DestroyBodies(_body_ids.data(), _body_ids.size());

}

void PhysicsScene::updatePhysics() {

    // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
    const float delta_time = 1.0f / 60.0f;

	// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
	const int collision_steps = 1;

	// Step the world
	_physics_system.Update(delta_time, collision_steps, &_physics_allocator, &_physics_job_system);

}

void PhysicsScene::updateGraphics(const LogicalDevice& device, vma::VulkanMemoryAllocator& allocator, DescriptorSetCache& node_descriptor_cache, TransferBuffer& transfer_buffer) {

    SceneGroup& scene_group = _graphics_scene.addGroup("physics");

    // update the graphics scene to represent the physics scene
    for(int i = 0; i < _body_ids.size(); i++) {

        std::string node_name = "physics_body_" + toStr(i);
        
        // make sure there is a node to represent the body
        if(scene_group.getRootNode().getChildNodeCount() == i) {
            scene_group.getRootNode().addChildNode(node_name, device, allocator, node_descriptor_cache);
        }

        // choose the correct mesh and size for the body
        if(_body_interface->GetShape(_body_ids.at(i)).GetPtr()->GetSubType() == EShapeSubType::Sphere) {

            scene_group.getRootNode().getChildNode(node_name)->setMesh("Sphere-mesh"); // should be the sphere mesh
        } else if(_body_interface->GetShape(_body_ids.at(i)).GetPtr()->GetSubType() == EShapeSubType::Box) {

            scene_group.getRootNode().getChildNode(node_name)->setMesh("Cube-mesh"); // should be the cube mesh
        } else {

            UND_WARNING << "unknown physics shape cant be represented\n";
        }

        // get the world transform
        RVec3 jph_position = _body_interface->GetCenterOfMassPosition(_body_ids.at(i));
        Quat jph_rotation = _body_interface->GetRotation(_body_ids.at(i));

        // create the model matrix with glm
        glm::vec3 position(jph_position.GetX(), jph_position.GetY(), jph_position.GetZ());
        glm::quat rotation(jph_rotation.GetW(), jph_rotation.GetX(), jph_rotation.GetY(), jph_rotation.GetZ());
        glm::mat4 model_matrix = glm::translate(position) * glm::toMat4(rotation) * glm::scale(_body_half_sizes.at(i));

        scene_group.getRootNode().getChildNode(node_name)->setLocalTransformation(model_matrix);
        scene_group.updateGlobalTransformations(); // should really be only called once
        scene_group.getRootNode().getChildNode(node_name)->updateUniformBuffer(transfer_buffer, scene_group);
    }

}

undicht::graphics::Scene& PhysicsScene::getScene() {

    return _graphics_scene;
}

const BodyID& PhysicsScene::addSphere(const glm::vec3& position, float radius, bool movable) {

    RVec3 jph_position(position.x, position.y, position.z);
    EMotionType e_motion_type = movable ? EMotionType::Dynamic : EMotionType::Static;
    ObjectLayer layer = movable ? layers::MOVING : layers::NON_MOVING;
    EActivation activation = movable ? EActivation::Activate : EActivation::DontActivate;

    SphereShapeSettings shape_settings(radius);
	ShapeSettings::ShapeResult shape_result = shape_settings.Create();
    ShapeRefC shape = shape_result.Get();
	BodyCreationSettings sphere_settings(shape, jph_position, Quat::sIdentity(), e_motion_type, layer);
	Body *sphere = _body_interface->CreateBody(sphere_settings); // Note that if we run out of bodies this can return nullptr
	
    assert(sphere);

    // Add it to the world
	_body_interface->AddBody(sphere->GetID(), activation);

    // store the body id
    _body_ids.push_back(sphere->GetID());
    _body_half_sizes.push_back(glm::vec3(radius));

    return sphere->GetID();
}

const BodyID& PhysicsScene::addBox(const glm::vec3& position, const glm::vec3& half_size, bool movable) {

    RVec3 jph_position(position.x, position.y, position.z);
    EMotionType e_motion_type = movable ? EMotionType::Dynamic : EMotionType::Static;
    ObjectLayer layer = movable ? layers::MOVING : layers::NON_MOVING;
    EActivation activation = movable ? EActivation::Activate : EActivation::DontActivate;

    BoxShapeSettings shape_settings(Vec3Arg(half_size.x, half_size.y, half_size.z));
	ShapeSettings::ShapeResult shape_result = shape_settings.Create();
    ShapeRefC shape = shape_result.Get();
	BodyCreationSettings box_settings(shape, jph_position, Quat::sIdentity(), e_motion_type, layer);
	Body *box = _body_interface->CreateBody(box_settings); // Note that if we run out of bodies this can return nullptr

    assert(box);

	// Add it to the world
	_body_interface->AddBody(box->GetID(), activation);

    // store the body id
    _body_ids.push_back(box->GetID());
    _body_half_sizes.push_back(half_size);

    return box->GetID();
}


/////////////////////////////////// internal functions ////////////////////////////////

void PhysicsScene::initGraphics(const undicht::vulkan::LogicalDevice& device, undicht::vma::VulkanMemoryAllocator& allocator, undicht::vulkan::DescriptorSetCache& material_descriptor_cache, undicht::vulkan::DescriptorSetCache& node_descriptor_cache, const undicht::vulkan::Sampler& sampler) {

    _graphics_scene.init();

    // init objects to upload data to the gpu
    TransferBuffer transfer_buffer;
    transfer_buffer.init(allocator, {device.getGraphicsQueueFamily()}, 2000000);
    CommandBuffer transfer_command;
    transfer_command.init(device.getDevice(), device.getGraphicsCmdPool());

    // init scene meshes + textures
    SceneLoader scene_loader;
    scene_loader.setInitObjects(device, allocator, transfer_buffer, material_descriptor_cache, node_descriptor_cache, sampler);
    scene_loader.importScene("res/sphere.dae", _graphics_scene.addGroup("physics"));
    scene_loader.importScene("res/cube.dae", _graphics_scene.addGroup("physics"));

    // upload data
    transfer_command.beginCommandBuffer(true);
    transfer_buffer.completeTransfers(transfer_command);
    _graphics_scene.genMipMaps(transfer_command);
    transfer_command.endCommandBuffer();
    device.submitOnGraphicsQueue(transfer_command.getCommandBuffer());
    device.waitGraphicsQueueIdle();

    // clear nodes created by the scene loader
    _graphics_scene.getGroup("physics")->getRootNode().clearChildNodes();

    // cleanup
    transfer_buffer.cleanUp();
    transfer_command.cleanUp();
}

void PhysicsScene::initPhysics() {

    // init physics
    _physics_system.Init(1024, 0, 1024, 1024, _broad_phase_layer_interface, _object_vs_broadphase_layer_filter, _object_vs_object_layer_filter);
    //_physics_system.SetContactListener(&_contact_listener); // prints debug info
    //_physics_system.SetBodyActivationListener(&_activation_listener); // prints debug info
    _body_interface = &_physics_system.GetBodyInterface();

    // init objects
    const BodyID& sphere0 = addSphere(glm::vec3(1.0f, 6.0f, 0.2f), 2.0f, true);
    const BodyID& sphere1 = addSphere(glm::vec3(0.0f, 2.0f, 0.0f), 0.8f, true);
    const BodyID& sphere2 = addSphere(glm::vec3(0.5f, 0.0f, -0.3f), 0.5f, true);
    const BodyID& sphere3 = addSphere(glm::vec3(0.0f, 2.0f, 3.0f), 1.3f, true);
    const BodyID& sphere4 = addSphere(glm::vec3(2.5f, 8.0f, -0.3f), 0.7f, true);
    const BodyID& box0 = addBox(glm::vec3(0.0f, 9.0f, 0.0f), glm::vec3(1.5f, 1.5f, 1.5f), true);
    const BodyID& box1 = addBox(glm::vec3(3.0f, 9.0f, 3.0f), glm::vec3(1.0f, 1.0f, 1.0f), true);
    const BodyID& box2 = addBox(glm::vec3(3.0f, 9.0f, -3.0f), glm::vec3(1.0f, 0.5f, 1.5f), true);

    const BodyID& bounding_box0 = addBox(glm::vec3(0.0f, -5.5f, 0.0f), glm::vec3(11.0f, 0.5f, 11.0f), false);
    const BodyID& bounding_box1 = addBox(glm::vec3(10.5f, -4.0f, 0.0f), glm::vec3(0.5f, 1.0f, 10.0f), false);
    const BodyID& bounding_box2 = addBox(glm::vec3(-10.5f, -4.0f, 0.0f), glm::vec3(0.5f, 1.0f, 10.0f), false);
    const BodyID& bounding_box3 = addBox(glm::vec3(0.0f, -4.0f, 10.5f), glm::vec3(10.0f, 1.0f, 0.5f), false);
    const BodyID& bounding_box4 = addBox(glm::vec3(0.0f, -4.0f, -10.5f), glm::vec3(10.0f, 1.0f, 0.5f), false);

    // _body_interface->SetLinearVelocity(sphere0, Vec3Arg(0.0f, -500.0f, 0.0f));
    // _physics_system.SetGravity(Vec3Arg(0.0f, -0.81f, 0.0f));

	_physics_system.OptimizeBroadPhase();

}