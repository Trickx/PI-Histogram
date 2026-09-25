// ============================================================================
// HistogramViewer - PixInsight module (PCL / C++)
// ----------------------------------------------------------------------------
// Port of the histogram view from Seti Astro Suite Pro (SASpro)
//   original: setiastro/setiastrosuitepro  src/setiastro/saspro/histogram.py
//   (c) Franklin Marek, GPL-3.0
// This port is likewise released under the GNU GPL v3.
// SPDX-License-Identifier: GPL-3.0-only
// ============================================================================

#include "HistogramViewerIcon.h"
#include "HistogramViewerInterface.h"
#include "HistogramViewerParameters.h"
#include "HistogramViewerProcess.h"

#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramViewerInterface* TheHistogramViewerInterface = nullptr;

// ----------------------------------------------------------------------------

static const char* StatNames[] =
{
   "Min", "Max", "Mean", "Median", "StdDev", "Variance", "MAD", "IQR (p75-p25)",
   "p0.1", "p1", "p5", "p25", "p50", "p75", "p95", "p99", "p99.9",
   "Low Clipped (<=0)", "High Clipped (>=TrueMax)"
};

static double StatValue( const ChannelStatistics& s, int row )
{
   switch ( row )
   {
   case  0: return s.min;
   case  1: return s.max;
   case  2: return s.mean;
   case  3: return s.median;
   case  4: return s.stddev;
   case  5: return s.variance;
   case  6: return s.mad;
   case  7: return s.iqr;
   case  8: return s.p01;
   case  9: return s.p1;
   case 10: return s.p5;
   case 11: return s.p25;
   case 12: return s.median;
   case 13: return s.p75;
   case 14: return s.p95;
   case 15: return s.p99;
   case 16: return s.p999;
   default: return 0;
   }
}

static const int LowClippedRow = 17;
static const int HighClippedRow = 18;

static const char* ToggleOnStyle = "background: #555a60; color: #f0f0f0;";

// Same format as JavaScript's Number.prototype.toExponential( 1 ), e.g. 1.0e-6
static String ToExponential1( double value )
{
   char buffer[ 32 ];
   std::snprintf( buffer, sizeof( buffer ), "%.1e", value );
   IsoString s( buffer );
   size_type e = s.Find( 'e' );
   if ( e == IsoString::notFound )
      return String( s );
   IsoString mantissa = s.Left( e );
   int exponent = std::atoi( s.c_str() + e + 1 );
   return String( mantissa ) + 'e' + ((exponent < 0) ? "-" : "+") + String( std::abs( exponent ) );
}

// ----------------------------------------------------------------------------

HistogramViewerInterface::HistogramViewerInterface()
   : m_instance( TheHistogramViewerProcess )
{
   TheHistogramViewerInterface = this;
}

// ----------------------------------------------------------------------------

HistogramViewerInterface::~HistogramViewerInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString HistogramViewerInterface::Id() const
{
   return "HistogramViewer";
}

// ----------------------------------------------------------------------------

MetaProcess* HistogramViewerInterface::Process() const
{
   return TheHistogramViewerProcess;
}

// ----------------------------------------------------------------------------

IsoString HistogramViewerInterface::IconImageSVG() const
{
   return HistogramViewerIconSVG;
}

// ----------------------------------------------------------------------------

