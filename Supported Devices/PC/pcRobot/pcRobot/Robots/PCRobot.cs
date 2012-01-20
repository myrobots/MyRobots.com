using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Timers;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Net.NetworkInformation;
using pcRobot.Services;
using Microsoft.Win32;

namespace pcRobot.Robots {
  class PCRobot : INotifyPropertyChanged {

    #region Fields

    private const string REG_KEY_PC_ROBOT = "pcRobot";
    private const string REG_VALUE_ROBOT_KEY = "robotKey";
    private const string REG_VALUE_ROBOT_URL = "robotUrl";
    private const string REG_VALUE_MY_ROBOTS_INTERVAL = "myRobotsInterval";

    private RegistryKey _registryKey;

    private string _robotKey;
    private string _robotUrl;
    private int _myRobotsUpdateInterval;

    private string _status;
    private bool _hasStatusChanged;
    private float _cpuUsage;
    private float _ramUsage;
    private int _nbProcesses;
    private int _nbThreads;
    private TimeSpan _upTime;
    private int _diskIO;      // KB/Sec
    private long _networkIO;   // Kbps

    private string _myRobotsUpdateStatus;

    private Timer _sensorsTimer;
    private PerformanceCounter _cpuCounter;
    private PerformanceCounter _ramCounter;
    private PerformanceCounter _threadCounter;
    private PerformanceCounter _upTimeCounter;
    private PerformanceCounter _diskIOCounter;
    private PerformanceCounter[] _networkCounters;
    private int _totalRAM;

    private Timer _myRobotsTimer;

    #endregion

    #region Properties

    public string RobotKey {
      get { return _robotKey; }
      set { 
        _robotKey = value;
        this.UpdateMyRobotsTimer();
        this.NotifyPropertyChanged("RobotKey");
        this.SetRegistryKey(REG_VALUE_ROBOT_KEY, value);
      }
    }

    public string RobotUrl {
      get { return _robotUrl; }
      set { 
        _robotUrl = value;
        this.UpdateMyRobotsTimer(); 
        this.NotifyPropertyChanged("RobotUrl");
        this.SetRegistryKey(REG_VALUE_ROBOT_URL, value);
      }
    }

    public int MyRobotsUpdateInterval {
      get { return _myRobotsUpdateInterval; }
      set { 
        _myRobotsUpdateInterval = value;
        this.UpdateMyRobotsTimer();
        this.NotifyPropertyChanged("MyRobotsUpdateInterval");
        this.SetRegistryKey(REG_VALUE_MY_ROBOTS_INTERVAL, value);
      }
    }

    public string Status {
      get { return _status; }
      set {
          _status = value;
          this._hasStatusChanged = true;
          this.NotifyPropertyChanged("Status");
      }
    }

    public float CPUUsage {
      get { return _cpuUsage; }
      private set { _cpuUsage = value; this.NotifyPropertyChanged("CPUUsage"); }
    }

    public float RAMUsage {
      get { return _ramUsage; }
      private set { _ramUsage = value; this.NotifyPropertyChanged("RAMUsage"); }
    }

    public int NbProcesses {
      get { return _nbProcesses; }
      private set { _nbProcesses = value; this.NotifyPropertyChanged("NbProcesses"); }
    }

    public int NbThreads {
      get { return _nbThreads; }
      private set { _nbThreads = value; this.NotifyPropertyChanged("NbThreads"); }
    }

    public TimeSpan UpTime {
      get { return _upTime; }
      set { _upTime = value; this.NotifyPropertyChanged("UpTime"); }
    }

    public int DiskIO {
      get { return _diskIO; }
      private set { _diskIO = value; this.NotifyPropertyChanged("DiskIO"); }
    }

    public long NetworkIO {
      get { return _networkIO; }
      private set { _networkIO = value; this.NotifyPropertyChanged("NetworkIO"); }
    }

    public string MyRobotsUpdateStatus {
      get { return this._myRobotsUpdateStatus; }
      private set {
        this._myRobotsUpdateStatus = value;
        this.NotifyPropertyChanged("MyRobotsUpdateStatus");
      }
    }

    #endregion

    #region Constructors

