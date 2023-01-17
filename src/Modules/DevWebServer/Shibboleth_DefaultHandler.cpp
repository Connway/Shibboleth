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

#include "Shibboleth_DefaultHandler.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Defines.h>

NS_SHIBBOLETH

static void SendDefaultPage(mg_connection* conn)
{
	mg_printf(conn, "HTTP/1.1 200 OK\r\n");
	mg_printf(conn, "Content-Type: text/html; charset=utf-8\r\n");
	mg_printf(conn, "Connection: close\r\n\r\n");

	mg_printf(conn, "<html><body>\r\n");
	mg_printf(conn, "<h1>Available Commands</h1>\r\n");
	mg_printf(conn, "Foo\r\n");
	mg_printf(conn, "</body></html>\r\n");
}

static bool SendFile(mg_connection* conn, IFileSystem& fs, const char8_t* file_path)
{
	IFile* const file = fs.openFile(file_path);

	if (file) {
		const char* const mime_type = mg_get_builtin_mime_type(reinterpret_cast<const char*>(file_path));

		mg_printf(conn, "HTTP/1.1 200 OK\r\n");
		mg_printf(conn, "Content-Type: %s; charset=utf-8\r\n", mime_type);
		mg_printf(conn, "Connection: close\r\n\r\n");
		mg_write(conn, file->getBuffer(), file->size());

		fs.closeFile(file);
		return true;
	}

	return false;
}

bool DefaultHandler::handleGet(CivetServer*, mg_connection* conn)
{
	const struct mg_request_info* const req_info = mg_get_request_info(conn);

	// Requesting home page.
	if (req_info->local_uri && req_info->local_uri[0] == '/' && req_info->local_uri[1] == 0) {
		SendDefaultPage(conn);

	} else if (req_info->local_uri) {
		U8String file_path(U8String::CtorSprintf(), u8"WebRoot%s", req_info->local_uri);
		IFileSystem& fs = GetApp().getFileSystem();

		if (!SendFile(conn, fs, file_path.data())) {
			if (file_path.back() == '/') {
				file_path += u8"index.html";
			} else {
				file_path += u8"/index.html";
			}

			if (!SendFile(conn, fs, file_path.data())) {
				mg_send_http_error(conn, 404, "Error: File not found");
			}
		}


	} else {
		mg_send_http_error(conn, 404, "Error: File not found");
	}

	return true;
}

NS_END
