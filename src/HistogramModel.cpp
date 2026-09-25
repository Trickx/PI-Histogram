// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#include "HistogramModel.h"

#include <pcl/Image.h>

#include <algorithm>
#include <cmath>

namespace pcl
{

// ----------------------------------------------------------------------------

static double Percentile( const std::vector<double>& values, double fraction )
{
   if ( values.empty() )
      return 0;

   double index = (values.size() - 1) * fraction;
   size_type lower = size_type( std::floor( index ) );
   size_type upper = size_type( std::ceil( index ) );
   if ( lower == upper )
      return values[lower];
   return values[lower] + (values[upper] - values[lower]) * (index - lower);
}

// ----------------------------------------------------------------------------

static ChannelStatistics ComputeStatistics( std::vector<double>& values )
{
   ChannelStatistics s;
   if ( values.empty() )
      return s;

   std::sort( values.begin(), values.end() );

   double sum = 0;
   for ( double v : values )
      sum += v;
   double mean = sum/values.size();

   double variance = 0;
   for ( double v : values )
      variance += (v - mean)*(v - mean);

   double median = Percentile( values, 0.5 );

   std::vector<double> deviations;
   deviations.reserve( values.size() );
   for ( double v : values )
   {
      deviations.push_back( std::abs( v - median ) );
      if ( v <= 0 )
         ++s.low;
      if ( v >= 1 - 1.0e-6 )
         ++s.high;
   }
   std::sort( deviations.begin(), deviations.end() );

   s.min      = values.front();
   s.max      = values.back();
   s.mean     = mean;
   s.median   = median;
   s.variance = variance/values.size();
   s.stddev   = std::sqrt( s.variance );
   s.mad      = Percentile( deviations, 0.5 );
   s.p01      = Percentile( values, 0.001 );
   s.p1       = Percentile( values, 0.01 );
   s.p5       = Percentile( values, 0.05 );
   s.p25      = Percentile( values, 0.25 );
   s.p75      = Percentile( values, 0.75 );
   s.p95      = Percentile( values, 0.95 );
   s.p99      = Percentile( values, 0.99 );
   s.p999     = Percentile( values, 0.999 );
   s.iqr      = s.p75 - s.p25;
   return s;
}

// ----------------------------------------------------------------------------

template <class P>
static void CollectSamples( const GenericImage<P>& image, int channels, std::vector<std::vector<double>>& samples )
{
   size_type total = size_type( image.Width() ) * size_type( image.Height() );
   // Use a regular image grid instead of visiting every pixel.
   int step = std::max( 1, int( std::ceil( std::sqrt( double( total )/HistogramModel::MaxStatSamples ) ) ) );

   for ( int c = 0; c < channels; ++c )
      samples[c].reserve( size_type( (image.Width() + step - 1)/step ) * size_type( (image.Height() + step - 1)/step ) );

   for ( int y = 0; y < image.Height(); y += step )
      for ( int c = 0; c < channels; ++c )
      {
         const typename P::sample* row = image.ScanLine( y, c );
         for ( int x = 0; x < image.Width(); x += step )
         {
            double value;
            P::FromSample( value, row[x] );
            samples[c].push_back( std::max( 0.0, std::min( 1.0, value ) ) );
         }
      }
}

// ----------------------------------------------------------------------------

HistogramModel::HistogramModel( const ImageVariant& image )
{
   if ( !image || image.IsComplexSample() )
      return;

   color = image.IsColor();
   channels = color ? 3 : 1;

   std::vector<std::vector<double>> samples( channels );
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: CollectSamples( static_cast<const Image&>( *image ), channels, samples ); break;
      case 64: CollectSamples( static_cast<const DImage&>( *image ), channels, samples ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: CollectSamples( static_cast<const UInt8Image&>( *image ), channels, samples ); break;
      case 16: CollectSamples( static_cast<const UInt16Image&>( *image ), channels, samples ); break;
      case 32: CollectSamples( static_cast<const UInt32Image&>( *image ), channels, samples ); break;
      }

   linear.assign( channels, histogram( Bins, 0.0 ) );
   logarithmic.assign( channels, histogram( Bins, 0.0 ) );
   statistics.resize( channels );

   double positiveMinimum = 1;
   for ( int c = 0; c < channels; ++c )
      for ( double value : samples[c] )
      {
         ++linear[c][std::min( Bins - 1, int( value * Bins ) )];
         if ( value > 0 )
            positiveMinimum = std::min( positiveMinimum, value );
      }

   sampleCount = samples[0].size();
   logMinimumExponent = std::log10( std::max( 1.0e-6, positiveMinimum ) );

   for ( int c = 0; c < channels; ++c )
   {
      for ( double value : samples[c] )
      {
         if ( value <= 0 )
            continue;
         // All positive samples are 1 when logMinimumExponent is 0. The script
         // divides by zero there and loses them; count them in the last bin.
         double logFraction = (logMinimumExponent < 0) ?
               (std::log10( value ) - logMinimumExponent) / -logMinimumExponent : 1.0;
         ++logarithmic[c][std::min( Bins - 1, std::max( 0, int( std::floor( logFraction * Bins ) ) ) )];
      }
      // Sorts the samples in place; they are not needed afterwards.
      statistics[c] = ComputeStatistics( samples[c] );
   }
}

// ----------------------------------------------------------------------------

} // pcl
