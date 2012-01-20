using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Timers;
using pcRobot.Robots;
using pcRobot.Services;
using pcRobot.Infrastructure;

namespace pcRobot {
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {

    public MainWindow() {
      InitializeComponent();

      MinimizeToTray.Enable(this);

      // Let the window shoe right away
      this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Background,
          new Action(
            delegate() {
              this.DataContext = new PCRobot();
            }
        ));
    }
  }
}
