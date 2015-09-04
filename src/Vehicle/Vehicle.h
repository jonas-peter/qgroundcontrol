/*=====================================================================
 
 QGroundControl Open Source Ground Control Station
 
 (c) 2009 - 2014 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 
 This file is part of the QGROUNDCONTROL project
 
 QGROUNDCONTROL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 QGROUNDCONTROL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.
 
 ======================================================================*/

/// @file
///     @author Don Gagne <don@thegagnes.com>

#ifndef Vehicle_H
#define Vehicle_H

#include <QObject>
#include <QGeoCoordinate>
#include <QQmlListProperty>

#include "LinkInterface.h"
#include "QGCMAVLink.h"
#include "MissionItem.h"

class UAS;
class UASInterface;
class FirmwarePlugin;
class AutoPilotPlugin;
class UASWaypointManager;

Q_DECLARE_LOGGING_CATEGORY(VehicleLog)

class Vehicle : public QObject
{
    Q_OBJECT
    
public:
    Vehicle(LinkInterface* link, int vehicleId, MAV_AUTOPILOT firmwareType);
    ~Vehicle();
    
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(AutoPilotPlugin* autopilot MEMBER _autopilotPlugin CONSTANT)
    
    Q_PROPERTY(QGeoCoordinate coordinate MEMBER _geoCoordinate NOTIFY coordinateChanged)
    
    Q_INVOKABLE QString     getMavIconColor();
    
    //-- System Messages
    Q_PROPERTY(bool         messageTypeNone     READ messageTypeNone    NOTIFY messageTypeChanged)
    Q_PROPERTY(bool         messageTypeNormal   READ messageTypeNormal  NOTIFY messageTypeChanged)
    Q_PROPERTY(bool         messageTypeWarning  READ messageTypeWarning NOTIFY messageTypeChanged)
    Q_PROPERTY(bool         messageTypeError    READ messageTypeError   NOTIFY messageTypeChanged)
    Q_PROPERTY(int          newMessageCount     READ newMessageCount    NOTIFY newMessageCountChanged)
    Q_PROPERTY(int          messageCount        READ messageCount       NOTIFY messageCountChanged)
    Q_PROPERTY(QString      latestError         READ latestError        NOTIFY latestErrorChanged)
    //-- UAV Stats
    Q_PROPERTY(float        roll                READ roll               NOTIFY rollChanged)
    Q_PROPERTY(float        pitch               READ pitch              NOTIFY pitchChanged)
    Q_PROPERTY(float        heading             READ heading            NOTIFY headingChanged)
    Q_PROPERTY(float        groundSpeed         READ groundSpeed        NOTIFY groundSpeedChanged)
    Q_PROPERTY(float        airSpeed            READ airSpeed           NOTIFY airSpeedChanged)
    Q_PROPERTY(float        climbRate           READ climbRate          NOTIFY climbRateChanged)
    Q_PROPERTY(float        altitudeRelative    READ altitudeRelative   NOTIFY altitudeRelativeChanged)
    Q_PROPERTY(float        altitudeWGS84       READ altitudeWGS84      NOTIFY altitudeWGS84Changed)
    Q_PROPERTY(float        altitudeAMSL        READ altitudeAMSL       NOTIFY altitudeAMSLChanged)
    Q_PROPERTY(float        latitude            READ latitude           NOTIFY latitudeChanged)
    Q_PROPERTY(float        longitude           READ longitude          NOTIFY longitudeChanged)
    Q_PROPERTY(double       batteryVoltage      READ batteryVoltage     NOTIFY batteryVoltageChanged)
    Q_PROPERTY(double       batteryPercent      READ batteryPercent     NOTIFY batteryPercentChanged)
    Q_PROPERTY(double       batteryConsumed     READ batteryConsumed    NOTIFY batteryConsumedChanged)
    Q_PROPERTY(bool         systemArmed         READ systemArmed        NOTIFY systemArmedChanged)
    Q_PROPERTY(QString      currentMode         READ currentMode        NOTIFY currentModeChanged)
    Q_PROPERTY(QString      systemPixmap        READ systemPixmap       NOTIFY systemPixmapChanged)
    Q_PROPERTY(int          satelliteCount      READ satelliteCount     NOTIFY satelliteCountChanged)
    Q_PROPERTY(QString      currentState        READ currentState       NOTIFY currentStateChanged)
    Q_PROPERTY(QString      systemName          READ systemName         NOTIFY systemNameChanged)
    Q_PROPERTY(int          satelliteLock       READ satelliteLock      NOTIFY satelliteLockChanged)
    Q_PROPERTY(double       waypointDistance    READ waypointDistance   NOTIFY waypointDistanceChanged)
    Q_PROPERTY(uint16_t     currentWaypoint     READ currentWaypoint    NOTIFY currentWaypointChanged)
    Q_PROPERTY(unsigned int heartbeatTimeout    READ heartbeatTimeout   NOTIFY heartbeatTimeoutChanged)
    
