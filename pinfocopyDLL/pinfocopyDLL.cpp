// pinfocopyDLL.cpp : コンソール アプリケーションのエントリ ポイント
#include "stdafx.h"

// 内部ロジック (pinfocopy.cpp 等にあるはず) のプロトタイプ宣言
int pinfocopy(int argc, _TCHAR* argv[]);

// 標準のエントリーポイント
// OSが引数を解析して argc, argv に入れて渡してくれる
int _tmain(int argc, _TCHAR* argv[])
{
	// そのまま内部ロジックに渡すだけ
	// argv[0] には実行ファイルパスが入っているので、pinfocopy関数側の仕様(argv[0]スキップ)とも一致する
	return pinfocopy(argc, argv);
}
