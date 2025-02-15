[TOPページ](README.md)

# 閾値と領域成長法による領域分割（Seg Threshold and Region grow モード）

## 閾値による分割

<!-- seg_threshold.mp4 -->
https://github.com/user-attachments/assets/b33239ae-5eb3-402b-9991-46465c18e6e3

## 領域成長法による分割

<!-- seg_regiongrow.mp4 -->
https://github.com/user-attachments/assets/ad05cb05-0181-4a32-aba2-6a23aa99986c

## ダイアログ

<!-- dialog_threthold.png -->
![dialog_threthold](https://github.com/user-attachments/assets/c4823a13-7ef6-47a2-a535-c999c862ebaf)

### 1. 閾値の設定（赤枠）
| 操作 | 説明 |
| --- | --- |
| min | 関心領域とみなす閾値の最小値を設定 |
| max | 関心領域とみなす閾値の最大値を設定 |

### 2. 現在のフレームに対する処理（青枠）
| 操作 | 説明 |
| --- | --- |
| Threshold | 現在のフレームにおいて，1で設定した閾値に基づいて分割を行う |
| Region Grow | 現在のフレームにおいて，配置した制御点により領域成長法による分割を行う |

### 3. 全てのフレームに対する処理（緑枠）
| 操作 | 説明 |
| --- | --- |
| Threshold | すべてのフレームにおいて，1で設定した閾値に基づいて分割を行う |
| Region Grow | すべてのフレームにおいて，配置した制御点により領域成長法による分割を行う |
| erode | すべてのフレームにおいて，関心領域を収縮させる |
| dilate | すべてのフレームにおいて，関心領域を膨張させる |
| fill hole | すべてのフレームにおいて，関心領域に存在する微細な穴を埋めるよう試みる |

### 4. 完了処理（黄枠）
| 操作 | 説明 |
| --- | --- |
| Finish and Store | 作業を完了し，分割結果を新しい領域として登録する |
| Cancel | 作業内容を取り消す |