    //-- MissionItem management
    Q_PROPERTY(QQmlListProperty<MissionItem> missionItems READ missionItems NOTIFY missionItemsChanged)
    
    /// Returns the number of buttons which are reserved for firmware use in the MANUAL_CONTROL mavlink
    /// message. For example PX4 Flight Stack reserves the first 8 buttons to simulate rc switches.
    /// The remainder can be assigned to Vehicle actions.
    /// @return -1: reserver all buttons, >0 number of buttons to reserve
    Q_PROPERTY(int manualControlReservedButtonCount READ manualControlReservedButtonCount CONSTANT)
    
    typedef enum {
        JoystickModeRC,         ///< Joystick emulates an RC Transmitter
        JoystickModeAttitude,
        JoystickModePosition,
        JoystickModeForce,
        JoystickModeVelocity,
        JoystickModeMax
    } JoystickMode_t;
    
    /// The joystick mode associated with this vehicle. Joystick modes are stored keyed by mavlink system id.
    Q_PROPERTY(int joystickMode READ joystickMode WRITE setJoystickMode NOTIFY joystickModeChanged)
    int joystickMode(void);
    void setJoystickMode(int mode);
    
    /// List of joystick mode names
    Q_PROPERTY(QStringList joystickModes READ joystickModes CONSTANT)
    QStringList joystickModes(void);
    
    // Enable/Disable joystick for this vehicle
    Q_PROPERTY(bool joystickEnabled READ joystickEnabled WRITE setJoystickEnabled NOTIFY joystickEnabledChanged)
    bool joystickEnabled(void);
    void setJoystickEnabled(bool enabled);
    
    // Is vehicle active with respect to current active vehicle in QGC
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    bool active(void);
    void setActive(bool active);
    
    // Property accesors
    int id(void) { return _id; }
    MAV_AUTOPILOT firmwareType(void) { return _firmwareType; }
    
    /// Sends this specified message to all links accociated with this vehicle
    void sendMessage(mavlink_message_t message);
    
    /// Provides access to uas from vehicle. Temporary workaround until UAS is fully phased out.
    UAS* uas(void) { return _uas; }
    
    /// Provides access to uas from vehicle. Temporary workaround until AutoPilotPlugin is fully phased out.
    AutoPilotPlugin* autopilotPlugin(void) { return _autopilotPlugin; }
    
    QList<LinkInterface*> links(void);
    
    int manualControlReservedButtonCount(void);
    
    typedef enum {
        MessageNone,
        MessageNormal,
        MessageWarning,
        MessageError
    } MessageType_t;
    
    enum {
        ROLL_CHANGED,
        PITCH_CHANGED,
        HEADING_CHANGED,
        GROUNDSPEED_CHANGED,
        AIRSPEED_CHANGED,
        CLIMBRATE_CHANGED,
        ALTITUDERELATIVE_CHANGED,
        ALTITUDEWGS84_CHANGED,
        ALTITUDEAMSL_CHANGED
    };
    
