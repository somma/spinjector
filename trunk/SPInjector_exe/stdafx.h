// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#ifndef _WIN32_WINNT		// Windows XP �̻󿡼��� ����� ����� �� �ֽ��ϴ�.                   
#define _WIN32_WINNT 0x0501	// �ٸ� ������ Windows�� �µ��� ������ ������ ������ �ֽʽÿ�.
#endif						

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#define STRSAFE_NO_DEPRECATE
#define STRSAFE_LIB
#include <strsafe.h>
#pragma comment(lib, "strsafe.lib")