#include "MeanShiftClusteringPlugin.h"

#include "PointData.h"
#include "ClusterData.h"

#include <QDebug>
#include <QtCore>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPainter>

#include <assert.h>
#include <algorithm>
#include <functional>
#include <random>
#include <vector>
#include <unordered_map>

#define NO_FILE 0

Q_PLUGIN_METADATA(IID "nl.BioVault.MeanShift")

using namespace hdps;
using namespace hdps::gui;

MeanShiftClusteringPlugin::MeanShiftClusteringPlugin(const PluginFactory* factory) :
    AnalysisPlugin(factory),
    _offscreenBuffer(),
    _meanShift(),
    _settingsAction(this),
    _rng(0)
{
}

MeanShiftClusteringPlugin::~MeanShiftClusteringPlugin()
{
}

void MeanShiftClusteringPlugin::init()
{
    // Create clusters output dataset
    setOutputDatasetName(_core->addData("Cluster", "Clusters", getInputDatasetName()));

    _core->notifyDataAdded(getOutputDatasetName());

    // Get input and output datasets
    auto& inputDataset  = getInputDataset<Points>();
    auto& outputDataset = getOutputDataset<Clusters>();

    // Inject the setting action into the output dataset
    outputDataset.addAction(_settingsAction);

    // Get the number of dimensions in the input dataset
    const auto numberOfDimensions = inputDataset.getNumDimensions();

    QStringList dimensionNames;
    
    if (numberOfDimensions == inputDataset.getDimensionNames().size()) {
        for (auto dimensionName : inputDataset.getDimensionNames())
            dimensionNames << dimensionName;
    }
    else {
        for (std::uint32_t dimensionIndex = 0; dimensionIndex < numberOfDimensions; dimensionIndex++)
            dimensionNames << QString("Dim %1").arg(QString::number(dimensionIndex));
    }

    _settingsAction.getDimensionOneAction().setOptions(dimensionNames);
    _settingsAction.getDimensionTwoAction().setOptions(dimensionNames);

    // Establish whether dimension can/should be picked
    const auto canPickDimensions = numberOfDimensions > 2;

    _settingsAction.getDimensionOneAction().setEnabled(canPickDimensions);
    _settingsAction.getDimensionTwoAction().setEnabled(canPickDimensions);

    // Handle dataset with only one dimension
    if (numberOfDimensions == 1) {
        _settingsAction.getDimensionOneAction().setCurrentIndex(0);
        _settingsAction.getDimensionOneAction().setDefaultIndex(0);
        _settingsAction.getDimensionTwoAction().setCurrentIndex(0);
        _settingsAction.getDimensionTwoAction().setDefaultIndex(0);

        QMessageBox warning;

        warning.setText("Selected data must have at least two dimensions");
        warning.exec();

        _settingsAction.setEnabled(false);
    }

    // Handle dataset with two or more dimensions
    if (numberOfDimensions >= 2) {
        _settingsAction.getDimensionOneAction().setCurrentIndex(0);
        _settingsAction.getDimensionOneAction().setDefaultIndex(0);
        _settingsAction.getDimensionTwoAction().setCurrentIndex(1);
        _settingsAction.getDimensionTwoAction().setDefaultIndex(1);
    }

    // Updates the cluster colors depending on the color configuration
    const auto updateColors = [this, &outputDataset]() -> void {
        const auto colorBy = static_cast<SettingsAction::ColorBy>(_settingsAction.getColorByAction().getCurrentIndex());

        switch (colorBy) {
            case SettingsAction::ColorBy::PseudoRandomColors:
            {
                // Seed the random number generator
                _rng.seed(_settingsAction.getRandomSeedAction().getValue());

                // Generate pseudo-random cluster colors
                for (auto& cluster : outputDataset.getClusters()) {
                    const auto randomHue        = _rng.bounded(360);
                    const auto randomSaturation = _rng.bounded(150, 255);
                    const auto randomLightness  = _rng.bounded(50, 200);

                    // Create random color from hue, saturation and lightness
                    cluster.setColor(QColor::fromHsl(randomHue, randomSaturation, randomLightness));
                }

                break;
            }

            case SettingsAction::ColorBy::ColorMap:
            {
                // Get output clusters
                auto& clusters = outputDataset.getClusters();

                // Get scaled version of the color map image that matches the width to the number of clusters
                const auto& colorMapImage = _settingsAction.getColorMapAction().getColorMapImage().scaled(static_cast<std::int32_t>(clusters.size()), 4);

                auto clusterIndex = 0;

                // Color clusters according to the color map image
                for (auto& cluster : clusters) {
                    cluster.setColor(colorMapImage.pixel(clusterIndex, 0));
                    clusterIndex++;
                }

                break;
            }
        }
    };

    connect(&_settingsAction.getComputeAction(), &TriggerAction::triggered, this, [this, &inputDataset, &outputDataset, updateColors]() {

        // Do not run if the selected dimensions are the same
        if (_settingsAction.getDimensionOneAction().getCurrentIndex() == _settingsAction.getDimensionTwoAction().getCurrentIndex()) {
            setTaskDescription("Input error: identical dimensions");
            return;
        }

        // Update the sigma value
        _meanShift.setSigma(_settingsAction.getSigmaAction().getValue());

        // Disable the settings when computing
        _settingsAction.setEnabled(false);

        setTaskRunning();
        setTaskName("Mean-shift clustering");
        setTaskProgress(0.0f);
        setTaskDescription("Initializing");

        QCoreApplication::processEvents();

        std::vector<hdps::Vector2f> data;
        inputDataset.extractDataForDimensions(data, _settingsAction.getDimensionOneAction().getCurrentIndex(), _settingsAction.getDimensionTwoAction().getCurrentIndex());

        setTaskDescription("Clustering");

        QCoreApplication::processEvents();

        _meanShift.setData(&data);

        std::vector<std::vector<unsigned int>> clusters;

        setTaskProgress(0.2f);

        QCoreApplication::processEvents();

        _offscreenBuffer.bindContext();
        _meanShift.cluster(data, clusters);
        _offscreenBuffer.releaseContext();

        setTaskProgress(0.7f);

        QCoreApplication::processEvents();

        // Remove existing clusters
        outputDataset.getClusters().clear();
        
        std::int32_t clusterIndex = 0;

        // Add found clusters
        for (auto c : clusters)
        {
            Cluster cluster;

            cluster.setName(QString("cluster %1").arg(QString::number(clusterIndex + 1)));
            cluster.setIndices(c);

            outputDataset.addCluster(cluster);

            clusterIndex++;
        }

        updateColors();

        _core->notifyDataChanged(getOutputDatasetName());

        setTaskFinished();

        _settingsAction.setEnabled(true);
    });

    connect(&_settingsAction.getDimensionOneAction(), &OptionAction::currentIndexChanged, this, [this, dimensionNames](const std::int32_t& currentIndex) mutable -> void {
        _settingsAction.getComputeAction().trigger();
    });
    
    connect(&_settingsAction.getDimensionTwoAction(), &OptionAction::currentIndexChanged, this, [this, dimensionNames](const std::int32_t& currentIndex) mutable -> void {
        _settingsAction.getComputeAction().trigger();
    });

    connect(&_settingsAction.getSigmaAction(), &DecimalAction::valueChanged, this, [this](const double& value) {
        _settingsAction.getComputeAction().trigger();
    });

    connect(&_settingsAction.getColorByAction(), &OptionAction::currentIndexChanged, this, [this, updateColors](const std::int32_t& currentIndex) {
        if (_settingsAction.getUpdateColorsManuallyAction().isChecked())
            return;

        updateColors();

        _core->notifyDataChanged(getOutputDatasetName());
    });

    connect(&_settingsAction.getColorMapAction(), &ColorMapAction::imageChanged, this, [this, updateColors](const QImage& image) {
        if (_settingsAction.getUpdateColorsManuallyAction().isChecked())
            return;

        updateColors();

        _core->notifyDataChanged(getOutputDatasetName());
    });

    connect(&_settingsAction.getRandomSeedAction(), &IntegralAction::valueChanged, this, [this, updateColors](const std::int32_t& value) {
        if (_settingsAction.getUpdateColorsManuallyAction().isChecked())
            return;

        updateColors();

        _core->notifyDataChanged(getOutputDatasetName());
    });

    connect(&_settingsAction.getApplyColorsAction(), &TriggerAction::triggered, this, [this, updateColors](const std::int32_t& value) {
        updateColors();

        _core->notifyDataChanged(getOutputDatasetName());
    });

    _offscreenBuffer.bindContext();
    _meanShift.init();
    _offscreenBuffer.releaseContext();

    // Do an initial computation
    _settingsAction.getComputeAction().trigger();
}

