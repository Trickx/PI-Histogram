// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// ----------------------------------------------------------------------------
// Port of the histogram view from Seti Astro Suite Pro (SASpro)
//   original: setiastro/setiastrosuitepro  src/setiastro/saspro/histogram.py
//   (c) Franklin Marek, GPL-3.0
// This port is likewise released under the GNU GPL v3.
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2026
#define MODULE_RELEASE_MONTH     9
#define MODULE_RELEASE_DAY       25

#include "HistogramViewerModule.h"
#include "HistogramViewerInterface.h"
#include "HistogramViewerProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramViewerModule::HistogramViewerModule()
{
}

// ----------------------------------------------------------------------------

const char* HistogramViewerModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString HistogramViewerModule::Name() const
{
   return "HistogramViewer";
}

// ----------------------------------------------------------------------------

String HistogramViewerModule::Description() const
{
   return "Non-destructive histogram and statistics viewer for PixInsight. "
          "Port of the histogram view from Seti Astro Suite Pro (SASpro).";
}

// ----------------------------------------------------------------------------

String HistogramViewerModule::Company() const
{
   return "Tricx";
}

// ----------------------------------------------------------------------------

String HistogramViewerModule::Author() const
{
   return "Tricx (PixInsight port), Franklin Marek (original SASpro implementation)";
}

// ----------------------------------------------------------------------------

String HistogramViewerModule::Copyright() const
{
   return "Copyright (c) 2026 Tricx, Copyright (c) Franklin Marek. GNU GPL v3.";
}

// ----------------------------------------------------------------------------

String HistogramViewerModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String HistogramViewerModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "HistogramViewer-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "HistogramViewer-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "HistogramViewer-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "HistogramViewer-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void HistogramViewerModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::HistogramViewerModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::HistogramViewerProcess;
      new pcl::HistogramViewerInterface;
   }

   return 0;
}
