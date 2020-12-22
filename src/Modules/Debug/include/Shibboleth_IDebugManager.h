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

#pragma once

#include <Shibboleth_Utilities.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_IApp.h>
#include <Gleam_Transform.h>
#include <Gleam_Color.h>

struct ImGuiContext;

NS_SHIBBOLETH

// To force getting the context from the DLL.
class IDebugManager : public IManager
{
protected:
	enum class DebugRenderType
	{
		Line,
		Sphere,
		Box,
		Cone,
		Capsule,
		Arrow,

		Count
	};

public:
	struct DebugRenderInstance final
	{
		Gleam::Transform transform;
		Gleam::Color::RGB color;
	};

	class DebugRenderHandle final
	{
	public:
		DebugRenderHandle(void) = default;

		~DebugRenderHandle(void)
		{
			if (_instance) {
				GetApp().GETMANAGERT(IDebugManager, DebugManager).removeDebugRender(*this);
			}
		}

		const DebugRenderHandle& operator=(const DebugRenderHandle& rhs)
		{
			_instance = rhs._instance;
			_type = rhs._type;
			_depth = rhs._depth;

			const_cast<DebugRenderHandle&>(rhs)._instance = nullptr;
			const_cast<DebugRenderHandle&>(rhs)._type = DebugRenderType::Count;

			return *this;
		}

		const DebugRenderInstance& getInstance(void) const { return *_instance; }
		DebugRenderInstance& getInstance(void) { return *_instance; }

		bool isValid(void) const { return _instance != nullptr; }

	private:
		DebugRenderHandle(DebugRenderInstance* instance, DebugRenderType type, bool depth):
			_instance(instance),
			_type(type),
			_depth(depth)
		{
		}

		DebugRenderInstance* _instance = nullptr;
		DebugRenderType _type = DebugRenderType::Count;
		bool _depth = false;

		friend class DebugManager;
	};


	virtual ~IDebugManager(void) {}

	virtual ImGuiContext* getImGuiContext(void) const = 0;

	virtual DebugRenderHandle renderDebugArrow(const glm::vec3& start, const glm::vec3& end, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) = 0;
	virtual DebugRenderHandle renderDebugLine(const glm::vec3& start, const glm::vec3& end, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) = 0;
	virtual DebugRenderHandle renderDebugSphere(const glm::vec3& pos, float radius = 1.0f, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) = 0;
	virtual DebugRenderHandle renderDebugCone(const glm::vec3& pos, const glm::vec3& size = glm::vec3(1.0f), const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) = 0;
	virtual DebugRenderHandle renderDebugBox(const glm::vec3& pos, const glm::vec3& size = glm::vec3(1.0f), const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) = 0;
	virtual DebugRenderHandle renderDebugCapsule(const glm::vec3& pos, float radius = 1.0f, float height = 1.0f, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) = 0;

	virtual void registerDebugMenuItems(void* object, const Gaff::IReflectionDefinition& ref_def) = 0;
	virtual void unregisterDebugMenuItems(void* object, const Gaff::IReflectionDefinition& ref_def) = 0;

protected:
	virtual void removeDebugRender(const DebugRenderHandle& handle) = 0;

	friend class DebugRenderHandle;
};

NS_END
