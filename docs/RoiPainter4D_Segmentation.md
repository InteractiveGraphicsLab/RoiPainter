[English](RoiPainter4D_README_en.md)
# RoiPainter4D

[**RoiPainter4D**](RoiPainter4D_README.md) | [**I/O**](RoiPainter4D_IO.md) | [**Visualization**](RoiPainter4D_Visualization.md) | [**Segmentation**](RoiPainter4D_Segmentation.md)

<hr>

## 領域分割 / Segmentation

Roipainter4Dでは，以下のSegmantationツールを用いて4次元CT画像を領域分割することが可能です．あるツールで領域を分割し "Finish Segmentation" ボタンを押すと，前景領域に固有のIDが付与されます．一つの画素には一つのIDをつけることができ（一つの画素が複数のIDを持つことはできません），IDの種類は最大 255です．
画像読み込み時、初期値としてすべての画素にID=0が付与されています.
領域分割データは, マスクデータとして保存・読み込み可能です．また，形状モデル（wavefront obj）として出力することも可能です．


### Seg Paint and Lasso

voxelを直接ペイントしたり，lassoで囲む事で領域を分割するツールです．手間と時間がかかるので，他のツールではどうしてもうまく分割できない領域に利用すると良いです．


### Seg Threshold and Region grow

閾値処理や，閾値を利用した領域拡張法により領域を分割するツールです．他の領域と輝度値の差が大きい領域（骨や造影剤など）の分割に利用できます．


### Seg Rigid Tracking (ICP) 

剛体追跡（Rigid Tracking）により分割するツールです． シードとなる領域形状Aと等値面の閾値を与えると，その領域を等値面にフィットするように剛体変換する事で全てのフレームの分割が可能です． 剛体運動する領域（骨など）の分割に利用できます．


### Seg Local Region growing (shere)

球体状のシードを複数配置すると，そのシード領域内のみで領域拡張が行なわれるツールです． これにより，輝度値にむらのある領域でもうまく分割できる事が有ります．


### Seg Local Region growing (cylinder)

円筒状のシードを複数配置すると，そのシード領域内のみで領域拡張が行なわれるツールです． これにより，輝度値にむらのある領域でもうまく分割できる事が有ります．


### Seg Swallow Organs (FFD)

### Seg Swallow Organs (strokeFFD)