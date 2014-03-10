/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gleam_ISceneNode.h"

NS_GLEAM

class Scene
{
public:
	Scene(unsigned int num_stages = 1);
	~Scene(void);

	bool init(void);
	INLINE void destroy(void);

	INLINE const ISceneNode* getRoot(void) const;
	INLINE ISceneNode* getRoot(void);

	INLINE const ISceneNode* getCamera(void) const;
	INLINE ISceneNode* getCamera(void);
	INLINE void setCamera(ISceneNode* camera);

	INLINE const GleamArray<ISceneNode*>& getNodesOfType(unsigned int type) const;
	INLINE GleamArray<ISceneNode*>& getNodesOfType(unsigned int type);

	// these should be used for all nodes added to/removed from the scene
	void addNode(ISceneNode* node, ISceneNode* parent);
	INLINE void addNode(ISceneNode* node);
	void removeNode(ISceneNode* node);

	INLINE const GleamArray<ISceneNode*>& getChangeList(void) const;
	INLINE GleamArray<ISceneNode*>& getChangeList(void);

	void update(float dt);

private:
	GleamArray< GleamArray<ISceneNode*> > _node_types;
	GleamArray<ISceneNode*> _change_list;

	ISceneNode* _camera;
	ISceneNode* _root;

	GAFF_NO_COPY(Scene);
};

NS_END
