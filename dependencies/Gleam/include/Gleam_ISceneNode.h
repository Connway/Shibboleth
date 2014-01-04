/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gleam_RefCounted.h"
#include "Gleam_Transform.h"
#include "Gleam_Array.h"
#include "Gleam_OBB.h"

NS_GLEAM

class IRenderDevice;
//class IAnimator;
class Scene;

// another class that probably would qualify to be called <class_name>Base
class ISceneNode : public RefCounted
{
public:
	ISceneNode(Scene& scene, const Transform& model_transform);
	ISceneNode(Scene& scene);
	virtual ~ISceneNode(void);

	virtual void destroy(void);

	virtual void render(IRenderDevice& rd);

	const Transform& getWorldTransform(void) const;
	const Transform& getModelTransform(void) const;
	Transform& getModelTransform(void);
	INLINE void setModelTransform(const Transform& transform);

	INLINE unsigned int getNodeType(void) const;
	INLINE void setNodeType(unsigned int node_type);

	INLINE const OBB& getBoundingBox(void) const;
	INLINE void setBoundingBox(const OBB& bounding_box);

	INLINE const GleamArray(ISceneNode*)& getChildren(void) const;
	INLINE GleamArray(ISceneNode*)& getChildren(void);
	void addChild(ISceneNode* child);
	void removeChild(const ISceneNode* child);

	INLINE const ISceneNode* getParent(void) const;
	void setParent(ISceneNode* parent);
	void removeFromParent(void);

	INLINE bool hasChanged(void) const;
	INLINE void setChanged(bool changed);

	INLINE bool isEnabled(void) const;
	INLINE void setEnabled(bool enabled);

	virtual void updateTransform(void);
	virtual void update(float dt);

private:
	//GleamArray(IAnimator*) _animators;
	GleamArray(ISceneNode*) _children;
	ISceneNode* _parent;

	Transform _world_transform;
	Transform _model_transform;

	unsigned int _node_type;
	OBB _bounding_box;
	bool _changed;
	bool _enabled;

protected:
	Scene& _scene;
};

NS_END
