/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

// TODO: Listen for level change messages or region unload messages to clear the render pass info.

#include "Shibboleth_InGameRenderPipeline.h"
#include <Shibboleth_FrameManager.h>

#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#include <Gleam_IRenderDevice.h>
#include <Gleam_IProgram.h>
#include <Gleam_IBuffer.h>
#include <Gleam_IModel.h>

#define INSTANCE_ALLOC_CHUNK 256
#define INSTANCE_BUFFER_ELEMENT_SIZE 16 * 1 // sizeof(matrix) * num_matrices

NS_SHIBBOLETH

struct RenderInfo
{
	RenderInfo(ProgramBuffersPtr pbs, ModelPtr mo, size_t me):
		program_buffers(pbs), model(mo), mesh(me)
	{
	}

	Array<const Gleam::TransformCPU*> object_transforms;

	ProgramBuffersPtr program_buffers;
	ModelPtr model;
	size_t mesh;
};

struct RasterInfo
{
	RasterInfo(RasterStatePtr rs, ProgramPtr p):
		raster_state(rs), program(p), obj_count(0)
	{
	}

	RasterStatePtr raster_state;
	ProgramPtr program;

	size_t obj_count;

	Map<unsigned int, RenderInfo> render_info; // Key is hash of ProgramBuffersPtr, ModelPtr, and mesh index
};

using RenderPassInfo = Map<unsigned int, RasterInfo>; // Key is hash of RasterStatePtr and ProgramPtr.

static THREAD_LOCAL Array< Gaff::Pair<BufferPtr, ShaderResourceViewPtr> > gInstance_buffer_pool; // [Device]
static THREAD_LOCAL RenderPassInfo gRender_pass_info[RP_COUNT];
static THREAD_LOCAL bool gInit = false;

InGameRenderPipeline::InGameRenderPipeline(void):
	_job_cache(GetApp().getJobPool().getNumTotalThreads()),
	_counter(nullptr),
	_render_mgr(GetApp().getManagerT<RenderManager>())
{
}

InGameRenderPipeline::~InGameRenderPipeline(void)
{
	if (_counter) {
		GetApp().getJobPool().freeCounter(_counter);
	}
}

bool InGameRenderPipeline::init(void)
{
	// Create RenderStates for each render pass
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	Gleam::IDepthStencilState::DepthStencilStateSettings dss;
	Gleam::IBlendState::BlendStateSettings bss;

	// RenderState for opaque pass
	bss.blend_src_color = Gleam::IBlendState::BLEND_FACTOR_SRC_COLOR;
	bss.blend_dst_color = Gleam::IBlendState::BLEND_FACTOR_DEST_COLOR;
	bss.blend_op_color = Gleam::IBlendState::BLEND_OP_ADD;
	bss.blend_src_alpha = Gleam::IBlendState::BLEND_FACTOR_SRC_ALPHA;
	bss.blend_dst_alpha = Gleam::IBlendState::BLEND_FACTOR_DEST_ALPHA;
	bss.blend_op_alpha = Gleam::IBlendState::BLEND_OP_SUBTRACT;
	bss.color_write_mask = Gleam::IBlendState::COLOR_ALL;
	bss.enable_alpha_blending = false;

	dss.front_face.stencil_depth_fail = Gleam::IDepthStencilState::STENCIL_KEEP;
	dss.front_face.stencil_depth_pass = Gleam::IDepthStencilState::STENCIL_KEEP;
	dss.front_face.stencil_pass_depth_fail = Gleam::IDepthStencilState::STENCIL_KEEP;
	dss.front_face.comp_func = Gleam::IDepthStencilState::COMPARE_ALWAYS;

	dss.back_face = dss.front_face;

	dss.stencil_read_mask = 0xFF;
	dss.stencil_write_mask = 0xFF;
	dss.stencil_test = false;
	dss.stencil_ref = 0;

	dss.depth_func = Gleam::IDepthStencilState::COMPARE_LESS;
	dss.depth_test = true;

	_ds_states[RP_OPAQUE].resize(rd.getNumDevices());
	_ds_states[RP_TRANSPARENT].resize(rd.getNumDevices());
	_blend_states[RP_OPAQUE].resize(rd.getNumDevices());
	_blend_states[RP_TRANSPARENT].resize(rd.getNumDevices());

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		Gleam::IDepthStencilState* ds = _render_mgr.createDepthStencilState();
		rd.setCurrentDevice(i);

		if (!ds) {
			// log error
			return false;
		}

		if (!ds->init(rd, dss)) {
			// log error
			return false;
		}

		_ds_states[RP_OPAQUE][i] = ds;
		_ds_states[RP_TRANSPARENT][i] = ds;

		Gleam::IBlendState* bs = _render_mgr.createBlendState();

		if (!bs) {
			// log error
			return false;
		}

		if (!bs->init(rd, bss)) {
			// log error
			return false;
		}

		_blend_states[RP_OPAQUE][i] = bs;

		bss.enable_alpha_blending = true;

		bs = _render_mgr.createBlendState();

		if (!bs) {
			// log error
			return false;
		}

		if (!bs->init(rd, bss)) {
			// log error
			return false;
		}

		_blend_states[RP_TRANSPARENT][i] = bs;
	}

	return true;
}

