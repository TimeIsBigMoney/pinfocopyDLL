// pinfocopyDLL.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "pinfocopyDLL.h"
#include "resource.h"
#include <vector>
#include <string>

// バージョン情報取得関数（変更なし）
_TCHAR* getVer();
int pinfocopy(int argc, _TCHAR* argv[]);

PINFOCOPYDLL_API LPCWSTR __stdcall version()
{
	static _TCHAR _version[256];
	_tcsset_s(_version,'\0',sizeof(_version));
	_tcscpy_s(_version, getVer());
	_tcscat_s(_version, L"(DLL Ver." VER_STR_PRODUCTVERSION L" " VER_STR_LEGALCOPYRIGHT L")");
	return _version;
}

// 引数解析用ヘルパー関数
// スペース区切りだが、ダブルクォーテーションで囲まれた部分はひとかたまりとして扱う
int ParseArgs(const _TCHAR* args, std::vector<std::wstring>& argv_storage)
{
    const _TCHAR* p = args;
    bool in_quote = false;
    std::wstring current_arg;

    // 最初のダミー引数 (argv[0]) 用
    argv_storage.push_back(L"pinfocopy"); 

    while (*p)
    {
        // 空白スキップ (引用符外)
        while (*p && iswspace(*p) && !in_quote) p++;
        if (!*p) break;

        current_arg.clear();
        while (*p)
        {
            if (*p == L'"')
            {
                // 引用符の切り替え (文字としては追加しない)
                in_quote = !in_quote;
            }
            else if (iswspace(*p) && !in_quote)
            {
                // 引用符外での空白は引数の区切り
                break;
            }
            else
            {
                // 文字を追加
                current_arg += *p;
            }
            p++;
        }
        argv_storage.push_back(current_arg);
    }

    return (int)argv_storage.size();
}

PINFOCOPYDLL_API int __stdcall pinfocopy(LPCWSTR args)
{
    // std::vector を使って動的に管理 (バッファオーバーフロー防止)
    std::vector<std::wstring> argv_storage;
    
    // 自前のパーサーで引数を分割
    int argc = ParseArgs(args, argv_storage);

    // 古い argv 配列形式に変換 (pinfocopy関数に渡すため)
    // 最大引数数は余裕を見て確保
    std::vector<_TCHAR*> argv(argc + 1);
    for (int i = 0; i < argc; ++i)
    {
        // wstring のバッファへのポインタを渡す (const_castが必要な場合あり)
        argv[i] = (_TCHAR*)argv_storage[i].c_str();
    }
    argv[argc] = NULL;

    // 元の処理関数を呼び出す
    return pinfocopy(argc, argv.data());
}
