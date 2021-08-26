#include "MeanShiftClusteringPlugin.h"

#include "PointData.h"
#include "ClusterData.h"

#include <QDebug>
#include <QtCore>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QRandomGenerator>

#include <assert.h>
#include <algorithm>
#include <functional>
#include <random>
#include <vector>
#include <unordered_map>

#define NO_FILE 0

Q_PLUGIN_METADATA(IID "nl.tudelft.MeanShift")

using namespace hdps;
using namespace hdps::gui;

MeanShiftClusteringPlugin::MeanShiftClusteringPlugin(const PluginFactory* factory) :
    AnalysisPlugin(factory),
    _offscreenBuffer(),
    _meanShift(),
    _settingsAction(this)
{
}

MeanShiftClusteringPlugin::~MeanShiftClusteringPlugin(void)
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

    /*
    connect(&_meanShift, &MeanShift::progressSection, this, [this](const QString& section) {
        setTaskDescription(section);
    });

    connect(&_meanShift, &MeanShift::progressPercentage, this, [this](const float& percentage) {
        setTaskProgress(percentage);
    });
    */

    connect(&_settingsAction.getComputeAction(), &TriggerAction::triggered, this, [this, &inputDataset, &outputDataset]() {

        // Disable the settings when computing
        _settingsAction.setEnabled(false);

        setTaskName("Mean-shift clustering");
        setTaskProgress(0.0f);
        setTaskDescription("Initializing");

        // Data sanity check
        if (inputDataset.getNumDimensions() != 2)
        {
            QMessageBox warning;
            warning.setText("Selected data must be 2-dimensional.");
            warning.exec();

            setTaskAborted();

            _settingsAction.setEnabled(true);
            return;
        }

        setTaskDescription("Extracting data");

        std::vector<hdps::Vector2f> data;
        inputDataset.extractDataForDimensions(data, 0, 1);

        setTaskProgress(0.1f);

        setTaskDescription("Clustering");

        _meanShift.setData(&data);

        std::vector<std::vector<unsigned int>> clusters;

        setTaskProgress(0.2f);

        _offscreenBuffer.bindContext();
        _meanShift.cluster(data, clusters);
        _offscreenBuffer.releaseContext();

        setTaskProgress(0.7f);

        // Remove existing clusters
        outputDataset.getClusters().clear();
        
        std::int32_t clusterIndex = 0;

        //const auto hueStep = 360.0f / clusters.size();

        // Create random number generator for pseudo random colors
        QRandomGenerator randomGenerator(500);

        // Add found clusters
        for (auto c : clusters)
        {
            Cluster cluster;

            cluster._name       = QString("cluster %1").arg(QString::number(clusterIndex + 1));
            cluster._indices    = c;

            const auto randomHue        = randomGenerator.bounded(360);
            const auto randomSaturation = randomGenerator.bounded(150, 255);
            const auto randomLightness  = randomGenerator.bounded(50, 200);

            cluster._color = QColor::fromHsl(randomHue, randomSaturation, randomLightness);

            outputDataset.addCluster(cluster);

            clusterIndex++;
        }

        // Inform observers that the clusters data changed
        _core->notifyDataChanged(getOutputDatasetName());

        setTaskFinished();

        _settingsAction.setComputationUpToDate(true);
        _settingsAction.setEnabled(true);
    });

    _offscreenBuffer.bindContext();
    _meanShift.init();
    _offscreenBuffer.releaseContext();
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
