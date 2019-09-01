/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

SHIB_TEMPLATE_REFLECTION_DEFINE(Resource, T)

NS_SHIBBOLETH

template <class T>
void Resource::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const ResourceType& value)
{
	ecs_mgr.getComponentShared<Resource>(archetype)->value = value;
}

template <class T>
void Resource::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, ResourceType&& value)
{
	ecs_mgr.getComponentShared<Resource>(archetype)->value = std::move(value);
}

template <class T>
void Resource::SetShared(ECSManager& ecs_mgr, EntityID id, const ResourceType& value)
{
	ecs_mgr.getComponentShared<Resource>(id)->value = value;
}

template <class T>
void Resource::SetShared(ECSManager& ecs_mgr, EntityID id, ResourceType&& value)
{
	ecs_mgr.getComponentShared<Resource>(id)->value = std::move(value);
}

template <class T>
const ResourceType& Resource::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return ecs_mgr.getComponentShared<Resource>(archetype)->value;
}

template <class T>
const ResourceType& Resource::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return ecs_mgr.getComponentShared<Resource>(id)->value;
}

template <class T>
void Resource::CopyShared(const void* old_value, void* new_value)
{
	reinterpret_cast<Resource*>(new_value)->value = reinterpret_cast<const Resource*>(old_value)->value;
}

NS_END
