/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Shibboleth_DebugManager.h"
#include "Shibboleth_DebugAttributes.h"
#include <Shibboleth_ECSComponentCommon.h>
#include <Shibboleth_RenderManagerBase.h>
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_InputManager.h>
#include <Shibboleth_ECSManager.h>
#include <Shibboleth_GameTime.h>
#include <Gleam_MeshGeneration.h>
#include <gtx/euler_angles.hpp>
#include <gtx/transform.hpp>
#include <imgui.h>

namespace
{
	static Shibboleth::ProxyAllocator g_allocator;

	static void* ImGuiAlloc(size_t size, void*)
	{
		return SHIB_ALLOC(size, g_allocator);
	}

	static void ImGuiFree(void* ptr, void*)
	{
		SHIB_FREE(ptr, g_allocator);
	}

	static constexpr const char* const g_imgui_vertex_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			cbuffer vertexBuffer : register(b0)
			{
				float4x4 projection_matrix;
			};

			struct VS_INPUT
			{
				float2 pos : POSITION;
				float4 col : COLOR0;
				float2 uv  : TEXCOORD0;
			};

			struct PS_INPUT
			{
				float4 pos : SV_POSITION;
				float4 col : COLOR0;
				float2 uv  : TEXCOORD0;
			};

			PS_INPUT VertexMain(VS_INPUT input)
			{
				PS_INPUT output;
				output.pos = mul(projection_matrix, float4(input.pos.xy, 0.f, 1.f));
				output.col = input.col;
				output.uv  = input.uv;
				return output;
			}
		)",
		R"(
			$TODO: NEED TO IMPLEMENT!
		)"
	};

	static constexpr const char* const g_imgui_pixel_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			struct PS_INPUT
			{
				float4 pos : SV_POSITION;
				float4 col : COLOR0;
				float2 uv  : TEXCOORD0;
			};

			Texture2D texture0;
			sampler sampler0;

			float4 PixelMain(PS_INPUT input) : SV_Target
			{
				float4 out_col = input.col * texture0.Sample(sampler0, input.uv); 
				return out_col; 
			}
		)",
		R"(
		)"
	};

	static constexpr const char* const g_line_vertex_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			cbuffer InstanceBuffer
			{
				float4x4 view_proj_matrix;
			};

			struct InstanceData
			{
				float3 color;
				float3 start;
				float3 end;
			};

			struct VertexInputType
			{
				uint instance_id : SV_InstanceID;
				uint vertex_id : SV_VertexID;
			};

			struct PixelInputType
			{
				float4 position : SV_POSITION;
				float3 color : COLOR0;
			};

			StructuredBuffer<InstanceData> instance_data;

			PixelInputType VertexMain(VertexInputType input)
			{
				PixelInputType output;

				float vert_scale = float(input.vertex_id);

				float3 line_pos = instance_data[input.instance_id].start +
					instance_data[input.instance_id].end * float3(vert_scale, vert_scale, vert_scale);

				output.position = mul(view_proj_matrix, float4(line_pos, 1.0));
				output.color = instance_data[input.instance_id].color;

				return output;
			}
		)",
		R"(
			$TODO: NEED TO IMPLEMENT!
		)"
	};

	static constexpr const char* const g_debug_vertex_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			struct InstanceData
			{
				float4x4 mvp;
				float3 color;
			};

			struct VertexInputType
			{
				float3 position : POSITION;
				uint instance_id : SV_InstanceID;
			};

			struct PixelInputType
			{
				float4 position : SV_POSITION;
				float3 color : COLOR0;
			};

			StructuredBuffer<InstanceData> instance_data;

			PixelInputType VertexMain(VertexInputType input)
			{
				PixelInputType output;

				output.position = mul(instance_data[input.instance_id].mvp, float4(input.position, 1.0));
				output.color = instance_data[input.instance_id].color;

				return output;
			}
		)",
		R"(
			$TODO: NEED TO IMPLEMENT!
		)"
	};
	static constexpr const char* const g_debug_pixel_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			struct PixelInputType
			{
				float4 position : SV_POSITION;
				float3 color : COLOR0;
			};

			float4 PixelMain(PixelInputType input) : SV_TARGET0
			{
				return float4(input.color, 1.0);
			}
		)",
		R"(
			$TODO: NEED TO IMPLEMENT!
		)"
	};

	struct VertexConstantBuffer final
	{
		float mvp[4][4];
	};

	static constexpr int32_t k_num_instances_per_buffer = 32;
}

GAFF_STATIC_FILE_FUNC
{
	ImGui::SetAllocatorFunctions(ImGuiAlloc, ImGuiFree);
}

SHIB_REFLECTION_DEFINE_BEGIN(DebugManager)
	.BASE(IDebugManager)
	.base<IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugManager)

SHIB_REFLECTION_DEFINE_BEGIN(DebugRenderSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugRenderSystem)

SHIB_REFLECTION_DEFINE_BEGIN(DebugSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DebugRenderSystem)
SHIB_REFLECTION_CLASS_DEFINE(DebugManager)
SHIB_REFLECTION_CLASS_DEFINE(DebugSystem)

static void HandleKeyboardInput(Gleam::IInputDevice*, int32_t key_code, float value)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key_code] = value > 0.0f;

	if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTSHIFT)) ||
		(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTSHIFT))) {

		io.KeyShift = value > 0.0f;

	} else if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTCONTROL)) ||
				(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTCONTROL))) {

		io.KeyCtrl = value > 0.0f;

	} else if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTALT)) ||
				(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTALT))) {
		io.KeyAlt = value > 0.0f;

	} else if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTWINDOWS)) ||
				(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTWINDOWS))) {

		io.KeySuper = value > 0.0f;
	}
}

static void HandleMouseInput(Gleam::IInputDevice*, int32_t mouse_event, float value)
{
	ImGuiIO& io = ImGui::GetIO();

	if (mouse_event < static_cast<int32_t>(Gleam::MouseCode::MOUSE_BUTTON_COUNT)) {
		if (mouse_event < 5) {
			io.MouseDown[mouse_event] = (value > 0.0f);
		}

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_WHEEL_HORIZ)) {
		io.MouseWheelH = value;

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_WHEEL_VERT)) {
		io.MouseWheel = value;

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_POS_X)) {
		io.MousePos.x = value;

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_POS_Y)) {
		io.MousePos.y = value;
	}
}

void DebugManager::HandleKeyboardCharacter(Gleam::IKeyboard*, uint32_t character)
{
	DebugManager& dbg_mgr = GetApp().getManagerTFast<DebugManager>();
	dbg_mgr._character_buffer[dbg_mgr._char_buffer_cache_index].emplace_back(character);
}

