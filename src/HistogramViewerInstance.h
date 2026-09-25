// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#ifndef __HistogramViewerInstance_h
#define __HistogramViewerInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * The instance only carries display settings. Executing it never modifies the
 * target: it opens the viewer interface on the target view instead.
 */
class HistogramViewerInstance : public ProcessImplementation
{
public:

   HistogramViewerInstance( const MetaProcess* );
   HistogramViewerInstance( const HistogramViewerInstance& );

   void Assign( const ProcessImplementation& ) override;

   bool IsHistoryUpdater( const View& ) const override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;

   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;

   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

private:

   pcl_bool p_logX;
   pcl_bool p_logY;
   float    p_zoom;
   int32    p_pan;
   pcl_bool p_stats;
   pcl_bool p_showRed;
   pcl_bool p_showGreen;
   pcl_bool p_showBlue;

   friend class HistogramViewerInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HistogramViewerInstance_h
