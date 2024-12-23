[日本語](README.md) | [English](README_en.md)

# RoiPainter

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

※他の機能の説明は後日追加予定

<!-- * 輝度の差により関心領域を分割したい → [領域成長法による領域分割](ModeRGrow.md)
* テンプレート形状に基づいて組織形状を作成したい → [曲線制約によるテンプレート変形](ModeStrokeFfd.md)
* 読み込んだマスク画像を編集したい → [曲線制約によるマスク編集](ModeCurveDeform.md)・[マスク画像から特定の領域を削除](ModeStrokeTrim.md) -->
