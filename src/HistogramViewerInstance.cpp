// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#include "HistogramViewerInstance.h"
#include "HistogramViewerInterface.h"
#include "HistogramViewerParameters.h"

#include <pcl/ImageWindow.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramViewerInstance::HistogramViewerInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_logX( TheHVLogXParameter->DefaultValue() )
   , p_logY( TheHVLogYParameter->DefaultValue() )
   , p_zoom( TheHVZoomParameter->DefaultValue() )
   , p_pan( TheHVPanParameter->DefaultValue() )
   , p_stats( TheHVStatsParameter->DefaultValue() )
   , p_showRed( TheHVShowRedParameter->DefaultValue() )
   , p_showGreen( TheHVShowGreenParameter->DefaultValue() )
   , p_showBlue( TheHVShowBlueParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

HistogramViewerInstance::HistogramViewerInstance( const HistogramViewerInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void HistogramViewerInstance::Assign( const ProcessImplementation& p )
{
   const HistogramViewerInstance* x = dynamic_cast<const HistogramViewerInstance*>( &p );
   if ( x != nullptr )
   {
      p_logX      = x->p_logX;
      p_logY      = x->p_logY;
      p_zoom      = x->p_zoom;
      p_pan       = x->p_pan;
      p_stats     = x->p_stats;
      p_showRed   = x->p_showRed;
      p_showGreen = x->p_showGreen;
      p_showBlue  = x->p_showBlue;
   }
}

// ----------------------------------------------------------------------------

bool HistogramViewerInstance::IsHistoryUpdater( const View& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool HistogramViewerInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool HistogramViewerInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "HistogramViewer cannot be executed on complex images.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

bool HistogramViewerInstance::ExecuteOn( View& view )
{
   TheHistogramViewerInterface->LaunchFor( view, *this );
   return true;
}

// ----------------------------------------------------------------------------

bool HistogramViewerInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

bool HistogramViewerInstance::ExecuteGlobal()
{
   ImageWindow window = ImageWindow::ActiveWindow();
   TheHistogramViewerInterface->LaunchFor( window.IsNull() ? View::Null() : window.CurrentView(), *this );
   return true;
}

// ----------------------------------------------------------------------------

void* HistogramViewerInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheHVLogXParameter )
      return &p_logX;
   if ( p == TheHVLogYParameter )
      return &p_logY;
   if ( p == TheHVZoomParameter )
      return &p_zoom;
   if ( p == TheHVPanParameter )
      return &p_pan;
   if ( p == TheHVStatsParameter )
      return &p_stats;
   if ( p == TheHVShowRedParameter )
      return &p_showRed;
   if ( p == TheHVShowGreenParameter )
      return &p_showGreen;
   if ( p == TheHVShowBlueParameter )
      return &p_showBlue;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl
