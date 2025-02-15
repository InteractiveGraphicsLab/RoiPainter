[TOPページ](README.md)

# 曲線制約によるマスク編集（Ref Curve Deform モード）
このモードでは，関心領域の正確な境界を曲線で直接指定することにより，マスク画像を編集できます．

## 前準備

1. まだ4DCT画像を読み込んでいない場合は，[こちら](ModeVisNormal.md)を参考に4DCT画像を読み込んでください．
2. まだマスク画像を読み込んでいない場合は，[こちら](ModeVisMask.md)を参考にマスク画像を読み込んでください．
3. ウィンドウ左上の"mode switch"から，"Ref Curve Deform"モードを選択してください．
4. 編集の対象とするマスクを選択し，"OK"をクリックしてください．

## 使い方

<!-- ref_curve_deform_demo.mp4 -->
https://github.com/user-attachments/assets/ac83da3b-0555-48ea-95de-bb3ac88154b1

1. 「**Convert mask -> mesh**」ボタンを押し，マスクをメッシュに変換します．
2. 関心領域の正しい境界に曲線制約を配置します．曲線制約は制御点により配置でき，制御点はSHIFT+左クリックで配置できます．
3. 曲線制約を十分に配置したら，「Deform」ボタンを押します．これにより，関心領域が配置した曲線に沿うように変形されます．
4. 編集が完了したら，「**Convert mesh -> mask**」ボタンを押し，メッシュをマスクに戻します．


## 曲線の操作

<!-- ref_curve_deform_curveediting.mp4 -->
https://github.com/user-attachments/assets/1ece9fd3-1a54-413f-a9a0-7bd2cfc2f854

### 制御点の操作

| 操作 | 説明 |
| --- | --- |
| （断面に対して）SHIFT+左クリック | 制御点を追加 |
| （制御点に対して）SHIFT+左ドラッグ | 制御点を移動 |
| （制御点に対して）SHIFT+右クリック | 制御点を削除 |

### 曲線の操作
| 操作 | 説明 |
| --- | --- |
| （非選択状態（黄色）の曲線の制御点に対して）SHIFT+左クリック | 曲線を選択状態に |
| （選択状態（赤色）の曲線があるとき，断面に対して）SHIFT+左クリック | 選択状態の曲線に制御点を追加 |
| （選択状態（赤色）の曲線がないとき，断面に対して）SHIFT+左クリック | 新しい曲線のための制御点を追加（3つ以上で曲線に） |


## ダイアログ

<!-- dialog_curvedeform.png -->
![dialog_curvedeform](https://github.com/user-attachments/assets/3a96967c-47c4-4122-a954-3d4b78831463)

### メッシュとマスクの操作
| 操作 | 説明 |
| --- | --- |
| Convert Mask -> Mesh | 選択したマスクをメッシュに変換 |
| Deform | 配置した曲線に沿うようにメッシュを変形 |
| Reload mesh | メッシュを変形前に戻す |
| Convert mesh -> mask | 現在のメッシュをマスクに変換 |

### 制御点と曲線の操作
| 操作 | 説明 |
| --- | --- |
| UNDO, REDO | 曲線の編集を戻す，やり直す |
| Set as all frame curve | 選択中の曲線を共有曲線に設定 |
| Load state | 保存済の編集状態を読込 |
| Save state | 現在の編集状態を保存 |
| Copy from prev frame | 前のフレームで配置した曲線を現在のフレームに上書き |
| Copy strokes to all frame | 現在のフレームの曲線を他のすべてのフレームに上書き |
| CP size | 制御点の大きさを変更 |
