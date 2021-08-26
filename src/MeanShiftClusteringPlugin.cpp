#include "MeanShiftClusteringPlugin.h"

#include "PointData.h"
#include "ClusterData.h"

#include <QDebug>
#include <QtCore>
#include <QMessageBox>
#include <QHBoxLayout>

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

    /*
    connect(&_meanShift, &MeanShift::progressSection, this, [this](const QString& section) {
        setTaskDescription(section);
    });

    connect(&_meanShift, &MeanShift::progressPercentage, this, [this](const float& percentage) {
        setTaskProgress(percentage);
    });
    */

    const auto updateColors = [this, &outputDataset]() -> void {
        const auto colorBy = static_cast<SettingsAction::ColorBy>(_settingsAction.getColorByAction().getCurrentIndex());

        switch (colorBy) {
            case SettingsAction::ColorBy::PsuedoRandomColors:
            {
                _rng.seed(_settingsAction.getRandomSeedAction().getValue());

                for (auto& cluster : outputDataset.getClusters()) {
                    const auto randomHue        = _rng.bounded(360);
                    const auto randomSaturation = _rng.bounded(150, 255);
                    const auto randomLightness  = _rng.bounded(50, 200);

                    cluster._color = QColor::fromHsl(randomHue, randomSaturation, randomLightness);
                }

                break;
            }

            case SettingsAction::ColorBy::ColorMap:
            {
                break;
            }
        }
    };

    connect(&_settingsAction.getComputeAction(), &TriggerAction::triggered, this, [this, &inputDataset, &outputDataset, updateColors]() {

        // Update the sigma value
        _meanShift.setSigma(_settingsAction.getSigmaAction().getValue());

        // Disable the settings when computing
        _settingsAction.setEnabled(false);

        setTaskRunning();
        setTaskName("Mean-shift clustering");
        setTaskProgress(0.0f);
        setTaskDescription("Initializing");

        QCoreApplication::processEvents();

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

        std::vector<hdps::Vector2f> data;
        inputDataset.extractDataForDimensions(data, 0, 1);

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

            cluster._name       = QString("cluster %1").arg(QString::number(clusterIndex + 1));
            cluster._indices    = c;

            outputDataset.addCluster(cluster);

            clusterIndex++;
        }

        updateColors();

        _core->notifyDataChanged(getOutputDatasetName());

        setTaskFinished();

        _settingsAction.setEnabled(true);
    });

    connect(&_settingsAction.getSigmaAction(), &DecimalAction::valueChanged, this, [this](const double& value) {
        _settingsAction.getComputeAction().trigger();
    });

    connect(&_settingsAction.getColorByAction(), &OptionAction::currentIndexChanged, this, [this, updateColors](const std::int32_t& currentIndex) {
        updateColors();

        _core->notifyDataChanged(getOutputDatasetName());
    });

    connect(&_settingsAction.getRandomSeedAction(), &IntegralAction::valueChanged, this, [this, updateColors](const std::int32_t& value) {
        updateColors();

        _core->notifyDataChanged(getOutputDatasetName());
    });

    _offscreenBuffer.bindContext();
    _meanShift.init();
    _offscreenBuffer.releaseContext();

    // Do an initial computation
    _settingsAction.getComputeAction().trigger();
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
