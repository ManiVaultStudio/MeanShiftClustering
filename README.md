# Mean shift clustering plugin ![Build Status](https://github.com/ManiVaultStudio/MeanShiftClustering/actions/workflows/build.yml/badge.svg?branch=master)

Mean shift clustering plugin for the [ManiVault](https://github.com/ManiVaultStudio/core) visual analytics framework.

```bash
git clone git@github.com:ManiVaultStudio/MeanShiftClustering.git
```

Applies [mean shift](https://en.wikipedia.org/wiki/Mean_shift) clustering to a selected dataset based on [Lampe and Hausers real-time
KDE](https://doi.org/10.1109/PACIFICVIS.2011.5742387) implemented in ManiVault's [core](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/util/MeanShift.h).

<p align="middle">
  <img src="https://github.com/ManiVaultStudio/MeanShiftClustering/assets/58806453/2ecfd02b-0104-4414-8bc3-e1f649520877" align="middle" width="45%" />
  <img src="https://github.com/ManiVaultStudio/MeanShiftClustering/assets/58806453/94ac9bf1-9861-45bf-b8dc-cca85a3cc022" align="middle" width="25%" /> </br>
  Cluster-colored embedding of the MNIST dataset (left) and UI of the mean shift analysis plugin (right)
</p>

## Features
- Real-time updates of the clustering when adjusting the `sigma` parameter
- Several colormaps for automatic coloring of clusters
- Manual color selection for clusters

The plugin requires at least OpenGL 3.3.
