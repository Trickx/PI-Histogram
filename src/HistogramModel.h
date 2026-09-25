// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#ifndef __HistogramModel_h
#define __HistogramModel_h

#include <pcl/ImageVariant.h>

#include <vector>

namespace pcl
{

// ----------------------------------------------------------------------------

struct ChannelStatistics
{
   double min = 0, max = 0, mean = 0, median = 0, stddev = 0, variance = 0, mad = 0, iqr = 0;
   double p01 = 0, p1 = 0, p5 = 0, p25 = 0, p75 = 0, p95 = 0, p99 = 0, p999 = 0;
   size_type low = 0;   // samples <= 0
   size_type high = 0;  // samples >= 1 - 1e-6
};

// ----------------------------------------------------------------------------

/*
 * Histograms and statistics of an image, computed on a regular sampling grid
 * of at most MaxStatSamples pixels so that results are reproducible and fast
 * for large images.
 */
class HistogramModel
{
public:

   static constexpr int Bins = 512;
   static constexpr size_type MaxStatSamples = 300000;

   using histogram = std::vector<double>;

   bool                           color = false;
   int                            channels = 0;
   std::vector<histogram>         linear;
   std::vector<histogram>         logarithmic;
   std::vector<ChannelStatistics> statistics;
   size_type                      sampleCount = 0;
   double                         logMinimumExponent = -6;

   HistogramModel() = default;
   explicit HistogramModel( const ImageVariant& );

   bool IsEmpty() const
   {
      return channels == 0;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HistogramModel_h
