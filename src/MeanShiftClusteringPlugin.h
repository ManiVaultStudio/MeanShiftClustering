#pragma once

#include "AnalysisPlugin.h"
#include "SettingsAction.h"

#include "util/MeanShift.h"
#include "graphics/OffscreenBuffer.h"
#include "actions/Actions.h"

#include <actions/TriggerAction.h>

#include <QRandomGenerator>

using namespace hdps::plugin;
using namespace hdps::gui;

class PointsPlugin;

class MeanShiftClusteringPlugin : public AnalysisPlugin
{
    Q_OBJECT

public:
    MeanShiftClusteringPlugin(const PluginFactory* factory);

    ~MeanShiftClusteringPlugin() override = default;
    
    void init() override;

    /** Establishes whether a computation can be executed */
    bool canCompute() const;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:
    OffscreenBuffer     _offscreenBuffer;       /** Off-screen buffer */
    hdps::MeanShift     _meanShift;             /** Mean-shift analysis */
    SettingsAction      _settingsAction;        /** Settings action */
    QRandomGenerator    _rng;                   /** Random number generator for pseudo-random colors */
};

class MeanShiftClusteringPluginFactory : public AnalysisPluginFactory
{
    Q_INTERFACES(hdps::plugin::AnalysisPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.BioVault.MeanShiftClusteringPlugin"
                      FILE  "MeanShiftClusteringPlugin.json")
    
public:
    MeanShiftClusteringPluginFactory(void) {}
    ~MeanShiftClusteringPluginFactory(void) override {}

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    AnalysisPlugin* produce() override;

    /**
     * Get a list of plugin trigger actions given a vector of input datasets
     * @param datasets Vector of input datasets
     * @return List of plugin trigger actions
     */
    QList<PluginTriggerAction*> getPluginTriggerActions(const hdps::Datasets& datasets) const override;
};
