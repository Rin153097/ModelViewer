#pragma once

#include <windows.h>
#include <string>

inline bool SetClipBoard(const char* text) {
	int		bufSize = static_cast<int>(strlen(text)) + 1;
	char* buf;
	// secure global alloc
	HANDLE	hMem = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, bufSize);
	if (!hMem) return false;

	// be can access
	buf = static_cast<char*>(GlobalLock(hMem));
	if (buf) {
		strcpy_s(buf, bufSize, text);
		GlobalUnlock(hMem);

		if (OpenClipboard(NULL)) {
			EmptyClipboard(); // relearse old data
			SetClipboardData(CF_TEXT, hMem);

			CloseClipboard();

			return true;
		}
	}
	return false;
}