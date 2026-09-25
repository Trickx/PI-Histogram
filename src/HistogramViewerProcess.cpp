// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#include "HistogramViewerIcon.h"
#include "HistogramViewerInstance.h"
#include "HistogramViewerInterface.h"
#include "HistogramViewerParameters.h"
#include "HistogramViewerProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramViewerProcess* TheHistogramViewerProcess = nullptr;

// ----------------------------------------------------------------------------

HistogramViewerProcess::HistogramViewerProcess()
{
   TheHistogramViewerProcess = this;

   new HVLogX( this );
   new HVLogY( this );
   new HVZoom( this );
   new HVPan( this );
   new HVStats( this );
   new HVShowRed( this );
   new HVShowGreen( this );
   new HVShowBlue( this );
}

// ----------------------------------------------------------------------------

IsoString HistogramViewerProcess::Id() const
{
   // Not "Histogram": that name is taken by the PJSR Histogram core object.
   return "HistogramViewer";
}

// ----------------------------------------------------------------------------

IsoString HistogramViewerProcess::Categories() const
{
   return "Tricx";
}

// ----------------------------------------------------------------------------

uint32 HistogramViewerProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String HistogramViewerProcess::Description() const
{
   return
   "<html>"
   "<p>Non-destructive histogram and statistics viewer for the selected image, with separate "
   "RGB curves and grayscale support.</p>"
   "<p>Provides linear or logarithmic axes, zoom and range controls, reproducible sampling "
   "for large images, percentiles and clipping statistics.</p>"
   "<p>Executing an instance on a view (or globally) opens the viewer for that view "
   "(or the active view) with the instance's display settings. The target image is never modified.</p>"
   "<p>Port of the histogram view from Seti Astro Suite Pro (SASpro) by Franklin Marek.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

IsoString HistogramViewerProcess::IconImageSVG() const
{
   return HistogramViewerIconSVG;
}

// ----------------------------------------------------------------------------

ProcessInterface* HistogramViewerProcess::DefaultInterface() const
{
   return TheHistogramViewerInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* HistogramViewerProcess::Create() const
{
   return new HistogramViewerInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* HistogramViewerProcess::Clone( const ProcessImplementation& p ) const
{
   const HistogramViewerInstance* instance = dynamic_cast<const HistogramViewerInstance*>( &p );
   return (instance != nullptr) ? new HistogramViewerInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------

bool HistogramViewerProcess::CanProcessViews() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool HistogramViewerProcess::CanProcessGlobal() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl
