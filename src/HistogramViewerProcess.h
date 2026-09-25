// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#ifndef __HistogramViewerProcess_h
#define __HistogramViewerProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class HistogramViewerProcess : public MetaProcess
{
public:

   HistogramViewerProcess();

   IsoString Id() const override;
   IsoString Categories() const override;
   uint32 Version() const override;
   String Description() const override;
   IsoString IconImageSVG() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool CanProcessViews() const override;
   bool CanProcessGlobal() const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern HistogramViewerProcess* TheHistogramViewerProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HistogramViewerProcess_h