    // Called when the message drop-down is invoked to clear current count
    void resetMessages();
    
    
    bool            messageTypeNone     () { return _currentMessageType == MessageNone; }
    bool            messageTypeNormal   () { return _currentMessageType == MessageNormal; }
    bool            messageTypeWarning  () { return _currentMessageType == MessageWarning; }
    bool            messageTypeError    () { return _currentMessageType == MessageError; }
    int             newMessageCount     () { return _currentMessageCount; }
    int             messageCount        () { return _messageCount; }
    QString         latestError         () { return _latestError; }
    float           roll                () { return _roll; }
    float           pitch               () { return _pitch; }
    float           heading             () { return _heading; }
    float           groundSpeed         () { return _groundSpeed; }
    float           airSpeed            () { return _airSpeed; }
    float           climbRate           () { return _climbRate; }
    float           altitudeRelative    () { return _altitudeRelative; }
    float           altitudeWGS84       () { return _altitudeWGS84; }
    float           altitudeAMSL        () { return _altitudeAMSL; }
    float           latitude            () { return _latitude; }
    float           longitude           () { return _longitude; }
    bool            mavPresent          () { return _mav != NULL; }
    int             satelliteCount      () { return _satelliteCount; }
    double          batteryVoltage      () { return _batteryVoltage; }
    double          batteryPercent      () { return _batteryPercent; }
    double          batteryConsumed     () { return _batteryConsumed; }
    bool            systemArmed         () { return _systemArmed; }
    QString         currentMode         () { return _currentMode; }
    QString         systemPixmap        () { return _systemPixmap; }
    QString         currentState        () { return _currentState; }
    QString         systemName          () { return _systemName; }
    int             satelliteLock       () { return _satelliteLock; }
    double          waypointDistance    () { return _waypointDistance; }
    uint16_t        currentWaypoint     () { return _currentWaypoint; }
    unsigned int    heartbeatTimeout    () { return _currentHeartbeatTimeout; }
    
    QQmlListProperty<MissionItem> missionItems() {return QQmlListProperty<MissionItem>(this, _waypoints); }
    
public slots:
    void setLatitude(double latitude);
    void setLongitude(double longitude);
    
signals:
    void allLinksDisconnected(Vehicle* vehicle);
    void coordinateChanged(QGeoCoordinate coordinate);
    void joystickModeChanged(int mode);
    void joystickEnabledChanged(bool enabled);
    void activeChanged(bool active);
    
    /// Used internally to move sendMessage call to main thread
    void _sendMessageOnThread(mavlink_message_t message);
    
    void messageTypeChanged     ();
    void newMessageCountChanged ();
    void messageCountChanged    ();
    void latestErrorChanged     ();
    void rollChanged            ();
    void pitchChanged           ();
    void headingChanged         ();
    void groundSpeedChanged     ();
    void airSpeedChanged        ();
    void climbRateChanged       ();
    void altitudeRelativeChanged();
    void altitudeWGS84Changed   ();
    void altitudeAMSLChanged    ();
    void latitudeChanged        ();
    void longitudeChanged       ();
    void batteryVoltageChanged  ();
    void batteryPercentChanged  ();
    void batteryConsumedChanged ();
    void systemArmedChanged     ();
    void heartbeatTimeoutChanged();
    void currentModeChanged     ();
    void currentConfigChanged   ();
    void systemPixmapChanged    ();
    void satelliteCountChanged  ();
    void currentStateChanged    ();
    void systemNameChanged      ();
    void satelliteLockChanged   ();
    void waypointDistanceChanged();
    void currentWaypointChanged ();
    void missionItemsChanged    ();
    
private slots:
    void _mavlinkMessageReceived(LinkInterface* link, mavlink_message_t message);
    void _linkDisconnected(LinkInterface* link);
    void _sendMessage(mavlink_message_t message);
    
