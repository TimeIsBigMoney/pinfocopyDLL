// pinfocopyDLL.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "pinfocopyDLL.h"
#include "resource.h"
#include <vector>
#include <string>

// 内部関数のプロトタイプ宣言
_TCHAR* getVer();
int pinfocopy(int argc, _TCHAR* argv[]);

// バージョン情報取得関数
PINFOCOPYDLL_API LPCWSTR __stdcall version()
{
	static _TCHAR _version[256];
	_tcsset_s(_version, '\0', sizeof(_version));
	_tcscpy_s(_version, getVer());
	_tcscat_s(_version, L"(DLL Ver." VER_STR_PRODUCTVERSION L" " VER_STR_LEGALCOPYRIGHT L")");
	return _version;
}

// 引数解析関数
// スペース区切り文字列を分割するが、ダブルクォーテーションで囲まれた部分は
// スペースを含めて一つの引数として扱う。
// 戻り値: 引数の数 (argc)
int ParseArgs(const _TCHAR* args, std::vector<std::wstring>& argv_storage)
{
	const _TCHAR* p = args;
	bool in_quote = false;
	std::wstring current_arg;

	// pinfocopy の main ロジックは argv[0] をスキップするため、
	// ダミーの argv[0] を先頭に追加しておく。
	argv_storage.push_back(L"pinfocopy");

	// 文字列末尾までループ
	while (*p)
	{
		// 引数の開始位置まで空白をスキップ (引用符外の場合のみ)
		while (*p && iswspace(*p) && !in_quote)
		{
			p++;
		}
		
		if (!*p) break; // 文字列終了

		current_arg.clear();
		
		// 1つの引数を切り出す
		while (*p)
		{
			if (*p == L'"')
			{
				// 引用符の開始・終了を切り替え
				// (引用符自体は引数文字列に含めない)
				in_quote = !in_quote;
			}
			else if (iswspace(*p) && !in_quote)
			{
				// 引用符の外にある空白は引数の区切り
				break;
			}
			else
			{
				// 通常の文字を追加
				current_arg += *p;
			}
			p++;
		}
		
		// 切り出した引数をリストに追加
		argv_storage.push_back(current_arg);
	}

	return (int)argv_storage.size();
}

// メインのエクスポート関数
PINFOCOPYDLL_API int __stdcall pinfocopy(LPCWSTR args)
{
	// 安全対策: 例外が発生してもクラッシュさせずにエラーコードを返す
	try
	{
		// 引数文字列を解析してベクターに格納
		std::vector<std::wstring> argv_storage;
		int argc = ParseArgs(args, argv_storage);

		// 古いCスタイルの argv 配列 (_TCHAR* []) を作成
		// +1 は末尾の NULL 終端用
		std::vector<_TCHAR*> argv(argc + 1);
		
		for (int i = 0; i < argc; ++i)
		{
			// wstring の内部バッファへのポインタを取得
			// 注意: pinfocopy 内部で書き換えが行われない前提 (通常は const扱い)
			// もし書き換えが必要なら _tcsdup 等でコピーする必要があるが、
			// 通常の main(argc, argv) 処理ならこれで動作するはず。
			argv[i] = (_TCHAR*)argv_storage[i].c_str();
		}
		argv[argc] = NULL; // 番兵

		// 実処理を行う pinfocopy 関数 (mainロジック) を呼び出し
		return pinfocopy(argc, argv.data());
	}
	catch (...)
	{
		// 予期せぬエラー (C++例外) を捕捉
		// -999 を「DLL内部クラッシュ」のエラーコードとする
		return -999;
	}
}
