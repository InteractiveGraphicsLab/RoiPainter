[日本語](README.md) | [English](README_en.md)

# RoiPainter

<!-- teaser.png -->
![teaser](https://github.com/user-attachments/assets/192d0330-7498-45b4-a484-5511ec20487f)


## 導入方法
[こちら](https://github.com/InteractiveGraphicsLab/RoiPainter/releases)より，最新のRoiPainter4Dをダウンロードできます．
1. [ダウンロードページ](https://github.com/InteractiveGraphicsLab/RoiPainter/releases)のAssets → `RoiPainter4D.zip`をダウンロードし，解凍します．
2. 解凍したフォルダ中の`RoiPainter4D.exe`を起動します．

または，ソースコードから[自分でビルド](HowToBuild.md)することもできます．

## 使い方

### データの読み込み

* 4DCT画像を読み込みたい → [4DCT画像の読込・表示](ModeVisNormal.md)
* マスク画像を読み込みたい → [マスク画像の読込・表示](ModeVisMask.md)

### 領域の編集

* 読み込んだマスク画像を編集したい → [曲線制約によるマスク編集](ModeCurveDeform.md)
* 不透明度の差から領域分割したい → [閾値と領域成長法による領域分割](ModeThreshold.md)
* 関心領域の不要な部分を削除したい → [曲線による領域削除](ModeStrokeTrim.md)
