[English](RoiPainter4D_README_en.md)
# RoiPainter4D

[**RoiPainter4D**](RoiPainter4D_README.md) | [**I/O**](RoiPainter4D_IO.md) | [**Visualization**](RoiPainter4D_Visualization.md) | [**Segmentation**](RoiPainter4D_Segmentation.md) | [**Refinement**](RoiPainter4D_Refinement.md)

<hr>

## Input / Output functions of RoiPainter4D
RoiPainter4Dでのデータの読み込みと書き出しについての説明です．

<img align="left" width="300" src="assets/file_menu.png" alt="File menu">
<strong> Open data </strong> <br>
<a href="#d1_open_dcm3d"> D-1) file > Open 4DCT (dcm 3D files) </a> <br>
<a href="#d2_open_dcm2d"> D-2) file > Open 4DCT (dcm 2D files) </a> <br>
<a href="#d3_open_traw3d"> D-3) file > Open 4DCT (traw 3D files) </a> <br>
<a href="#d4_open_mha"> D-2) file > Open 4DCT (mha files) </a> <br>
<br>
<strong> Save and Load mask </strong> <br>
<a href="#m1_save_msk4"> M-1) file > SaveMask (.msk4) </a> <br>
<a href="#m2_save_mha"> M-2) file > SaveMask (.mha) </a> <br>
<a href="#m3_save_trawub"> M-3) file > SaveMask (traw_ub) </a> <br>
<a href="#m4_load_msk4"> M-4) file > LoadMask (.msk4) </a> <br>
<a href="#m5_load_mha"> M-5) file > LoadMask (.mha) </a> <br>
<a href="#m6_load_trawub"> M-6) file > LoadMask (.traw_ub) </a> <br>
<strong> Export data </strong> <br>
<a href="#e1_export_trawss"> E-1) file > Export 4DCT (.traw_ss) </a> <br>
<a href="#e2_export_mha"> E-2) file > Export 4DCT (.mha) </a> <br>
<a href="#e3_Open_raw"> E-3) file > Open 3D 8bit raw (.raw) </a> <br>
<a href="#e4_export_centroid"> E-4) file > Export Mask Centroid (.csv) </a> <br>
<a href="#e5_export_eigenvalue"> E-5) file > Export Mask Eigenvalue (.csv) </a> <br>
<br clear="left"/>

<hr>


### Open data


<a id="d1_open_dcm3d"></a>

**D-1) file > Open 4DCT (dcm 3D files)**

複数の3次元Dicom画像を読み込みます． </br>
各3次元dicomファイルがひとつのフレームを表します． </br>
また，ファイル選択後，名前またはファイルの更新時間によりファイルを並び替えられるダイアログが表示されます． このダイアログを利用して正しい順序に並び替えを行なってください．


<a id="d2_open_dcm2d"></a>

**D-2) file > Open 4DCT (dcm 2D files)**

2次元dicomスライスにより構成される4次元画像を読み込みます．</br>
具体的には，以下のような階層で構成されるフォルダについて，top_folderを指定します．

```
+top_folder
--- +child_dir1
------ dcm_slice1
------ dcm_slice2
------ dcm_slice3
------ ...
--- +child_dir2
------ dcm_slice1
------ dcm_slice2
------ dcm_slice3
------ ...
--- +child_dir3
------ dcm_slice1
------ dcm_slice2
------ dcm_slice3
------ ...
...
```

上の 各child_dirTが，T番目のフレームを表します．


<a id="d3_open_traw3d"></a>

**D-3) file > Open 4DCT (traw 3D files)**

独自形式 traw3d_ss ファイル出力された3次元画像を，複数指定します．このファイルはピッチ情報を持ち，一つの画素をsigned short（16bit）として保持します．
<!--データフォーマットの詳細については<a href="#detail_msk4"> こちら </a>を参照してください．-->


<a id="d4_open_mha"></a>

**D-4) file > Open 4DCT (mha files)**

MetaImageフォーマット（.mha）の3次元画像ファイルを複数読み込みます． </br>
各mhaファイルがひとつのフレームを表します．mhaファイルは，画像の寸法やピクセル間隔（スペーシング）などのメタデータと，実際のボリュームデータが単一のファイルに統合されている形式です． </br>
また，ファイル選択後，名前またはファイルの更新時間によりファイルを並び替えられるダイアログが表示されます． このダイアログを利用して正しい順序に並び替えを行なってください．


<hr>


### Save and Load mask


<a id="m1_save_msk4"></a>

**M-1) file > SaveMask (.msk4)**

作成したマスクデータを独自形式（.msk4）で保存します． </br>
この形式では，全フレームのマスクデータに加え，各領域の名前，色，透明度，ロック状態などのメタデータが1つのファイルにまとめて保存されます．<!--データフォーマットの詳細については[こちら]()を参照してください．-->


<a id="m2_save_mha"></a>

**M-2) file > SaveMask (.mha)**

作成したマスクデータをMetaImageフォーマット（.mha）で保存します．　</br>
1つのフレームにつき1つのファイルとして，連番ファイル（例: `filename00.mha`, `filename01.mha`...）の形式で出力されます．


<a id="m3_save_trawub"></a>

**M-3) file > SaveMask (.traw_ub)**

作成したマスクデータを独自形式 .traw3D_ub で保存します．このファイルはピッチ情報を持ち，一つの画素をunsigned byte（8bit）として保持します． </br>
1つのフレームにつき1つのファイルとして，連番ファイル（例: `filename00.traw_ub`, `filename01.traw_ub`...）の形式で出力されます．