bool MeanShiftClusteringPlugin::canCompute() const
{
    if (!_input.isValid())
        return false;

    return true;
}

QIcon MeanShiftClusteringPluginFactory::getIcon() const
{
    const auto margin       = 3;
    const auto pixmapSize   = QSize(100, 100);
    const auto pixmapRect   = QRect(QPoint(), pixmapSize).marginsRemoved(QMargins(margin, margin, margin, margin));
    const auto halfSize     = pixmapRect.size() / 2;
    const auto quarterSize  = halfSize / 2;

    // Create pixmap
    QPixmap pixmap(pixmapSize);

    // Fill with a transparent background
    pixmap.fill(Qt::transparent);

    // Create a painter to draw in the pixmap
    QPainter painter(&pixmap);

    // Enable anti-aliasing
    painter.setRenderHint(QPainter::Antialiasing);

    // Get the text color from the application
    const auto textColor = QApplication::palette().text().color();

    // Configure painter
    painter.setPen(QPen(textColor, 1, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));
    painter.setFont(QFont("Arial", 38, 250));

    const auto textOption = QTextOption(Qt::AlignCenter);

    // Do the painting
    painter.drawText(QRect(QPoint(pixmapRect.left(), pixmapRect.center().y() - quarterSize.height()), halfSize), "M", textOption);
    painter.drawText(QRect(QPoint(pixmapRect.center().x(), pixmapRect.center().y() - quarterSize.height()), halfSize), "S", textOption);

    return QIcon(pixmap);
}

AnalysisPlugin* MeanShiftClusteringPluginFactory::produce()
{
    return new MeanShiftClusteringPlugin(this);
}

hdps::DataTypes MeanShiftClusteringPluginFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;
    supportedTypes.append(PointType);
    return supportedTypes;
}
