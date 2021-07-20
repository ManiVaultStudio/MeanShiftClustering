#include "MeanShiftClusteringPlugin.h"

#include "PointData.h"
#include "ClusterData.h"

#include <QtCore>
#include <QMessageBox>
#include <QtDebug>


#include <assert.h>
#include <algorithm>
#include <functional>
#include <random>
#include <vector>

#include <unordered_map>

#define NO_FILE 0

Q_PLUGIN_METADATA(IID "nl.tudelft.MeanShift")

using namespace hdps;

// =============================================================================
// View
// =============================================================================

MeanShiftAnalysisPlugin::~MeanShiftAnalysisPlugin(void)
{
    
}

void MeanShiftAnalysisPlugin::init()
{
    connect(&_settings, SIGNAL(startClustering()), this, SLOT(startComputation()));

    offscreen.bindContext();
    _meanShift.init();
    offscreen.releaseContext();

    registerDataEventByType(PointType, std::bind(&MeanShiftAnalysisPlugin::onDataEvent, this, std::placeholders::_1));
}

void MeanShiftAnalysisPlugin::onDataEvent(DataEvent* dataEvent)
{
    if (dataEvent->getType() == EventType::DataAdded)
    {
        _settings.addDataOption(dataEvent->dataSetName);
    }
}

SettingsWidget* const MeanShiftAnalysisPlugin::getSettings()
{
    return &_settings;
}

void MeanShiftAnalysisPlugin::dataSetPicked(const QString& name)
{

}

void MeanShiftAnalysisPlugin::startComputation()
{
    QString setName = _settings.getCurrentDataOption();

    // Do nothing if we have no data set selected
    if (setName.isEmpty()) {
        QMessageBox warning;
        warning.setText("No data is selected for clustering.");
        warning.exec();
        return;
    }

    Points& inputData = _core->requestData<Points>(setName);

    if (inputData.getNumDimensions() != 2)
    {
        QMessageBox warning;
        warning.setText("Selected data must be 2-dimensional.");
        warning.exec();
        return;
    }

    std::vector<hdps::Vector2f> data;
    inputData.extractDataForDimensions(data, 0, 1);

    _meanShift.setData(&data);

    std::vector<std::vector<unsigned int>> clusters;

    offscreen.bindContext();
    _meanShift.cluster(data, clusters);
    offscreen.releaseContext();

    QString clusterSetName = _core->addData("Cluster", "ClusterSet");
    Clusters& clusterSet = _core->requestData<Clusters>(clusterSetName);

    for (auto c : clusters)
    {
        Cluster cluster;
        cluster.indices = c;

        clusterSet.addCluster(cluster);
    }

    _core->notifyDataAdded(clusterSetName);
}

// =============================================================================
// Factory
// =============================================================================

AnalysisPlugin* MeanShiftAnalysisPluginFactory::produce()
{
    return new MeanShiftAnalysisPlugin();
}
