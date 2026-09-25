// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// ----------------------------------------------------------------------------
// Port of the histogram view from Seti Astro Suite Pro (SASpro)
//   original: setiastro/setiastrosuitepro  src/setiastro/saspro/histogram.py
//   (c) Franklin Marek, GPL-3.0
// This port is likewise released under the GNU GPL v3.
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#ifndef __HistogramViewerModule_h
#define __HistogramViewerModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class HistogramViewerModule : public MetaModule
{
public:

   HistogramViewerModule();

   const char* Version() const override;
   IsoString Name() const override;
   String Description() const override;
   String Company() const override;
   String Author() const override;
   String Copyright() const override;
   String TradeMarks() const override;
   String OriginalFileName() const override;
   void GetReleaseDate( int& year, int& month, int& day ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HistogramViewerModule_h