InterfaceFeatures HistogramViewerInterface::Features() const
{
   return InterfaceFeature::DragObject | InterfaceFeature::TrackViewButton | InterfaceFeature::ResetButton;
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::ResetInstance()
{
   HistogramViewerInstance defaultInstance( TheHistogramViewerProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool HistogramViewerInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Histogram" );
      UpdateControls();
   }

   if ( m_view.IsNull() )
   {
      ImageWindow window = ImageWindow::ActiveWindow();
      if ( !window.IsNull() )
         SetTargetView( window.CurrentView() );
   }

   dynamic = false;
   return &P == TheHistogramViewerProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* HistogramViewerInterface::NewProcess() const
{
   return new HistogramViewerInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool HistogramViewerInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const HistogramViewerInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a HistogramViewer instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool HistogramViewerInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool HistogramViewerInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr && active )
   {
      ImageWindow window = ImageWindow::ActiveWindow();
      if ( !window.IsNull() )
         SetTargetView( window.CurrentView() );
   }
}

// ----------------------------------------------------------------------------

bool HistogramViewerInterface::WantsImageNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::ImageUpdated( const View& view )
{
   if ( GUI != nullptr && view == m_view )
      RefreshModel();
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::ImageFocused( const View& view )
{
   if ( GUI != nullptr && IsTrackViewActive() && !view.IsNull() && view != m_view )
      SetTargetView( view );
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::ImageDeleted( const View& view )
{
   if ( GUI != nullptr && view == m_view )
      SetTargetView( View::Null() );
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::LaunchFor( const View& view, const HistogramViewerInstance& instance )
{
   m_instance.Assign( instance );
   if ( !view.IsNull() )
      m_view = view;
   ProcessInterface::Launch();
   UpdateControls();
   RefreshModel();
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::SetTargetView( const View& view )
{
   m_view = view;
   if ( GUI != nullptr && !m_view.IsNull() )
      GUI->Target_ViewList.SelectView( m_view );
   RefreshModel();
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::RefreshModel()
{
   if ( GUI == nullptr )
      return;

   // Avoid computing statistics for every focus change or image update while
   // the interface is closed; e_Show() and e_Paint() pick up pending work.
   if ( IsVisible() )
      ComputeModel();
   else
      m_modelDirty = true;
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::ComputeModel()
{
   m_modelDirty = false;
   m_model = m_view.IsNull() ? HistogramModel() : HistogramModel( m_view.Image() );

   for ( int c = 0; c < 3; ++c )
      GUI->Channel_CheckBox[c].Enable( m_model.color );

   UpdateStatistics();
   GUI->Histogram_Control.Update();
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::UpdateControls()
{
   if ( GUI == nullptr )
      return;

   GUI->Zoom_Slider.SetValue( RoundInt( m_instance.p_zoom * 100 ) );
   GUI->Range_Slider.SetValue( m_instance.p_pan );
   GUI->Range_Slider.Enable( m_instance.p_zoom > 1.001 );

   GUI->Channel_CheckBox[0].SetChecked( m_instance.p_showRed );
   GUI->Channel_CheckBox[1].SetChecked( m_instance.p_showGreen );
   GUI->Channel_CheckBox[2].SetChecked( m_instance.p_showBlue );

   UpdateStatsVisibility();

   if ( !m_view.IsNull() )
      GUI->Target_ViewList.SelectView( m_view );

   UpdateToggleButtons();
   GUI->Histogram_Control.Update();
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::UpdateStatsVisibility()
{
   if ( GUI->StatsShown != bool( m_instance.p_stats ) )
   {
      GUI->StatsShown = m_instance.p_stats;
      GUI->Stats_Control.SetVisible( GUI->StatsShown );
      AdjustToContents();
   }
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::UpdateToggleButtons()
{
   GUI->LogX_PushButton.SetStyleSheet( m_instance.p_logX ? ToggleOnStyle : "" );
   GUI->LogY_PushButton.SetStyleSheet( m_instance.p_logY ? ToggleOnStyle : "" );
   GUI->Stats_PushButton.SetStyleSheet( m_instance.p_stats ? ToggleOnStyle : "" );
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::UpdateStatistics()
{
   const char* grayHeader[] = { "Gray", "", "" };
   const char* colorHeader[] = { "R", "G", "B" };
   for ( int c = 0; c < 3; ++c )
      GUI->Stat_Label[0][c+1].SetText( m_model.color || m_model.IsEmpty() ? colorHeader[c] : grayHeader[c] );

   for ( int row = 0; row < NumberOfStatRows; ++row )
      for ( int c = 0; c < 3; ++c )
      {
         Label& cell = GUI->Stat_Label[row+1][c+1];
         if ( m_model.IsEmpty() )
            cell.SetText( "-" );
         else if ( c >= m_model.channels )
            cell.SetText( String() );
         else
         {
            const ChannelStatistics& s = m_model.statistics[c];
            if ( row == LowClippedRow || row == HighClippedRow )
            {
               size_type clipped = (row == LowClippedRow) ? s.low : s.high;
               double percent = (m_model.sampleCount > 0) ? 100.0*clipped/m_model.sampleCount : 0.0;
               cell.SetText( String().Format( "%llu (%.3f%%)", (unsigned long long)clipped, percent ) );
            }
            else
               cell.SetText( String().Format( "%.6f", StatValue( s, row ) ) );
         }
      }
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::PaintHistogram( Graphics& G )
{
   const Control& canvas = GUI->Histogram_Control;
   const double s = canvas.DisplayPixelRatio();
   auto px = [s]( double v ) { return RoundInt( v*s ); };

   G.FillRect( canvas.BoundsRect(), Brush( 0xff20242a ) );

   if ( m_model.IsEmpty() )
   {
      G.SetPen( Pen( 0xffeeeeee ) );
      G.DrawText( px( 24 ), px( 32 ), "No active image." );
      return;
   }

   const int left = px( 52 ), top = px( 30 ), bottom = px( 48 ), right = px( 12 );
   const int bins = HistogramModel::Bins;
   const int visibleWidth = std::max( 1, canvas.Width() - left - right );
   const double width = std::max( 1.0, visibleWidth * double( m_instance.p_zoom ) );
   const double offset = std::max( 0.0, width - visibleWidth ) * m_instance.p_pan / 1000;
   const int height = canvas.Height() - top - bottom;

   const std::vector<HistogramModel::histogram>& counts = m_instance.p_logX ? m_model.logarithmic : m_model.linear;
   double maxCount = 1;
   for ( const HistogramModel::histogram& h : counts )
      for ( double n : h )
         maxCount = std::max( maxCount, n );

   const bool channelVisible[] = { bool( m_instance.p_showRed ), bool( m_instance.p_showGreen ), bool( m_instance.p_showBlue ) };
   const RGBA colors[] = { 0x99ff5555, 0x9955ff77, 0x996699ff };

   for ( int channel = 0; channel < int( counts.size() ); ++channel )
   {
      if ( m_model.color && !channelVisible[channel] )
         continue;
      Brush brush( m_model.color ? colors[channel] : RGBA( 0xffd0d0d0 ) );
      const int barWidth = std::max( 1, int( std::ceil( width/bins ) ) );
      for ( int bin = 0; bin < bins; ++bin )
      {
         double normalized = counts[channel][bin]/maxCount;
         if ( m_instance.p_logY )
            normalized = std::log( 1 + counts[channel][bin] )/std::log( 1 + maxCount );
         int x = left + int( std::floor( bin*width/bins ) ) - int( offset );
         int barHeight = int( std::floor( normalized*height ) );
         if ( barHeight > 0 )
         {
            int x0 = std::max( left, x );
            int x1 = std::min( left + visibleWidth, x + barWidth );
            if ( x1 > x0 )
               G.FillRect( x0, top + height - barHeight, x1, top + height, brush );
         }
      }
   }

   G.SetPen( Pen( 0xffeeeeee ) );
   G.DrawLine( left, top + height, left + visibleWidth, top + height );
   G.DrawLine( left, top, left, top + height );

   for ( int tick = 0; tick <= 10; ++tick )
   {
      int tickX = left + int( std::floor( tick*width/10 ) ) - int( offset );
      if ( tickX < left || tickX > left + visibleWidth )
         continue;
      G.DrawLine( tickX, top + height, tickX, top + height + px( 5 ) );
      String xLabel;
      if ( m_instance.p_logX )
         xLabel = ToExponential1( std::pow( 10.0, m_model.logMinimumExponent * (1 - tick/10.0) ) );
      else
         xLabel = String().Format( "%.1f", tick/10.0 );
      G.DrawText( tickX - px( 18 ), top + height + px( 20 ), xLabel );
   }

   for ( int yTick = 0; yTick <= 5; ++yTick )
   {
      double yFraction = yTick/5.0;
      int y = top + height - int( std::floor( yFraction*height ) );
      G.DrawLine( left - px( 5 ), y, left, y );
      if ( yTick > 0 && yTick < 5 )
         G.DrawLine( left, y, left + visibleWidth, y );
      String yLabel;
      if ( m_instance.p_logY )
         yLabel = String().Format( "%.0f", std::pow( 10.0, yFraction*std::log10( maxCount ) ) );
      else
         yLabel = String().Format( "%.0f", std::floor( yFraction*maxCount ) );
      G.DrawText( px( 4 ), y + px( 4 ), yLabel );
   }
   G.DrawText( px( 4 ), top - px( 10 ), "Count" );
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::e_Paint( Control& sender, const pcl::Rect& /*updateRect*/ )
{
   if ( m_modelDirty )
      ComputeModel();

   Graphics G( sender );
   G.SetFont( sender.Font() );
   PaintHistogram( G );
   G.EndPaint();
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::e_Show( Control& /*sender*/ )
{
   if ( m_modelDirty )
      ComputeModel();
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->LogX_PushButton )
      m_instance.p_logX = !m_instance.p_logX;
   else if ( sender == GUI->LogY_PushButton )
      m_instance.p_logY = !m_instance.p_logY;
   else if ( sender == GUI->Stats_PushButton )
   {
      m_instance.p_stats = !m_instance.p_stats;
      UpdateStatsVisibility();
   }
   else if ( sender == GUI->Channel_CheckBox[0] )
      m_instance.p_showRed = checked;
   else if ( sender == GUI->Channel_CheckBox[1] )
      m_instance.p_showGreen = checked;
   else if ( sender == GUI->Channel_CheckBox[2] )
      m_instance.p_showBlue = checked;

   UpdateToggleButtons();
   GUI->Histogram_Control.Update();
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::e_ValueUpdated( Slider& sender, int value )
{
   if ( sender == GUI->Zoom_Slider )
   {
      m_instance.p_zoom = value/100.0F;
      GUI->Range_Slider.Enable( m_instance.p_zoom > 1.001 );
      if ( m_instance.p_zoom <= 1.001 )
      {
         m_instance.p_pan = 0;
         GUI->Range_Slider.SetValue( 0 );
      }
   }
   else if ( sender == GUI->Range_Slider )
      m_instance.p_pan = value;

   GUI->Histogram_Control.Update();
}

// ----------------------------------------------------------------------------

void HistogramViewerInterface::e_ViewSelected( ViewList& /*sender*/, View& view )
{
   if ( !view.IsNull() && view != m_view )
      SetTargetView( view );
}

// ----------------------------------------------------------------------------

HistogramViewerInterface::GUIData::GUIData( HistogramViewerInterface& w )
{
   const int labelWidth = w.Font().Width( String( "Image" ) ) + w.LogicalPixelsToPhysical( 8 );
   const char* cellStyle = "border: 1px solid #4a5158; padding: 2px 4px;";

   // Histogram canvas

   Histogram_Control.SetScaledMinSize( 720, 300 );
   Histogram_Control.OnPaint( (Control::paint_event_handler)&HistogramViewerInterface::e_Paint, w );

   // Target view

   Target_Label.SetText( "Image" );
   Target_Label.SetMinWidth( labelWidth );
   Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Target_ViewList.GetAll();
   Target_ViewList.OnViewSelected( (ViewList::view_event_handler)&HistogramViewerInterface::e_ViewSelected, w );

   Target_Sizer.SetSpacing( 6 );
   Target_Sizer.Add( Target_Label );
   Target_Sizer.Add( Target_ViewList, 100 );

   // Zoom and range

   Zoom_Label.SetText( "Zoom" );
   Zoom_Label.SetMinWidth( labelWidth );
   Zoom_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Zoom_Slider.SetRange( 100, 300 );
   Zoom_Slider.OnValueUpdated( (Slider::value_event_handler)&HistogramViewerInterface::e_ValueUpdated, w );

   Zoom_Sizer.SetSpacing( 8 );
   Zoom_Sizer.Add( Zoom_Label );
   Zoom_Sizer.Add( Zoom_Slider, 100 );

   Range_Label.SetText( "Range" );
   Range_Label.SetMinWidth( labelWidth );
   Range_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Range_Slider.SetRange( 0, 1000 );
   Range_Slider.OnValueUpdated( (Slider::value_event_handler)&HistogramViewerInterface::e_ValueUpdated, w );

   Range_Sizer.SetSpacing( 8 );
   Range_Sizer.Add( Range_Label );
   Range_Sizer.Add( Range_Slider, 100 );

   // Buttons

   LogX_PushButton.SetText( "Log X" );
   LogX_PushButton.SetToolTip( "<p>Logarithmic intensity axis.</p>" );
   LogX_PushButton.OnClick( (Button::click_event_handler)&HistogramViewerInterface::e_Click, w );

   LogY_PushButton.SetText( "Log Y" );
   LogY_PushButton.SetToolTip( "<p>Logarithmic count axis.</p>" );
   LogY_PushButton.OnClick( (Button::click_event_handler)&HistogramViewerInterface::e_Click, w );

   Stats_PushButton.SetText( "Stats" );
   Stats_PushButton.SetToolTip( "<p>Show or hide the statistics table.</p>" );
   Stats_PushButton.OnClick( (Button::click_event_handler)&HistogramViewerInterface::e_Click, w );

   const char* channelNames[] = { "R", "G", "B" };
   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( LogX_PushButton );
   Buttons_Sizer.Add( LogY_PushButton );
   Buttons_Sizer.Add( Stats_PushButton );
   for ( int c = 0; c < 3; ++c )
   {
      Channel_CheckBox[c].SetText( channelNames[c] );
      Channel_CheckBox[c].SetChecked( true );
      Channel_CheckBox[c].OnClick( (Button::click_event_handler)&HistogramViewerInterface::e_Click, w );
      Buttons_Sizer.Add( Channel_CheckBox[c] );
   }

   Histogram_Sizer.SetSpacing( 6 );
   Histogram_Sizer.Add( Histogram_Control, 100 );
   Histogram_Sizer.Add( Target_Sizer );
   Histogram_Sizer.Add( Zoom_Sizer );
   Histogram_Sizer.Add( Range_Sizer );
   Histogram_Sizer.Add( Buttons_Sizer );

   // Statistics table

   Stats_Sizer.SetSpacing( 2 );
   for ( int row = 0; row <= NumberOfStatRows; ++row )
   {
      for ( int col = 0; col < 4; ++col )
      {
         Label& cell = Stat_Label[row][col];
         cell.SetScaledFixedWidth( (col == 0) ? 150 : 72 );
         cell.EnableWordWrapping( col == 0 );
         cell.SetStyleSheet( w.ScaledStyleSheet( cellStyle ) );
         cell.SetText( (row == 0) ? String() : String( (col == 0) ? StatNames[row-1] : "-" ) );
         StatRow_Sizer[row].Add( cell );
      }
      Stats_Sizer.Add( StatRow_Sizer[row] );
   }
   Stats_Sizer.AddStretch();

   Stats_Control.SetSizer( Stats_Sizer );
   Stats_Control.Hide();

   // Global layout

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( Histogram_Sizer, 100 );
   Global_Sizer.Add( Stats_Control );

   w.OnShow( (Control::event_handler)&HistogramViewerInterface::e_Show, w );
   w.SetSizer( Global_Sizer );
   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetVariableSize();
}

// ----------------------------------------------------------------------------

} // pcl
