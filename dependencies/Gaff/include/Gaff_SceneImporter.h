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

#include "Gaff_Function.h"
#include "Gaff_Scene.h"
#include <assimp/ProgressHandler.hpp>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

NS_GAFF

enum SceneImportFlags
{
	SI_CALC_TANGENTS = aiProcess_CalcTangentSpace,
	SI_JOIN_IDENTICAL_VERTS = aiProcess_JoinIdenticalVertices,
	SI_MAKE_LEFT_HANDED = aiProcess_MakeLeftHanded,
	SI_TRIANGULATE = aiProcess_Triangulate,
	SI_REMOVE_UNNEEDED_DATA = aiProcess_RemoveComponent,
	SI_GENERATE_NORMALS = aiProcess_GenNormals,
	SI_GENERATE_SMOOTH_NORMALS = aiProcess_GenSmoothNormals,
	SI_SPLIT_LARGE_MESHES = aiProcess_SplitLargeMeshes,
	SI_PRE_TRANSFORM_VERTS = aiProcess_PreTransformVertices,
	SI_LIMIT_BONE_WEIGHTS = aiProcess_LimitBoneWeights,
	SI_VALIDATE = aiProcess_ValidateDataStructure,
	SI_IMPROVE_CACHE_LOCALITY = aiProcess_ImproveCacheLocality,
	SI_REMOVE_REDUNDANT_MATERIALS = aiProcess_RemoveRedundantMaterials,
	SI_FIX_IN_FACING_NORMALS = aiProcess_FixInfacingNormals,
	SI_SORT_BY_TYPE = aiProcess_SortByPType,
	SI_FIND_DEGENERATES = aiProcess_FindDegenerates,
	SI_FIND_INVALID_DATA = aiProcess_FindInvalidData,
	SI_GENERATE_UVS = aiProcess_GenUVCoords,
	SI_TRANFORM_UVS = aiProcess_TransformUVCoords,
	SI_FIND_INSTANCES = aiProcess_FindInstances,
	SI_OPTIMIZE_MESHES = aiProcess_OptimizeMeshes,
	SI_OPTIMIZE_GRAPH = aiProcess_OptimizeGraph,
	SI_FLIP_UVS = aiProcess_FlipUVs,
	SI_FLIP_WINDING = aiProcess_FlipWindingOrder,
	SI_SPLIT_BY_BONE_COUNT = aiProcess_SplitByBoneCount,
	SI_DEBONE = aiProcess_Debone
};

class SceneImporter
{
public:
	template <class T>
	void setProgressHandler(T* object, bool (T::*func)(float))
	{
		_loading_handler.setProgressFunc(Bind(object, func));
	}

	template <class T>
	void setProgressHandler(const T& functor)
	{
		_loading_handler.setProgressFunc(Bind(functor));
	}

	SceneImporter(void);
	~SceneImporter(void);

	INLINE Scene loadFile(const char* file_name, unsigned int processing_flags = 0);
	INLINE Scene parseMemory(const void* file, size_t file_len, unsigned int processing_flags = 0, const char* extension_hint = "");

	INLINE void destroyScene(void);
	INLINE Scene getScene(void) const;

	INLINE Scene applyProcessing(unsigned int processing_flags);

	INLINE void setProgressHandler(bool (*func)(float));

	// Register/Unregister file loader
	// Register/Unregister processing

	INLINE const char* getError(void) const;
	INLINE size_t getNumImporters(void) const;
	INLINE void setExtraVerboseMode(bool enable);

private:
	class ProgressHandler : public Assimp::ProgressHandler
	{
	public:
		ProgressHandler(void):
			_progress_func(Bind(this, &ProgressHandler::defaultUpdate))
		{
		}

		bool Update(float percentage)
		{
			return _progress_func(percentage);
		}

		bool defaultUpdate(float)
		{
			return true;
		}

		void setProgressFunc(const Gaff::FunctionBinder<bool, float>& func)
		{
			_progress_func = func;
		}

	private:
		Gaff::FunctionBinder<bool, float> _progress_func;
	};

	Assimp::Importer _importer;
	ProgressHandler _loading_handler;

	GAFF_NO_COPY(SceneImporter);
	GAFF_NO_MOVE(SceneImporter);
};

NS_END