<a id="m4_load_msk4"></a>

**M-4) file > LoadMask (.msk4)**

マスクデータ（.msk4）を読み込みます．既存のマスクデータが上書きされる事があるので注意してください．


<a id="m5_load_mha"></a>

**M-5) file > LoadMask (.mha)**

マスクデータ（.mha）を複数選択して読み込みます．</br>
読み込み時に，スタック方向（Z軸）を逆転させるかどうかのダイアログが表示されるため，必要に応じて反転を行ってください．読み込まれたデータは，マスクの値（ID）に応じて自動的に色分けされ，新しい領域として追加されます．


<a id="m6_load_trawub"></a>

**M-6) file > LoadMask (.traw_ub)**

マスクデータ（.traw_ub）を複数選択して読み込みます．</br>
読み込んだマスクデータは，現在のマスクに新しく追加されます．この際，すでにロックされている領域は保護され，上書きされません．


<hr>


### Export data


<a id="e1_export_trawss"></a>

**E-1) file > Export 4DCT (.traw_ss)**

現在読み込んでいる4次元画像を，独自形式（.traw3D_ss）で保存します． </br>
各フレームの画像データが，連番ファイル（例: `filename00.traw3D_ss`, `filename01.traw3D_ss`...）として個別に出力されます．


<a id="e2_export_mha"></a>

**E-2) file > Export 4DCT (.mha)**

現在読み込んでいる4次元画像を，MetaImageフォーマット（.mha）で保存します． </br>
各フレームの画像データが，連番ファイル（例: `filename00.mha`, `filename01.mha`...）として個別に出力されます．


<a id="e3_Open_raw"></a>

**E-3) file > Open 3D 8bit raw (.raw)**

ヘッダー情報を持たない，8bitのRAW形式の3次元画像データを読み込みます．


<a id="e4_export_centroid"></a>

**E-4) file > Export Mask Centroid (.csv)**

選択されているマスク領域について，各フレームごとの重心座標（x, y, z）を計算し，CSVファイル（.csv）として出力します．


<a id="e5_export_eigenvalue"></a>

**E-5) file > Export Mask Eigenvalue**

選択されているマスク領域について，各フレームごとの固有値（Eigenvalue）および固有ベクトル（Eigenvector）を計算し，CSVファイル（.csv）として出力します．


<hr>


<a id="detail_msk4"></a>

<!--
### .msk4 ファイル仕様

`.msk4` は本ツール独自のバイナリ形式です。自作の解析プログラム等で読み込む際は、以下のデータ構造（Little Endian）を参照してください。

| セクション | データ型 | 内容 |
| :--- | :--- | :--- |
| **Header** | `int` | バージョン情報 (現在: 0) |
| **Resolution** | `int` x 4 | 幅(W), 高さ(H), 奥行(D), フレーム数(F) |
| **Mask Info** | `int` | 登録されているマスクの数 (maskN) |
| **Mask Details** | (可変) | 各マスクのメタデータ（※下記参照） |
| **Mask Volume** | `byte`配列 | W×H×D サイズのデータ × Fフレーム分 |

#### Mask Details の内訳（各マスクごと）
1. **名前の長さ**: `int` (nLen)
2. **名前文字列**: `char`配列 (nLen + 1 バイト / null終端含む)
3. **透明度**: `double` (alpha)
4. **色**: `int` x 3 (RGB)
5. **ロック状態**: `int` (0:解除, 1:ロック)

#### Mask Volume の構造
各ピクセルは `byte` 型で保存されており、その値は **Mask ID** に対応しています。
-->

<!--
## Open 4DCT data
4DCT画像（dcm，traw3d，mha）を読み込みます．

以下の手順で4DCT画像を読み込みます．
1. メインウィンドウ左上のFileメニューから，ファイルの形式に応じた`Open 4DCT`を選択します．
2. ファイル選択ウィンドウより，読み込む4DCT画像のファイルを**すべて**選択します．
3. ファイルの読み込む順番を変更できます．特に問題が無ければ，`Import Files`を選択します．
4. しばらくすると，読み込みが完了します．


<!-- load_4dct.mp4 
https://github.com/user-attachments/assets/47884331-e0bb-47e1-a75e-43cf0e63fb50



## Save and Load Mask
### Save Mask
作成したマスクをマスクデータ（.msk4，.mha，.traw3d_ub）として保存します．

### Load Mask
マスクデータ（.msk4，.mha，.traw3d_ub）を読み込みます．ソフトウエア上でマスクを作成中・編集中の場合，そのマスクデータが上書きされるので注意してください．

4DCT画像を読み込んだら，以下の手順でマスク画像を読み込みます．
1. メインウィンドウ左上のFileメニューから，ファイルの形式に応じた`Load Mask`を選択します．
2. ファイル選択ウィンドウより，読み込むマスク画像のファイルを**すべて**選択します．
3. スタック方向を逆転するか選択するダイアログが出現します．基本的に「いいえ」を選択します．  
読み込むファイルによってはスタック方向が逆になっているので，「いいえ」を選択して上手くいかなかった場合は1.からやり直して「はい」を選択してください．
4. しばらくすると，読み込みが完了します．

<!-- load_mask.mp4 
https://github.com/user-attachments/assets/c3a25626-7648-4a00-aba5-ed6be988611d



## Export
### Export 4DCT 
現在読み込んである画像データを保存します．
-->



[RoiPainter4D Top](RoiPainter4D_README.md)