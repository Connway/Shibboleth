/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include <Log/Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

static constexpr const char8_t* const k_log_channel_name_resource = u8"Resource";
static constexpr Gaff::Hash32 k_log_channel_resource = Gaff::FNV1aHash32StringConst(k_log_channel_name_resource);

#define LogWarningResource(msg, ...) LogWarning(Shibboleth::k_log_channel_resource, msg, ##__VA_ARGS__)
#define LogErrorResource(msg, ...) LogError(Shibboleth::k_log_channel_resource, msg, ##__VA_ARGS__)
#define LogInfoResource(msg, ...) LogInfo(Shibboleth::k_log_channel_resource, msg, ##__VA_ARGS__)

#define LogInfoStringResource(msg, ...) Shibboleth::GetApp().getLogManager().logMessage(Shibboleth::LogType::Info, Shibboleth::k_log_channel_resource, msg);

NS_END
