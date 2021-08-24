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
    _settingsAction(this)
{
}

MeanShiftClusteringPlugin::~MeanShiftClusteringPlugin(void)
{
}

void MeanShiftClusteringPlugin::init()
{
    // Create clusters output dataset
    setOutputDatasetName(_core->addData("Cluster", getInputDatasetName()));

    // Get input and output datasets
    auto& inputDataset  = getInputDataset<Points>();
    auto& outputDataset = getOutputDataset<Clusters>();

    outputDataset.addAction(_settingsAction);

    connect(&_settingsAction.getComputeAction(), &TriggerAction::triggered, this, [this, inputDataset]() {
        _settingsAction.setEnabled(false);

        setTaskProgress(0.0f);
        setTaskDescription("Initializing");

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

        QString clusterSetName = _core->addData("Cluster", "ClusterSet");

        //Clusters& clusterSet = _core->requestData<Clusters>(_outputDatsetName);

        //for (auto c : clusters)
        //{
            //Cluster cluster;
            //cluster.indices = c;

            //clusterSet.addCluster(cluster);
        //}

        setTaskFinished();

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
