// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#ifndef __HistogramViewerInterface_h
#define __HistogramViewerInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/Slider.h>
#include <pcl/View.h>
#include <pcl/ViewList.h>

#include "HistogramModel.h"
#include "HistogramViewerInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class Graphics;

class HistogramViewerInterface : public ProcessInterface
{
public:

   HistogramViewerInterface();
   virtual ~HistogramViewerInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   IsoString IconImageSVG() const override;
   InterfaceFeatures Features() const override;

   void ResetInstance() override;

   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;

   ProcessImplementation* NewProcess() const override;

   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

   void TrackViewUpdated( bool active ) override;

   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& ) override;
   void ImageFocused( const View& ) override;
   void ImageDeleted( const View& ) override;

   // Opens the interface on the specified view (or the current target if
   // null) with the display settings of the specified instance.
   void LaunchFor( const View&, const HistogramViewerInstance& );

private:

   static constexpr int NumberOfStatRows = 19;

   HistogramViewerInstance m_instance;
   View                    m_view;
   HistogramModel          m_model;
   bool                    m_modelDirty = false; // recompute when next shown

   struct GUIData
   {
      GUIData( HistogramViewerInterface& );

      HorizontalSizer   Global_Sizer;
         VerticalSizer     Histogram_Sizer;
            Control           Histogram_Control;
            HorizontalSizer   Target_Sizer;
               Label             Target_Label;
               ViewList          Target_ViewList;
            HorizontalSizer   Zoom_Sizer;
               Label             Zoom_Label;
               Slider            Zoom_Slider;
            HorizontalSizer   Range_Sizer;
               Label             Range_Label;
               Slider            Range_Slider;
            HorizontalSizer   Buttons_Sizer;
               PushButton        LogX_PushButton;
               PushButton        LogY_PushButton;
               PushButton        Stats_PushButton;
               CheckBox          Channel_CheckBox[ 3 ];
         Control           Stats_Control;
         VerticalSizer     Stats_Sizer;
            HorizontalSizer   StatRow_Sizer[ NumberOfStatRows + 1 ];
               Label             Stat_Label[ NumberOfStatRows + 1 ][ 4 ];

      bool StatsShown = false;
   };

   GUIData* GUI = nullptr;

   void SetTargetView( const View& );
   void RefreshModel();
   void ComputeModel();
   void UpdateStatsVisibility();
   void UpdateControls();
   void UpdateToggleButtons();
   void UpdateStatistics();
   void PaintHistogram( Graphics& );

   void e_Paint( Control& sender, const pcl::Rect& updateRect );
   void e_Show( Control& sender );
   void e_Click( Button& sender, bool checked );
   void e_ValueUpdated( Slider& sender, int value );
   void e_ViewSelected( ViewList& sender, View& view );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern HistogramViewerInterface* TheHistogramViewerInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HistogramViewerInterface_h
