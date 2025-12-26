[English](RoiPainter4D_README_en.md)

# RoiPainter4D

[RoiPainter4D](RoiPainter4D_README_jp.md) [I/O]() [Visualization]() [Segmentation]()

<!-- teaser.png -->
![teaser](https://github.com/user-attachments/assets/192d0330-7498-45b4-a484-5511ec20487f)

## 概要
RoiPainter4Dは，X線CTやMRI装置で撮影された4次元グレースケール画像を観察・領域分割するためのソフトウエアです． 
井尻敬（芝浦工大）が主体となり，芝浦工大・立命館大・慶応大の学生や，研究機関・医療機関の医師・技師・研究者の協力のもと開発しています．
2018年11月より，C++/CLIベースの開発に切り替え，オープンソース化しました．

本ソフトウエアは研究のためのプロトタイプです．
開発者，開発協力者，及び，開発者所属機関は，本ソフトウエアを利用して起きた損害に関し責任を負いません．

各ツールの利用方法は現在作成中です．

## 導入方法
[こちら](https://github.com/InteractiveGraphicsLab/RoiPainter/releases)より，最新のRoiPainter4Dをダウンロードできます．
1. [ダウンロードページ](https://github.com/InteractiveGraphicsLab/RoiPainter/releases)のAssets → `RoiPainter4D.zip`をダウンロードし，解凍します．
2. 解凍したフォルダ中の`RoiPainter4D.exe`を起動します．

または，ソースコードから[自分でビルド](HowToBuild.md)することもできます．

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

<!--
* 読み込んだマスク画像を編集したい → [曲線制約によるマスク編集](ModeCurveDeform.md)
* 不透明度の差から領域分割したい → [閾値と領域成長法による領域分割](ModeThreshold.md)
* 関心領域の不要な部分を削除したい → [曲線による領域削除](ModeStrokeTrim.md)
-->