const char* InGameRenderPipeline::getName(void) const
{
	return "In-Game Render Pipeline";
}

void InGameRenderPipeline::run(double, void* frame_data)
{
	FrameData* fd = reinterpret_cast<FrameData*>(frame_data);

	for (auto it = fd->camera_object_data.begin(); it != fd->camera_object_data.end(); ++it) {
		if (it->second.command_lists.empty()) {
			unsigned int size = _render_mgr.getRenderDevice().getNumDevices();
			it->second.command_lists.resize(size);
			it->second.cmd_lists_locks.resize(size);
		}

		it->second.curr_device = 0;
	}

	// Maybe pre-allocate the destination arrays so that we don't have to lock in helper threads?

	GenerateJobData job_data(this, fd);

	for (size_t i = 0; i < GetApp().getJobPool().getNumTotalThreads(); ++i) {
		_job_cache.emplacePush(&InGameRenderPipeline::GenerateCommandLists, &job_data);
	}

	auto& jp = GetApp().getJobPool();
	jp.addJobs(_job_cache.getArray(), _job_cache.size(), &_counter);
	jp.helpWhileWaiting(_counter);

	_job_cache.clearNoFree();
}

void InGameRenderPipeline::GenerateCommandLists(void* job_data)
{
	GenerateJobData* jd = reinterpret_cast<GenerateJobData*>(job_data);
	Array<RenderManager::RenderDevicePtr>& rds = jd->first->_render_mgr.getDeferredRenderDevices(Gaff::Thread::GetCurrentThreadID());

	GenerateCameraCommandLists(rds, jd);
	GenerateLightCommandLists(rds, jd);
}

void InGameRenderPipeline::GenerateCameraCommandLists(Array<RenderManager::RenderDevicePtr>& rds, GenerateJobData* jd)
{
	if (!gInit) {
		Gaff::construct(&gInstance_buffer_pool, jd->first->_render_mgr.getRenderDevice().getNumDevices(), Gaff::Pair<BufferPtr, ShaderResourceViewPtr>());
		Gaff::construct(gRender_pass_info + RP_OPAQUE);
		Gaff::construct(gRender_pass_info + RP_TRANSPARENT);
		gInit = true;
	}

	FrameData* fd = jd->second;

	// For each camera
	for (auto it = fd->camera_object_data.begin(); it != fd->camera_object_data.end(); ++it) {
		if (!it->second.active) {
			continue;
		}

		// Grab an available camera
		unsigned int device_index = AtomicIncrement(&it->second.curr_device) - 1;
		auto& devices = it->first->getDevices();

		// Create command list for each device
		while (device_index < devices.size()) {
			unsigned int device = devices[device_index];
			auto& rd = rds[device];
			auto& od = it->second;

			// Change to pre-allocate command lists and re-use them instead of destroying and re-creating them.
			Gleam::ICommandList* cmd_list = jd->first->_render_mgr.createCommandList();

			if (!cmd_list) {
				// log error
				continue;
			}

			SortIntoRenderPasses(rd.get(), od, device);

			Gleam::IRenderTargetPtr& rt = it->first->getRenderTarget()->render_targets[device];
			rt->bind(*rd);

			ClearCamera(rd.get(), it->second, rt);
			RunCommands(rd.get(), jd, device, od);

			// generate command list
			if (!rd->finishCommandList(cmd_list)) {
				// log error
			}

			it->second.cmd_lists_locks[device].lock();
			it->second.command_lists[device].emplacePush(cmd_list);
			it->second.cmd_lists_locks[device].unlock();

			device_index = AtomicIncrement(&it->second.curr_device) - 1;
		}
	}
}

