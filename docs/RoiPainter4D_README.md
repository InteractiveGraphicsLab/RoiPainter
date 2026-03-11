[English](RoiPainter4D_README_en.md)
# RoiPainter4D

[**RoiPainter4D**](RoiPainter4D_README.md) | [**I/O**](RoiPainter4D_IO.md) | [**Visualization**](RoiPainter4D_Visualization.md) | [**Segmentation**](RoiPainter4D_Segmentation.md) | [**Refinement**](RoiPainter4D_Refinement.md)

<hr>

## 起動方法
[こちら](https://github.com/InteractiveGraphicsLab/RoiPainter/releases)より，最新のRoiPainter4Dをダウンロードできます．
1. [ダウンロードページ](https://github.com/InteractiveGraphicsLab/RoiPainter/releases)のAssets → `RoiPainter4D.zip`をダウンロードし，解凍します．
2. 解凍したフォルダ中の`RoiPainter4D.exe`を起動します．

または，ソースコードから[自分でビルド](HowToBuild.md)することもできます．

実行にはGPUを搭載した計算機が必要です．（開く画像の解像度によりますが，GRAMが2GByte以上あることが望ましいです）



## 起動画面
![teaser](https://github.com/user-attachments/assets/192d0330-7498-45b4-a484-5511ec20487f)
RoiPainter4Dを起動するとこのような画面が表示されます．各Windowの役割は以下の通りです．
* `d-1` MainWindow：３次元画像を表示する．このWindowを利用して３次元空間へのシード配置や輪郭指定を行う．
* `d-2` 可視化パラメータダイアログ：表示に関するパラメータを指定する．詳しくは[こちら]()
* `d-3` ツール操作用ダイアログ：各可視化・領域分割ツールを操作．ツールごとに異なるダイアログが表示されるので各ツールの説明ページを参照．

もし，MainWindow上に二つの球が表示されず真っ白い画面が表示される場合は，GPUが使えない状態（そもそもグラフィックスカードがない．または，省電力モードのためGPUの機能がoffになっている）だと思われます．



<!--
## 使い方

### データの読み込み・出力
幅広い医療画像フォーマットに対応しています．

* [**Open 4DCT**](ModeVisNormal.md): 4DCT画像の読込・表示 (.dcm, .mha, .traw3d_ub)
* [**Load Mask**](ModeVisMask.md): マスク画像の読込・表示 (.msk4, .mha, .traw3d_ub)
* [**Export・Save**](DataExport.md): データの出力・保存．4DCT画像保存（.traw_ss, .mha），マスク画像保存（.msk4, .mha, .traw3d_ub）のほか，関心領域の重心や固有値のCSVエクスポートが可能です．

### セグメンテーション・編集モード
目的や部位に応じて，多様なセグメンテーション手法を切り替えて使用します．

#### Segmentation
* [**Seg Paint and Lasso**](): 
* [**Seg Threshold and Region grow**](): 閾値処理と領域成長法
* [**Seg Rigid Tracking (ICP)**](): 剛体位置合わせ
* [**Seg Local Region Growing**](): 
* [**Seg Swallow Organs**]():

#### 
* [**Ref Stroke Trim**](): ストロークによる不要な領域の削除


* 読み込んだマスク画像を編集したい → [曲線制約によるマスク編集](ModeCurveDeform.md)
* 不透明度の差から領域分割したい → [閾値と領域成長法による領域分割](ModeThreshold.md)
* 関心領域の不要な部分を削除したい → [曲線による領域削除](ModeStrokeTrim.md)
-->
