// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#ifndef __HistogramViewerParameters_h
#define __HistogramViewerParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------
// Parameter identifiers are the same as those used by the Histogram.js script.
// ----------------------------------------------------------------------------

class HVLogX : public MetaBoolean
{
public:

   HVLogX( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern HVLogX* TheHVLogXParameter;

// ----------------------------------------------------------------------------

class HVLogY : public MetaBoolean
{
public:

   HVLogY( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern HVLogY* TheHVLogYParameter;

// ----------------------------------------------------------------------------

class HVZoom : public MetaFloat
{
public:

   HVZoom( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern HVZoom* TheHVZoomParameter;

// ----------------------------------------------------------------------------

class HVPan : public MetaInt32
{
public:

   HVPan( MetaProcess* );

   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern HVPan* TheHVPanParameter;

// ----------------------------------------------------------------------------

class HVStats : public MetaBoolean
{
public:

   HVStats( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern HVStats* TheHVStatsParameter;

// ----------------------------------------------------------------------------

class HVShowRed : public MetaBoolean
{
public:

   HVShowRed( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern HVShowRed* TheHVShowRedParameter;

// ----------------------------------------------------------------------------

class HVShowGreen : public MetaBoolean
{
public:

   HVShowGreen( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern HVShowGreen* TheHVShowGreenParameter;

// ----------------------------------------------------------------------------

class HVShowBlue : public MetaBoolean
{
public:

   HVShowBlue( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern HVShowBlue* TheHVShowBlueParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __HistogramViewerParameters_h
