// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#include "HistogramViewerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

HVLogX*      TheHVLogXParameter = nullptr;
HVLogY*      TheHVLogYParameter = nullptr;
HVZoom*      TheHVZoomParameter = nullptr;
HVPan*       TheHVPanParameter = nullptr;
HVStats*     TheHVStatsParameter = nullptr;
HVShowRed*   TheHVShowRedParameter = nullptr;
HVShowGreen* TheHVShowGreenParameter = nullptr;
HVShowBlue*  TheHVShowBlueParameter = nullptr;

// ----------------------------------------------------------------------------

HVLogX::HVLogX( MetaProcess* P ) : MetaBoolean( P )
{
   TheHVLogXParameter = this;
}

IsoString HVLogX::Id() const
{
   return "logX";
}

bool HVLogX::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

HVLogY::HVLogY( MetaProcess* P ) : MetaBoolean( P )
{
   TheHVLogYParameter = this;
}

IsoString HVLogY::Id() const
{
   return "logY";
}

bool HVLogY::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

HVZoom::HVZoom( MetaProcess* P ) : MetaFloat( P )
{
   TheHVZoomParameter = this;
}

IsoString HVZoom::Id() const
{
   return "zoom";
}

int HVZoom::Precision() const
{
   return 2;
}

double HVZoom::MinimumValue() const
{
   return 1;
}

double HVZoom::MaximumValue() const
{
   return 3;
}

double HVZoom::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

HVPan::HVPan( MetaProcess* P ) : MetaInt32( P )
{
   TheHVPanParameter = this;
}

IsoString HVPan::Id() const
{
   return "pan";
}

double HVPan::MinimumValue() const
{
   return 0;
}

double HVPan::MaximumValue() const
{
   return 1000;
}

double HVPan::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

HVStats::HVStats( MetaProcess* P ) : MetaBoolean( P )
{
   TheHVStatsParameter = this;
}

IsoString HVStats::Id() const
{
   return "stats";
}

bool HVStats::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

HVShowRed::HVShowRed( MetaProcess* P ) : MetaBoolean( P )
{
   TheHVShowRedParameter = this;
}

IsoString HVShowRed::Id() const
{
   return "showRed";
}

bool HVShowRed::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

HVShowGreen::HVShowGreen( MetaProcess* P ) : MetaBoolean( P )
{
   TheHVShowGreenParameter = this;
}

IsoString HVShowGreen::Id() const
{
   return "showGreen";
}

bool HVShowGreen::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

HVShowBlue::HVShowBlue( MetaProcess* P ) : MetaBoolean( P )
{
   TheHVShowBlueParameter = this;
}

IsoString HVShowBlue::Id() const
{
   return "showBlue";
}

bool HVShowBlue::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl
