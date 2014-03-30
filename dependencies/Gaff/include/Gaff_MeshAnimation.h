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

struct aiMeshAnim;
struct aiMeshKey;

NS_GAFF

class MeshKey
{
public:
	MeshKey(const MeshKey& key);
	MeshKey(void);
	~MeshKey(void);

	INLINE double getTime(void) const;
	INLINE unsigned int getIndex(void) const;

	INLINE bool valid(void) const;

	INLINE const MeshKey& operator=(const MeshKey& rhs);

	INLINE bool operator==(const MeshKey& rhs) const;
	INLINE bool operator!=(const MeshKey& rhs) const;
	INLINE bool operator<(const MeshKey& rhs) const;
	INLINE bool operator>(const MeshKey& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiMeshKey* _key;
	//double _time;
	//double _index;

	friend class MeshAnimation;
	MeshKey(const aiMeshKey* key);
};

class MeshAnimation
{
public:
	MeshAnimation(const MeshAnimation& mesh_animation);
	MeshAnimation(void);
	~MeshAnimation(void);

	INLINE const char* getName(void) const;
	INLINE unsigned int getNumKeys(void) const;
	INLINE MeshKey getKey(unsigned int key) const;

	INLINE bool valid(void) const;

	INLINE const MeshAnimation& operator=(const MeshAnimation& rhs);

	INLINE bool operator==(const MeshAnimation& rhs) const;
	INLINE bool operator!=(const MeshAnimation& rhs) const;

	INLINE operator bool(void) const
	{
		return valid();
	}

private:
	const aiMeshAnim* _mesh_animation;

	friend class Animation;
	MeshAnimation(const aiMeshAnim* mesh_animation);
};

NS_END
