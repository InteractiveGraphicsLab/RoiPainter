[English](RoiPainter4D_README_en.md)
# RoiPainter4D

[**RoiPainter4D**](RoiPainter4D_README.md) | [**I/O**](RoiPainter4D_IO.md) | [**Visualization**](RoiPainter4D_Visualization.md) | [**Segmentation**](RoiPainter4D_Segmentation.md) | [**Refinement**](RoiPainter4D_Refinement.md)

<hr>

## Visualization in RoiPainter4D

### 起動画面
![teaser](https://github.com/user-attachments/assets/192d0330-7498-45b4-a484-5511ec20487f)
RoiPainter4Dを起動するとこのような画面が表示されます．各Windowの役割は以下の通りです．
* `d-1` MainWindow：３次元画像を表示する．このWindowを利用して３次元空間へのシード配置や輪郭指定を行う．
* `d-2` 可視化パラメータダイアログ：表示に関するパラメータを指定する．詳しくは[こちら]()
* `d-3` ツール操作用ダイアログ：各可視化・領域分割ツールを操作．ツールごとに異なるダイアログが表示されるので各ツールの説明ページを参照．



### MainWindow
4DCT画像を読み込んだ後は，様々な操作を行うことができます．

#### 視点操作
視点の移動：マウス左ボタンを押したまま移動  
視点の角度変更：マウス右ボタンを押したまま移動  
ズーム：マウスホイールを押したまま上下でズームイン/ズームアウト

<!-- move_viewpoint.mp4 -->
https://github.com/user-attachments/assets/dd32bb60-9242-41bc-9ee0-28ca09f91fe6

#### 断面操作
断面にカーソルを合わせてマウスホイールを転がすと可視化する断面を変更できます．  
また，右上のダイアログでどの断面を可視化するか選択できます．

<!-- move_slice.mp4 -->
https://github.com/user-attachments/assets/0572d184-1a4f-4dbd-b83f-d4d8e1420ba5



### 可視化パラメータダイアログ
RoiPainter4Dでは，2つのVisualizationモードと，7つのSegmentationモード，2つのRefinementモーどが使用可能です（2025/12現在）．
すべてのモードで，下の”可視化パラメータダイアログ”が表示され，これを利用して可視化に関する項目を指定可能です．

<!-- dialog_param.png -->
![dialog_param](https://github.com/user-attachments/assets/0891b9c1-df11-44f8-b4d2-a7444a2c8abc)

#### 1. 全体設定（赤枠）

| 項目 | 説明 |
| --- | --- |
| size | 読み込んだ画像の解像度を表示． |
| pitch | 画像のpitch(画素の大きさ)を提示．自身で編集することも可能． |
| WinLv | 表示する画像のWindow Levelの最小値/最大値を指定． |
| frame | 画像周囲のフレーム表示の有無． |
| volume | 三次元画像のvolume renderingの有無． |
| pseudo | 疑似カラー利用の有無． |
| indicator | 左下の方向インジケータ表示の有無． |
| Plane | xy, yz, zx断面の表示の有無．(これに加えて ctrl + マウス左ドラッグで曲断面を生成できる) |
| Background | 背景色を指定するチェックボックス． |

#### 2. ボリューム可視化設定（青枠）

| 項目 | 説明 |
| --- | --- |
| Opacify | ボリュームの透明度を変更できます． |
| Num of slice | ボリュームの細かさを変更できます． |
| ヒストグラム | マウスでドラッグすると，表示する範囲を変更できます． |

#### 3. フレーム移動（緑枠）
このスライダーを動かすと，4DCT画像のフレームを移動できます．



### Visualization Normal
このVisualization Normalは，3次元画像を観察するためのツールです．単純なvolume renderingや，平面断面可視化，曲面断面可視化が可能です．

"`Menu`>`ModeSwitch`>`Vis Normal`" をクリックすることで起動できます．

このモードを起動すると，下のツールダイアログが表示され，切断面上にカーソルを置くとその位置の輝度値がダイアログに表示されます．下の図の通り，各ツールのダイアログにはそのツールの簡単な操作方法が記載されています．



### Visualization Mask
このVisualization Maskは，作成したマスク（分割領域）を観察・編集するためのツールです．

"`Menu`>`ModeSwitch`>`Vis Mask`" をクリックすることで起動できます．
または，領域分割ツールを終了した場合も自動的にこのツールが起動します．

このツールを起動すると，下の通りMain Windowに分割した領域(ROI)が色付きで表示され，右下にツールダイアログも表示されます

<!-- Vis Maskモードの画像 -->

このVisualization Maskでは，ツールダイアログより以下の操作が可能です．

<!-- dialog_mask.png -->
![dialog_mask](https://github.com/user-attachments/assets/fae3fcef-6449-4b18-99cd-45e822addfd9)

#### 1. マスク選択（赤枠）
分割した領域を選択できます．（一番上0番は，読み込み時にすべての画素に付与される背景IDです）

#### 2. マスク管理（青枠）

| 操作 | 説明 |
| --- | --- |
| Lock | オンにすると，選択領域をLockし，後に領域分割ツールにより新たな領域IDがつけられることを防げます． |
| color | 選択領域の色を変更できます． |
| alpha | 選択領域の透明度を変更できます． |
| DELETE | 選択領域を削除します． |
| MARGE TO | 選択領域と新たに選択する領域が，1つの領域として結合されます． |


#### 3. マスク編集（緑枠）
左側が現在のフレームに対する操作，右側が全フレームに対する操作となります．

| 操作 | 説明 |
| --- | --- |
| erode | 選択領域に対し，収縮処理を施します． |
| dilate | 選択領域に対し，膨張処理を施します． |
| fill hole | 選択領域に対し，中空領域を埋める処理を行います． |
| Exp Obj | 選択領域をobjメッシュとして書き出します． |
| Imp Obj | objメッシュを取り込み新たな領域として登録します． |



[RoiPainter4D Top](RoiPainter4D_README.md)