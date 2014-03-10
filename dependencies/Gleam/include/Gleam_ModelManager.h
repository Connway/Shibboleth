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

#include "Gleam_HashMap.h"
#include "Gleam_String.h"
#include "Gleam_Array.h"

NS_GLEAM

struct LayoutDescription;
class IRenderDevice;
class IShader;
class IModel;

class ModelManager
{
public:
	enum DATA_TO_GENERATE
	{
		DTG_VERTS = 0x01,
		DTG_NORMALS = 0x02,
		DTG_UVS = 0x04,
		DTG_TANGENTS = 0x08,
		DTG_BITANGENTS = 0x10
	};

	ModelManager(void);
	~ModelManager(void);

	void destroy(void);

	int addModel(IModel* model, const GleamAString& name = "");
	bool removeModel(const GleamAString& name);
	bool removeModel(const IModel* model);
	void removeModel(int index);

	const IModel* getModel(const GleamAString& name) const;
	IModel* getModel(const GleamAString& name);

	INLINE const IModel* getModel(int index) const;
	INLINE IModel* getModel(int index);

	INLINE int getIndex(const GleamAString& name) const;
	INLINE int getIndex(const IModel* model) const;

	bool createPlaneModel(IRenderDevice& rd, unsigned int subdivisions, unsigned int dtg_flags, const IShader* shader, IModel* model);
	bool createSphereModel(IRenderDevice& rd, unsigned int subdivisions, unsigned int dtg_flags, const IShader* shader, IModel* model);
	bool createCubeModel(IRenderDevice& rd, unsigned int subdivisions, unsigned int dtg_flags, const IShader* shader, IModel* model);

private:
	GleamHashMap<GleamAString, int> _name_map;
	GleamArray<IModel*> _models;

	void calculateLayoutDescSizeAndNameString(unsigned int dtg_flags, unsigned int subdivisions, 
											GleamArray<LayoutDescription>& layout_desc, unsigned int& size,
											GleamAString& name) const;
	void generateTangents(float* vertices, unsigned int uv_offset, unsigned int tan_offset,
						unsigned int bitan_offset, unsigned int num_verts, unsigned int vert_size,
						const GleamArray<unsigned int>& indices, unsigned int dtg_flags);
	void addEntry(const GleamAString& name);
};

NS_END
