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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_ShaderInclude.h"
#include "Gaff_IncludeAssert.h"
#include "Gleam_Global.h"
//#include "Gaff_File.h"

NS_GLEAM

ShaderInclude ShaderInclude::gInclude;

HRESULT __stdcall ShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	assert(IncludeType == D3D_INCLUDE_LOCAL || IncludeType == D3D_INCLUDE_SYSTEM);

	return S_OK;
}

HRESULT __stdcall ShaderInclude::Close(LPCVOID pData)
{
	GleamFree((void*)pData);
	return S_OK;
}

const GleamAString& ShaderInclude::getWorkingDir(void) const
{
	return _working_dir;
}

void ShaderInclude::setWorkingDir(const char* dir)
{
	_working_dir = dir;
}

NS_END

//const char* gSystemDir = "..\\Shader";
// 
//HRESULT __stdcall CShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) {
//    try {
//        std::string finalPath;
//        switch(IncludeType) {
//        case D3D_INCLUDE_LOCAL:
//            finalPath = m_ShaderDir + "\\" + pFileName;
//            break;
//        case D3D_INCLUDE_SYSTEM:
//            finalPath = m_SystemDir + "\\" + pFileName;
//            break;
//        default:
//            assert(0);
//        }
// 
//        std::ifstream includeFile(finalPath.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
// 
//        if (includeFile.is_open()) {
//            long long fileSize = includeFile.tellg();
//            char* buf = new char[fileSize];
//            includeFile.seekg (0, std::ios::beg);
//            includeFile.read (buf, fileSize);
//            includeFile.close();
//            *ppData = buf;
//            *pBytes = fileSize;
//        } else {
//            return E_FAIL;
//        }
//        return S_OK;
//    }
//    catch(std::exception& e) {
//        return E_FAIL;
//    }
//}
// 
//HRESULT __stdcall CShaderInclude::Close(LPCVOID pData) {
//    char* buf = (char*)pData;
//    delete[] buf;
//    return S_OK;
//}

#endif