void InGameRenderPipeline::GenerateLightCommandLists(Array<RenderManager::RenderDevicePtr>& rds, GenerateJobData* jd)
{
}

void InGameRenderPipeline::SortIntoRenderPasses(Gleam::IRenderDevice* rd, ObjectData& od, unsigned int device)
{
	for (size_t obj = 0, mesh = 0; obj < od.transforms.size(); ++obj) {
		auto& model = od.models[obj];

		if (model[device]) {
			const Gleam::IModel* m = model[device].get();
			uint32_t model_hash = Gaff::FNV1aHash32T(&m);

			for (size_t i = 0; i < model[device]->getMeshCount(); ++i, ++mesh) {
				auto& rss = od.raster_states[mesh];
				auto& pbs = od.program_buffers[mesh];
				auto& programs = od.programs[mesh];

				assert(rss[device] && pbs[device] && programs[device]);

				unsigned int first_hash = GenerateFirstInstanceHash(od, mesh, device);
				unsigned int second_hash = GenerateSecondInstanceHash(od, mesh, device, i, model_hash);

				auto& rpi = gRender_pass_info[od.render_pass[mesh]];
				auto it1 = rpi.findElementWithKey(first_hash);

				if (it1 == rpi.end()) {
					it1 = rpi.emplace(first_hash, rss[device], programs[device]);
				}

				auto it2 = it1->second.render_info.findElementWithKey(second_hash);

				if (it2 == it1->second.render_info.end()) {
					it2 = it1->second.render_info.emplace(second_hash, pbs[device], model[device], i);
				}

				it2->second.object_transforms.emplacePush(&od.transforms[obj]);
				++it1->second.obj_count;
			}
		}
	}
}

void InGameRenderPipeline::RunCommands(Gleam::IRenderDevice* rd, GenerateJobData* jd, unsigned int device, const ObjectData& od)
{
	for (int i = 0; i < RP_COUNT; ++i) {
		jd->first->_ds_states[i][device]->set(*rd);
		jd->first->_blend_states[i == RP_TRANSPARENT][device]->set(*rd);

		for (auto it1 = gRender_pass_info[i].begin(); it1 != gRender_pass_info[i].end(); ++it1) {
			// Don't process unless we actually have objects
			if (it1->second.obj_count) {
				// Bind the shared raster state and shaders.
				it1->second.raster_state->set(*rd);
				it1->second.program->bind(*rd);

				// Render each object
				for (auto it2 = it1->second.render_info.begin(); it2 != it1->second.render_info.end(); ++it2) {
					// Don't process unless we actually have objects
					if (!it2->second.object_transforms.empty()) {
						auto& buffer = gInstance_buffer_pool[device].first;
						auto& srv = gInstance_buffer_pool[device].second;
						size_t num_instances = it2->second.object_transforms.size();

						if (!buffer || (buffer->getSize() / (sizeof(float) * INSTANCE_BUFFER_ELEMENT_SIZE)) < it2->second.object_transforms.size()) {
							buffer = CreateInstanceBuffer(jd->first->_render_mgr, device, static_cast<unsigned int>(num_instances));
							srv = CreateInstanceResourceView(jd->first->_render_mgr, buffer.get());
						}

						float* inst_buffer = reinterpret_cast<float*>(buffer->map(*rd));

						if (!inst_buffer) {
							// log error
							continue;
						}

						for (size_t j = 0; j < num_instances; ++j) {
							Gleam::TransformCPU final_transform = od.inv_eye_transform + *it2->second.object_transforms[j];
							Gleam::Matrix4x4CPU final_matrix = od.projection_matrix * final_transform.matrix();

							memcpy(inst_buffer + j * INSTANCE_BUFFER_ELEMENT_SIZE, final_matrix.getBuffer(), sizeof(float) * 16);
						}

						buffer->unmap(*rd);

						it2->second.program_buffers->addResourceView(Gleam::IShader::SHADER_VERTEX, srv.get());
						it2->second.program_buffers->bind(*rd);
						it2->second.model->renderInstanced(*rd, it2->second.mesh, static_cast<unsigned int>(num_instances));
						it2->second.program_buffers->popResourceView(Gleam::IShader::SHADER_VERTEX);

						it2->second.object_transforms.clearNoFree();
					}
				}

				it1->second.obj_count = 0;
			}
		}
	}
}