    public PCRobot() {
      this.Status = "Started at " + DateTime.Now.ToString();

      this._myRobotsTimer = new Timer();
      this._myRobotsTimer.Enabled = false;
      this._myRobotsTimer.Elapsed += new ElapsedEventHandler(UpdateMyRobots);

      this._cpuCounter = new PerformanceCounter("Processor", "% Processor Time", "_Total");
      this._ramCounter = new PerformanceCounter("Memory", "Available MBytes");
      this._totalRAM = GetTotalMBytes();
      this._threadCounter = new PerformanceCounter("Process", "Thread Count", "_Total");
      this._upTimeCounter = new PerformanceCounter("System", "System Up Time");
      this._diskIOCounter = new PerformanceCounter("PhysicalDisk", "Disk Transfers/sec", "_Total"); // KB/sec

      // Update sensors at least once and then start timer
      UpdateSensors(null,null);
      this._sensorsTimer = new Timer(1000);
      this._sensorsTimer.Elapsed += new ElapsedEventHandler(UpdateSensors);
      this._sensorsTimer.Enabled = true;

      // Check for url and key in registry
      this._registryKey = Registry.LocalMachine.OpenSubKey("SOFTWARE\\" + REG_KEY_PC_ROBOT, true);
      if (this._registryKey != null) {
        try {
          this.RobotKey = (string)this._registryKey.GetValue(REG_VALUE_ROBOT_KEY);
          this.RobotUrl = (string)this._registryKey.GetValue(REG_VALUE_ROBOT_URL);
          this.MyRobotsUpdateInterval = (int)this._registryKey.GetValue(REG_VALUE_MY_ROBOTS_INTERVAL);
        }
        catch (Exception ex) {

        }
      }
      else {
        // Create the key with default values
        this._registryKey = Registry.LocalMachine.CreateSubKey("SOFTWARE\\" + REG_KEY_PC_ROBOT);
        this.RobotUrl = "http://bots.myrobots.com/";
        this.MyRobotsUpdateInterval = 30;
      }
    }

    #endregion

    #region Sensors Logic

    void UpdateMyRobotsTimer() {
      // Make sure we have all information
      if (!string.IsNullOrEmpty(this.RobotKey) && !string.IsNullOrEmpty(this.RobotUrl) && this.MyRobotsUpdateInterval > 0) {
        this._myRobotsTimer.Interval = this.MyRobotsUpdateInterval * 1000;
        this._myRobotsTimer.Enabled = true;

        // Do not wait, send right now
        UpdateMyRobots(null, null);
      }
      else {
        this._myRobotsTimer.Enabled = false;
      }
    }

    void UpdateSensors(object sender, ElapsedEventArgs e) {
      this.CPUUsage = this._cpuCounter.NextValue();
      this.RAMUsage = (this._totalRAM - this._ramCounter.NextValue()) / this._totalRAM * 100.0f;
      this.NbProcesses = Process.GetProcesses().Length;
      this.NbThreads = (int)this._threadCounter.NextValue();
      this.UpTime = TimeSpan.FromSeconds(this._upTimeCounter.NextValue());
      this.DiskIO = (int)this._diskIOCounter.NextValue();
      this.NetworkIO = GetNetworkIO();
    }

    void UpdateMyRobots(object sender, ElapsedEventArgs e) {
      ThingSpeak.APIKey = this.RobotKey;
      ThingSpeak.Url = this.RobotUrl;

      this.MyRobotsUpdateStatus = DateTime.Now.ToString() + " : Sending information to myRobots";

      try {
        short response;

        // Update status
        if (this._hasStatusChanged && !string.IsNullOrEmpty(this.Status)) {
          if (!ThingSpeak.UpdateThingkSpeakStatus(this.Status, out response)) {
            throw new Exception("ThingSpeak error response : " + response.ToString());
          }

          this._hasStatusChanged = false;
        }

        // Send sensors
        if (!ThingSpeak.SendDataToThingSpeak(string.Format("{0:0.00}", this.CPUUsage),
                                              string.Format("{0:0.00}", this.RAMUsage),
                                              this.NbProcesses.ToString(),
                                              this.NbThreads.ToString(),
                                              this.UpTime.ToString(),
                                              this.DiskIO.ToString(), 
                                              this.NetworkIO.ToString(), 
                                              null,
                                              out response)) {
          throw new Exception("ThingSpeak error response : " + response.ToString());
        }

        this.MyRobotsUpdateStatus = DateTime.Now.ToString() + " : Information sent to myRobots";
      }
      catch (Exception ex) {
        this.MyRobotsUpdateStatus = DateTime.Now.ToString() + " : " + ex.Message ;
      }
    }

    #endregion

    #region Helpers

    int GetTotalMBytes() {
      return (int)(new Microsoft.VisualBasic.Devices.ComputerInfo().TotalPhysicalMemory / 1024 / 1024);
    }

    long GetNetworkIO() {
      if (this._networkCounters == null) {
          PerformanceCounterCategory category =  new PerformanceCounterCategory("Network Interface");
          string[] instanceNames = category.GetInstanceNames();

          this._networkCounters = new PerformanceCounter[instanceNames.Length];

          for (int i = 0; i < instanceNames.Length; i++) {
            this._networkCounters[i] = new PerformanceCounter("Network Interface", "Bytes Total/sec", instanceNames[i]);
          }
      }


      long bps = 0;

      for (int i = 0; i < this._networkCounters.Length; i++) {
        bps += (int)this._networkCounters[i].NextValue();
      }

      return bps / 1024;
    }

    void SetRegistryKey(string key, object value)
    {
      try {
        if (this._registryKey != null) {
          this._registryKey.SetValue(key, value);
        }
      }
      catch (Exception ex) {
        this._registryKey = null;
      }
    }

    #endregion

    #region INotifyPropertyChanged Members

    public event PropertyChangedEventHandler PropertyChanged;

    private void NotifyPropertyChanged(string propertyName) {
      if (PropertyChanged != null) {
        PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
      }
    }

    #endregion
  }
}
