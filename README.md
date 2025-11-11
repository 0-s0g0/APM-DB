## 錠剤をマスタに追加するシステム

### 一個ずつマスタに登録する方法
1. ターミナルを開く
2. 該当ディレクトリに移動
```
 cd .\x64\Debug\
```
3. <実行フォルダ><読み込む画像パス><読み込む画像の錠剤番号><追加するDB名>
```
 .\addPillToMasterDB.exe ..\Release\imageTablet_21\No000_300dpi.bmp No000 ..\Release\master_test.db
```

### 一気にマスタに登録する方法
1.batファイルをクリックする
D:\addPillToMasterDB\x64\Debug\makemaster_21.bat


