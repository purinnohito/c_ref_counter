Objective-CのMRC環境のようにretainとreleaseを手動で行い、参照カウントでメモリ確保の寿命を管理できます。

使用法
c_reference_counter.hをincludeすることでフルネーム版のメモリ確保、及び管理用関数が使用できます。
test.cppにあるようにc_ref_name.hをincludeする事でshortネームでの使用ができます。
以下簡単な使用例

// 参照カウンタとメモリ確保を同時に行う場合
const char* test_ref() {
  long* lp = (long*)refAlloc(sizeof(long)); // 参照カウンタ領域を付与してメモリの確保
  st_Refcounter ref = getRefCount(lp);      // 確保したメモリから参照カウンタ構造体を取り出す
  mu_assert("error, lp == null", lp != NULL);//メモリの確保チェック
  mu_assert("error, lp cnt != 1", ref->ref_count == 1);//初期状態の参照カウントは1
  refRetain(lp);// 参照カウントを増やす
  mu_assert("error, lp cnt != 2", ref->ref_count == 2);//参照カウント2
  mu_assert("error, lp cnt != 1", refRelease(lp) == 1);//参照カウントを減らす(この時点ではメモリは開放されない)
  mu_assert("error, lp cnt != 0", refRelease(lp) == 0);//参照カウントが0になるとrefAllocで確保した領域が開放される
  return 0;
}

// 参照カウントとメモリを別に確保する例
const char* test_ref_another() {
  st_Refcounter ref_st = {0}; // 参照カウンタの確保
  long* lp = (long*)malloc(sizeof(long)); //参照カウンタで管理するメモリの確保
  link_obj(lp,&ref_st);//参照カウンタとメモリの紐づけ
  mu_assert("error, lp cnt != 1", ref_st.ref_count == 1); //参照カウンタ1
  refCounterRetain(&ref_st); //参照カウンタ1増加
  mu_assert("error, lp cnt != 2", ref_st.ref_count == 2);//参照カウンタ2
  mu_assert("error, lp cnt != 1", refCounterRelease(&ref_st) == 1); //参照カウンタ1
  mu_assert("error, lp cnt != 0", refCounterRelease(&ref_st) == 0); //参照カウンタ0(管理対象メモリlpの開放)
  return 0;
}

## License

These codes are licensed under CC0.

[![CC0](http://i.creativecommons.org/p/zero/1.0/88x31.png "CC0")](http://creativecommons.org/publicdomain/zero/1.0/deed.ja)

