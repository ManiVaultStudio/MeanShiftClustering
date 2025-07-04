#include "MeanShiftClusteringPlugin.h"

#include <PointData/PointData.h>
#include <ClusterData/ClusterData.h>

#include <util/Serialization.h>

#include <actions/PluginTriggerAction.h>

#include <QDebug>
#include <QtCore>
#include <QMessageBox>

#include <vector>

#include <util/Icon.h>

Q_PLUGIN_METADATA(IID "studio.manivault.MeanShift")

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

MeanShiftClusteringPlugin::MeanShiftClusteringPlugin(const PluginFactory* factory) :
    AnalysisPlugin(factory),
    _offscreenBuffer(),
    _meanShift(),
    _settingsAction(this),
    _rng(0)
{
    setObjectName("Mean-shift");
}

void MeanShiftClusteringPlugin::init()
{
    // Create clusters output dataset
    if (!outputDataInit()) {
        setOutputDataset(mv::data().createDataset("Cluster", "Clusters (mean-shift)", getInputDataset()));
    }

    // Get input and output datasets
    auto inputDataset  = getInputDataset<Points>();
    auto outputDataset = getOutputDataset<Clusters>();

    // Inject the setting action into the output dataset
    outputDataset->addAction(_settingsAction);

    // Select the dataset
    outputDataset->getDataHierarchyItem().select();

    // Get the number of dimensions in the input dataset
    const auto numberOfDimensions = inputDataset->getNumDimensions();

    QStringList dimensionNames;
    
    if (numberOfDimensions == inputDataset->getDimensionNames().size()) {
        for (const auto& dimensionName : inputDataset->getDimensionNames())
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
//        _settingsAction.getDimensionOneAction().setDefaultIndex(0);
        _settingsAction.getDimensionTwoAction().setCurrentIndex(0);
//        _settingsAction.getDimensionTwoAction().setDefaultIndex(0);

        QMessageBox warning;

        warning.setText("Selected data must have at least two dimensions");
        warning.exec();

        _settingsAction.setEnabled(false);
    }

    // Handle dataset with two or more dimensions
    if (numberOfDimensions >= 2) {
        _settingsAction.getDimensionOneAction().setCurrentIndex(0);
//        _settingsAction.getDimensionOneAction().setDefaultIndex(0);
        _settingsAction.getDimensionTwoAction().setCurrentIndex(1);
//        _settingsAction.getDimensionTwoAction().setDefaultIndex(1);
    }

    // Updates the cluster colors depending on the color configuration
    const auto updateColors = [this]() -> void {
        const auto colorBy = static_cast<SettingsAction::ColorBy>(_settingsAction.getColorByAction().getCurrentIndex());

        switch (colorBy) {
            case SettingsAction::ColorBy::PseudoRandomColors:
                Cluster::colorizeClusters(getOutputDataset<Clusters>()->getClusters(), _settingsAction.getRandomSeedAction().getValue());
                break;

            case SettingsAction::ColorBy::ColorMap:
                Cluster::colorizeClusters(getOutputDataset<Clusters>()->getClusters(), _settingsAction.getColorMapAction().getColorMapImage());
                break;
        }
    };

    auto& task = getOutputDataset()->getTask();

    connect(&_settingsAction.getComputeAction(), &TriggerAction::triggered, this, [this, updateColors, &task]() {

        // Do compute if the settings action is disabled (for instance due to serialization)
        if (!_settingsAction.isEnabled())
            return;

        // Do not run if the selected dimensions are the same
        if (_settingsAction.getDimensionOneAction().getCurrentIndex() == _settingsAction.getDimensionTwoAction().getCurrentIndex()) {
            task.setProgressDescription("Input error: identical dimensions");
            return;
        }

        // Update the sigma value
        _meanShift.setSigma(_settingsAction.getSigmaAction().getValue());

        // Disable the settings when computing
        _settingsAction.setEnabled(false);

        task.reset();
        task.setRunning();
        task.setName("Mean-shift on " + getInputDataset()->getLocation());
        task.setProgressDescription("Initializing");

        QCoreApplication::processEvents();

        std::vector<mv::Vector2f> data;

        getInputDataset<Points>()->extractDataForDimensions(data, _settingsAction.getDimensionOneAction().getCurrentIndex(), _settingsAction.getDimensionTwoAction().getCurrentIndex());

        task.setProgressDescription("Clustering");

        QCoreApplication::processEvents();

        auto [xMinIt, xMaxIt] = std::minmax_element(data.begin(), data.end(), [](const mv::Vector2f& a, const mv::Vector2f& b) {
	        return a.x < b.x;
        });

        auto [yMinIt, yMaxIt] = std::minmax_element(data.begin(), data.end(), [](const mv::Vector2f& a, const mv::Vector2f& b) {
            return a.y < b.y;
        });

        const auto xRange = xMaxIt->x - xMinIt->x;
        const auto yRange = yMaxIt->y - yMinIt->y;

        for (auto& v : data) {
            v.x = ((v.x - xMinIt->x) / xRange);
            v.y = ((v.y - yMinIt->y) / yRange);
        }

        _meanShift.setData(&data);

        std::vector<std::vector<unsigned int>> clusters;

        task.setProgress(0.2f);

        QCoreApplication::processEvents();

        _offscreenBuffer.bindContext();
        _meanShift.cluster(data, clusters);
        _offscreenBuffer.releaseContext();

        task.setProgress(0.7f);

        QCoreApplication::processEvents();

        // Remove existing clusters
        auto outDataset = getOutputDataset<Clusters>();
        outDataset->getClusters().clear();
        
        std::int32_t clusterIndex = 0;

        std::vector<std::uint32_t> globalIndices;

        getInputDataset<Points>()->getSourceDataset<Points>()->getGlobalIndices(globalIndices);

        // Add found clusters
        for (auto& clusterIndicesLocal : clusters)
        {
            Cluster cluster;

            cluster.setName(QString("cluster %1").arg(QString::number(clusterIndex + 1)));

            std::vector<std::uint32_t> clusterIndicesGlobal;

            clusterIndicesGlobal.reserve(clusterIndicesLocal.size());

            for (auto clusterIndexLocal : clusterIndicesLocal)
                clusterIndicesGlobal.push_back(globalIndices[clusterIndexLocal]);

            cluster.setIndices(clusterIndicesGlobal);

            outDataset->addCluster(cluster);

            clusterIndex++;
        }

        updateColors();

        events().notifyDatasetDataChanged(outDataset);

        task.setFinished();

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

        events().notifyDatasetDataChanged(getOutputDataset());
    });

    connect(&_settingsAction.getColorMapAction(), &ColorMapAction::imageChanged, this, [this, updateColors](const QImage& image) {
        if (_settingsAction.getUpdateColorsManuallyAction().isChecked())
            return;

        updateColors();

        events().notifyDatasetDataChanged(getOutputDataset());
    });

    connect(&_settingsAction.getRandomSeedAction(), &IntegralAction::valueChanged, this, [this, updateColors](const std::int32_t& value) {
        if (_settingsAction.getUpdateColorsManuallyAction().isChecked())
            return;

        updateColors();

        events().notifyDatasetDataChanged(getOutputDataset());
    });

    connect(&_settingsAction.getApplyColorsAction(), &TriggerAction::triggered, this, [this, updateColors](const std::int32_t& value) {
        updateColors();

        events().notifyDatasetDataChanged(getOutputDataset());
    });

    _offscreenBuffer.bindContext();
    _meanShift.init();
    _offscreenBuffer.releaseContext();

    if (!mv::projects().isOpeningProject() && !mv::projects().isImportingProject())
		_settingsAction.getComputeAction().trigger();
}

