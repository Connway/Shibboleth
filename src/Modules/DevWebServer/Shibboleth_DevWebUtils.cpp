/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_DevWebUtils.h"
#include <Shibboleth_Vector.h>
#include <CivetServer.h>

NS_SHIBBOLETH

static ProxyAllocator g_allocator("DevWeb");

Gaff::JSON ReadJSON(mg_connection& connection, const mg_request_info& request)
{
	if (request.content_length <= 0) {
		return Gaff::JSON::CreateNull();
	}

	Vector<int8_t> data_buffer(static_cast<size_t>(request.content_length) + 1, g_allocator);
	int32_t curr_bytes_read = 0;

	while (curr_bytes_read < request.content_length) {
		curr_bytes_read += mg_read(&connection, data_buffer.data(), data_buffer.size());
	}

	data_buffer.back() = 0;

	Gaff::JSON req_data;

	if (!req_data.parse(reinterpret_cast<char8_t*>(data_buffer.data()))) {
		return Gaff::JSON::CreateNull();
	}

	return req_data;
}

void WriteResponse(mg_connection& connection, const char* response)
{
	WriteResponseHeader(connection);
	mg_printf(&connection, "%s", response);
}

void WriteResponseHeader(mg_connection& connection)
{
	mg_printf(&connection, "HTTP/1.1 200 OK\r\n");
	mg_printf(&connection, "Content-Type: application/json; charset=utf-8\r\n");
	mg_printf(&connection, "Connection: close\r\n\r\n");
}

NS_END
