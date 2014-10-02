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

#include "Gaff_Defines.h"
#include <assimp/anim.h>

NS_GAFF

enum AnimationBehavior
{
	AB_DEFAULT = aiAnimBehaviour_DEFAULT,
	AB_CONSTANT = aiAnimBehaviour_CONSTANT,
	AB_LINEAR = aiAnimBehaviour_LINEAR,
	AB_REPEAT = aiAnimBehaviour_REPEAT
};

class RotationKey
{
public:
	RotationKey(const RotationKey& key);
	RotationKey(void);
	~RotationKey(void);

	INLINE double getTime(void) const;
	INLINE const float* getQuaternion(void) const; // w, x, y, z

	INLINE bool valid(void) const;

	INLINE const RotationKey& operator=(const RotationKey& rhs);

	INLINE bool operator==(const RotationKey& rhs) const;
	INLINE bool operator!=(const RotationKey& rhs) const;
	INLINE bool operator<(const RotationKey& rhs) const;
	INLINE bool operator>(const RotationKey& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiQuatKey* _key;
	//double _time;
	//float _quat[4];

	friend class NodeAnimation;
	RotationKey(const aiQuatKey* key);
};

class VectorKey
{
public:
	VectorKey(const VectorKey& key);
	VectorKey(void);
	~VectorKey(void);

	INLINE double getTime(void) const;
	INLINE const float* getVector(void) const;

	INLINE bool valid(void) const;

	INLINE const VectorKey& operator=(const VectorKey& rhs);

	INLINE bool operator==(const VectorKey& rhs) const;
	INLINE bool operator!=(const VectorKey& rhs) const;
	INLINE bool operator<(const VectorKey& rhs) const;
	INLINE bool operator>(const VectorKey& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiVectorKey* _key;
	//double _time;
	//float _vec[3];

	friend class NodeAnimation;
	VectorKey(const aiVectorKey* key);
};

class NodeAnimation
{
public:
	NodeAnimation(const NodeAnimation& node_animation);
	NodeAnimation(void);
	~NodeAnimation(void);

	INLINE const char* getName(void) const;
	INLINE unsigned int getNumPositionKeys(void) const;
	INLINE unsigned int getNumRotationKeys(void) const;
	INLINE unsigned int getNumScaleKeys(void) const;
	INLINE AnimationBehavior getPreAnimState(void) const;
	INLINE AnimationBehavior getPostAnimState(void) const;

	INLINE VectorKey getPositionKey(unsigned int key) const;
	INLINE RotationKey getRotationKey(unsigned int key) const;
	INLINE VectorKey getScaleKey(unsigned int key) const;

	INLINE bool valid(void) const;

	INLINE const NodeAnimation& operator=(const NodeAnimation& rhs);

	INLINE bool operator==(const NodeAnimation& rhs) const;
	INLINE bool operator!=(const NodeAnimation& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiNodeAnim* _node_animation;

	friend class Animation;
	NodeAnimation(const aiNodeAnim* node_animation);
};

NS_END