void InGameRenderPipeline::ClearCamera(Gleam::IRenderDevice* rd, ObjectData& od, Gleam::IRenderTargetPtr& rt)
{
	switch (od.clear_mode) {
		case CameraComponent::CM_SKYBOX:
			// Currently unsupported
			break;

		case CameraComponent::CM_COLOR:
			rt->clear(*rd, Gleam::IRenderTarget::CLEAR_ALL, 1.0f, 0, od.clear_color);
			break;

		case CameraComponent::CM_DEPTH_STENCIL:
			rt->clear(*rd, Gleam::IRenderTarget::CLEAR_DEPTH | Gleam::IRenderTarget::CLEAR_STENCIL);
			break;

		case CameraComponent::CM_STENCIL:
			rt->clear(*rd, Gleam::IRenderTarget::CLEAR_STENCIL);
			break;

		case CameraComponent::CM_NOTHING:
			break;
	}
}

unsigned int InGameRenderPipeline::GenerateFirstInstanceHash(ObjectData& od, size_t mesh_index, unsigned int device)
{
	auto& rss = od.raster_states[mesh_index];
	auto& programs = od.programs[mesh_index];

	const auto* raster_state = rss[device].get();
	const auto* prog = programs[device].get();

	unsigned int hash = Gaff::FNV1aHash32T(&raster_state);
	return Gaff::FNV1aHash32T(&prog, hash);
}

unsigned int InGameRenderPipeline::GenerateSecondInstanceHash(ObjectData& od, size_t mesh_index, unsigned int device, size_t submesh_index, unsigned int hash_init)
{
	unsigned int hash = hash_init;

	auto& pbs = od.program_buffers[mesh_index];
	const auto* prog_bufs = pbs[device].get();

	for (int i = 0; i < Gleam::IShader::SHADER_COMPUTE; ++i) {
		Gleam::IShader::SHADER_TYPE st = static_cast<Gleam::IShader::SHADER_TYPE>(i);
		const char* cb_buffer = reinterpret_cast<const char*>(prog_bufs->getConstantBuffers(st).getArray());
		const char* srv_buffer = reinterpret_cast<const char*>(prog_bufs->getResourceViews(st).getArray());
		const char* samp_buffer = reinterpret_cast<const char*>(prog_bufs->getSamplerStates(st).getArray());

		hash = Gaff::FNV1aHash32(cb_buffer, sizeof(void*) * prog_bufs->getConstantBufferCount(st), hash);
		hash = Gaff::FNV1aHash32(srv_buffer, sizeof(void*) * prog_bufs->getResourceViewCount(st), hash);
		hash = Gaff::FNV1aHash32(samp_buffer, sizeof(void*) * prog_bufs->getSamplerCount(st), hash);
	}

	return Gaff::FNV1aHash32T(&submesh_index, hash);
}

Gleam::IBuffer* InGameRenderPipeline::CreateInstanceBuffer(RenderManager& render_mgr, unsigned int device, unsigned int num_elements)
{
	unsigned int buffer_size = ((num_elements + INSTANCE_ALLOC_CHUNK - 1) / INSTANCE_ALLOC_CHUNK) * INSTANCE_ALLOC_CHUNK;

	Gleam::IBuffer* buffer = render_mgr.createBuffer();

	if (!buffer) {
		// log error
		return nullptr;
	}

	auto& rd = render_mgr.getRenderDevice();
	rd.setCurrentDevice(device);

	if (!buffer->init(
		rd, nullptr, sizeof(float) * INSTANCE_BUFFER_ELEMENT_SIZE * buffer_size, Gleam::IBuffer::STRUCTURED_DATA,
		0, Gleam::IBuffer::WRITE, true, sizeof(float) * INSTANCE_BUFFER_ELEMENT_SIZE)) {

		// Free the memory
		buffer->addRef();
		buffer->release();

		// log error
		buffer = nullptr;
	}

	return buffer;
}

Gleam::IShaderResourceView* InGameRenderPipeline::CreateInstanceResourceView(RenderManager& render_mgr, Gleam::IBuffer* buffer)
{
	Gleam::IShaderResourceView* srv = render_mgr.createShaderResourceView();

	if (!srv) {
		// log error
		return nullptr;
	}

	auto& rd = render_mgr.getRenderDevice();

	if (!srv->init(rd, buffer)) {
		// Free the memory
		srv->addRef();
		srv->release();

		// log error
		srv = nullptr;
	}

	return srv;
}

NS_END