void DebugManager::RenderDebugShape(uintptr_t thread_id_int, void* data)
{
	DebugRenderJobData& job_data = *reinterpret_cast<DebugRenderJobData*>(data);
	auto& debug_data = job_data.debug_mgr->_debug_data.instance_data[static_cast<int32_t>(job_data.type)];

	const int32_t num_cameras = static_cast<int32_t>(job_data.debug_mgr->_camera.size());
	glm::mat4x4 final_camera = glm::identity<glm::mat4x4>();
	EntityID camera_id = EntityID_None;

	// Nothing to render.
	if (!debug_data.render_list[0].size() && !debug_data.render_list[1].size()) {
		return;
	}

	for (int32_t i = 0; i < num_cameras; ++i) {
		job_data.debug_mgr->_ecs_mgr->iterate<Position, Rotation, Camera>(
			job_data.debug_mgr->_camera_position[i],
			job_data.debug_mgr->_camera_rotation[i],
			job_data.debug_mgr->_camera[i],
			[&](EntityID id, const Position& position, const Rotation& rotation, const Camera& camera) -> void
			{
				constexpr Gaff::Hash32 main_tag = Gaff::FNV1aHash32Const("main");

				if (camera.device_tag == main_tag) {
					const glm::ivec2 size = job_data.debug_mgr->_main_output->getSize();
					const glm::mat4x4 projection = glm::perspectiveFovLH(
						camera.GetVerticalFOV() * Gaff::TurnsToRad,
						static_cast<float>(size.x),
						static_cast<float>(size.y),
						camera.z_near, camera.z_far
					);

					const glm::vec3 euler_angles = rotation.value * Gaff::TurnsToRad;
					glm::mat4x4 camera_transform = glm::yawPitchRoll(euler_angles.y, euler_angles.x, euler_angles.z);
					camera_transform[3] = glm::vec4(position.value, 1.0f);

					final_camera = projection * glm::inverse(camera_transform);
					camera_id = id;
				}
			}
		);
	}

	if (camera_id < 0) {
		// $TODO: Log error.
		return;
	}

	const auto* const devices = job_data.debug_mgr->_render_mgr->getDevicesByTag("main");
	GAFF_ASSERT(devices && devices->size() > 0);

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	Gleam::IRenderDevice* const rd = job_data.debug_mgr->_render_mgr->getDeferredDevice(*devices->front(), thread_id);
	GAFF_REF(rd);

	const RenderManagerBase::GBufferData* const g_buffer = job_data.debug_mgr->_render_mgr->getGBuffer(camera_id, *devices->front());

	if (!g_buffer) {
		// $TODO: Log error.
		return;
	}

	if (job_data.type == DebugRenderType::Line) {
		Gleam::IBuffer* const const_buffer =
			job_data.debug_mgr->_debug_data.instance_data[static_cast<int32_t>(job_data.type)].constant_buffer.get();

		void* const buffer = const_buffer->map(*rd);

		if (buffer) {
			glm::mat4x4* const view_proj_matrix = reinterpret_cast<glm::mat4x4*>(buffer);
			*view_proj_matrix = final_camera;

			const_buffer->unmap(*rd);
		}
	}

	RenderManagerBase::RenderCommandList* const cmds[] = {
		&job_data.debug_mgr->_render_mgr->getRenderCommands(
			*job_data.debug_mgr->_main_device,
			RenderManagerBase::RenderOrder::InWorldNoDepthTest,
			job_data.debug_mgr->_render_cache_index
		),
		&job_data.debug_mgr->_render_mgr->getRenderCommands(
			*job_data.debug_mgr->_main_device,
			RenderManagerBase::RenderOrder::InWorldWithDepthTest,
			job_data.debug_mgr->_render_cache_index
		)
	};

	for (int32_t i = 0; i < 2; ++i) {
		const int32_t num_buffers_needed = static_cast<int32_t>(
			ceilf(static_cast<float>(debug_data.render_list[i].size()) / k_num_instances_per_buffer)
		);

		// Nothing to render.
		if (!num_buffers_needed) {
			continue;
		}

		int32_t secondary_buffer_size_scalar = 0;

		switch (job_data.type) {
			case DebugRenderType::Capsule:
				secondary_buffer_size_scalar = 2;
				break;

			case DebugRenderType::Arrow:
				secondary_buffer_size_scalar = 1;
				break;

			default:
				break;
		}

		const int32_t size_scalar = (secondary_buffer_size_scalar > 0) ? 2 : 1;

		// Make sure we have enough buffers on the GPU to hold our instance data.
		while (static_cast<int32_t>(debug_data.instance_data[i].size() / size_scalar) < num_buffers_needed) {
			Gleam::IBuffer* const buffer = job_data.debug_mgr->_render_mgr->createBuffer();

			if (!buffer) {
				// $TODO: Log error.
				continue;
			}

			const int32_t data_size = (job_data.type == DebugRenderType::Line) ?
				static_cast<int32_t>(sizeof(Gleam::Color::RGB) + sizeof(glm::vec3) * 2) :
				static_cast<int32_t>(sizeof(glm::mat4x4) + sizeof(Gleam::Color::RGB));

			Gleam::IBuffer::Settings settings = {
				nullptr,
				static_cast<size_t>(data_size) * static_cast<size_t>(k_num_instances_per_buffer),
				data_size,
				data_size,
				Gleam::IBuffer::Type::StructuredData,
				Gleam::IBuffer::MapType::Write,
				true
			};

			if (!buffer->init(*job_data.debug_mgr->_main_device, settings)) {
				SHIB_FREET(buffer, g_allocator);
				// $TODO: Log error.
				continue;
			}

			Gleam::IShaderResourceView* const view = job_data.debug_mgr->_render_mgr->createShaderResourceView();

			if (!view) {
				SHIB_FREET(buffer, g_allocator);
				// $TODO: Log error.
				continue;
			}

			if (!view->init(*job_data.debug_mgr->_main_device, buffer)) {
				SHIB_FREET(buffer, g_allocator);
				SHIB_FREET(view, g_allocator);
				// $TODO: Log error.
				continue;
			}

			Gleam::IShaderResourceView* view2 = nullptr;
			Gleam::IBuffer* buffer2 = nullptr;

			if (secondary_buffer_size_scalar > 0) {
				buffer2 = job_data.debug_mgr->_render_mgr->createBuffer();

				if (!buffer2) {
					// $TODO: Log error.
					continue;
				}

				settings.size = static_cast<size_t>(data_size) * static_cast<size_t>(k_num_instances_per_buffer) * secondary_buffer_size_scalar;

				if (!buffer2->init(*job_data.debug_mgr->_main_device, settings)) {
					SHIB_FREET(buffer, g_allocator);
					SHIB_FREET(view, g_allocator);
					SHIB_FREET(buffer2, g_allocator);

					// $TODO: Log error.
					continue;
				}

				view2 = job_data.debug_mgr->_render_mgr->createShaderResourceView();

				if (!view2) {
					SHIB_FREET(buffer, g_allocator);
					SHIB_FREET(view, g_allocator);
					SHIB_FREET(buffer2, g_allocator);

					// $TODO: Log error.
					continue;
				}

				if (!view2->init(*job_data.debug_mgr->_main_device, buffer2)) {
					SHIB_FREET(buffer, g_allocator);
					SHIB_FREET(view, g_allocator);
					SHIB_FREET(buffer2, g_allocator);
					SHIB_FREET(view2, g_allocator);

					// $TODO: Log error.
					continue;
				}
			}

			debug_data.instance_data_view[i].emplace_back(view);
			debug_data.instance_data[i].emplace_back(buffer);

			if (buffer2 && view2) {
				debug_data.instance_data_view[i].emplace_back(view2);
				debug_data.instance_data[i].emplace_back(buffer2);
			}
		}

		g_buffer->render_target->bind(*rd);
		job_data.debug_mgr->_debug_data.depth_stencil_state[i]->bind(*rd);
		job_data.debug_mgr->_debug_data.raster_state[i]->bind(*rd);
		job_data.debug_mgr->_blend_state->bind(*rd);

		if (job_data.type == DebugRenderType::Line) {
			job_data.debug_mgr->_debug_data.line_program->bind(*rd);
		} else {
			job_data.debug_mgr->_debug_data.program->bind(*rd);
			job_data.debug_mgr->_debug_data.layout->bind(*rd);
		}

		int32_t total_items = static_cast<int32_t>(debug_data.render_list[i].size());
		int32_t curr_index = 0;

		for (int32_t j = 0; j < static_cast<int32_t>(debug_data.instance_data[i].size() / size_scalar); ++j) {
			const int32_t count = Gaff::Min(total_items, k_num_instances_per_buffer);
			int8_t* mapped_buffer = reinterpret_cast<int8_t*>(debug_data.instance_data[i][j * size_scalar]->map(*rd));

			if (!mapped_buffer) {
				// $TODO: Log error.
				continue;
			}

			int8_t* secondary_mapped_buffer = nullptr;

			if (size_scalar > 1) {
				secondary_mapped_buffer = reinterpret_cast<int8_t*>(debug_data.instance_data[i][j * size_scalar + 1]->map(*rd));

				if (!secondary_mapped_buffer) {
					// $TODO: Log error.
					continue;
				}
			}

			// Update instance buffers(s).
			for (int32_t k = 0; k < count; ++k) {
				const DebugRenderInstance& inst = *debug_data.render_list[i][curr_index];

				switch (job_data.type) {
					case DebugRenderType::Line: {
						constexpr size_t k_instance_size = 2 * sizeof(glm::vec3) + sizeof(Gleam::Color::RGB);

						memcpy(mapped_buffer, &inst.color, sizeof(Gleam::Color::RGB));
						memcpy(mapped_buffer + sizeof(glm::vec3), &inst.transform.getTranslation(), sizeof(glm::vec3));
						memcpy(mapped_buffer + 2 * sizeof(glm::vec3), &inst.transform.getScale(), sizeof(glm::vec3));

						mapped_buffer += k_instance_size;
					} break;

					case DebugRenderType::Capsule: {
						constexpr size_t k_instance_size = sizeof(glm::mat4x4) + sizeof(Gleam::Color::RGB);

						// Copy data for cylinder.
						glm::mat4x4 transform = final_camera * inst.transform.toMatrix();

						memcpy(mapped_buffer, &transform, sizeof(glm::mat4x4));
						memcpy(mapped_buffer + sizeof(glm::mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						// Copy data for top half-sphere of capsule.
						const glm::vec3& scale = inst.transform.getScale(); // scale = (radius, height, radius)
						Gleam::Transform modified_transform = inst.transform;

						modified_transform.setScale(1.0f);
						const glm::mat4x4 sphere_base_transform = modified_transform.toMatrix();

						transform = glm::scale(glm::vec3(scale.x));
						transform[3] = glm::vec4(0.0f, scale.y * 0.5f, 0.0f, 1.0f);

						transform = final_camera * sphere_base_transform * transform;

						memcpy(secondary_mapped_buffer, &transform, sizeof(glm::mat4x4));
						memcpy(secondary_mapped_buffer + sizeof(glm::mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						secondary_mapped_buffer += k_instance_size;

						// Copy data for bottom half-sphere of capsule.
						transform = glm::eulerAngleX(Gaff::Pi); // Flip upside down.
						transform[3] = glm::vec4(0.0f, scale.y * -0.5f, 0.0f, 1.0f);
						transform = glm::scale(transform, glm::vec3(scale.x));

						transform = final_camera * sphere_base_transform * transform;

						memcpy(secondary_mapped_buffer, &transform, sizeof(glm::mat4x4));
						memcpy(secondary_mapped_buffer + sizeof(glm::mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						secondary_mapped_buffer += k_instance_size;
						mapped_buffer += k_instance_size;
					} break;

					case DebugRenderType::Arrow: {
						constexpr size_t k_instance_size = sizeof(glm::mat4x4) + sizeof(Gleam::Color::RGB);

						// Copy data for cylinder.
						const glm::vec3& dir = inst.transform.getScale();
						glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
						const float length = glm::length(dir);

						if (const auto result = glm::equal(dir, up, Gaff::Epsilon); result.x == true && result.y == true && result.z == true) {
							up = glm::vec3(0.0f, 0.0f, 1.0f);
						}

						constexpr float k_cylinder_scale = 0.025f;

						const glm::mat4x4 look_at = glm::lookAt(glm::zero<glm::vec3>(), dir, up);
						const glm::mat4x4 rotate_forward = glm::eulerAngleX(Gaff::Pi * 0.5f); // Orient top facing forward;

						glm::mat4x4 transform = 
							final_camera *
							glm::translate(inst.transform.getTranslation() + dir * 0.5f) *
							look_at *
							glm::scale(rotate_forward, glm::vec3(k_cylinder_scale, length, k_cylinder_scale));

						memcpy(mapped_buffer, &transform, sizeof(glm::mat4x4));
						memcpy(mapped_buffer + sizeof(glm::mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						// Copy data for cone.
						constexpr float k_cone_scale = 0.085f;

						transform =
							final_camera *
							glm::translate(inst.transform.getTranslation() + dir + glm::normalize(dir) * k_cone_scale * 0.5f) *
							look_at *
							glm::scale(rotate_forward, glm::vec3(k_cone_scale));

						memcpy(secondary_mapped_buffer, &transform, sizeof(glm::mat4x4));
						memcpy(secondary_mapped_buffer + sizeof(glm::mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						secondary_mapped_buffer += k_instance_size;
						mapped_buffer += k_instance_size;
					} break;

					default: {
						const glm::mat4x4 transform = final_camera * inst.transform.toMatrix();

						memcpy(mapped_buffer, &transform, sizeof(glm::mat4x4));
						memcpy(mapped_buffer + sizeof(glm::mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						mapped_buffer += sizeof(glm::mat4x4) + sizeof(Gleam::Color::RGB);
					} break;
				}

				++curr_index;
			}

			debug_data.instance_data[i][j * size_scalar]->unmap(*rd);

			if (secondary_mapped_buffer) {
				debug_data.instance_data[i][j * size_scalar + 1]->unmap(*rd);
			}

			switch (job_data.type) {
				case DebugRenderType::Line:
					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j].get());
					debug_data.program_buffers->bind(*rd);

					rd->renderLineNoVertexInputInstanced(count);
					break;

				case DebugRenderType::Capsule:
					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j * 2].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[0]->renderInstanced(*rd, count);

					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j * 2 + 1].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[1]->renderInstanced(*rd, count * 2);
					break;

				case DebugRenderType::Arrow:
					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j * 2].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[0]->renderInstanced(*rd, count);

					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j * 2 + 1].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[1]->renderInstanced(*rd, count);
					break;

				default:
					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[0]->renderInstanced(*rd, count);
					break;
			}

			total_items -= k_num_instances_per_buffer;
		}

		if (!rd->finishCommandList(*job_data.cmd_list[job_data.debug_mgr->_render_cache_index][i])) {
			// $TODO: Log error
			continue;
		}

		cmds[i]->lock.Lock();
		auto& cmd = cmds[i]->command_list.emplace_back();
		cmd.cmd_list.reset(job_data.cmd_list[job_data.debug_mgr->_render_cache_index][i].get());
		cmd.owns_command = false;
		cmds[i]->lock.Unlock();
	}
}

void DebugManager::SetupModuleToUseImGui(void)
{
	// Go through IDebugManager so that we get DebugModule's ImGui context.
	const IDebugManager& dbg_mgr = GetApp().GETMANAGERT(IDebugManager, DebugManager);
	ImGui::SetCurrentContext(dbg_mgr.getImGuiContext());
}

DebugManager::~DebugManager(void)
{
	ImGui::DestroyContext();
}

bool DebugManager::initAllModulesLoaded(void)
{
	_time = &GetApp().getManagerTFast<GameTimeManager>().getRealTime();
	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_input_mgr = &GetApp().getManagerTFast<InputManager>();
	_main_output = _render_mgr->getOutput("main");

	GAFF_ASSERT(_main_output);

	const auto* const devices = _render_mgr->getDevicesByTag("main");
	GAFF_ASSERT(devices && devices->size() == 1);

	_main_device = devices->front();

	ECSQuery camera_query;
	camera_query.add<Position>(_camera_position);
	camera_query.add<Rotation>(_camera_rotation);
	camera_query.add<Camera>(_camera);

	_ecs_mgr = &GetApp().getManagerTFast<ECSManager>();
	_ecs_mgr->registerQuery(std::move(camera_query));

	return initDebugRender() && initImGui();
}

void DebugManager::update(void)
{
	constexpr Gaff::Hash32 k_debug_menu_toggle = Gaff::FNV1aHash32Const("Debug_Menu_Toggle");
	constexpr Gaff::Hash32 k_debug_input_mode = Gaff::FNV1aHash32Const("Debug");
	const float toggle = _input_mgr->getAliasValue(k_debug_menu_toggle, _input_mgr->getKeyboardMousePlayerID());

	if (toggle > 0.0f) {
		if (_flags.toggle(Flag::ShowDebugMenu)) {
			_input_mgr->setMode(k_debug_input_mode);
		} else {
			_input_mgr->setModeToPrevious();
		}
	}

	const glm::ivec2& size = _main_output->getSize();

	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = _time->getDeltaFloat();
	io.DisplaySize.x = static_cast<float>(size.x);
	io.DisplaySize.y = static_cast<float>(size.y);

	const ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();

	// Update mouse cursor.
	if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
		if (_prev_cursor != mouse_cursor) {
			_prev_cursor = mouse_cursor;

			if (mouse_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
				Gleam::SetCursor(Gleam::Cursor::None);

			} else {
				Gleam::Cursor cursor = Gleam::Cursor::Arrow;

				switch (mouse_cursor)
				{
					case ImGuiMouseCursor_Arrow:
						cursor = Gleam::Cursor::Arrow;
						break;

					case ImGuiMouseCursor_TextInput:
						cursor = Gleam::Cursor::IBeam;
						break;

					case ImGuiMouseCursor_ResizeAll:
						cursor = Gleam::Cursor::SizeAll;
						break;

					case ImGuiMouseCursor_ResizeEW:
						cursor = Gleam::Cursor::SizeWE;
						break;

					case ImGuiMouseCursor_ResizeNS:
						cursor = Gleam::Cursor::SizeNS;
						break;

					case ImGuiMouseCursor_ResizeNESW:
						cursor = Gleam::Cursor::SizeNESW;
						break;

					case ImGuiMouseCursor_ResizeNWSE:
						cursor = Gleam::Cursor::SizeNWSE;
						break;

					case ImGuiMouseCursor_Hand:
						cursor = Gleam::Cursor::Hand;
						break;

					case ImGuiMouseCursor_NotAllowed:
						cursor = Gleam::Cursor::No;
						break;
				}

				Gleam::SetCursor(cursor);
			}
		}
	}

	const int32_t char_buf_index = _char_buffer_cache_index;
	_char_buffer_cache_index = (_char_buffer_cache_index + 1) % 2;

	for (uint32_t character : _character_buffer[char_buf_index]) {
		ImGui::GetIO().AddInputCharacter(character);
	}

	_character_buffer[char_buf_index].clear();

	ImGui::NewFrame();

	if (_flags.testAll(Flag::ShowDebugMenu)) {
		if (ImGui::BeginMainMenuBar()) {
			// Read debug menu bar list and call appropriate functions.
			if (ImGui::BeginMenu("Test Menu")) {
				ImGui::MenuItem("Test 1");
				ImGui::MenuItem("Test 2");

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Test Menu 2/Submenu")) {
				ImGui::MenuItem("Test 1");
				ImGui::MenuItem("Test 2");

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}

void DebugManager::render(uintptr_t thread_id_int)
{
	renderPostCamera(thread_id_int);
	renderPreCamera(thread_id_int);

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	GetApp().getJobPool().helpWhileWaiting(thread_id, _debug_data.job_counter);

	_render_cache_index = (_render_cache_index + 1) % 2;
}

ImGuiContext* DebugManager::getImGuiContext(void) const
{
	return ImGui::GetCurrentContext();
}

DebugManager::DebugRenderHandle DebugManager::renderDebugArrow(const glm::vec3& start, const glm::vec3& end, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Arrow)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(start);
	instance->transform.setScale(end - start);
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Arrow, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugLine(const glm::vec3& start, const glm::vec3& end, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Line)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(start);
	instance->transform.setScale(end - start);
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Line, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugSphere(const glm::vec3& pos, float radius, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Sphere)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(radius * 2.0f); // Sphere is unit sphere (radius = 0.5). Double radius to get correct scale.
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Sphere, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugCone(const glm::vec3& pos, const glm::vec3& size, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Cone)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(size);
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Cone, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugBox(const glm::vec3& pos, const glm::vec3& size, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Box)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(size);
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Box, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugCapsule(const glm::vec3& pos, float radius, float height, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Capsule)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(glm::vec3(radius * 2.0f, height, radius * 2.0f)); // Sphere/Cylinder is unit (radius = 0.5). Double radius to get correct scale.
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Capsule, has_depth);
}

void DebugManager::registerDebugMenuItems(void* object, const Gaff::IReflectionDefinition& ref_def)
{
	GAFF_REF(object, ref_def);

	//const ProxyAllocator allocator("Debug");
	//DebugMenuEntry* root = &_debug_menu_root;

	//Vector< eastl::pair<Gaff::Hash32, const DebugMenuItemAttribute&> > results(allocator);
	//ref_def.getVarAttrs<DebugMenuItemAttribute>(results);

	//DebugMenuEntry menu_entry;

	//for (const auto& entry : results) {
	//	Gaff::IReflectionVar* const var = ref_def.getVar(entry.first);

	//	if (!var->isFlags() && &var->getReflection().getReflectionDefinition() != &Reflection<bool>::GetReflectionDefinition()) {
	//		// Menu items only support flags and bools.
	//		// $TODO: Log error.
	//		continue;
	//	}

	//	const U8String& path = entry.second.getPath();
	//	size_t prev_index = 0;
	//	size_t index = path.find_first_of('/');

	//	while (index != U8String::npos) {
	//		menu_entry.name = path.substr(prev_index, index - prev_index);

	//		auto it = Gaff::LowerBound(root->children, menu_entry.name);

	//		if (it == root->children.end() || it->name != menu_entry.name) {
	//			it = root->children.insert(it, menu_entry);
	//		}

	//		prev_index = index;
	//		root = &(*it);

	//		index = path.find_first_of("/", index + 1);
	//	}

	//	menu_entry.name = path.substr(prev_index + 1);

	//	auto it = Gaff::LowerBound(root->children, menu_entry.name);
	//	U8String base_name = menu_entry.name.getString();
	//	int32_t i = 2;

	//	while (it != root->children.end() && it->name == menu_entry.name) {
	//		menu_entry.name = base_name + U8String(U8String::CtorSprintf(), " %i", i++);
	//		it = Gaff::LowerBound(root->children, menu_entry.name);
	//	}

	//	menu_entry.object = object;
	//	menu_entry.var = var;
	//}
}

void DebugManager::renderPostCamera(uintptr_t thread_id_int)
{
	ImGui::Render();
	const ImDrawData* const draw_data = ImGui::GetDrawData();

	// Avoid rendering when minimized
	if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f) {
		return;
	}

	if (draw_data->TotalIdxCount <= 0 || draw_data->TotalVtxCount <= 0) {
		return;
	}

	bool buffers_changed = false;

	if (const size_t size = (_vertex_buffer) ? _vertex_buffer->getSize() : 0;
		size < (draw_data->TotalVtxCount * sizeof(ImDrawVert))) {

		_vertex_buffer.reset(_render_mgr->createBuffer());
		buffers_changed = true;

		if (!_vertex_buffer) {
			// $TODO: Log error periodic.
			return;
		}

		const Gleam::IBuffer::Settings settings = {
			nullptr,
			draw_data->TotalVtxCount * sizeof(ImDrawVert),
			static_cast<int32_t>(sizeof(ImDrawVert)),
			static_cast<int32_t>(sizeof(ImDrawVert)),
			Gleam::IBuffer::Type::VertexData,
			Gleam::IBuffer::MapType::Write,
			true
		};

		if (!_vertex_buffer->init(*_main_device, settings)) {
			// $TODO: Log error periodic.
			return;
		}
	}

	if (const size_t size = (_index_buffer) ? _index_buffer->getSize() : 0;
		size < (draw_data->TotalIdxCount * sizeof(ImDrawIdx))) {

		_index_buffer.reset(_render_mgr->createBuffer());
		buffers_changed = true;

		if (!_index_buffer) {
			// $TODO: Log error periodic.
			return;
		}

		const Gleam::IBuffer::Settings settings = {
			nullptr,
			draw_data->TotalIdxCount * sizeof(ImDrawIdx),
			static_cast<int32_t>(sizeof(ImDrawIdx)),
			static_cast<int32_t>(sizeof(ImDrawIdx)),
			Gleam::IBuffer::Type::IndexData,
			Gleam::IBuffer::MapType::Write,
			true
		};

		if (!_index_buffer->init(*_main_device, settings)) {
			// $TODO: Log error periodic.
			return;
		}
	}

	if (buffers_changed) {
		_mesh->clear();

		if (_vertex_buffer) {
			_mesh->addBuffer(_vertex_buffer.get());
		}

		_mesh->setIndiceBuffer(_index_buffer.get());
	}

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	Gleam::IRenderDevice* const deferred_device = _render_mgr->getDeferredDevice(*_main_device, thread_id);

	ImDrawVert* verts = reinterpret_cast<ImDrawVert*>(_vertex_buffer->map(*deferred_device));
	ImDrawIdx* indices = reinterpret_cast<ImDrawIdx*>(_index_buffer->map(*deferred_device));

	if (!verts) {
		// $TODO: Log error periodic.
		return;
	}

	if (!indices) {
		// $TODO: Log error periodic.
		_vertex_buffer->unmap(*deferred_device);
		return;
	}

	for (int32_t i = 0; i < draw_data->CmdListsCount; ++i) {
		const ImDrawList* const cmd_list = draw_data->CmdLists[i];

		memcpy(indices, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		memcpy(verts, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));

		indices += cmd_list->IdxBuffer.Size;
		verts += cmd_list->VtxBuffer.Size;
	}

	_vertex_buffer->unmap(*deferred_device);
	_index_buffer->unmap(*deferred_device);

	const float L = draw_data->DisplayPos.x;
	const float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	const float T = draw_data->DisplayPos.y;
	const float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	const float mvp[4][4] =
	{
		{ 2.0f / (R - L),		0.0f,				0.0f,	0.0f },
		{ 0.0f,					2.0f / (T - B),		0.0f,	0.0f },
		{ 0.0f,					0.0f,				0.5f,	0.0f },
		{ (R + L) / (L - R),	(T + B) / (B - T),	0.5f,	1.0f },
	};

	void* const vert_const_data = _vert_constant_buffer->map(*deferred_device);

	if (!vert_const_data) {
		// $TODO: Log error periodic.
		return;
	}

	memcpy(vert_const_data, mvp, sizeof(mvp));
	_vert_constant_buffer->unmap(*deferred_device);

	_program_buffers->clearResourceViews();
	_program_buffers->addResourceView(Gleam::IShader::Type::Pixel, _font_srv.get());

	_main_output->getRenderTarget().bind(*deferred_device);
	_depth_stencil_state->bind(*deferred_device);
	_raster_state->bind(*deferred_device);
	_blend_state->bind(*deferred_device);
	_program->bind(*deferred_device);
	_program_buffers->bind(*deferred_device);
	_layout->bind(*deferred_device);

	int32_t index_offset_start = 0;
	int32_t vert_offset_start = 0;

	for (int32_t i = 0; i < draw_data->CmdListsCount; ++i) {
		const ImDrawList* const cmd_list = draw_data->CmdLists[i];

		for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; ++j) {
			const ImDrawCmd* const cmd = &cmd_list->CmdBuffer[j];

			if (_program_buffers->getResourceView(Gleam::IShader::Type::Pixel, 0) != cmd->TextureId) {
				_program_buffers->popResourceView(Gleam::IShader::Type::Pixel);

				_program_buffers->addResourceView(
					Gleam::IShader::Type::Pixel,
					reinterpret_cast<Gleam::IShaderResourceView*>(cmd->TextureId)
				);

				_program_buffers->bind(*deferred_device);
			}

			// Apply scissor/clipping rectangle
			const glm::ivec4 rect{
				static_cast<int32_t>(cmd->ClipRect.x - draw_data->DisplayPos.x),
				static_cast<int32_t>(cmd->ClipRect.y - draw_data->DisplayPos.y),
				static_cast<int32_t>(cmd->ClipRect.z - draw_data->DisplayPos.x),
				static_cast<int32_t>(cmd->ClipRect.w - draw_data->DisplayPos.y)
			};

			deferred_device->setScissorRect(rect);

			_mesh->setIndexCount(cmd->ElemCount);
			_mesh->render(*deferred_device, index_offset_start + cmd->IdxOffset, vert_offset_start + cmd->VtxOffset);
		}

		index_offset_start += cmd_list->IdxBuffer.Size;
		vert_offset_start += cmd_list->VtxBuffer.Size;
	}

	if (!deferred_device->finishCommandList(*_cmd_list[_render_cache_index])) {
		// $TODO: Log error periodic.
		return;
	}

	// Submit command
	auto& render_cmds = _render_mgr->getRenderCommands(*_main_device, RenderManagerBase::RenderOrder::ScreenSpaceDirect, _render_cache_index);

	render_cmds.lock.Lock();
	auto& cmd = render_cmds.command_list.emplace_back();
	cmd.cmd_list.reset(_cmd_list[_render_cache_index].get());
	cmd.owns_command = false;
	render_cmds.lock.Unlock();
}

void DebugManager::renderPreCamera(uintptr_t /*thread_id_int*/)
{
	auto& job_pool = GetApp().getJobPool();
	job_pool.addJobs(_debug_data.job_data_cache, static_cast<int32_t>(DebugRenderType::Count), _debug_data.job_counter);
}

void DebugManager::removeDebugRender(const DebugRenderHandle& handle)
{
	auto& render_list = _debug_data.instance_data[static_cast<int32_t>(handle._type)].render_list[handle._depth];
	const auto it = eastl::lower_bound(render_list.begin(), render_list.end(), handle._instance, [](const auto& lhs, auto rhs) -> bool { return lhs.get() < rhs; });

	if (it != render_list.end() && it->get() == handle._instance) {
		render_list.erase(it);
	}
}

bool DebugManager::initDebugRender(void)
{
	const size_t renderer_index = static_cast<size_t>(_render_mgr->getRendererType());

	// Init line shaders and program.
	_debug_data.line_vertex_shader.reset(_render_mgr->createShader());

	if (!_debug_data.line_vertex_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_debug_data.line_vertex_shader->initVertexSource(*_main_device, g_line_vertex_shader[renderer_index])) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.pixel_shader.reset(_render_mgr->createShader());

	if (!_debug_data.pixel_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_debug_data.pixel_shader->initPixelSource(*_main_device, g_debug_pixel_shader[renderer_index])) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.line_program.reset(_render_mgr->createProgram());

	if (!_debug_data.line_program) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.line_program->attach(_debug_data.line_vertex_shader.get());
	_debug_data.line_program->attach(_debug_data.pixel_shader.get());

	// Init regular shaders and program.
	_debug_data.vertex_shader.reset(_render_mgr->createShader());

	if (!_debug_data.vertex_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_debug_data.vertex_shader->initVertexSource(*_main_device, g_debug_vertex_shader[renderer_index])) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.program.reset(_render_mgr->createProgram());

	if (!_debug_data.program) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.program->attach(_debug_data.vertex_shader.get());
	_debug_data.program->attach(_debug_data.pixel_shader.get());


	// Init layout for meshes.
	_debug_data.layout.reset(_render_mgr->createLayout());

	if (!_debug_data.layout) {
		// $TODO: Log error.
		return false;
	}

	if (!_debug_data.layout->init(*_main_device, *_debug_data.vertex_shader)) {
		// $TODO: Log error.
		return false;
	}


	// Init Depth-Stencil states
	_debug_data.depth_stencil_state[0].reset(_render_mgr->createDepthStencilState());
	_debug_data.depth_stencil_state[1].reset(_render_mgr->createDepthStencilState());

	if (!_debug_data.depth_stencil_state[0] || !_debug_data.depth_stencil_state[1]) {
		// $TODO: Log error.
		return false;
	}

	Gleam::IDepthStencilState::Settings depth_stencil_settings;

	if (!_debug_data.depth_stencil_state[1]->init(*_main_device, depth_stencil_settings)) {
		// $TODO: Log error.
		return false;
	}

	depth_stencil_settings.depth_test = false;

	if (!_debug_data.depth_stencil_state[0]->init(*_main_device, depth_stencil_settings)) {
		// $TODO: Log error.
		return false;
	}

	// Init Raster states
	_debug_data.raster_state[0].reset(_render_mgr->createRasterState());
	_debug_data.raster_state[1].reset(_render_mgr->createRasterState());

	if (!_debug_data.raster_state[0] || !_debug_data.raster_state[1]) {
		// $TODO: Log error.
		return false;
	}

	Gleam::IRasterState::Settings raster_settings;
	raster_settings.two_sided = true;
	raster_settings.wireframe = true;

	if (!_debug_data.raster_state[1]->init(*_main_device, raster_settings)) {
		// $TODO: Log error.
		return false;
	}

	raster_settings.depth_clip_enabled = false;

	if (!_debug_data.raster_state[0]->init(*_main_device, raster_settings)) {
		// $TODO: Log error.
		return false;
	}

	for (int32_t i = 0; i < static_cast<int32_t>(DebugRenderType::Count); ++i) {
		_debug_data.render_job_data_cache[i].type = static_cast<DebugRenderType>(i);
		_debug_data.render_job_data_cache[i].debug_mgr = this;
		_debug_data.job_data_cache[i].job_data = &_debug_data.render_job_data_cache[i];
		_debug_data.job_data_cache[i].job_func = RenderDebugShape;

		_debug_data.render_job_data_cache[i].cmd_list[0][0].reset(_render_mgr->createCommandList());
		_debug_data.render_job_data_cache[i].cmd_list[1][0].reset(_render_mgr->createCommandList());
		_debug_data.render_job_data_cache[i].cmd_list[0][1].reset(_render_mgr->createCommandList());
		_debug_data.render_job_data_cache[i].cmd_list[1][1].reset(_render_mgr->createCommandList());

		if (!_debug_data.render_job_data_cache[i].cmd_list[0][0] || !_debug_data.render_job_data_cache[i].cmd_list[1][0] ||
			!_debug_data.render_job_data_cache[i].cmd_list[0][1] || !_debug_data.render_job_data_cache[i].cmd_list[1][1]) {
			// $TODO: Log error.
			return false;
		}

		DebugRenderInstanceData& instance_data =_debug_data.instance_data[i];

		instance_data.program_buffers.reset(_render_mgr->createProgramBuffers());

		if (!instance_data.program_buffers) {
			// $TODO: Log error.
			return false;
		}

		// Generate shape data.
		Gleam::Vector<glm::vec3> points[2];
		Gleam::Vector<int16_t> indices[2];

		constexpr int32_t k_subdivisions = 16;

		switch (static_cast<DebugRenderType>(i)) {
			case DebugRenderType::Line: {
				instance_data.constant_buffer.reset(_render_mgr->createBuffer());

				if (!instance_data.constant_buffer) {
					// $TODO: Log error.
					return false;
				}

				Gleam::IBuffer::Settings cb_settings;
				cb_settings.size = sizeof(glm::mat4x4);
				cb_settings.cpu_access = Gleam::IBuffer::MapType::Write;

				if (!instance_data.constant_buffer->init(*_main_device, cb_settings)) {
					// $TODO: Log error.
					return false;
				}

				instance_data.program_buffers->addConstantBuffer(
					Gleam::IShader::Type::Vertex,
					instance_data.constant_buffer.get()
				);
			} break;

			case DebugRenderType::Sphere:
				Gleam::GenerateDebugSphere(k_subdivisions, points[0], indices[0]);
				break;

			case DebugRenderType::Box:
				Gleam::GenerateDebugBox(points[0], indices[0]);
				break;

			case DebugRenderType::Cone:
				Gleam::GenerateDebugCone(k_subdivisions, points[0], indices[0]);
				break;

			case DebugRenderType::Capsule:
				Gleam::GenerateDebugCylinder(k_subdivisions, points[0], indices[0], false);
				Gleam::GenerateDebugHalfSphere(k_subdivisions, points[1], indices[1]);
				break;

			case DebugRenderType::Arrow:
				Gleam::GenerateDebugCylinder(k_subdivisions, points[0], indices[0]);
				Gleam::GenerateDebugCone(k_subdivisions, points[1], indices[1]);
				break;

			default:
				// This should never happen.
				GAFF_ASSERT(false);
				break;
		}

		for (int32_t j = 0; j < 2; ++j) {
			if (!points[j].empty() && !indices[j].empty()) {
				instance_data.mesh[j].reset(_render_mgr->createMesh());

				if (!instance_data.mesh[j]) {
					// $TODO: Log error.
					return false;
				}

				// Vertices
				instance_data.vertices[j].reset(_render_mgr->createBuffer());

				if (!instance_data.vertices[j]) {
					// $TODO: Log error.
					return false;
				}

				Gleam::IBuffer::Settings buffer_settings;
				buffer_settings.type = Gleam::IBuffer::Type::VertexData;
				buffer_settings.size = sizeof(glm::vec3) * points[j].size();
				buffer_settings.element_size = sizeof(glm::vec3);
				buffer_settings.stride = sizeof(glm::vec3);
				buffer_settings.data = points[j].data();

				if (!instance_data.vertices[j]->init(*_main_device, buffer_settings)) {
					// $TODO: Log error.
					return false;
				}

				// Indices
				instance_data.indices[j].reset(_render_mgr->createBuffer());

				if (!instance_data.indices[j]) {
					// $TODO: Log error.
					return false;
				}

				buffer_settings.type = Gleam::IBuffer::Type::IndexData;
				buffer_settings.size = sizeof(int16_t) * indices[j].size();
				buffer_settings.element_size = sizeof(int16_t);
				buffer_settings.stride = sizeof(int16_t);
				buffer_settings.data = indices[j].data();

				if (!instance_data.indices[j]->init(*_main_device, buffer_settings)) {
					// $TODO: Log error.
					return false;
				}

				instance_data.mesh[j]->setTopologyType(Gleam::IMesh::TopologyType::TriangleList);
				instance_data.mesh[j]->setIndexCount(static_cast<int32_t>(indices[j].size()));
				instance_data.mesh[j]->setIndiceBuffer(instance_data.indices[j].get());
				instance_data.mesh[j]->addBuffer(instance_data.vertices[j].get());
			}
		}
	}

	return true;
}

bool DebugManager::initImGui(void)
{
	IMGUI_CHECKVERSION();

	if (!ImGui::CreateContext()) {
		// $TODO: Log error.
		return false;
	}

	ImGui::StyleColorsDark();

	const Gleam::IWindow* const window = _render_mgr->getWindow("main");
	GAFF_ASSERT(window);

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;	// We can honor GetMouseCursor() values (optional)
	//io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;	// We can honor io.WantSetMousePos requests (optional, rarely used)
	io.ImeWindowHandle = window->getPlatformHandle();
	io.BackendRendererName = "Gleam";
	io.BackendPlatformName = "Gleam";
	io.IniFilename = nullptr;

	io.KeyMap[ImGuiKey_Tab] = static_cast<int32_t>(Gleam::KeyCode::KEY_TAB);
	io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int32_t>(Gleam::KeyCode::KEY_LEFT);
	io.KeyMap[ImGuiKey_RightArrow] = static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHT);
	io.KeyMap[ImGuiKey_UpArrow] = static_cast<int32_t>(Gleam::KeyCode::KEY_UP);
	io.KeyMap[ImGuiKey_DownArrow] = static_cast<int32_t>(Gleam::KeyCode::KEY_DOWN);
	io.KeyMap[ImGuiKey_PageUp] = static_cast<int32_t>(Gleam::KeyCode::KEY_PAGEUP);
	io.KeyMap[ImGuiKey_PageDown] = static_cast<int32_t>(Gleam::KeyCode::KEY_PAGEDOWN);
	io.KeyMap[ImGuiKey_Home] = static_cast<int32_t>(Gleam::KeyCode::KEY_HOME);
	io.KeyMap[ImGuiKey_End] = static_cast<int32_t>(Gleam::KeyCode::KEY_END);
	io.KeyMap[ImGuiKey_Insert] = static_cast<int32_t>(Gleam::KeyCode::KEY_INSERT);
	io.KeyMap[ImGuiKey_Delete] = static_cast<int32_t>(Gleam::KeyCode::KEY_DELETE);
	io.KeyMap[ImGuiKey_Backspace] = static_cast<int32_t>(Gleam::KeyCode::KEY_BACKSPACE);
	io.KeyMap[ImGuiKey_Space] = static_cast<int32_t>(Gleam::KeyCode::KEY_SPACE);
	io.KeyMap[ImGuiKey_Enter] = static_cast<int32_t>(Gleam::KeyCode::KEY_ENTER);
	io.KeyMap[ImGuiKey_Escape] = static_cast<int32_t>(Gleam::KeyCode::KEY_ESCAPE);
	io.KeyMap[ImGuiKey_KeyPadEnter] = static_cast<int32_t>(Gleam::KeyCode::KEY_NUMPADENTER);
	io.KeyMap[ImGuiKey_A] = static_cast<int32_t>(Gleam::KeyCode::KEY_A);
	io.KeyMap[ImGuiKey_C] = static_cast<int32_t>(Gleam::KeyCode::KEY_C);
	io.KeyMap[ImGuiKey_V] = static_cast<int32_t>(Gleam::KeyCode::KEY_V);
	io.KeyMap[ImGuiKey_X] = static_cast<int32_t>(Gleam::KeyCode::KEY_X);
	io.KeyMap[ImGuiKey_Y] = static_cast<int32_t>(Gleam::KeyCode::KEY_Y);
	io.KeyMap[ImGuiKey_Z] = static_cast<int32_t>(Gleam::KeyCode::KEY_Z);

	InputManager& input = GetApp().getManagerTFast<InputManager>();
	input.getKeyboard()->addCharacterHandler(Gaff::Func(HandleKeyboardCharacter));
	input.getKeyboard()->addInputHandler(Gaff::Func(HandleKeyboardInput));
	input.getMouse()->addInputHandler(Gaff::Func(HandleMouseInput));

	// Command List
	_cmd_list[0].reset(_render_mgr->createCommandList());
	_cmd_list[1].reset(_render_mgr->createCommandList());

	if (!_cmd_list[0] || !_cmd_list[1]) {
		// $TODO: Log error.
		return false;
	}


	// Shaders
	_vertex_shader.reset(_render_mgr->createShader());

	if (!_vertex_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_vertex_shader->initVertexSource(*_main_device, g_imgui_vertex_shader[static_cast<size_t>(_render_mgr->getRendererType())])) {
		// $TODO: Log error.
		return false;
	}

	_pixel_shader.reset(_render_mgr->createShader());

	if (!_pixel_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_pixel_shader->initPixelSource(*_main_device, g_imgui_pixel_shader[static_cast<size_t>(_render_mgr->getRendererType())])) {
		// $TODO: Log error.
		return false;
	}

	_program.reset(_render_mgr->createProgram());

	if (!_program) {
		// $TODO: Log error.
		return false;
	}

	_program->attach(_vertex_shader.get());
	_program->attach(_pixel_shader.get());


	// Blend State
	_blend_state.reset(_render_mgr->createBlendState());

	if (!_blend_state) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::IBlendState::Settings blend_settings = {
		Gleam::IBlendState::BlendFactor::SourceAlpha,
		Gleam::IBlendState::BlendFactor::InverseSourceAlpha,
		Gleam::IBlendState::BlendOp::Add,
		Gleam::IBlendState::BlendFactor::InverseSourceAlpha,
		Gleam::IBlendState::BlendFactor::Zero,
		Gleam::IBlendState::BlendOp::Add,
		Gleam::IBlendState::ColorMask::All,
		true
	};

	if (!_blend_state->init(*_main_device, blend_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Raster State
	_raster_state.reset(_render_mgr->createRasterState());

	if (!_raster_state) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::IRasterState::Settings raster_settings = {
		0.0f,
		0.0f,
		0,
		true,
		false,
		true,
		false,
		false
	};

	if (!_raster_state->init(*_main_device, raster_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Depth Stencil State
	_depth_stencil_state.reset(_render_mgr->createDepthStencilState());

	if (!_depth_stencil_state) {
		// $TODO: Log error.
		return false;
	}

	Gleam::IDepthStencilState::Settings depth_stencil_settings;
	depth_stencil_settings.depth_func = Gleam::ComparisonFunc::Always;
	depth_stencil_settings.depth_test = false;

	if (!_depth_stencil_state->init(*_main_device, depth_stencil_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Layout
	_layout.reset(_render_mgr->createLayout());

	if (!_layout) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::ILayout::Description layout_description[] = {
		{
			Gleam::ILayout::Semantic::Position,
			0,
			Gleam::ITexture::Format::RG_32_F,
			0,
			static_cast<int32_t>((size_t)(&((ImDrawVert*)0)->pos)),
			Gleam::ILayout::PerDataType::Vertex
		},
		{
			Gleam::ILayout::Semantic::TexCoord,
			0,
			Gleam::ITexture::Format::RG_32_F,
			0,
			static_cast<int32_t>((size_t)(&((ImDrawVert*)0)->uv)),
			Gleam::ILayout::PerDataType::Vertex
		},
		{
			Gleam::ILayout::Semantic::Color,
			0,
			Gleam::ITexture::Format::RGBA_8_UNORM,
			0,
			static_cast<int32_t>((size_t)(&((ImDrawVert*)0)->col)),
			Gleam::ILayout::PerDataType::Vertex
		}
	};

	if (!_layout->init(*_main_device, layout_description, ARRAY_SIZE(layout_description), *_vertex_shader)) {
		// $TODO: Log error.
		return false;
	}


	// Constant Buffer
	_vert_constant_buffer.reset(_render_mgr->createBuffer());

	if (!_vert_constant_buffer) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::IBuffer::Settings constant_buffer_settings = {
		nullptr,
		sizeof(VertexConstantBuffer),
		0,
		sizeof(VertexConstantBuffer),
		Gleam::IBuffer::Type::ShaderConstantData,
		Gleam::IBuffer::MapType::Write,
		true
	};

	if (!_vert_constant_buffer->init(*_main_device, constant_buffer_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Font Texture
	_font_srv.reset(_render_mgr->createShaderResourceView());
	_font_texture.reset(_render_mgr->createTexture());

	if (!_font_texture) {
		// $TODO: Log error.
		return false;
	}

	if (!_font_srv) {
		// $TODO: Log error.
		return false;
	}

	int32_t bytes_per_pixel = 0;
	uint8_t* font = nullptr;
	int32_t height = 0;
	int32_t width = 0;

	io.Fonts->GetTexDataAsRGBA32(&font, &width, &height, &bytes_per_pixel);
	GAFF_ASSERT(bytes_per_pixel == 4);
	GAFF_REF(bytes_per_pixel);

	if (!_font_texture->init2D(*_main_device, width, height, Gleam::ITexture::Format::RGBA_8_UNORM, 1, font)) {
		// $TODO: Log error.
		return false;
	}

	if (!_font_srv->init(*_main_device, _font_texture.get())) {
		// $TODO: Log error.
		return false;
	}

	io.Fonts->TexID = _font_srv.get();


	// Sampler
	_sampler.reset(_render_mgr->createSamplerState());

	if (!_sampler) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::ISamplerState::Settings sampler_settings = {
		Gleam::ISamplerState::Filter::LinearLinearLinear,
		Gleam::ISamplerState::Wrap::Repeat,
		Gleam::ISamplerState::Wrap::Repeat,
		Gleam::ISamplerState::Wrap::Repeat,
		0.0f,
		0.0f,
		0.0f,
		1,
		Gleam::Color::Black,
		Gleam::ComparisonFunc::Always
	};

	if (!_sampler->init(*_main_device, sampler_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Program Buffers
	_program_buffers.reset(_render_mgr->createProgramBuffers());

	if (!_program_buffers) {
		// $TODO: Log error.
		return false;
	}

	_program_buffers->addConstantBuffer(Gleam::IShader::Type::Vertex, _vert_constant_buffer.get());
	_program_buffers->addResourceView(Gleam::IShader::Type::Pixel, _font_srv.get());
	_program_buffers->addSamplerState(Gleam::IShader::Type::Pixel, _sampler.get());

	_mesh.reset(_render_mgr->createMesh());

	if (!_mesh) {
		// $TODO: Log error.
		return false;
	}

	_mesh->setTopologyType(Gleam::IMesh::TopologyType::TriangleList);
	return true;
}



bool DebugRenderSystem::init(void)
{
	_debug_mgr = &GetApp().getManagerTFast<DebugManager>();

	static auto capsule = _debug_mgr->renderDebugCapsule(glm::vec3(5.0f, 0.0f, 5.0f), 1.0f, 2.0f, Gleam::Color::Red, true);
	static auto sphere = _debug_mgr->renderDebugSphere(glm::vec3(0.0f, 0.5f, 5.0f), 1.0f, Gleam::Color::Red, true);
	static auto line = _debug_mgr->renderDebugLine(glm::vec3(-50.0f, 0.0f, 20.0f), glm::vec3(50.0f, 0.0f, 20.0f), Gleam::Color::Red, true);
	static auto cone = _debug_mgr->renderDebugCone(glm::vec3(-5.0f, 5.0f, 5.0f), glm::vec3(1.0f), Gleam::Color::Red, true);
	static auto box = _debug_mgr->renderDebugBox(glm::vec3(-5.0f, 0.0f, 5.0f), glm::vec3(1.0f), Gleam::Color::Red, true);
	static auto arrow = _debug_mgr->renderDebugArrow(glm::vec3(-5.0f, 1.0f, 20.0f), glm::vec3(5.0f, 1.0f, 20.0f), Gleam::Color::Red, true);

	capsule.getInstance().transform.setRotation(glm::angleAxis(Gaff::Pi * 0.25f, glm::vec3(1.0f, 0.0f, 0.0f)));

	return true;
}

void DebugRenderSystem::update(uintptr_t thread_id_int)
{
	_debug_mgr->render(thread_id_int);
}

bool DebugSystem::init(void)
{
	_debug_mgr = &GetApp().getManagerTFast<DebugManager>();
	return true;
}

void DebugSystem::update(uintptr_t /*thread_id_int*/)
{
	_debug_mgr->update();
}

NS_END