    void _handleTextMessage                 (int newCount);
    /** @brief Attitude from main autopilot / system state */
    void _updateAttitude                    (UASInterface* uas, double roll, double pitch, double yaw, quint64 timestamp);
    /** @brief Attitude from one specific component / redundant autopilot */
    void _updateAttitude                    (UASInterface* uas, int component, double roll, double pitch, double yaw, quint64 timestamp);
    /** @brief Speed */
    void _updateSpeed                       (UASInterface* uas, double _groundSpeed, double _airSpeed, quint64 timestamp);
    /** @brief Altitude */
    void _updateAltitude                    (UASInterface* uas, double _altitudeAMSL, double _altitudeWGS84, double _altitudeRelative, double _climbRate, quint64 timestamp);
    void _updateNavigationControllerErrors  (UASInterface* uas, double altitudeError, double speedError, double xtrackError);
    void _updateNavigationControllerData    (UASInterface *uas, float navRoll, float navPitch, float navBearing, float targetBearing, float targetDistance);
    void _checkUpdate                       ();
    void _updateBatteryRemaining            (UASInterface*, double voltage, double, double percent, int);
    void _updateBatteryConsumedChanged      (UASInterface*, double current_consumed);
    void _updateArmingState                 (bool armed);
    void _updateState                       (UASInterface* system, QString name, QString description);
    void _updateMode                        (int system, QString name, QString description);
    void _updateName                        (const QString& name);
    void _setSystemType                     (UASInterface* uas, unsigned int systemType);
    void _heartbeatTimeout                  (bool timeout, unsigned int ms);
    void _updateCurrentWaypoint             (quint16 id);
    void _updateWaypointDistance            (double distance);
    void _setSatelliteCount                 (double val, QString name);
    void _setSatLoc                         (UASInterface* uas, int fix);
    void _updateWaypointViewOnly            (int uas, MissionItem* wp);
    void _waypointViewOnlyListChanged       ();

private:
    bool _containsLink(LinkInterface* link);
    void _addLink(LinkInterface* link);
    void _loadSettings(void);
    void _saveSettings(void);
    void _startJoystick(bool start);
    
    bool    _isAirplane                     ();
    void    _addChange                      (int id);
    float   _oneDecimal                     (float value);

private:
    int     _id;            ///< Mavlink system id
    bool    _active;
    
    MAV_AUTOPILOT       _firmwareType;
    FirmwarePlugin*     _firmwarePlugin;
    AutoPilotPlugin*    _autopilotPlugin;
    
    /// List of all links associated with this vehicle. We keep SharedLinkInterface objects
    /// which are QSharedPointer's in order to maintain reference counts across threads.
    /// This way Link deletion works correctly.
    QList<SharedLinkInterface> _links;
    
    JoystickMode_t  _joystickMode;
    bool            _joystickEnabled;
    
    UAS* _uas;
    
    QGeoCoordinate  _geoCoordinate;
    
    UASInterface*   _mav;
    int             _currentMessageCount;
    int             _messageCount;
    int             _currentErrorCount;
    int             _currentWarningCount;
    int             _currentNormalCount;
    MessageType_t   _currentMessageType;
    QString         _latestError;
    float           _roll;
    float           _pitch;
    float           _heading;
    float           _altitudeAMSL;
    float           _altitudeWGS84;
    float           _altitudeRelative;
    float           _groundSpeed;
    float           _airSpeed;
    float           _climbRate;
    float           _navigationAltitudeError;
    float           _navigationSpeedError;
    float           _navigationCrosstrackError;
    float           _navigationTargetBearing;
    float           _latitude;
    float           _longitude;
    QTimer*         _refreshTimer;
    QList<int>      _changes;
    double          _batteryVoltage;
    double          _batteryPercent;
    double          _batteryConsumed;
    bool            _systemArmed;
    QString         _currentState;
    QString         _currentMode;
    QString         _systemName;
    QString         _systemPixmap;
    unsigned int    _currentHeartbeatTimeout;
    double          _waypointDistance;
    quint16         _currentWaypoint;
    int             _satelliteCount;
    int             _satelliteLock;
    UASWaypointManager* _wpm;
    int             _updateCount;
    QList<MissionItem*>_waypoints;
    
    static const char* _settingsGroup;
    static const char* _joystickModeSettingsKey;
    static const char* _joystickEnabledSettingsKey;
};
#endif
