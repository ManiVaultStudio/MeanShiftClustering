#pragma once

#include "AnalysisPlugin.h"
#include "graphics/OffscreenBuffer.h"
#include "util/MeanShift.h"

#include "SettingsAction.h"

#include <QRandomGenerator>

using namespace mv::plugin;
using namespace mv::gui;

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
    OffscreenBuffer         _offscreenBuffer;       /** Off-screen buffer */
    mv::MeanShift           _meanShift;             /** Mean-shift analysis */
    SettingsAction          _settingsAction;        /** Settings action */
    QRandomGenerator        _rng;                   /** Random number generator for pseudo-random colors */
};

class MeanShiftClusteringPluginFactory : public AnalysisPluginFactory
{
    Q_INTERFACES(mv::plugin::AnalysisPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.MeanShiftClusteringPlugin"
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
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;
};