bool MeanShiftClusteringPlugin::canCompute() const
{
    if (!const_cast<MeanShiftClusteringPlugin*>(this)->getInputDataset<Points>().isValid())
        return false;

    return true;
}

void MeanShiftClusteringPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    AnalysisPlugin::fromVariantMap(variantMap);

    _settingsAction.fromParentVariantMap(variantMap);
}

QVariantMap MeanShiftClusteringPlugin::toVariantMap() const
{
    auto variantMap = AnalysisPlugin::toVariantMap();

    _settingsAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

MeanShiftClusteringPluginFactory::MeanShiftClusteringPluginFactory()
{
    setIcon(StyledIcon(createPluginIcon("MS")));
}

AnalysisPlugin* MeanShiftClusteringPluginFactory::produce()
{
    return new MeanShiftClusteringPlugin(this);
}

PluginTriggerActions MeanShiftClusteringPluginFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getInstance = [this](const Dataset<Points>& dataset) -> MeanShiftClusteringPlugin* {
        return dynamic_cast<MeanShiftClusteringPlugin*>(plugins().requestPlugin(getKind(), Datasets({ dataset })));
    };

    if (!datasets.isEmpty() && PluginFactory::areAllDatasetsOfTheSameType(datasets, PointType)) {
        auto pluginTriggerAction = new PluginTriggerAction(const_cast<MeanShiftClusteringPluginFactory*>(this), this, "Mean-shift analysis", "Apply mean-shift analysis on each selected dataset(s)", icon(), [this, getInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
            for (const auto& dataset : datasets)
                getInstance(dataset);
        });

        pluginTriggerActions << pluginTriggerAction;
    }

    return pluginTriggerActions;
}
